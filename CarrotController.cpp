#include "CarrotController.hpp"
#include "Hamster.hpp"

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
        carrots[i].load_animation(in_animation, i * 8);
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

void CarrotController::Carrot::update(float elapsed)
{
    Actor::update(elapsed);
    glm::vec2 hamster_pos = {controller->hamster->x_pos, controller->hamster->y_pos};
    glm::vec2 car_pos = {x_pos, y_pos};
    if (glm::distance(hamster_pos, car_pos) <= controller->eat_radius){
        set_current_animation(death);
        current_animation.frame_time = 0.1f;
    }
    if (current_state == death) return; // only update animation when dead
}
