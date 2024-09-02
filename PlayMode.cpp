#include "PlayMode.hpp"

//for the GL_ERRORS() macro:
#include "gl_errors.hpp"

//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>

#include <random>

extern AssetController Asset_Controller;

PlayMode::PlayMode()
: hamster(10.0f,10.0f,&ppu, 20.0f)
{

	// tiles should be entirely based on the asset controller
	ppu.tile_table = Asset_Controller.tiles;

	// temp override for the background
	ppu.tile_table[63].bit0 = {
		0,0,0,0,0,0,0,0
	};

	ppu.tile_table[63].bit1 = {
		0,0,0,0,0,0,0,0
	};

	ppu.palette_table = Asset_Controller.palettes;
    hamster.load_animation(Asset_Controller.Hamster_Animations);

	AssetController::LoadedSprite background_sprites = Asset_Controller.Background_Sprites;
	// background, random assortment of stars
	std::srand(static_cast<unsigned int>(std::time(nullptr)));
	for (uint32_t y = 0; y < PPU466::BackgroundHeight; ++y) {
		for (uint32_t x = 0; x < PPU466::BackgroundWidth; ++x) {
			uint8_t rand_num = std::rand() & 0x7F;
			uint8_t bg_index = 0;
			if ((rand_num & 0x70) ==  0x70) bg_index = rand_num & 0xf;
			ppu.background[x+PPU466::BackgroundWidth*y] = background_sprites.tile_index + bg_index;
		}
	}
}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_LEFT) {
			hamster.left.downs += 1;
			hamster.left.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			hamster.right.downs += 1;
			hamster.right.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			hamster.up.downs += 1;
			hamster.up.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			hamster.down.downs += 1;
			hamster.down.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_SPACE) {
			hamster.space.downs += 1;
			hamster.space.pressed = true;
			return true;
		}
	} else if (evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_LEFT) {
			hamster.left.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			hamster.right.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			hamster.up.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			hamster.down.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_SPACE) {
			hamster.space.pressed = false;
			return true;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {

	//slowly rotates through [0,1):
	// (will be used to set background color)
	background_fade += elapsed / 10.0f;
	background_fade -= std::floor(background_fade);

	hamster.update(elapsed);
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//--- set ppu state based on game state ---

	//background color will be some blue fade;
	ppu.background_color = glm::u8vec4(
		std::min(70,std::max(0,int32_t(255 * 0.5f * (0.5f + std::sin( 2.0f * M_PI * (background_fade + 0.0f / 3.0f) ) ) ))),
		std::min(70,std::max(0,int32_t(255 * 0.5f * (0.5f + std::sin( 2.0f * M_PI * (background_fade + 1.0f / 3.0f) ) ) ))),
		155,
		0xff
	);

	//background scroll:
	ppu.background_position.x = int32_t(-0.5f * hamster.x_pos);
	ppu.background_position.y = int32_t(-0.5f * hamster.y_pos);

	hamster.draw();

	//--- actually draw ---
	ppu.draw(drawable_size);
}
