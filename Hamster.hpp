#include "Actor.hpp"

struct Hamster : Actor {
    Hamster(float x, float y, PPU466* ppu, float speed) :
        Actor(x, y, ppu),
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
    State current_state = idle;

    void update(float elapsed) override;

};