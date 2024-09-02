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
    };

    std::vector<Carrot> carrots;

    const uint8_t max_carrots = 4;
    Hamster* hamster;
    PPU466* ppu;
    Carrot carrot;

    void load_animation(std::vector<std::vector<AssetController::LoadedSprite>> in_animation);
    void spawn_carrot(float x, float y);

    void update(float elapsed);
    void draw();
};
