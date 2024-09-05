#include "Actor.hpp"

struct Hamster : Actor {
    Hamster(uint8_t ppu_start_index, float x, float y, PPU466* ppu, float speed) :
        Actor(ppu_start_index, x, y, ppu),
        speed(speed),
        dash_top_speed(7*speed)
    {};
    //input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up, space;

    const float speed;
    const float dash_top_speed;
    glm::vec2 momentum = glm::vec2(0);

    void update(float elapsed) override;
    void set_current_animation(State state) override;

};