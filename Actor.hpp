#pragma once
#include "PPU466.hpp"
#include "AssetController.hpp"
#include <string>
#include <vector>

struct Actor {
    Actor(float x, float y, PPU466* ppu);

    struct Animation {
        uint8_t current_index;
        float frame_time = .5f;
        std::vector<AssetController::LoadedSprite> sprites;
    };

    enum State {
        idle,
        moving,
        rolling,
        death,
    };
    bool has_loaded_animation = false;
    uint8_t palette_index;
    uint8_t ppu_start_index;
    uint8_t tile_start_index;
    float x_pos, y_pos; // 0 - 255, 240+ is offscreen, origin is bottom left
    float time_since_last_frame = 0;
    PPU466* ppu;
    std::vector<std::vector<AssetController::LoadedSprite>> animations;
    Animation current_animation;
    State current_state = death;

    void load_animation(std::vector<std::vector<AssetController::LoadedSprite>> in_animation, uint8_t offset = 0);

    virtual void update(float elapsed);
    void draw();
    void on_death();

    virtual void set_current_animation(State state);
    PPU466::Sprite get_current_sprite_at_index(uint8_t i);
    uint8_t get_row_count();

};