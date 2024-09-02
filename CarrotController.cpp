#include "CarrotController.hpp"

CarrotController::CarrotController(Hamster *hamster, PPU466 *ppu, 
    std::vector<std::vector<AssetController::LoadedSprite>> in_animation) : 
    hamster(hamster), 
    ppu(ppu),
    carrots{
        Carrot(this, 100.f, 100.f, ppu),
        Carrot(this, 200.f, 200.f, ppu),
        Carrot(this, 150.f, 150.f, ppu),
        Carrot(this, 50.f, 50.f, ppu),
    }
{
    for (uint8_t i = 0; i< carrots.size(); ++i)
        carrots[i].load_animation(in_animation, i * 4);
}

void CarrotController::load_animation(std::vector<std::vector<AssetController::LoadedSprite>> in_animation)
{
    // std::cout<<"loading"<<std::endl;
    // for (Carrot carrot : carrots){
    //     carrot.load_animation(in_animation);
    // }
}

void CarrotController::spawn_carrot(float x, float y)
{
    // Carrot new_carrot = Carrot(this, x, y, ppu);
    // carrots.push_back(new_carrot);
}

void CarrotController::update(float elapsed)
{
    for (uint8_t i = 0; i< carrots.size(); ++i)
        carrots[i].update(elapsed);
    
}

void CarrotController::draw()
{
    for (uint8_t i = 0; i< carrots.size(); ++i)
        carrots[i].draw();
    
}
