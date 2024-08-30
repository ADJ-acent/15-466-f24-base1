#include "PNGConverter.hpp"

#include "load_save_png.hpp"
#include "read_write_chunk.hpp"

#include <filesystem>
#include <fstream>
#include <bitset>



void PNGConverter::convert_pngs_to_assets(std::string png_folder)
{
    // identify all pngs in the folder
    // finding png's code adapted from https://stackoverflow.com/questions/11140483/how-to-get-list-of-files-with-a-specific-extension-in-a-given-folder
    std::string ext(".png");
    for (auto &p : std::filesystem::recursive_directory_iterator(png_folder))
    {
        if (p.path().extension() == ext){
            std::string path_name = p.path().string();
            std::string tile_name = p.path().stem().string();

            glm::uvec2 size;
            std::vector<glm::u8vec4> data;
            load_png(path_name, &size, &data, OriginLocation::UpperLeftOrigin);
            
            // all importing pngs needs to be divisible by the tile size
            assert((size.x / tile_dimension) * tile_dimension == size.x 
                && (size.y / tile_dimension) * tile_dimension == size.y);

            // check how many tiles are needed:
            uint16_t x_tile_count = uint16_t(size.x / tile_dimension);
            uint16_t y_tile_count = uint16_t(size.y / tile_dimension);
            uint16_t total_tiles_count = x_tile_count * y_tile_count;

            std::vector<glm::u8vec4> seen_colors;

            uint16_t tile_col = 0;
            uint16_t row = 0;
            uint16_t tile_index = 0;
            


            std::vector<SingleTile> tiles(total_tiles_count);

            for (uint32_t i = 0; i < data.size(); i += tile_dimension){
                
                // pack the entire tile row into two uint8
                uint8_t curr_tile_row_bit0 = 0;
                uint8_t curr_tile_row_bit1 = 0;
                for (uint32_t curr_i = i; curr_i < i + tile_dimension; ++curr_i){
                    glm::u8vec4 pixel_color = data[curr_i];
                    uint8_t color_index = 0;

                    {// find index of the color, insert new color if needed
                        auto found_it = std::find(seen_colors.begin(), seen_colors.end(), pixel_color);
                        if (found_it == seen_colors.end()){
                            seen_colors.push_back(pixel_color);
                            assert(seen_colors.size() <= size_t(4)); // shouldn't have more than 4 colors
                            color_index = uint8_t(seen_colors.size() - 1);
                        }
                        else{
                            color_index = uint8_t(found_it - seen_colors.begin());
                        }
                    }
                    
                    // convert index to two bits
                    bool bit0 = color_index & 1;
                    bool bit1 = color_index & 2;
                    curr_tile_row_bit0 = curr_tile_row_bit0 << 1 | uint8_t(bit0);
                    curr_tile_row_bit1 = curr_tile_row_bit1 << 1 | uint8_t(bit1);

                }
                tiles[tile_index].bit0.push_back(curr_tile_row_bit0);
                tiles[tile_index].bit1.push_back(curr_tile_row_bit1);

                // update indices
                tile_col++;
                tile_index++;
                // need to switch to next pixel row
                if (tile_col == x_tile_count) {
                    tile_col = 0;
                    row++;
                    // we are staying at the same tile row
                    if (row != tile_dimension) {
                        tile_index -= x_tile_count;
                    }
                }

            }
            // for (SingleTile tile : tiles){
            //     for (int i = 0; i < tile.bit0.size(); i++){
            //         std::bitset<8> bit0(tile.bit0[ i ]);
            //         std::bitset<8> bit1(tile.bit1[ i ]);
            //         std::cout<< bit0 << " " << bit1 << std::endl;
            //     }
            // }
            for (auto color : seen_colors){
                std::cout<< uint32_t(color.x) << " "<<uint32_t(color.y) << " " << uint32_t(color.z) << " "<< uint32_t(color.w)  << std::endl;
            }
            
            generate_tile_data_file(tiles, tile_name, uint8_t(x_tile_count), uint8_t(y_tile_count), 3);
        }
    }
    
    return;
}

void PNGConverter::generate_tile_data_file(std::vector<SingleTile> &tiles, std::string tile_name,
     uint8_t x_tile_count, uint8_t y_tile_count, uint8_t pallet_index)
{
    assert(asset_root != std::string()); // shouldn't be uninitialized
    assert(tiles.size() < size_t(64)); // can only have 64 total tiles
    assert(tiles.size() == x_tile_count * y_tile_count);
    static std::string tile_root = asset_root + "/" + "tiles";
    std::string tile_directory = tile_root;
    uint32_t tile_count = uint32_t(tiles.size());
    bool is_single_tile = tile_count == 1;
    if (!is_single_tile) {
        tile_directory = asset_root + "/" + tile_name;
        std::filesystem::create_directory(tile_directory);
    }
    for (uint8_t i = 0; i < x_tile_count; ++i){
        for (uint8_t j = 0; j < y_tile_count; ++j){
            SingleTile tile = tiles[i * x_tile_count + j];
            /* position info row contains the current row and column of the tile
                x x x x         x x x x 
                --row--      --column--

                pallet info row contains the pallet index
                these rows are stored in the first two rows of bit 0
            */
            tile.bit0.insert(tile.bit0.begin(), pallet_index);
            uint8_t position_info_row = ((i << 4) | j);
            tile.bit0.insert(tile.bit0.begin(), position_info_row);
            std::ofstream bit0_file(tile_directory + "/" + tile_name + "_" + std::to_string(i * x_tile_count + j) + "_" + "bit0");
            std::ofstream bit1_file(tile_directory + "/" + tile_name + "_" + std::to_string(i * x_tile_count + j) + "_" + "bit1");
            write_chunk("BIT0", tile.bit0, &bit0_file);
            write_chunk("BIT1", tile.bit1, &bit1_file);
            bit0_file.close();


            std::ifstream bit0_file_i(tile_directory + "/" + tile_name + "_" + std::to_string(i * x_tile_count + j) + "_" + "bit0", std::ios::binary);
            std::cout<< tile_directory + "/" + tile_name + "_" + std::to_string(i * x_tile_count + j) + "_" + "bit0"<< std::endl;
            std::vector<uint8_t> bit0;
            read_chunk(bit0_file_i, "BIT0", &bit0);
            // for (int k = 0; k < bit0.size(); ++k) {
            //     std::bitset<8> bi_t0(bit0[ i ]);
            //     std::cout<< bi_t0  << std::endl;
            // }
        }
    }
}
