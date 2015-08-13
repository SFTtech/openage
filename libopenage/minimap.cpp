// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#include "minimap.h"

#include <SDL2/SDL.h>

#include "log/log.h"
#include "coord/window.h"
#include "coord/chunk.h"
#include "terrain/terrain.h"
#include "unit/unit.h"

#include "player.h"
#include "engine.h"


namespace openage {
namespace minimap_shader {
shader::Program *program;
GLint size, orig, color;
} // namespace minimap_shader


Minimap::Minimap(coord::camhud_delta size, coord::camhud hudpos)
	:
	size{size},
	hudpos{hudpos} {

	// Initial update to setup the minimap resolution, etc
	// TODO: Change this
	this->set_mapping(coord::chunk{-1, -1}, 3);

	// Setup minimap dimensions for rendering
	this->left = this->hudpos.x;
	this->right = this->hudpos.x + this->size.x;
	this->bottom = this->hudpos.y;
	this->top = this->hudpos.y + this->size.y;
	this->center_vertical = (float)(this->bottom + this->top) / 2;
	this->center_horizontal = (float)(this->left + this->right) / 2;

	// TODO: this could be gotten from game_renderer?
	util::Dir *data_dir = Engine::get().get_data_dir();
	util::Dir asset_dir = data_dir->append("converted");
	util::read_csv_file(asset_dir.join("player_palette.docx"), this->player_palette);
	util::read_csv_file(asset_dir.join("general_palette.docx"), this->palette);

	glGenBuffers(1, &this->terrain_vertbuf);
	glGenBuffers(1, &this->view_vertbuf);
	glGenBuffers(1, &this->unit_vertbuf);

	// Generate minimap terrain texture
	glGenTextures(1, &this->terrain_tex);
	glBindTexture(GL_TEXTURE_2D, this->terrain_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	minimap_shader::program->use();
	glUniform2i(minimap_shader::orig, (GLint)this->left, (GLint)this->bottom);
	glUniform2i(minimap_shader::size, (GLint)this->size.x, (GLint)this->size.y); 
	minimap_shader::program->stopusing();
}


Minimap::~Minimap() {}


void Minimap::auto_mapping() {
	GameMain *game = Engine::get().get_game();
	if (game == nullptr) {
		return;
	}
	std::vector<coord::chunk> used = game->terrain->used_chunks();
	coord::chunk ne_least, ne_most, se_least, se_most;
	ne_least = ne_most = se_least = se_most = {0, 0};

	for (coord::chunk c : used) {
		if (c.ne < ne_least.ne) ne_least = c;
		if (c.se < se_least.se) se_least = c;
		if (c.ne > ne_most.ne) ne_most = c;
		if (c.se > se_most.se) se_most = c;
	}

	int ne_res = ne_most.ne - ne_least.ne;
	int se_res = se_most.se - se_least.se;

	if (ne_res < se_res) {
		this->set_mapping({ne_least.ne, se_least.se}, se_res + 1);
	} else {
		this->set_mapping({ne_least.ne, se_least.se}, ne_res + 1);
	}
}


void Minimap::set_mapping(coord::chunk west, int resolution) {
	if (resolution < 1) {
		resolution = 1; 
	}
	this->resolution = resolution * 16;
	this->west  = west;
	this->east  = west + coord::chunk_delta{resolution - 1, resolution - 1};
	this->north = west + coord::chunk_delta{resolution - 1, 0};
	this->south = west + coord::chunk_delta{0, resolution - 1};

	coord::camhud_delta north_rel = this->north.to_tile({16, 0}).to_tile3(0).to_phys3({0, 0, 0}).to_camgame().to_window().to_camhud().as_relative();
	coord::camhud_delta east_rel  = this->east.to_tile({16, 16}).to_tile3(0).to_phys3({0, 0, 0}).to_camgame().to_window().to_camhud().as_relative();
	coord::camhud_delta south_rel = this->south.to_tile({0, 16}).to_tile3(0).to_phys3({0, 0, 0}).to_camgame().to_window().to_camhud().as_relative();
	coord::camhud_delta west_rel  = this->west.to_tile({0,   0}).to_tile3(0).to_phys3({0, 0, 0}).to_camgame().to_window().to_camhud().as_relative();

	this->ratio_horizontal = (float)this->size.x / (float)(east_rel.x - west_rel.x);
	this->ratio_vertical   = (float)this->size.y / (float)(north_rel.y - south_rel.y);
}


void Minimap::generate_background() { 
	GameMain *game = Engine::get().get_game();
	if (game == nullptr) {
		return;
	}
	std::shared_ptr<Terrain> &terrain = game->terrain;
	// Generate terrain texture
	uint8_t *pixels = new uint8_t[this->resolution * this->resolution * 3];
	for (int i = 0; i < this->resolution; i++){
		for (int j = 0; j < this->resolution; j++) {
			coord::tile tile_pos{i + this->west.ne * 16, j + this->west.se * 16};
			TileContent *tile_data = terrain->get_data(tile_pos);

			if (tile_data == nullptr) {
				pixels[i * this->resolution * 3 + j * 3 + 0] = 0;
				pixels[i * this->resolution * 3 + j * 3 + 1] = 0;
				pixels[i * this->resolution * 3 + j * 3 + 2] = 0;  
			} else {
				uint8_t pal_idx = terrain->map_color_hi(tile_data->terrain_id);
				gamedata::palette_color hi_color = this->palette[pal_idx];
				pixels[i * this->resolution * 3 + j * 3 + 0] = hi_color.r;
				pixels[i * this->resolution * 3 + j * 3 + 1] = hi_color.g;
				pixels[i * this->resolution * 3 + j * 3 + 2] = hi_color.b; 
			}
		}
	}

	glBindTexture(GL_TEXTURE_2D, this->terrain_tex);

	glTexImage2D(
		GL_TEXTURE_2D, 0,
		GL_RGB, this->resolution, this->resolution, 0,
		GL_RGB, GL_UNSIGNED_BYTE, pixels
	);

	delete pixels;
}


coord::camhud Minimap::from_phys(coord::phys3 coord) { 
	coord::camhud_delta west_rel = this->west.to_tile({0, 0}).to_tile3(0).to_phys3({0, 0, 0}).to_camgame().to_window().to_camhud().as_relative();
	coord::camhud_delta coord_rel = coord.to_camgame().to_window().to_camhud().as_relative() - west_rel;
	coord_rel.x = coord_rel.x * this->ratio_horizontal;
	coord_rel.y = coord_rel.y * this->ratio_vertical;

	return (coord_rel + coord::camhud{(coord::pixel_t)this->left, (coord::pixel_t)this->center_vertical});
}


coord::phys3 Minimap::to_phys(coord::camhud coord) { 
	coord::camhud_delta coord_rel{(coord::pixel_t)((float)(coord.x - (coord::pixel_t)this->left)/this->ratio_horizontal),
	                              (coord::pixel_t)((float)(coord.y - (coord::pixel_t)this->center_vertical)/this->ratio_vertical)}; 
	coord::camhud_delta west_rel = this->west.to_tile({0, 0}).to_tile3(0).to_phys3({0, 0, 0}).to_camgame().to_window().to_camhud().as_relative();
	return (coord_rel + west_rel).as_absolute().to_window().to_camgame().to_phys3();;
}


bool Minimap::is_within(coord::camhud coord) {
	coord::camhud_delta center = this->size/2;
	coord::camhud rel = this->hudpos + center - coord.as_relative();

	if (abs(rel.x) * center.y + abs(rel.y) * center.x <= center.x * center.y)
		return true;
	else
		return false;
}


void Minimap::draw_unit(Unit *unit) {
	int *color, *pos_id;
	minimap_shader::program->use();
	color = &minimap_shader::color;
	pos_id = &minimap_shader::program->pos_id;

	coord::camhud unit_pos = this->from_phys(unit->location.get()->pos.draw);

	// player color
	GLfloat rgb[3];
	rgb[0] = 0.0; rgb[1] = 0.0; rgb[2] = 0.0;

	// TODO: fix resource colors
	if (unit->selected) {
		rgb[0] = 1.0; rgb[1] = 1.0; rgb[2] = 1.0;
	} else if (unit->has_attribute(attr_type::resource)) {
		return;  // ignore resources
	} else if (unit->has_attribute(attr_type::owner)) {
		int player_color_index = (unit->get_attribute<attr_type::owner>().player.color - 1) * 8;
		gamedata::palette_color player_color = this->player_palette[player_color_index];
		rgb[0] = player_color.r; rgb[1] = player_color.g; rgb[2] = player_color.b;
	}

	GLfloat unit_vdata[] {
		(GLfloat)unit_pos.x, (GLfloat)unit_pos.y,
		rgb[0], rgb[1], rgb[2]
	};

	glBindBuffer(GL_ARRAY_BUFFER, this->view_vertbuf);
	glVertexAttribPointer(*pos_id, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(*pos_id);
	glVertexAttribPointer(*color, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)(2*sizeof(GLfloat)));
	glEnableVertexAttribArray(*color);

	glPointSize(4); 
	glBufferData(GL_ARRAY_BUFFER, sizeof(unit_vdata), unit_vdata, GL_STATIC_DRAW);
	glDrawArrays(GL_POINTS, 0, 1);

	// unbind buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	minimap_shader::program->stopusing(); 
}


bool Minimap::on_drawhud() {
	Engine &engine = Engine::get();

	if (engine.get_game() == nullptr) {
		return true;
	}

	this->generate_background();
	GLint terrain_vdata[] {
		// vertices
		this->left, this->center_vertical,
		this->center_horizontal, this->top,
		this->right, this->center_vertical,
		this->center_horizontal, this->bottom,
		// tex coords
		0, 0,
		0, 1,
		1, 1,
		1, 0
	};

	int *pos_id, *tex_coord;
	
	glBindTexture(GL_TEXTURE_2D, this->terrain_tex);
	texture_shader::program->use();
	pos_id = &texture_shader::program->pos_id;
	tex_coord = &texture_shader::tex_coord;

	// store vertex buffer data, TODO: prepare this sometime earlier.
	glBindBuffer(GL_ARRAY_BUFFER, this->terrain_vertbuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(terrain_vdata), terrain_vdata, GL_STATIC_DRAW);
	glVertexAttribPointer(*pos_id, 2, GL_INT, GL_FALSE, 2 * sizeof(GLint), (GLvoid*)0);
	glEnableVertexAttribArray(*pos_id);
	glVertexAttribPointer(*tex_coord, 2, GL_INT, GL_FALSE, 2 * sizeof(GLint), (GLvoid*)(8*sizeof(GLint)));
	glEnableVertexAttribArray(*tex_coord);

	// draw the vertex array
	glDrawArrays(GL_QUADS, 0, 4);

	// unbind the current buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	texture_shader::program->stopusing();

	coord::window view_size{(coord::pixel_t)(engine.get_coord_data()->window_size.x / 2 * this->ratio_horizontal),
													(coord::pixel_t)(engine.get_coord_data()->window_size.y / 2 * this->ratio_vertical)};
	coord::camhud view = this->from_phys(engine.get_coord_data()->camgame_phys);

	// Draw units
	std::vector<Unit *> units = engine.get_game()->placed_units.all_units();
	for (Unit *u : units) {
		this->draw_unit(u);
	}

	// Draw minimap view box 
	int *color;
	minimap_shader::program->use();
	color = &minimap_shader::color;
	pos_id = &minimap_shader::program->pos_id;

	GLint view_bg_vdata[] {
		// black border
		view.x + view_size.x + 1, view.y + view_size.y - 1,
		view.x - view_size.x + 1, view.y + view_size.y - 1,
		view.x - view_size.x + 1, view.y - view_size.y - 1,
		view.x + view_size.x + 1, view.y - view_size.y - 1,
		// colors
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
	};

	GLint view_fg_vdata[] {
		// white border
		view.x + view_size.x, view.y + view_size.y,
		view.x - view_size.x, view.y + view_size.y,
		view.x - view_size.x, view.y - view_size.y,
		view.x + view_size.x, view.y - view_size.y,
		// colors
		1, 1, 1,
		1, 1, 1,
		1, 1, 1,
		1, 1, 1,
	};

	glBindBuffer(GL_ARRAY_BUFFER, this->view_vertbuf);
	glVertexAttribPointer(*pos_id, 2, GL_INT, GL_FALSE, 2 * sizeof(GLint), (GLvoid*)0);
	glEnableVertexAttribArray(*pos_id);
	glVertexAttribPointer(*color, 3, GL_INT, GL_FALSE, 3 * sizeof(GLint), (GLvoid*)(8*sizeof(GLint)));
	glEnableVertexAttribArray(*color);

	glLineWidth(1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(view_bg_vdata), view_bg_vdata, GL_STATIC_DRAW);
	glDrawArrays(GL_LINE_LOOP, 0, 4);
	glBufferData(GL_ARRAY_BUFFER, sizeof(view_fg_vdata), view_fg_vdata, GL_STATIC_DRAW);
	glDrawArrays(GL_LINE_LOOP, 0, 4);

	// unbind view box buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	minimap_shader::program->stopusing();

	return true;
}


} // namespace openage
