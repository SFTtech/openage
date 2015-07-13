// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include <epoxy/gl.h>
#include <SDL2/SDL.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "console/console.h"
#include "coord/vec2f.h"
#include "engine.h"
#include "game_main.h"
#include "game_spec.h"
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

#include "game_renderer.h"

namespace openage {

GameRenderer::GameRenderer(openage::Engine *e)
	:
	debug_grid_active{false},
	engine{e} {

	// engine callbacks
	this->engine->register_draw_action(this);

	util::Dir *data_dir = engine->get_data_dir();
	util::Dir asset_dir = data_dir->append("converted");

	// load textures and stuff
	gaben = new Texture{data_dir->join("gaben.png")};

	auto player_color_lines = util::read_csv_file<gamedata::palette_color>(asset_dir.join("player_palette_50500.docx"));
	GLfloat *playercolors = new GLfloat[player_color_lines.size() * 4];
	for (size_t i = 0; i < player_color_lines.size(); i++) {
		auto line = &player_color_lines[i];
		playercolors[i*4]     = line->r / 255.0;
		playercolors[i*4 + 1] = line->g / 255.0;
		playercolors[i*4 + 2] = line->b / 255.0;
		playercolors[i*4 + 3] = line->a / 255.0;
	}

	// shader initialisation
	// read shader source codes and create shader objects for wrapping them.

	char *texture_vert_code;
	util::read_whole_file(&texture_vert_code, data_dir->join("shaders/maptexture.vert.glsl"));
	auto plaintexture_vert = new shader::Shader(GL_VERTEX_SHADER, texture_vert_code);
	delete[] texture_vert_code;

	char *texture_frag_code;
	util::read_whole_file(&texture_frag_code, data_dir->join("shaders/maptexture.frag.glsl"));
	auto plaintexture_frag = new shader::Shader(GL_FRAGMENT_SHADER, texture_frag_code);
	delete[] texture_frag_code;

	char *teamcolor_frag_code;
	util::read_whole_file(&teamcolor_frag_code, data_dir->join("shaders/teamcolors.frag.glsl"));
	auto teamcolor_frag = new shader::Shader(GL_FRAGMENT_SHADER, teamcolor_frag_code);
	delete[] teamcolor_frag_code;

	char *alphamask_vert_code;
	util::read_whole_file(&alphamask_vert_code, data_dir->join("shaders/alphamask.vert.glsl"));
	auto alphamask_vert = new shader::Shader(GL_VERTEX_SHADER, alphamask_vert_code);
	delete[] alphamask_vert_code;

	char *alphamask_frag_code;
	util::read_whole_file(&alphamask_frag_code, data_dir->join("shaders/alphamask.frag.glsl"));
	auto alphamask_frag = new shader::Shader(GL_FRAGMENT_SHADER, alphamask_frag_code);
	delete[] alphamask_frag_code;



	// create program for rendering simple textures
	texture_shader::program = new shader::Program(plaintexture_vert, plaintexture_frag);
	texture_shader::program->link();
	texture_shader::texture = texture_shader::program->get_uniform_id("texture");
	texture_shader::tex_coord = texture_shader::program->get_attribute_id("tex_coordinates");
	texture_shader::program->use();
	glUniform1i(texture_shader::texture, 0);
	texture_shader::program->stopusing();


	// create program for tinting textures at alpha-marked pixels
	// with team colors
	teamcolor_shader::program = new shader::Program(plaintexture_vert, teamcolor_frag);
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
	glUniform4fv(teamcolor_shader::player_color_var, 64, playercolors);
	teamcolor_shader::program->stopusing();
	delete[] playercolors;


	// create program for drawing textures that are alpha-masked before
	alphamask_shader::program = new shader::Program(alphamask_vert, alphamask_frag);
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

	// after linking, the shaders are no longer necessary
	delete plaintexture_vert;
	delete plaintexture_frag;
	delete teamcolor_frag;
	delete alphamask_vert;
	delete alphamask_frag;

	// Renderer keybinds
	// TODO: a renderer settings struct
	// would allow these to be put somewher better
	auto &global_input_context = engine->get_input_manager().get_global_context();
	global_input_context.bind(input::action_t::TOGGLE_DEBUG_GRID, [this](const input::action_arg_t &) {
		this->debug_grid_active = !this->debug_grid_active;
	});
	global_input_context.bind(input::action_t::TOGGLE_BLENDING, [this](const input::action_arg_t &) {
		GameMain *game = this->engine->get_game();
		if (game && game->terrain) {
			Terrain *terrain = game->terrain.get();
			terrain->blending_enabled = !terrain->blending_enabled;
		}
	});
	global_input_context.bind(input::action_t::TOGGLE_UNIT_DEBUG, [this](const input::action_arg_t &) {
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
}

bool GameRenderer::on_draw() {
	Engine &engine = Engine::get();

	// draw terrain
	GameMain *game = engine.get_game();
	if (game) {

		// draw gaben, our great and holy protector, bringer of the half-life 3.
		gaben->draw(coord::camgame{0, 0});

		// TODO move render code out of terrain
		if (game->terrain) {
			game->terrain->draw(&engine);
		}

		if (this->debug_grid_active) {
			this->draw_debug_grid();
		}
	}
	return true;
}

void GameRenderer::draw_debug_grid() {
	Engine &e = Engine::get();

	coord::camgame camera = coord::tile{0, 0}.to_tile3().to_phys3().to_camgame();

	int cam_offset_x = util::mod(camera.x, e.engine_coord_data->tile_halfsize.x * 2);
	int cam_offset_y = util::mod(camera.y, e.engine_coord_data->tile_halfsize.y * 2);

	int line_half_width = e.engine_coord_data->window_size.x / 2;
	int line_half_height = e.engine_coord_data->window_size.y / 2;

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
	int k = line_half_width / (e.engine_coord_data->tile_halfsize.x);

	int tilesize_x = e.engine_coord_data->tile_halfsize.x * 2;
	int common_x   = cam_offset_x + e.engine_coord_data->tile_halfsize.x;
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

} //namespace openage
