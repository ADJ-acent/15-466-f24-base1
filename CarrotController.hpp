#include "Actor.hpp"

struct Hamster;

struct CarrotController
{
    CarrotController(Hamster* hamster, PPU466* ppu, std::vector<std::vector<AssetController::LoadedSprite>> in_animation);
    struct Carrot : Actor {
        Carrot(CarrotController* controller, float x, float y, PPU466* ppu) : 
            Actor(x, y, ppu),
            controller(controller)
            {};
        CarrotController* controller;
        glm::vec2 velocity;

        void update(float elapsed) override;
        void on_death() override;
    };

    Hamster* hamster;
    PPU466* ppu;
    std::array<Carrot, 6> carrots; // have max of 6 carrots in the scene
    std::array<bool, 6> dead_carrots{false, false, false, false, false, false};
    const float respawn_time = 2.0f;
    float since_respawn = 0.0f;
    const uint8_t eat_radius = 10;
    const uint8_t speed = 100;

    void spawn_carrot(float x, float y);

    void update(float elapsed);
    void draw();

    // boid rules, code structure modified
    //https://medium.com/fragmentblog/simulating-flocking-with-the-boids-algorithm-92aef51b9e00:
    glm::vec2 rule1(uint8_t carrot_index); // attract to the center of the screen
    glm::vec2 rule2(uint8_t carrot_index); // avoid others
    glm::vec2 rule3(uint8_t carrot_index); // avoid le hamster
    glm::vec2 rule4(uint8_t carrot_index); // avoid the border

};
