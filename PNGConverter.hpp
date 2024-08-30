#pragma once

#include <string>

struct PNGConverter {
    PNGConverter(std::string save_file_path) : asset_root(save_file_path){};
    ~PNGConverter() = default;

    const uint8_t sprite_dimension = 8;
    std::string asset_root;

    bool convert_pngs_to_assets(std::string png_folder);

};