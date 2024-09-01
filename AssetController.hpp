#pragma once

#include "PPU466.hpp"
#include "Load.hpp"
#include "data_path.hpp"

#include <vector>
#include <utility>
#include <string>
#include "Actor.hpp"

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
    std::array<PPU466::Tile, 256> tiles;
    uint8_t tile_count = 0;

    // Sprites
    struct LoadedSprite {
        uint8_t row_count, col_count;
        std::vector<PPU466::Sprite> sprites;
    };

    AssetController();

    void load_all();

    void load_palettes(std::string palettes_path);
    LoadedSprite load_tiles(std::string tile_path);

    void load_animations();

    std::vector<std::pair<Actor::State, std::vector<AssetController::LoadedSprite>>> Hamster_Animations;

    void draw();

};