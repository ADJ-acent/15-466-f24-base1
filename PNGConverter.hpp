#pragma once

#include <string>
#include <vector>

struct PNGConverter {
    PNGConverter(std::string save_file_path) : asset_root(save_file_path){};
    ~PNGConverter() = default;

    const uint8_t tile_dimension = 8;
    std::string asset_root;

    void convert_pngs_to_assets(std::string png_folder);

    struct SingleTile {
            std::vector<uint8_t> bit0;
            std::vector<uint8_t> bit1;
    };

    void generate_tile_data_file(std::vector<SingleTile> &sprites, std::string sprite_name, 
        uint8_t x_sprite_count, uint8_t y_sprite_count, uint8_t pallet_index);

};