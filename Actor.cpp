#include "Actor.hpp"
extern AssetController Asset_Controller;

Actor::Actor()
{
    animations = Asset_Controller.Hamster_Animations;
    set_current_animation(idle);
}

Actor::Actor(std::vector<std::pair<Actor::State, std::vector<AssetController::LoadedSprite>>> in_animation)
{
    animations = in_animation;
    set_current_animation(idle);
}

void Actor::update(float elapsed)
{
    static float time_since_last_frame = 0;
    time_since_last_frame += elapsed;
    if (time_since_last_frame >= current_animation.frame_time) {
        // updates the animation to the next frame
        time_since_last_frame -= current_animation.frame_time;
        current_animation.current_index = 
            (current_animation.current_index + 1) % current_animation.sprites.size();
    }
}

void Actor::set_current_animation(State state)
{
    for (int i = 0; i < animations.size(); i++) {
        if (state == animations[i].first) {
            current_animation.sprites = animations[i].second;
            current_animation.current_index = 0;
        }
    }
    
}

PPU466::Sprite Actor::get_current_sprite_at_index(uint8_t i)
{
    return current_animation.sprites[current_animation.current_index].sprites[i];
}

uint8_t Actor::get_row_count()
{
    static uint8_t count = current_animation.sprites[0].row_count;
    return count;
}

