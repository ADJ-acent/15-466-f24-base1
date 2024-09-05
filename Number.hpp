#include <vector>
#include "AssetController.hpp"
#include "PPU466.hpp"

struct Number {
    Number(uint8_t ppu_start_index, uint8_t x, uint8_t y, PPU466 *ppu):
        ppu_start_index(ppu_start_index),
        x_pos(x),
        y_pos(y),
        ppu(ppu)
    {};

    void load_number_sprites(std::vector<AssetController::LoadedSprite> number_sprites, uint8_t offset);
    void draw(uint8_t number);

    std::vector<AssetController::LoadedSprite> number_sprites;
    uint8_t x_pos, y_pos,ppu_start_index, palette_index;
    PPU466 *ppu;

    void set_pos(uint8_t x, uint8_t y);

};