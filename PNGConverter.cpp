#include "PNGConverter.hpp"

#include "load_save_png.hpp"
#include "read_write_chunk.hpp"

#include <filesystem>
#include <fstream>
#include <unordered_set>

// hash function for glm::u8vec4
namespace std {
    template <>
    struct hash<glm::u8vec4> {
        size_t operator()(const glm::u8vec4& v) const noexcept {
            return ((static_cast<size_t>(v.r) << 24) |
                    (static_cast<size_t>(v.g) << 16) |
                    (static_cast<size_t>(v.b) << 8) |
                    static_cast<size_t>(v.a));
        }
    };
}

void PNGConverter::convert_pngs_to_assets(std::string png_folder)
{
    // vector of all the palettes we see from the pngs
    std::vector<SavedPalette> saved_palettes;
    std::vector<std::unordered_set<glm::u8vec4>> palettes; // temp structure for search

    std::string ext(".png");
    
    {// fetch all the color palettes:
        // identify all pngs in the folder
        // finding png's code adapted from https://stackoverflow.com/questions/11140483/how-to-get-list-of-files-with-a-specific-extension-in-a-given-folder
        for (auto &p : std::filesystem::recursive_directory_iterator(png_folder))
        {
            if (p.path().extension() == ext){
                std::unordered_set<glm::u8vec4> curr_palette;
                std::string path_name = p.path().string();
                glm::uvec2 size;
                std::vector<glm::u8vec4> data;
                load_png(path_name, &size, &data, OriginLocation::UpperLeftOrigin);
                for (glm::u8vec4 color : data) {
                    curr_palette.insert(color);
                }
                bool new_palette = true;
                for (std::unordered_set<glm::u8vec4> seen_palette : palettes) {
                    if (seen_palette == curr_palette) new_palette = false;
                }
                if (new_palette) palettes.push_back(curr_palette);
            }
        }
        assert(palettes.size() <= 8);
        for (uint8_t i = 0; i < uint8_t(palettes.size()); ++i){
            SavedPalette palette;
            int count = 0;
            for (auto color : palettes[i]) {
                palette.color[count][0] = color.r;
                palette.color[count][1] = color.g;
                palette.color[count][2] = color.b;
                palette.color[count][3] = color.a;
                ++count;
            }
            saved_palettes.push_back(palette);
        }
    }



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

            uint8_t palette_index = 0;
            { // find which palette we are using
                std::unordered_set<glm::u8vec4> curr_palette;
                for (glm::u8vec4 color : data) {
                    curr_palette.insert(color);
                }
                auto found_it = std::find(palettes.begin(), palettes.end(), curr_palette);
                assert(found_it != palettes.end());
                palette_index = uint8_t(found_it - palettes.begin());
            }

            uint16_t tile_col = 0;
            uint16_t row = 0;
            uint16_t tile_index = 0;

            struct SingleTile {
                std::vector<uint8_t> bit0;
                std::vector<uint8_t> bit1;
            };
            std::vector<SingleTile> tiles(total_tiles_count);

            // fill tiles with single tiles' color info
            
            for (uint32_t i = 0; i < size.x * size.y; i += tile_dimension){
                assert(tile_index < total_tiles_count);
                // pack the entire tile row into two uint8
                uint8_t curr_tile_row_bit0 = 0;
                uint8_t curr_tile_row_bit1 = 0;
                for (uint32_t curr_i = i; curr_i < i + tile_dimension; ++curr_i){
                    glm::u8vec4 pixel_color = data[curr_i];
                    uint8_t color_index = 4;
                    {// find index of the color:
                        SavedPalette palette = saved_palettes[palette_index];
                        for (uint8_t color_i = 0; color_i< 4; color_i++) {
                            glm::u8vec4 palette_color = {palette.color[color_i][0], palette.color[color_i][1], palette.color[color_i][2], palette.color[color_i][3]};
                            if (pixel_color == palette_color){
                                color_index = color_i;
                                break;
                            }
                        }
                        assert(color_index != 4); // only 0 - 3 is possible since 4 colors total
                    }
                    // convert index to two bits
                    bool bit0 = color_index & 1;
                    bool bit1 = color_index & 2;
                    curr_tile_row_bit0 = curr_tile_row_bit0 << 1 | uint8_t(bit0);
                    curr_tile_row_bit1 = curr_tile_row_bit1 << 1 | uint8_t(bit1);

                }
                tiles[tile_index].bit0.push_back(curr_tile_row_bit0);
                tiles[tile_index].bit1.push_back(curr_tile_row_bit1);

                assert(tiles[tile_index].bit0.size() <= 8);
                assert(tiles[tile_index].bit1.size() <= 8);

                // update indices
                tile_col++;
                tile_index++;
                // need to switch to next pixel row
                if (tile_col == x_tile_count) {
                    tile_col = 0;
                    row++;
                    // we are staying at the same tile row
                    if (row % tile_dimension != 0) {
                        tile_index -= x_tile_count;
                    }
                }

            }

            // for (auto color : seen_colors){
            //     std::cout<< uint32_t(color.x) << " "<<uint32_t(color.y) << " " << uint32_t(color.z) << " "<< uint32_t(color.w)  << std::endl;
            // }
            

            // convert tiles into write_chunk readable format
            std::vector<SavedTile> saved_tiles;
            for (uint16_t i = 0; i < uint16_t(tiles.size()); ++i){
                SavedTile cur_saved_tile;
                SingleTile read_tile = tiles[i];
                cur_saved_tile.row = uint8_t(i / x_tile_count);
                cur_saved_tile.col = uint8_t(i % x_tile_count);
                cur_saved_tile.palette = palette_index;
                assert(read_tile.bit0.size() == 8);
                assert(read_tile.bit1.size() == 8);
                std::copy(read_tile.bit0.begin(), read_tile.bit0.end(), cur_saved_tile.bit0);
                std::copy(read_tile.bit1.begin(), read_tile.bit1.end(), cur_saved_tile.bit1);

                saved_tiles.push_back(cur_saved_tile);
            }

            generate_tile_data_files(saved_tiles, tile_name);
        }
        
    }
    generate_palette_data_files(saved_palettes);
    return;
}

void PNGConverter::generate_tile_data_files(std::vector<SavedTile> &tiles, std::string tile_name)
{
    assert(asset_root != std::string()); // shouldn't be uninitialized
    assert(tiles.size() < size_t(64)); // can only have 64 total tiles
    static std::string tile_root = asset_root + "/" + "tiles";

    std::string tile_path = tile_root + "/" + tile_name + ".tile";
    std::ofstream tile_file(tile_path);
    write_chunk("TILE", tiles, &tile_file);
    tile_file.close();
}

void PNGConverter::load_tile_data_files(std::string tile_path, std::vector<SavedTile>& out_tiles)
{
    std::ifstream tile_file(tile_path, std::ios::binary);
    read_chunk(tile_file, "TILE", &out_tiles);
    for (auto saved : out_tiles){
        saved.print();
    }
    tile_file.close();
}

void PNGConverter::generate_palette_data_files(std::vector<SavedPalette> palettes)
{
    assert(asset_root != std::string()); // shouldn't be uninitialized

    std::string palettes_path = asset_root + "/palettes.pal";
    std::ofstream palettes_file(palettes_path);
    write_chunk("PALE", palettes, &palettes_file);
    palettes_file.close();

}
