#include "PlayMode.hpp"

//for the GL_ERRORS() macro:
#include "gl_errors.hpp"

//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>

#include <random>

extern AssetController Asset_Controller;

PlayMode::PlayMode() :
	hamster(0, 10.0f,10.0f,&ppu, 20.0f),
	carrot_controller(&hamster, &ppu,Asset_Controller.Carrot_Animations),
	timer_lower(28, 236, 230, &ppu),
	timer_upper(28, 228, 230, &ppu),
	score_lower(28, 20, 230, &ppu),
	score_upper(28, 10, 230, &ppu)
{
	// tiles should be entirely based on the asset controller
	ppu.tile_table = Asset_Controller.tiles;

	ppu.palette_table = Asset_Controller.palettes;
    hamster.load_animation(Asset_Controller.Hamster_Animations);
	timer_lower.load_number_sprites(Asset_Controller.numbers,0);
	timer_upper.load_number_sprites(Asset_Controller.numbers,4);
	score_lower.load_number_sprites(Asset_Controller.numbers,8);
	score_upper.load_number_sprites(Asset_Controller.numbers,12);

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
		} else if (evt.key.keysym.sym == SDLK_r) {
			ended = false;
			count_down = 60.0f;
			carrot_controller.score = 0;
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

	if (count_down <= 0) {
		ended = true;
		return;
	}
	// carrots shouldn't die when game has ended

	count_down -= elapsed;

	carrot_controller.update(elapsed);
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

	carrot_controller.draw();
	hamster.draw();
	if (!ended) {
		score_lower.set_pos(20,230);
		score_upper.set_pos(10,230);
	}
	else {
		score_lower.set_pos(135,120);
		score_upper.set_pos(125,120);
	}
	uint8_t score_lower_num = carrot_controller.score % 10;
	uint8_t score_upper_num = carrot_controller.score / 10;
	if (score_upper_num) {
		score_upper.draw(score_upper_num);
	}
	else {
		score_upper.set_pos(255,255);
		score_upper.draw(0);
	}
	score_lower.draw(score_lower_num);

	assert(count_down > -1.0f);
	uint8_t count_down_num = uint8_t(ceil(count_down));
	uint8_t timer_lower_num = count_down_num % 10;
	uint8_t timer_upper_num = count_down_num / 10;
	if (timer_upper_num) {
		timer_upper.set_pos(228, 230);
		timer_upper.draw(timer_upper_num);
	}
	else {
		timer_upper.set_pos(255,255);
		timer_upper.draw(0);
	}
	timer_lower.draw(timer_lower_num);
	

	//--- actually draw ---
	ppu.draw(drawable_size);
}
