#include "Number.hpp"

void Number::load_number_sprites(std::vector<AssetController::LoadedSprite> sprites, uint8_t offset)
{
    number_sprites = sprites;
    palette_index = sprites[0].sprites[0].attributes;
    ppu_start_index = ppu_start_index + offset;
}

void Number::draw(uint8_t number)
{
    assert(number <= 9); // only 1 digit
    // keep the center of the sprite to x_pos
    static float x_offset = float(number_sprites[number].col_count*4);
    static float y_offset = float(number_sprites[number].col_count*4 - 8);
	for (int i = 0; i < number_sprites[number].sprites.size(); ++i){
		PPU466::Sprite cur = number_sprites[number].sprites[i];
		ppu->sprites[i+ppu_start_index].x = uint8_t(x_pos + float(8 * (i % number_sprites[0].row_count)) - x_offset);
		ppu->sprites[i+ppu_start_index].y = uint8_t(y_pos - float(8 * (i / number_sprites[0].row_count)) + y_offset);
		ppu->sprites[i+ppu_start_index].index = cur.index;
		ppu->sprites[i+ppu_start_index].attributes = palette_index;
	}
}

void Number::set_pos(uint8_t x, uint8_t y)
{
    x_pos = x;
    y_pos = y;
}
