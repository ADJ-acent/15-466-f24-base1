#pragma once

#include <string>
#include <vector>
#include <bitset>
#include <iostream>

struct PNGConverter {
    PNGConverter(std::string save_file_path) : asset_root(save_file_path){};
    ~PNGConverter() = default;

    const uint8_t tile_dimension = 8;
    std::string asset_root;

    void convert_pngs_to_assets(std::string png_folder);

    struct SavedTile {
        uint8_t row, col, palette;
        uint8_t bit0[8];
        uint8_t bit1[8];

        void print(){
            std::cout<< "row: " << uint32_t(row) << "col: " << uint32_t(col) << "palette_index: " << uint32_t(palette) << std::endl;
            std::cout<< "bit0: " << std::endl;
            for (int i = 0; i < 8; i++) {
                std::cout<< std::bitset<8>(bit0[i]) << std::endl;
            } 
            std::cout<< "bit1: " << std::endl;
            for (int i = 0; i < 8; i++) {
                std::cout<< std::bitset<8>(bit1[i]) << std::endl;
            }
        }
    };

    struct SavedPalette {
        uint8_t color[4][4];
    };

    void generate_tile_data_files(std::vector<SavedTile> &tiles, std::string tile_name);    
    void load_tile_data_files(std::string file_location, std::vector<SavedTile>& out_tiles);

    void generate_palette_data_files(std::vector<SavedPalette>);

};