#include "PNGConverter.hpp"

#include "load_save_png.hpp"
#include "read_write_chunk.hpp"

#include <filesystem>
#include <bitset>

bool PNGConverter::convert_pngs_to_assets(std::string png_folder)
{
    // identify all pngs in the folder
    // finding png's code adapted from https://stackoverflow.com/questions/11140483/how-to-get-list-of-files-with-a-specific-extension-in-a-given-folder
    std::string ext(".png");
    for (auto &p : std::filesystem::recursive_directory_iterator(png_folder))
    {
        if (p.path().extension() == ext){
            std::string path_name = p.path().string();

            std::cout<< path_name << std::endl;
            glm::uvec2 size;
            std::vector<glm::u8vec4> data;
            load_png(path_name, &size, &data, OriginLocation::UpperLeftOrigin);
            
            // all importing pngs needs to be divisible by the tile size
            assert((size.x / sprite_dimension) * sprite_dimension == size.x 
                && (size.y / sprite_dimension) * sprite_dimension == size.y);

            // check how many sprites are needed:
            uint16_t x_sprite_count = uint16_t(size.x / sprite_dimension);
            uint16_t y_sprite_count = uint16_t(size.y / sprite_dimension);
            uint16_t total_sprites_count = x_sprite_count * y_sprite_count;

            std::vector<glm::u8vec4> seen_colors;

            uint16_t sprite_col = 0;
            uint16_t row = 0;
            uint16_t sprite_index = 0;
            
            struct SingleSprite {
                std::vector<uint8_t> bit0;
                std::vector<uint8_t> bit1;
            };

            std::vector<SingleSprite> sprites(total_sprites_count);
            for (uint32_t i = 0; i < data.size(); i += sprite_dimension){
                
                // pack the entire sprite row into two uint8
                uint8_t curr_sprite_row_bit0 = 0;
                uint8_t curr_sprite_row_bit1 = 0;
                for (uint32_t curr_i = i; curr_i < i + sprite_dimension; ++curr_i){
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
                    curr_sprite_row_bit0 = curr_sprite_row_bit0 << 1 | uint8_t(bit0);
                    curr_sprite_row_bit1 = curr_sprite_row_bit1 << 1 | uint8_t(bit1);

                }
                sprites[sprite_index].bit0.push_back(curr_sprite_row_bit0);
                sprites[sprite_index].bit1.push_back(curr_sprite_row_bit1);

                // update indices
                sprite_col++;
                sprite_index++;
                // need to switch to next pixel row
                if (sprite_col == x_sprite_count) {
                    sprite_col = 0;
                    row++;
                    // we are staying at the same sprite row
                    if (row != sprite_dimension) {
                        sprite_index -= x_sprite_count;
                    }
                }

            }
            for (SingleSprite sprite : sprites){
                for (int i = 0; i < sprite.bit0.size(); i++){
                    std::bitset<8> bit0(sprite.bit0[ i ]);
                    std::bitset<8> bit1(sprite.bit1[ i ]);
                    std::cout<< bit0 << " " << bit1 << std::endl;
                }
            }
            for (auto color : seen_colors){
                std::cout<< uint32_t(color.x) << " "<<uint32_t(color.y) << " " << uint32_t(color.z) << " "<< uint32_t(color.w)  << std::endl;
            }
            
            //save_png(asset_root + '/' + "test", size, data.data(), UpperLeftOrigin);
        }
    }
    
    return true;
}