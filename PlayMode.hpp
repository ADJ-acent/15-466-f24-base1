#pragma once
#include "PPU466.hpp"
#include "Mode.hpp"
#include "AssetController.hpp"
#include "Hamster.hpp"
#include "CarrotController.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- game state -----

	//some weird background animation:
	float background_fade = 0.0f;

	//----- drawing handled by PPU466 -----

	PPU466 ppu;
	
	// player hamster
	Hamster hamster;

	// carrots
	CarrotController carrot_controller;
};
