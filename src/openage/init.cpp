#include "init.h"

#include <stdio.h>

#include "../engine/texture.h"
#include "../engine/util/strings.h"
#include "../engine/util/file.h"
#include "../engine/util/error.h"
#include "../engine/callbacks.h"

#include "callbacks.h"
#include "gamestate.h"

using namespace engine;

namespace openage {

Texture **terrain_textures, **blending_textures;

//hardcoded for now
unsigned int blend_mode_count = 9;

int *terrain_priority_list;

unsigned int terrain_data[20][20] = {
	{  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7},
	{  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7},
	{  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7},
	{  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7, 11, 11, 11,  7,  7,  7,  7,  7},
	{  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7, 11, 11, 11, 11, 11,  7,  7,  7},
	{  7,  7, 20, 20, 20,  7,  7,  7,  7,  7,  7,  7, 11, 11, 11, 11, 11, 11,  7,  7},
	{  7,  7, 20,  7,  7, 20, 20,  7,  7,  7,  7,  7, 11, 11, 11, 11, 11,  7,  7,  7},
	{  7,  7, 20,  7,  7,  7,  7,  7,  7,  7,  7,  7, 11, 11, 11,  7,  7,  7,  7,  7},
	{  7, 20, 20, 20,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7},
	{  7,  7, 20,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7},
	{  7,  7, 20,  7,  7,  7,  9,  9,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7},
	{  7,  7,  7,  7,  7,  7,  9,  9,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7},
	{  7,  7,  7,  7, 13,  7,  9,  7,  7, 12, 12,  7,  7,  7,  7,  7,  7,  7,  7,  7},
	{  7,  7,  7,  7, 13,  9,  9,  7, 12,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7},
	{  7,  7,  7,  7, 13,  7,  7,  7, 12,  7,  7,  7,  7, 17, 17, 17,  7,  7,  7,  7},
	{  7,  7,  7,  7, 13,  7,  7,  7, 12,  7,  7,  7,  7, 18, 18, 18,  7,  7,  7,  7},
	{  7,  7, 12, 12, 12, 12, 12, 12, 12,  7,  7,  7,  7, 19, 19, 19,  7,  7,  7,  7},
	{  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  3,  3,  3,  7,  7,  7,  7},
	{  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  3,  3,  3,  3, 14, 14,  7},
	{  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  3,  3,  7,  7,  7}
};

void init() {
	//load textures and stuff
	gaben = new Texture("gaben.png");

	//TODO: dynamic generation of the file path
	//sync this with convert .py script !

	university = new Texture("age/raw/Data/graphics.drs/3836.slp.png", true, PLAYERCOLORED);

	terrain_priority_list = new int[terrain_texture_count];
	terrain_textures = new Texture*[terrain_texture_count];

	//set terrain priorities, TODO: get them from media files. hardcoded for now.
	terrain_priority_list[0]  = 70;
	terrain_priority_list[1]  = 102;
	terrain_priority_list[2]  = 139;
	terrain_priority_list[3]  = 155;
	terrain_priority_list[4]  = 157;
	terrain_priority_list[5]  = 101;
	terrain_priority_list[6]  = 106;
	terrain_priority_list[7]  = 90;
	terrain_priority_list[8]  = 100;
	terrain_priority_list[9]  = 80;
	terrain_priority_list[10] = 92;
	terrain_priority_list[11] = 60;
	terrain_priority_list[12] = 140;
	terrain_priority_list[13] = 141;
	terrain_priority_list[14] = 110;
	terrain_priority_list[15] = 122;
	terrain_priority_list[16] = 123;
	terrain_priority_list[17] = 150;
	terrain_priority_list[18] = 151;
	terrain_priority_list[19] = 152;
	terrain_priority_list[20] = 40;
	terrain_priority_list[21] = 130;
	terrain_priority_list[22] = 132;
	terrain_priority_list[23] = 134;
	terrain_priority_list[24] = 136;
	terrain_priority_list[25] = 162;
	terrain_priority_list[26] = 120;

	terrain = new Terrain(20, terrain_texture_count, blend_mode_count, terrain_priority_list);

	for (unsigned int i = 0; i < terrain_texture_count; i++) {
		int current_id = terrain_ids[i];
		char *terraintex_filename = util::format("age/raw/Data/terrain.drs/%d.slp.png", current_id);

		auto new_texture = new Texture(terraintex_filename, true, ALPHAMASKED);
		new_texture->fix_hotspots(48, 24);

		terrain_textures[i] = new_texture;
		terrain->set_texture(i, new_texture);

		delete[] terraintex_filename;
	}

	//add the blendomatic masks to the terrain
	blending_textures = new Texture*[blend_mode_count];

	for (unsigned int i = 0; i < blend_mode_count; i++) {
		char *mask_filename = util::format("age/alphamask/mode%02d.png", i);

		auto new_texture = new Texture(mask_filename, true);
		new_texture->fix_hotspots(48, 24);

		blending_textures[i] = new_texture;
		terrain->set_mask(i, new_texture);

		delete[] mask_filename;
	}


	//set the terrain types according to the data array.
	coord::tile pos = {0, 0};
	for (; pos.ne < (int) terrain->get_size(); pos.ne++) {
		for (pos.se = 0; pos.se < (int) terrain->get_size(); pos.se++) {
			int texid = terrain_data[pos.ne][pos.se];
			terrain->set_tile(pos, texid);
		}
	}




	//get the player colors from the sub-palette exported by script
	char *pcolor_file = util::read_whole_file("age/processed/player_color_palette.pal");

	char *pcolor_seeker = pcolor_file;
	char *currentline = pcolor_file;

	//hardcoded for now.
	const unsigned int num_pcolors = 64;
	GLfloat playercolors[num_pcolors*4];

	for(; *pcolor_seeker != '\0'; pcolor_seeker++) {

		if (*pcolor_seeker == '\n') {
			*pcolor_seeker = '\0';

			if (*currentline != '#') {
				uint n, r, g, b, a, idx;

				//TODO raise exception if stuff is specified multiple times.
				//alternatively, simply dont use indices, raise exception if values are > 255 or < 0, or if idx is < 0 or > 63

				if(sscanf(currentline, "n=%u", &n) == 1) {
					if (n != num_pcolors) {
						throw Error("the player colortable must have %u entries!", num_pcolors);
					}
				} else if(sscanf(currentline, "%u=%u,%u,%u,%u", &idx, &r, &g, &b, &a)) {
					playercolors[idx*4] = r / 255.0;
					playercolors[idx*4 + 1] = g / 255.0;
					playercolors[idx*4 + 2] = b / 255.0;
					playercolors[idx*4 + 3] = a / 255.0;
				} else {
					throw Error("unknown line content reading the player color table");
				}
			}
			currentline = pcolor_seeker + 1;
		}
	}

	delete[] pcolor_file;


	//shader initialisation

	//read shader source codes and create shader objects for wrapping them.

	char *texture_vert_code = util::read_whole_file("shaders/maptexture.vert.glsl");
	auto plaintexture_vert = new shader::Shader(GL_VERTEX_SHADER, texture_vert_code);
	delete[] texture_vert_code;

	char *texture_frag_code = util::read_whole_file("shaders/maptexture.frag.glsl");
	auto plaintexture_frag = new shader::Shader(GL_FRAGMENT_SHADER, texture_frag_code);
	delete[] texture_frag_code;

	char *teamcolor_frag_code = util::read_whole_file("shaders/teamcolors.frag.glsl");
	auto teamcolor_frag = new shader::Shader(GL_FRAGMENT_SHADER, teamcolor_frag_code);
	delete[] teamcolor_frag_code;

	char *alphamask_vert_code = util::read_whole_file("shaders/alphamask.vert.glsl");
	auto alphamask_vert = new shader::Shader(GL_VERTEX_SHADER, alphamask_vert_code);
	delete[] alphamask_vert_code;

	char *alphamask_frag_code = util::read_whole_file("shaders/alphamask.frag.glsl");
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
	//oh noes, release hl3 before that!
	delete gaben;

	delete university;
	delete texture_shader::program;
	delete teamcolor_shader::program;
	delete alphamask_shader::program;

	for (unsigned int i = 0; i < terrain_texture_count; i++) {
		delete terrain_textures[i];
	}
	for (unsigned int i = 0; i < blend_mode_count; i++) {
		delete blending_textures[i];
	}
	delete[] terrain_priority_list;
}

} //namespace openage
