// Copyright 2013-2015 the openage authors. See copying.md for legal info.
#include "minimap.h"

#include <SDL2/SDL.h>
#include <cmath>

#include "log/log.h"
#include "coord/window.h"
#include "coord/chunk.h"
#include <stdio.h>
#include "terrain/terrain.h"
#include "player.h"


namespace openage {
namespace minimap_shader {
shader::Program *program;
GLint size, orig, color;
} // namespace minimap_shader


void map_terrain_id(unsigned char *pixel, int terrain_id) {
  switch (terrain_id) {
/* Snow Forest,13 */
/* Forest,9 */
    case 13:
    case 9:
      *pixel = 240;
      *(pixel+1) = 170;
      *(pixel+2) = 30;
      break;
/* Ice,18 */
    case 18:
      *pixel = 200;
      *(pixel+1) = 245;
      *(pixel+2) = 255;
      break;
/* Foundation,19 130 140 80*/
    case 19:
      *pixel = 130;
      *(pixel+1) = 140;
      *(pixel+2) = 80;
      break;
/* Farm1,6 */
/* Farm2,7 */
/* Farm Cnst1,20 */
/* Farm Cnst2,21 */
/* Farm Cnst3,22 */
    case 6:
    case 7:
    case 20:
    case 21:
    case 22:
      *pixel     = 230;
      *(pixel+1) = 180;
      *(pixel+2) = 120;
      break;
    // Snow
    case 23:
      *pixel     = 230;
      *(pixel+1) = 250;
      *(pixel+2) = 255;
      break;
    // Dirt
    case 3: // Dirt 3
    case 5: // Dirt
    case 10: // Dirt 2
    case 24: // Snow Dirt
      *pixel     = 210;
      *(pixel+1) = 160;
      *(pixel+2) = 90;
      break;
    // Road
    case 16:
    case 17: // Road 2
    case 26: // Snow Road
    case 27: // Snow Road 2
      *pixel     = 235;
      *(pixel+1)     = 215;
      *(pixel+2)     = 190;
      break;
    // Grass
    case 0: // Grass
    case 8: // Grass
    case 11: // Grass
    case 25: // Snow Grass
      *pixel     = 50;
      *(pixel+1) = 150;
      *(pixel+2) = 40;
      break;
    // Water
    case 14: //Water 2
    case 15: // Water 3
    case 1: // Water
      *pixel     = 50;
      *(pixel+1) = 90;
      *(pixel+2) = 180;
      break;
    /* Palm Desert,12 */
    /* Beach,2 250 200 230*/
    case 12:
    case 2:
      *pixel     = 250;
      *(pixel+1) = 200;
      *(pixel+2) = 150;
      break;
    /* Shallows,4 */
    case 4:
      *pixel     = 80;
      *(pixel+1) = 150;
      *(pixel+2) = 180;
      break;
    /* KOH,28, ... other */
    default:
      *pixel     = 255;
      *(pixel+1) = 0;
      *(pixel+2) = 255;
  }
}


Minimap::Minimap(Engine *engine, UnitContainer *container, std::shared_ptr<Terrain> terrain, coord::camhud_delta size, coord::camhud hudpos)
  :
  engine{engine},
  container{container},
  terrain{terrain} {
  this->size = size;
  this->hudpos = hudpos;

  this->update();

  // Setup minimap dimensions for rendering
  this->left = (GLfloat)this->hudpos.x;
  this->right = (GLfloat)(this->hudpos.x + this->size.x);
  this->bottom = (GLfloat)(this->hudpos.y);
  this->top = (GLfloat)(this->hudpos.y + this->size.y);
  this->center_vertical = (this->bottom + this->top) / 2;
  this->center_horizontal = (this->left + this->right) / 2;

  glGenBuffers(1, &this->tervertbuf);
  glGenBuffers(1, &this->viewvertbuf);
  glGenBuffers(1, &this->unitvertbuf);

  // Generate minimap terrain texture
  glGenTextures(1, &this->tertex);
  glBindTexture(GL_TEXTURE_2D, this->tertex);
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


void Minimap::update() {
  std::vector<coord::chunk> used = this->terrain->used_chunks();
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
    ne_most.ne = ne_least.ne + (se_res);
  } else {
    se_most.se = se_least.se + (ne_res); 
  }

  this->north = {ne_most.ne, se_least.se};
  this->east  = {ne_most.ne, se_most.se};
  this->south = {ne_least.ne, se_most.se};
  this->west  = {ne_least.ne, se_least.se};

  // Compute resolution
  if ((this->east.se - this->north.se) > (this->east.ne - this->south.ne)) {
    this->resolution = (this->east.se - this->north.se + 1) * 16;
  } else {
    this->resolution = (this->east.ne - this->south.ne + 1) * 16;
  }

  // Corners to camhud
  coord::camhud_delta north_rel = this->north.to_tile({16, 0}).to_tile3(0).to_phys3({0, 0, 0}).to_camgame().to_window().to_camhud().as_relative();
  coord::camhud_delta east_rel = this->east.to_tile({16, 16}).to_tile3(0).to_phys3({0, 0, 0}).to_camgame().to_window().to_camhud().as_relative();
  coord::camhud_delta south_rel = this->south.to_tile({0, 16}).to_tile3(0).to_phys3({0, 0, 0}).to_camgame().to_window().to_camhud().as_relative();
  coord::camhud_delta west_rel = this->west.to_tile({0, 0}).to_tile3(0).to_phys3({0, 0, 0}).to_camgame().to_window().to_camhud().as_relative();

  this->ratio_horizontal =  (double)this->size.x / (double)(east_rel.x - west_rel.x);
  this->ratio_vertical = (double)this->size.y / (double)(north_rel.y - south_rel.y);
}


void Minimap::generate_background() { 
  // Generate terrain texture
  unsigned char *pixels = new unsigned char[this->resolution * this->resolution * 3];
  for (int i = 0; i < this->resolution; i++){
    for (int j = 0; j < this->resolution; j++) {
      coord::tile tile_pos{i + this->west.ne * 16, j + this->west.se * 16};
      TileContent *tile_data = this->terrain->get_data(tile_pos);

      if (tile_data == nullptr) {
        pixels[i * this->resolution * 3 + j * 3 + 0] = 0;
        pixels[i * this->resolution * 3 + j * 3 + 1] = 0;
        pixels[i * this->resolution * 3 + j * 3 + 2] = 0;  
      } else {
        map_terrain_id(&pixels[i * this->resolution * 3 + j * 3], tile_data->terrain_id);
      }
    }
  }

  glBindTexture(GL_TEXTURE_2D, this->tertex);

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

void Minimap::draw_unit(Unit *unit) {
  int *color, *pos_id;
  minimap_shader::program->use();
  color = &minimap_shader::color;
  pos_id = &minimap_shader::program->pos_id;

  coord::camhud unit_pos = this->from_phys(unit->location.get()->pos.draw);

  // player color
  float rgb[3];

  if (unit->selected) {
    rgb[0] = 1.0; rgb[1] = 1.0; rgb[2] = 1.0;
  } else if (unit->has_attribute(attr_type::owner)) {
    rgb[0] = 1.0; rgb[1] = 1.0; rgb[2] = 0.0;
    Attribute<attr_type::owner> owner = unit->get_attribute<attr_type::owner>();
    Player *p = &owner.player;
    int col = p->color;
    switch (col) {
      case 1: // blue
        rgb[0] = 0.0; rgb[1] = 0.0; rgb[2] = 1.0;
        break;
      case 2: // red
        rgb[0] = 1.0; rgb[1] = 0.0; rgb[2] = 0.0;
        break;
      case 3: // green
        rgb[0] = 0.0; rgb[1] = 1.0; rgb[2] = 0.0;
        break;
      case 4: // yellow
        rgb[0] = 1.0; rgb[1] = 1.0; rgb[2] = 0.0;
        break;
      case 5: // orange
        rgb[0] = 1.0; rgb[1] = 0.5; rgb[2] = 0.0;
        break;
      case 6: // cyan
        rgb[0] = 0.0; rgb[1] = 1.0; rgb[2] = 1.0;
        break;
      case 7: // purple
        rgb[0] = 1.0; rgb[1] = 0.0; rgb[2] = 1.0;
        break;
      case 8: // grey
        rgb[0] = 0.5; rgb[1] = 0.5; rgb[2] = 0.5;
        break;
      default:
        rgb[0] = 0.0; rgb[1] = 0.0; rgb[2] = 0.0;
        break;
    }
  } else { 
    rgb[0] = 0.5; rgb[1] = 0.5; rgb[2] = 0.5; 
  }


  GLfloat unit_vdata[] {
    unit_pos.x, unit_pos.y,
    rgb[0], rgb[1], rgb[2]
  };

  glBindBuffer(GL_ARRAY_BUFFER, this->viewvertbuf);
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

void Minimap::draw() {
  this->update();
  this->generate_background();

  GLfloat tervdata[] {
    // vertices
		this->left, this->center_vertical,
		this->center_horizontal, this->top,
		this->right, this->center_vertical,
		this->center_horizontal, this->bottom,
    // tex coords
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f
  };

  int *pos_id, *tex_coord;
  
  glBindTexture(GL_TEXTURE_2D, this->tertex);
  texture_shader::program->use();
  pos_id = &texture_shader::program->pos_id;
  tex_coord = &texture_shader::tex_coord;

	// store vertex buffer data, TODO: prepare this sometime earlier.
	glBindBuffer(GL_ARRAY_BUFFER, this->tervertbuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tervdata), tervdata, GL_STATIC_DRAW);
  glVertexAttribPointer(*pos_id, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
  glEnableVertexAttribArray(*pos_id);
  glVertexAttribPointer(*tex_coord, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)(8*sizeof(GLfloat)));
  glEnableVertexAttribArray(*tex_coord);

	// draw the vertex array
	glDrawArrays(GL_QUADS, 0, 4);

	// unbind the current buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
  texture_shader::program->stopusing();

  coord::window view_size{this->engine->engine_coord_data->window_size.x / 2 * this->ratio_horizontal, this->engine->engine_coord_data->window_size.y / 2 * this->ratio_vertical};
  coord::camhud view = this->from_phys(this->engine->engine_coord_data->camgame_phys);

  if (this->old_camgame_phys.ne != this->engine->engine_coord_data->camgame_phys.ne ||
      this->old_camgame_phys.se != this->engine->engine_coord_data->camgame_phys.se) {
    old_camgame_phys = this->engine->engine_coord_data->camgame_phys;
  }

  // Draw units
  std::vector<Unit *> units = this->container->all_units();
  for (Unit *u : units) {
    this->draw_unit(u);
  }

  // Draw minimap view box 
  int *color;
  minimap_shader::program->use();
  color = &minimap_shader::color;
  pos_id = &minimap_shader::program->pos_id;

  GLfloat view_bg_vdata[] {
    // black border
		view.x + view_size.x + 1, view.y + view_size.y - 1,
    view.x - view_size.x + 1, view.y + view_size.y - 1,
    view.x - view_size.x + 1, view.y - view_size.y - 1,
    view.x + view_size.x + 1, view.y - view_size.y - 1,
    // colors
    0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f,
  };

  GLfloat view_fg_vdata[] {
    // white border
    view.x + view_size.x, view.y + view_size.y,
    view.x - view_size.x, view.y + view_size.y,
    view.x - view_size.x, view.y - view_size.y,
    view.x + view_size.x, view.y - view_size.y,
    // colors
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
  };

  glBindBuffer(GL_ARRAY_BUFFER, this->viewvertbuf);
  glVertexAttribPointer(*pos_id, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
  glEnableVertexAttribArray(*pos_id);
  glVertexAttribPointer(*color, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)(8*sizeof(GLfloat)));
  glEnableVertexAttribArray(*color);

  glLineWidth(1);
  glBufferData(GL_ARRAY_BUFFER, sizeof(view_bg_vdata), view_bg_vdata, GL_STATIC_DRAW);
	glDrawArrays(GL_LINE_LOOP, 0, 4);
  glBufferData(GL_ARRAY_BUFFER, sizeof(view_fg_vdata), view_fg_vdata, GL_STATIC_DRAW);
	glDrawArrays(GL_LINE_LOOP, 0, 4);

  // unbind view box buffer
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  minimap_shader::program->stopusing();
}

} // namespace openage
