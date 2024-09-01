#pragma once
#include "AssetController.hpp"
#include <string>
#include <vector>
#include <utility>

struct Actor {

    struct Animation {
        uint8_t current_index;
        const float frame_time = .5f;
        std::vector<AssetController::LoadedSprite> sprites;
    };

    enum State {
        idle,
        moving,
        death,
    };
    Actor();
    Actor(std::vector<std::pair<Actor::State, std::vector<AssetController::LoadedSprite>>> in_animation);

    void update(float elapsed);

    std::vector<std::pair<Actor::State, std::vector<AssetController::LoadedSprite>>> animations;

    Animation current_animation;

    void set_current_animation(State state);

    PPU466::Sprite get_current_sprite_at_index(uint8_t i);

    uint8_t get_row_count();

};