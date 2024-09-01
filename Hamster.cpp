#include "Hamster.hpp"
#include <iostream>

void Hamster::update(float elapsed)
{
    // update animation
    Actor::update(elapsed);
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

    float momentum_sum = abs(momentum.x) + abs(momentum.y);
    if (momentum_sum < 0.4f){
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
    glm::vec2 velocity = (movement * speed + momentum) * elapsed;
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
