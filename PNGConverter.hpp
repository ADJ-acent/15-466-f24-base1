#pragma once

#include <string>
#include <vector>
#include <bitset>
#include <iostream>
#include "AssetController.hpp"

struct PNGConverter {
    PNGConverter(std::string save_file_path, std::string png_folder) : asset_root(save_file_path){
        convert_pngs_to_assets(png_folder);
    };
    ~PNGConverter() = default;

    const uint8_t tile_dimension = 8;
    std::string asset_root;

    void convert_pngs_to_assets(std::string png_folder);

    void generate_tile_data_files(std::vector<AssetController::SavedTile> &tiles, std::string tile_name);    

    void generate_palette_data_files(std::vector<AssetController::SavedPalette>);

};