#include "AssetController.hpp"
#include <fstream>
#include <filesystem>
#include "read_write_chunk.hpp"
#include "data_path.hpp"

AssetController::AssetController()
    : hamster_sprite(LoadTagDefault, [&](){
	    return load_tiles(data_path("assets/tiles/HamsterIdle.tile"));})
{
    load_all();
}

void AssetController::load_all()
{
    
}

void AssetController::load_palettes(std::string palettes_path)
{
    std::ifstream palettes_file(palettes_path, std::ios::binary);
    
}

AssetController::LoadedSprite* AssetController::load_tiles(std::string tile_path)
{
    std::ifstream tile_file(tile_path, std::ios::binary);
    if (!tile_file.is_open()) {
        throw std::runtime_error("Failed to open the file: " + tile_path);
    }
    LoadedSprite* out_sprite = new LoadedSprite;
    std::vector<SavedTile> out_tiles;
    read_chunk(tile_file, "TILE", &out_tiles);
    tile_file.close();

    uint8_t row_count = 0, col_count = 0;
    for (size_t i = 0; i < out_tiles.size(); ++i) {
        SavedTile cur_out_tile = out_tiles[i];
        // find dimension of this sprite
        row_count = std::max(cur_out_tile.row, row_count);
        col_count = std::max(cur_out_tile.col, col_count);
        // copy over the color bits
        PPU466::Tile cur_tile;
        std::copy(std::begin(cur_out_tile.bit0), std::end(cur_out_tile.bit0), cur_tile.bit0.begin());
        std::copy(std::begin(cur_out_tile.bit1), std::end(cur_out_tile.bit1), cur_tile.bit1.begin());
        tiles[tile_count] = cur_tile;
        // make the sprite
        PPU466::Sprite cur_sprite;
        cur_sprite.index = tile_count;
        cur_sprite.attributes = cur_out_tile.palette;
        out_sprite->sprites.push_back(cur_sprite);
        tile_count ++;
    }
    out_sprite->row_count = row_count;
    out_sprite->col_count = col_count;
    return out_sprite;
}

void AssetController::draw()
{
}
