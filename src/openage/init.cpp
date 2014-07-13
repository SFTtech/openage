#include "init.h"

#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include "../args.h"
#include "../engine/engine.h"
#include "../engine/texture.h"
#include "../engine/util/strings.h"
#include "../engine/util/file.h"
#include "../engine/util/error.h"
#include "../engine/callbacks.h"
#include "../engine/util/dir.h"
#include "../engine/util/file.h"
#include "../engine/log.h"
#include "../engine/terrain_chunk.h"
#include "../gamedata/color.h"
#include "../gamedata/gamedata.h"
#include "../gamedata/string_resource.h"

#include "callbacks.h"
#include "gamestate.h"

using namespace engine;

namespace openage {

constexpr coord::tile_delta terrain_data_size = {16, 16};
constexpr int terrain_data[16 * 16] = {
	  0,  0,  0,  0,  0,  0,  0,  0, 16,  0,  2,  1, 15, 15, 15,  1,
	  0, 18, 18, 18, 18, 18,  0,  0, 16,  0,  2,  1, 15, 14, 15,  1,
	 18, 18,  0,  0, 18, 18,  0,  0, 16,  0,  2,  1, 15, 15, 15,  1,
	 18, 18,  0,  0, 18, 18,  0,  0, 16,  0,  2,  1,  1,  1,  2,  2,
	 18, 18, 18,  0, 18, 18,  9,  9, 16,  0,  0,  2,  2,  2,  0,  0,
	 18, 18,  0,  0,  0,  0,  9,  9, 16,  0,  0,  0,  0,  0,  0,  0,
	  0, 18,  0,  0,  0,  9,  9,  9, 16,  0,  0, -1,  0,  0,  0,  0,
	  0,  0,  0,  2,  0,  9,  9,  0,  2,  2,  0,  0, -1,  0, 23, 23,
	  0,  0,  2, 15,  2,  9,  0,  2, 15, 15,  2,  0, -1,  0,  0,  0,
	  0,  0,  2, 15,  2,  2,  2, 15,  2,  2,  0,  0,  0,  0,  0,  0,
	  0,  0,  2, 15,  2,  2,  2, 15,  2,  0,  0,  0, 20, 20, 20,  0,
	  0,  2,  2, 15,  2,  2,  2, 14,  2,  0,  0,  0, 21, 21, 21,  0,
	  2, 15, 15, 15, 15, 15, 14, 14,  2,  0,  0,  0, 22, 22, 22,  0,
	  0,  2,  2,  2,  2,  2,  2,  2,  0,  0,  0,  0,  5,  5,  5,  0,
	  0,  0,  0,  0,  0,  0,  0,  0, 16, 16, 16, 16,  5,  5,  5,  5,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  5,  5
};

void init(util::Dir &data_dir) {
	util::Dir asset_dir = data_dir.merge("age/assets");

	//load textures and stuff
	gaben      = new Texture{data_dir.append("gaben.png")};
	university = new Texture{asset_dir.append("Data/graphics.drs/3836.slp.png"), true, PLAYERCOLORED};

	auto string_resources = util::read_csv_file<gamedata::string_resource>(asset_dir.append("string_resources.docx"));
	auto terrain_types  = util::read_csv_file<gamedata::terrain_type>(asset_dir.append("gamedata/gamedata-empiresdat/0000-terrains.docx"));
	auto blending_modes = util::read_csv_file<gamedata::blending_mode>(asset_dir.append("blending_modes.docx"));

	//create the terrain which will be filled by chunks
	terrain = new Terrain(asset_dir, terrain_types, blending_modes, true);

	//auto gamedata = util::read_csv_file<gamedata::empiresdat>("age/assets/gamedata/gamedata-empiresdat.docx");

	terrain->fill(terrain_data, terrain_data_size);

	auto player_color_lines = engine::util::read_csv_file<gamedata::palette_color>(asset_dir.append("player_palette_50500.docx"));

	GLfloat *playercolors = new GLfloat[player_color_lines.size() * 4];
	for (size_t i = 0; i < player_color_lines.size(); i++) {
		auto line = &player_color_lines[i];
		playercolors[i*4]     = line->r / 255.0;
		playercolors[i*4 + 1] = line->g / 255.0;
		playercolors[i*4 + 2] = line->b / 255.0;
		playercolors[i*4 + 3] = line->a / 255.0;
	}

	// initialize sounds
	build_uni_sound    = new audio::Sound{audio_manager->get_sound(audio::category_t::GAME, 5229)};
	destroy_uni_sound0 = new audio::Sound{audio_manager->get_sound(audio::category_t::GAME, 5316)};
	destroy_uni_sound1 = new audio::Sound{audio_manager->get_sound(audio::category_t::GAME, 5317)};

	//shader initialisation
	//read shader source codes and create shader objects for wrapping them.

	char *texture_vert_code;
	util::read_whole_file(&texture_vert_code, data_dir.append("shaders/maptexture.vert.glsl"));
	auto plaintexture_vert = new shader::Shader(GL_VERTEX_SHADER, texture_vert_code);
	delete[] texture_vert_code;

	char *texture_frag_code;
	util::read_whole_file(&texture_frag_code, data_dir.append("shaders/maptexture.frag.glsl"));
	auto plaintexture_frag = new shader::Shader(GL_FRAGMENT_SHADER, texture_frag_code);
	delete[] texture_frag_code;

	char *teamcolor_frag_code;
	util::read_whole_file(&teamcolor_frag_code, data_dir.append("shaders/teamcolors.frag.glsl"));
	auto teamcolor_frag = new shader::Shader(GL_FRAGMENT_SHADER, teamcolor_frag_code);
	delete[] teamcolor_frag_code;

	char *alphamask_vert_code;
	util::read_whole_file(&alphamask_vert_code, data_dir.append("shaders/alphamask.vert.glsl"));
	auto alphamask_vert = new shader::Shader(GL_VERTEX_SHADER, alphamask_vert_code);
	delete[] alphamask_vert_code;

	char *alphamask_frag_code;
	util::read_whole_file(&alphamask_frag_code, data_dir.append("shaders/alphamask.frag.glsl"));
	auto alphamask_frag = new shader::Shader(GL_FRAGMENT_SHADER, alphamask_frag_code);
	delete[] alphamask_frag_code;



	//create program for rendering simple textures
	texture_shader::program = new shader::Program(plaintexture_vert, plaintexture_frag);
	texture_shader::program->link();
	texture_shader::texture = texture_shader::program->get_uniform_id("texture");
	texture_shader::tex_coord = texture_shader::program->get_attribute_id("tex_coordinates");
	texture_shader::program->use();
	glUniform1i(texture_shader::texture, 0);
	texture_shader::program->stopusing();


	//create program for tinting textures at alpha-marked pixels
	//with team colors
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
	//fill the teamcolor shader's player color table:
	glUniform4fv(teamcolor_shader::player_color_var, 64, playercolors);
	teamcolor_shader::program->stopusing();
	delete[] playercolors;


	//create program for drawing textures that are alpha-masked before
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

	//after linking, the shaders are no longer necessary
	delete plaintexture_vert;
	delete plaintexture_frag;
	delete teamcolor_frag;
	delete alphamask_vert;
	delete alphamask_frag;

	//setup engine callback methods
	engine::callbacks::on_input.push_back(input_handler);
	engine::callbacks::on_engine_tick.push_back(on_engine_tick);
	engine::callbacks::on_drawgame.push_back(draw_method);
	engine::callbacks::on_drawhud.push_back(hud_draw_method);
}

void destroy() {
	for (auto &obj : buildings) {
		delete obj;
	}

	//oh noes, release hl3 before that!
	delete gaben;

	delete terrain;

	delete build_uni_sound;
	delete destroy_uni_sound0;
	delete destroy_uni_sound1;

	delete university;
	delete texture_shader::program;
	delete teamcolor_shader::program;
	delete alphamask_shader::program;
}

} //namespace openage
