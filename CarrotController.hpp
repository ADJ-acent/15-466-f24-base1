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

        void update(float elapsed) override;
    };

    Hamster* hamster;
    PPU466* ppu;
    std::array<Carrot, 4> carrots; // have max of 4 carrots in the scene
    uint8_t eat_radius = 10;

    void spawn_carrot(float x, float y);

    void update(float elapsed);
    void draw();
};
