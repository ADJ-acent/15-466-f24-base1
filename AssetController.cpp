#include "AssetController.hpp"
#include <fstream>
#include <filesystem>
#include "read_write_chunk.hpp"
#include "Actor.hpp"

#define ASSET_CONVERSION //only defined when we need to convert assets
//converts pngs to easy to read format for PPU466
// Global manager that loads everything, make sure all generated files are in the right place
#ifdef ASSET_CONVERSION
    #include "PNGConverter.hpp"
	PNGConverter png_converter = PNGConverter(data_path("assets"), "C:/Users/andyj/OneDrive/Desktop/Github/GameProgramming/15-466-f24-base1/assets/png");
#endif
AssetController Asset_Controller = AssetController();

AssetController::AssetController()
{
    load_all();
}

void AssetController::load_all()
{
    Background_Sprites = load_tiles(data_path("assets/tiles/background.tile"));
    load_palettes(data_path("assets/palettes.pal"));
    load_animations();
}

void AssetController::load_palettes(std::string palettes_path)
{
    std::ifstream palettes_file(palettes_path, std::ios::binary);
    if (!palettes_file.is_open()) {
        throw std::runtime_error("Failed to open the file: " + palettes_path);
    }
    std::vector<SavedPalette> out_palettes;
    read_chunk(palettes_file, "PALE", &out_palettes);
    for (size_t i = 0; i< out_palettes.size(); ++i) {
        SavedPalette cur_pal = out_palettes[i];
        palettes[i] = {
            glm::u8vec4(cur_pal.color[0][0], cur_pal.color[0][1], cur_pal.color[0][2], cur_pal.color[0][3]),
            glm::u8vec4(cur_pal.color[1][0], cur_pal.color[1][1], cur_pal.color[1][2], cur_pal.color[1][3]),
            glm::u8vec4(cur_pal.color[2][0], cur_pal.color[2][1], cur_pal.color[2][2], cur_pal.color[2][3]),
            glm::u8vec4(cur_pal.color[3][0], cur_pal.color[3][1], cur_pal.color[3][2], cur_pal.color[3][3]),
        };
    }
    palettes_file.close();
}

AssetController::LoadedSprite AssetController::load_tiles(std::string tile_path)
{
    std::ifstream tile_file(tile_path, std::ios::binary);
    if (!tile_file.is_open()) {
        throw std::runtime_error("Failed to open the file: " + tile_path);
    }
    LoadedSprite out_sprite;
    std::vector<SavedTile> out_tiles;
    read_chunk(tile_file, "TILE", &out_tiles);
    tile_file.close();
    uint8_t row_count = 0, col_count = 0;
    out_sprite.tile_bank_index = uint32_t(tile_bank.size());
    out_sprite.tile_index = tile_count;
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
        out_sprite.sprites.push_back(cur_sprite);
        tile_bank.push_back(cur_tile);
        tile_count ++;
    }
    out_sprite.row_count = row_count + 1;
    out_sprite.col_count = col_count + 1;
    return out_sprite;
}

void AssetController::load_animations()
{
    auto load_animation = [](std::string root_path){
        std::vector<AssetController::LoadedSprite> animation;
        // Vector to store file names
        std::vector<std::string> file_names;

        // Check if the directory exists
        if (!std::filesystem::is_directory(root_path)) {
            std::cerr << "Directory does not exist: " << root_path << std::endl;
            std::runtime_error("Attempted to load missing animations");
        }

        // Iterate over the directory and collect file names
        for (const auto& entry : std::filesystem::directory_iterator(root_path)) {
            file_names.push_back(entry.path().string());
        }

        // Sort file names alphabetically
        std::sort(file_names.begin(), file_names.end());

        for (std::string path : file_names) {

            animation.push_back(Asset_Controller.load_tiles(path));
        }
        return animation;
    };
    uint8_t temp_tile_count = tile_count;
    Hamster_Animations.push_back(load_animation(data_path("assets/tiles/hamIdle")));
    //resets tile count as hamster animation should use the same tile space
    tile_count = temp_tile_count;
    Hamster_Animations.push_back(load_animation(data_path("assets/tiles/hamWalk")));
    tile_count = temp_tile_count;
    Hamster_Animations.push_back(load_animation(data_path("assets/tiles/hamRoll")));
    Carrot_Animations.push_back(load_animation(data_path("assets/tiles/carIdle")));
    
}