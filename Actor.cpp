#include "Actor.hpp"

extern AssetController Asset_Controller;

Actor::Actor(uint8_t ppu_start_index, float x, float y, PPU466 *ppu):
    ppu_start_index(ppu_start_index),
    x_pos(x),
    y_pos(y),
    ppu(ppu) 
{
}

// offsets for multiple sprites at the same time
void Actor::load_animation(std::vector<std::vector<AssetController::LoadedSprite>> in_animation, uint8_t offset)
{
    assert(in_animation.size() > 0);
    animations = in_animation;
    palette_index = in_animation[0][0].sprites[0].attributes;
    ppu_start_index = ppu_start_index + offset;
    tile_start_index = in_animation[0][0].tile_index;
    has_loaded_animation = true;
    set_current_animation(idle);
}

void Actor::update(float elapsed)
{
    assert(has_loaded_animation);
    time_since_last_frame += elapsed;
    if (time_since_last_frame >= current_animation.frame_time) {
        // updates the animation to the next frame
        time_since_last_frame -= current_animation.frame_time;
        if (current_state == death && int(current_animation.current_index + 1) == int(current_animation.sprites.size()) && !in_on_death) {
            in_on_death = true;
            on_death();
            return;
        }
        current_animation.current_index = 
            (current_animation.current_index + 1) % current_animation.sprites.size();
    }
}

void Actor::draw()
{
    assert(has_loaded_animation);
    // keep the center of the sprite to x_pos
    static float x_offset = float(current_animation.sprites[current_animation.current_index].col_count*4);
    static float y_offset = float(current_animation.sprites[current_animation.current_index].col_count*4 - 8);
	for (int i = 0; i < int(current_animation.sprites.size()); ++i){
		PPU466::Sprite cur = get_current_sprite_at_index(uint8_t(i));
		ppu->sprites[i+ppu_start_index].x = uint8_t(x_pos + float(8 * (i % get_row_count())) - x_offset);
		ppu->sprites[i+ppu_start_index].y = uint8_t(y_pos - float(8 * (i / get_row_count())) + y_offset);
		ppu->sprites[i+ppu_start_index].index = cur.index;
		ppu->sprites[i+ppu_start_index].attributes = palette_index;
	}
}

void Actor::on_death()
{
    x_pos = 250;
    y_pos = 250;
}

void Actor::set_current_animation(State state)
{
    if (current_state == state) return;
    if (state != death) in_on_death = false;
    current_state = state;
    current_animation.sprites = animations[state];
    current_animation.current_index = 0;
    current_animation.frame_time = 0.25f;
    time_since_last_frame = 0.0f;
}

PPU466::Sprite Actor::get_current_sprite_at_index(uint8_t i)
{
    return current_animation.sprites[current_animation.current_index].sprites[i];
}

uint8_t Actor::get_row_count()
{
    static uint8_t count = current_animation.sprites[0].row_count;
    return count;
}

