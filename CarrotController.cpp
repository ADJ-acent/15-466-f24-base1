#include "CarrotController.hpp"
#include "Hamster.hpp"

CarrotController::CarrotController(Hamster *hamster, PPU466 *ppu, 
    std::vector<std::vector<AssetController::LoadedSprite>> in_animation) : 
    hamster(hamster), 
    ppu(ppu),
    carrots{
        Carrot(this, 0.f, 100.f, ppu),
        Carrot(this, 100.f, 0.f, ppu),
        Carrot(this, 150.f, 150.f, ppu),
        Carrot(this, 0.f, 50.f, ppu),
        Carrot(this, 200.f, 100.f, ppu),
        Carrot(this, 100.f, 200.f, ppu),
    }
{
    for (uint8_t i = 0; i< carrots.size(); ++i)
        carrots[i].load_animation(in_animation, i * 4);
}

void CarrotController::spawn_carrot(float x, float y)
{
    // Carrot new_carrot = Carrot(this, x, y, ppu);
    // carrots.push_back(new_carrot);
}

void CarrotController::update(float elapsed)
{
    for (uint8_t i = 0; i< carrots.size(); ++i) {
        carrots[i].velocity = rule1(i) + rule2(i)*5.0f +rule3(i)*5.0f + rule4(i) * 5.0f;
    }
    
    for (uint8_t i = 0; i< carrots.size(); ++i)
        carrots[i].update(elapsed);
    
}

void CarrotController::draw()
{
    for (uint8_t i = 0; i< carrots.size(); ++i)
        carrots[i].draw();
    
}

glm::vec2 CarrotController::rule1(uint8_t carrot_index)
{
    glm::vec2 self_pos = glm::vec2(carrots[carrot_index].x_pos, carrots[carrot_index].y_pos);
    const glm::vec2 center = glm::vec2(120.0f, 120.0f);
    glm::vec2 res = center - self_pos;

    return res / 50.0f;

}

glm::vec2 CarrotController::rule2(uint8_t carrot_index)
{
    const float distance_threshold = 50;
    glm::vec2 self_pos = glm::vec2(carrots[carrot_index].x_pos, carrots[carrot_index].y_pos);
    glm::vec2 res = glm::vec2(0,0);
    for (uint8_t i = 0; i < carrots.size(); i++) {
        if (i == carrot_index) continue;
        glm::vec2 other_pos = glm::vec2(carrots[i].x_pos, carrots[i].y_pos);
        if (glm::distance(self_pos, other_pos) <= distance_threshold){
            res += self_pos - other_pos;
        }
    }
    return res;
}

glm::vec2 CarrotController::rule3(uint8_t carrot_index)
{
    const float distance_threshold = 50;
    glm::vec2 self_pos = glm::vec2(uint8_t(carrots[carrot_index].x_pos), uint8_t(carrots[carrot_index].y_pos));
    glm::vec2 ham_pos = glm::vec2(uint8_t(hamster->x_pos), uint8_t(hamster->y_pos));
    if (glm::distance(self_pos , ham_pos) > distance_threshold) {
        return glm::vec2(0,0);
    }
    else {
        if (self_pos - ham_pos == glm::vec2(0,0)) return glm::vec2(distance_threshold,distance_threshold);
        glm::vec2 res = 1.0f / ((self_pos - ham_pos) / 100.f);
        return glm::vec2(std::min(distance_threshold, res.x), std::min(distance_threshold, res.y));
    }
}

glm::vec2 CarrotController::rule4(uint8_t carrot_index)
{
    glm::vec2 res = glm::vec2(0,0);
    glm::vec2 self_pos = glm::vec2(carrots[carrot_index].x_pos, carrots[carrot_index].y_pos);
    if (self_pos.x <= 16.0f) res += glm::vec2(1,.2);
    else if (self_pos.x >= (240.0f - 16.0f)) res -= glm::vec2(1,.2);
    if (self_pos.y <= 16.0f) res += glm::vec2(.2,1);
    else if (self_pos.y >= (240.0f - 16.0f)) res -= glm::vec2(.2,1);

    return res *= 1000;

}

void CarrotController::Carrot::update(float elapsed)
{
    Actor::update(elapsed);
    glm::vec2 hamster_pos = {uint8_t(controller->hamster->x_pos), uint8_t(controller->hamster->y_pos)};
    glm::vec2 car_pos = {uint8_t(x_pos), uint8_t(y_pos)};
    if (glm::distance(hamster_pos, car_pos) <= controller->eat_radius){
        set_current_animation(death);
        current_animation.frame_time = 0.1f;
    }
    if (current_state == death) return; // only update animation when dead

    float speed_top = glm::length(velocity);
    if (speed_top > controller->speed) {
        velocity = velocity / (speed_top / controller->speed);
    }
    x_pos += velocity.x * elapsed;
    y_pos += velocity.y * elapsed;

}
