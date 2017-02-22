// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include <epoxy/gl.h>
#include <SDL2/SDL.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sstream>

#include "coord/vec2f.h"
#include "engine.h"
#include "gamedata/color.gen.h"
#include "gamestate/game_main.h"
#include "gamestate/game_spec.h"
#include "input/input_manager.h"
#include "log/log.h"
#include "terrain/terrain.h"
#include "unit/action.h"
#include "unit/command.h"
#include "unit/producer.h"
#include "unit/unit.h"
#include "unit/unit_texture.h"
#include "util/timer.h"
#include "util/externalprofiler.h"
#include "renderer/text.h"
#include "game_renderer.h"

namespace openage {

RenderOptions::RenderOptions()
	:
	OptionNode{"RendererOptions"},
	draw_grid{this, "draw_grid", false},
	draw_debug{this, "draw_debug", false},
	terrain_blending{this, "terrain_blending", true} {
}

GameRenderer::GameRenderer(Engine *e)
	:
	engine{e} {

	// set options structure
	this->settings.set_parent(this->engine);

	// engine callbacks
	this->engine->register_draw_action(this);

	// fetch asset loading dir
	util::Path asset_dir = engine->get_root_dir()["assets"];

	// load textures and stuff
	gaben = new Texture{asset_dir["gaben.png"]};

	std::vector<gamedata::palette_color> player_color_lines = util::read_csv_file<gamedata::palette_color>(
		asset_dir["converted/player_palette.docx"]
	);

	std::unique_ptr<GLfloat[]> playercolors = std::make_unique<GLfloat[]>(player_color_lines.size() * 4);
	for (size_t i = 0; i < player_color_lines.size(); i++) {
		auto line = &player_color_lines[i];
		playercolors[i*4]     = line->r / 255.0;
		playercolors[i*4 + 1] = line->g / 255.0;
		playercolors[i*4 + 2] = line->b / 255.0;
		playercolors[i*4 + 3] = line->a / 255.0;
	}

	// shader initialisation
	// read shader source codes and create shader objects for wrapping them.
	const char *shader_header_code = "#version 120\n";
	std::string equals_epsilon_code = asset_dir["shaders/equalsEpsilon.glsl"].open().read();
	std::string texture_vert_code = asset_dir["shaders/maptexture.vert.glsl"].open().read();
	auto plaintexture_vert = std::make_unique<shader::Shader>(
		GL_VERTEX_SHADER,
		std::initializer_list<const char *>{shader_header_code, texture_vert_code.c_str()}
	);

	std::string texture_frag_code = asset_dir["shaders/maptexture.frag.glsl"].open().read();
	auto plaintexture_frag = std::make_unique<shader::Shader>(
		GL_FRAGMENT_SHADER,
		std::initializer_list<const char *>{shader_header_code, texture_frag_code.c_str()}
	);

	std::string teamcolor_frag_code = asset_dir["shaders/teamcolors.frag.glsl"].open().read();
	std::stringstream ss;
	ss << player_color_lines.size();
	auto teamcolor_frag = std::make_unique<shader::Shader>(
		GL_FRAGMENT_SHADER,
		std::initializer_list<const char *>{
			shader_header_code,
			("#define NUM_OF_PLAYER_COLORS " + ss.str() + "\n").c_str(),
			equals_epsilon_code.c_str(),
			teamcolor_frag_code.c_str()
		}
	);

	std::string alphamask_vert_code = asset_dir["shaders/alphamask.vert.glsl"].open().read();
	auto alphamask_vert = std::make_unique<shader::Shader>(
		GL_VERTEX_SHADER,
		std::initializer_list<const char *>{shader_header_code, alphamask_vert_code.c_str()}
	);

	std::string alphamask_frag_code = asset_dir["shaders/alphamask.frag.glsl"].open().read();
	auto alphamask_frag = std::make_unique<shader::Shader>(
		GL_FRAGMENT_SHADER,
		std::initializer_list<const char *>{shader_header_code, alphamask_frag_code.c_str()}
	);

	std::string texturefont_vert_code = asset_dir["shaders/texturefont.vert.glsl"].open().read();
	auto texturefont_vert = std::make_unique<shader::Shader>(
		GL_VERTEX_SHADER,
		std::initializer_list<const char *>{shader_header_code, texturefont_vert_code.c_str()}
	);

	std::string texturefont_frag_code = asset_dir["shaders/texturefont.frag.glsl"].open().read();
	auto texturefont_frag = std::make_unique<shader::Shader>(
		GL_FRAGMENT_SHADER,
		std::initializer_list<const char *>{shader_header_code, texturefont_frag_code.c_str()}
	);

	// create program for rendering simple textures
	texture_shader::program = new shader::Program(plaintexture_vert.get(), plaintexture_frag.get());
	texture_shader::program->link();
	texture_shader::texture = texture_shader::program->get_uniform_id("texture");
	texture_shader::tex_coord = texture_shader::program->get_attribute_id("tex_coordinates");
	texture_shader::program->use();
	glUniform1i(texture_shader::texture, 0);
	texture_shader::program->stopusing();


	// create program for tinting textures at alpha-marked pixels
	// with team colors
	teamcolor_shader::program = new shader::Program(plaintexture_vert.get(), teamcolor_frag.get());
	teamcolor_shader::program->link();
	teamcolor_shader::texture = teamcolor_shader::program->get_uniform_id("texture");
	teamcolor_shader::tex_coord = teamcolor_shader::program->get_attribute_id("tex_coordinates");
	teamcolor_shader::player_id_var = teamcolor_shader::program->get_uniform_id("player_number");
	teamcolor_shader::alpha_marker_var = teamcolor_shader::program->get_uniform_id("alpha_marker");
	teamcolor_shader::player_color_var = teamcolor_shader::program->get_uniform_id("player_color");
	teamcolor_shader::program->use();
	glUniform1i(teamcolor_shader::texture, 0);
	glUniform1f(teamcolor_shader::alpha_marker_var, 254.0/255.0);
	// fill the teamcolor shader's player color table:
	glUniform4fv(teamcolor_shader::player_color_var, 64, playercolors.get());
	teamcolor_shader::program->stopusing();


	// create program for drawing textures that are alpha-masked before
	alphamask_shader::program = new shader::Program(alphamask_vert.get(), alphamask_frag.get());
	alphamask_shader::program->link();
	alphamask_shader::base_coord = alphamask_shader::program->get_attribute_id("base_tex_coordinates");
	alphamask_shader::mask_coord = alphamask_shader::program->get_attribute_id("mask_tex_coordinates");
	alphamask_shader::show_mask = alphamask_shader::program->get_uniform_id("show_mask");
	alphamask_shader::base_texture = alphamask_shader::program->get_uniform_id("base_texture");
	alphamask_shader::mask_texture = alphamask_shader::program->get_uniform_id("mask_texture");
	alphamask_shader::program->use();
	glUniform1i(alphamask_shader::base_texture, 0);
	glUniform1i(alphamask_shader::mask_texture, 1);
	alphamask_shader::program->stopusing();

	// Create program for texture based font rendering
	texturefont_shader::program = new shader::Program(texturefont_vert.get(), texturefont_frag.get());
	texturefont_shader::program->link();
	texturefont_shader::texture = texturefont_shader::program->get_uniform_id("texture");
	texturefont_shader::color = texturefont_shader::program->get_uniform_id("color");
	texturefont_shader::tex_coord = texturefont_shader::program->get_attribute_id("tex_coordinates");
	texturefont_shader::program->use();
	glUniform1i(texturefont_shader::texture, 0);
	texturefont_shader::program->stopusing();

	// Renderer keybinds
	// TODO: a renderer settings struct
	// would allow these to be put somewher better
	input::ActionManager &action = this->engine->get_action_manager();
	auto &global_input_context = engine->get_input_manager().get_global_context();
	global_input_context.bind(action.get("TOGGLE_BLENDING"), [this](const input::action_arg_t &) {
		this->settings.terrain_blending.value = !this->settings.terrain_blending.value;
	});
	global_input_context.bind(action.get("TOGGLE_UNIT_DEBUG"), [this](const input::action_arg_t &) {
		this->settings.draw_debug.value = !this->settings.draw_debug.value;

		// TODO remove this hack, use render settings instead
		UnitAction::show_debug = !UnitAction::show_debug;
	});

	log::log(MSG(dbg) << "Loaded Renderer");
}

GameRenderer::~GameRenderer() {
	// oh noes, release hl3 before that!
	delete this->gaben;

	delete texture_shader::program;
	delete teamcolor_shader::program;
	delete alphamask_shader::program;
	delete texturefont_shader::program;
}

bool GameRenderer::on_draw() {
	// draw terrain
	GameMain *game = this->engine->get_game();

	if (game) {
		// draw gaben, our great and holy protector, bringer of the half-life 3.
		gaben->draw(coord::camgame{0, 0});

		// TODO move render code out of terrain
		if (game->terrain) {
			game->terrain->draw(this->engine, &this->settings);
		}

		if (this->settings.draw_grid.value) {
			this->draw_debug_grid();
		}
	}
	return true;
}

void GameRenderer::draw_debug_grid() {
	coord::camgame camera = coord::tile{0, 0}.to_tile3().to_phys3().to_camgame();

	int cam_offset_x = util::mod(camera.x, this->engine->get_coord_data()->tile_halfsize.x * 2);
	int cam_offset_y = util::mod(camera.y, this->engine->get_coord_data()->tile_halfsize.y * 2);

	int line_half_width = this->engine->get_coord_data()->window_size.x / 2;
	int line_half_height = this->engine->get_coord_data()->window_size.y / 2;

	// rounding so we get 2:1 proportion needed for the isometric perspective

	if (line_half_width > line_half_height * 2) {

		if (line_half_width & 1) {
			line_half_width += 1; // round up if it's odd
		}

		line_half_height = line_half_width / 2;

	} else {
		line_half_width = line_half_height * 2;
	}

	// quantity of lines to draw to each side from the center
	int k = line_half_width / (this->engine->get_coord_data()->tile_halfsize.x);

	int tilesize_x = this->engine->get_coord_data()->tile_halfsize.x * 2;
	int common_x   = cam_offset_x + this->engine->get_coord_data()->tile_halfsize.x;
	int x0         = common_x     - line_half_width;
	int x1         = common_x     + line_half_width;
	int y0         = cam_offset_y - line_half_height;
	int y1         = cam_offset_y + line_half_height;

	glLineWidth(1);
	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_LINES); {

		for (int i = -k; i < k; i++) {
			glVertex3f(i * tilesize_x + x0, y1, 0);
			glVertex3f(i * tilesize_x + x1, y0, 0);

			glVertex3f(i * tilesize_x + x0, y0 - 1, 0);
			glVertex3f(i * tilesize_x + x1, y1 - 1, 0);
		}

	} glEnd();
}

GameMain *GameRenderer::game() const {
	return this->engine->get_game();
}

GameSpec *GameRenderer::game_spec() const {
	return this->game()->get_spec();
}

} // openage
