#pragma once

#include "PPU466.hpp"
#include "Load.hpp"
#include "data_path.hpp"

#include <vector>
#include <string>

struct AssetController
{
    struct SavedTile {
        uint8_t row, col, palette;
        uint8_t bit0[8];
        uint8_t bit1[8];
    };

    struct SavedPalette {
        uint8_t color[4][4];
    };

    // Palettes
    std::array<PPU466::Palette, 8> palettes;

    // Tiles
    uint8_t tile_count = 0;
    std::array<PPU466::Tile, 256> tiles;
    std::vector<PPU466::Tile> tile_bank;

    // Sprites
    struct LoadedSprite {
        uint8_t row_count, col_count;
        uint8_t tile_index;
        uint32_t tile_bank_index;
        std::vector<PPU466::Sprite> sprites;
    };

    AssetController();

    void load_all();

    void load_palettes(std::string palettes_path);
    LoadedSprite load_tiles(std::string tile_path);

    void load_animations();

    std::vector<std::vector<AssetController::LoadedSprite>> Hamster_Animations;
    AssetController::LoadedSprite Background_Sprites;

};