#include "Hamster.hpp"
#include "AssetController.hpp"
extern AssetController Asset_Controller;

void Hamster::update(float elapsed)
{
    // update animation
    Actor::update(elapsed);
    if (current_state == death) return;
    // handle inputs
    static bool can_roll = true;

    glm::vec2 movement_not_normalized = glm::vec2(float(bool(right.pressed) - bool(left.pressed)), 
        float(bool(up.pressed) - bool(down.pressed)));
    glm::vec2 movement;
    if (movement_not_normalized != glm::vec2(0)){
        movement = glm::normalize(movement_not_normalized);
    }
    else movement = movement_not_normalized;

    static float dash_fall_off = 0.0f;
    const float fall_off_interval = 0.1f;
    const float roll_movement_damping = 0.4f; // makes controlling the hamster super difficult while rolling 

    // player can only roll when the rolling momentum has worn off
    float momentum_sum = abs(momentum.x) + abs(momentum.y);
    if (momentum_sum < 1.0f){
        momentum = glm::vec2(0);
        can_roll = true;
    }
    if (!can_roll) {
        dash_fall_off += elapsed;
        if (fall_off_interval <= dash_fall_off) {
            dash_fall_off -= fall_off_interval;
            momentum *= 0.8;
        }
        current_animation.frame_time = std::min(1.0f / momentum_sum * 2.0f, 0.25f);
    }
    if (space.pressed && can_roll) {
        can_roll = false;
        set_current_animation(rolling);
        momentum = movement * dash_top_speed;
    }
    //apply damping if rolling
    glm::vec2 velocity = can_roll ? 
        (movement * speed + momentum) * elapsed : 
        (movement * speed * roll_movement_damping + momentum) * elapsed;
	x_pos += velocity.x;
    y_pos += velocity.y;

    if (can_roll){
        if (velocity == glm::vec2(0)) {
            set_current_animation(idle);
        }
        else {
            set_current_animation(moving);
        }
    }
	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;
}

// sets the current animation by replacing the tiles inside the tile table
// to save space on the tile table (not needed to be honest, as I mixed up
// the limit of sprites and tiles)
void Hamster::set_current_animation(State state)
{
    if (current_state == state) return;
    current_state = state;
    current_animation.sprites = animations[state];
    current_animation.current_index = 0;
    current_animation.frame_time = 0.25f;
    time_since_last_frame = 0.0f;
    uint32_t start_index = animations[state][0].tile_bank_index;
    for (uint8_t i = 0; i < uint8_t(animations[state].size() * animations[state][0].sprites.size()); ++i) {
        ppu->tile_table[tile_start_index + i] = Asset_Controller.tile_bank[start_index + i];
    }
}
