#include "CarrotController.hpp"

CarrotController::CarrotController(Hamster *hamster, PPU466 *ppu, 
    std::vector<std::vector<AssetController::LoadedSprite>> in_animation) : 
    hamster(hamster), ppu(ppu), carrot(Carrot(this, 200.0f, 200.0f, ppu))
{
    carrot.load_animation(in_animation);
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
    Carrot new_carrot = Carrot(this, x, y, ppu);
    carrots.push_back(new_carrot);
}

void CarrotController::update(float elapsed)
{

        carrot.update(elapsed);
    
}

void CarrotController::draw()
{

        carrot.draw();
    
}
