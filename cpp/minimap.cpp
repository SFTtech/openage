// Copyright 2013-2015 the openage authors. See copying.md for legal info.
#include "minimap.h"

#include <SDL2/SDL.h>
#include <cmath>

#include "log/log.h"
#include "coord/window.h"
#include "coord/chunk.h"
#include <stdio.h>
#include "terrain/terrain.h"

namespace openage {
namespace minimap_shader {
shader::Program *program;
} // namespace texture_shader


coord::camhud_delta Minimap::tile_to_minimap_position(coord::tile tilepos, int ppmt_x, int ppmt_y) {
  // Tile difference from point to west corner 
  /* coord::tile_delta point_to_west{this->ne_least.ne * 16 + tilepos.ne, this->se_least.se * 16 + tilepos.se}; */

  coord::tile_delta relative_to_west{(-this->ne_least.ne * 16 + tilepos.ne), (-this->se_least.se * 16 + tilepos.se)};

  int d = relative_to_west.ne - relative_to_west.se;
  int x, y;
  x = y = 0;

  if (d == 0) {
    x = 2 * ppmt_x * relative_to_west.se;
  } else if (d > 0) {
    x = 2 * relative_to_west.ne * ppmt_x + relative_to_west.se * ppmt_x;
    y = relative_to_west.se * ppmt_y; 
  } else {
    x = 2 * relative_to_west.ne * ppmt_x + relative_to_west.se * ppmt_x;
    y = - relative_to_west.se * ppmt_y;
  }

  /* coord::camhud_delta minimap_pos{point_to_west / } */

  /* int minimap_pos_x, minimap_pos_y; */
  /* if (point_to_west.ne != 0) */
  /*   minimap_pos_x = (this->hudpos.x + this->size.x / 2) / (this->resolution * point_to_west.ne); */
  /* else */
  /*   minimap_pos_x = 0; */

  /* if (point_to_west.se != 0) */ 
  /*   minimap_pos_y = this->hudpos.y/(this->resolution * point_to_west.se); */
  /* else */
  /*   minimap_pos_y = 0; */

  log::log(MSG(dbg) << "tilepos = " << tilepos.ne << ", " << tilepos.se);
  /* log::log(MSG(dbg) << "LEAST NE: ne=" << this->ne_least.ne << ", se=" << this->ne_least.se); */
  /* log::log(MSG(dbg) << "LEAST SE: ne=" << this->se_least.ne << ", se=" << this->se_least.se); */
  /* return coord::camhud_delta{((-this->ne_least.ne * 16 + tilepos.ne) - (-this->se_least.se * 16 + tilepos.se)) * ppmt_x, */
  /*                            ((-this->se_least.se * 16 + tilepos.se) - (-this->ne_least.ne * 16 + tilepos.ne)) * ppmt_y}; */




  return coord::camhud_delta{x, y};
}


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


Minimap::Minimap(Engine *engine, std::shared_ptr<Terrain> terrain, coord::camhud_delta size, coord::camhud hudpos)
  :
  engine{engine},
  terrain{terrain} {
  this->size = size;
  this->hudpos = hudpos;

  glGenBuffers(1, &this->vertbuf);

  // Generate minimap terrain texture
  glGenTextures(1, &this->tertex);
  glBindTexture(GL_TEXTURE_2D, this->tertex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}


void Minimap::generate_background() {
  std::vector<coord::chunk> used = this->terrain->used_chunks();

  this->ne_least = this->ne_most = this->se_least = this->se_most = {0, 0};

  for (coord::chunk c : used) {
    if (c.ne < this->ne_least.ne) this->ne_least = c;
    if (c.se < this->se_least.se) this->se_least = c;
    if (c.ne > this->ne_most.ne) this->ne_most = c;
    if (c.se > this->se_most.se) this->se_most = c;
  }

  uint32_t ne_size = 1 + this->ne_most.ne - this->ne_least.ne;
  uint32_t se_size = 1 + this->se_most.se - this->se_least.se;

  if (ne_size > se_size) {
    this->resolution = ne_size * 16;
  } else {
    this->resolution = se_size * 16;
  }
  // Generate terrain texture
  unsigned char *pixels = new unsigned char[this->resolution * this->resolution * 3];
  for (int i = 0; i < this->resolution; i++){
    for (int j = 0; j < this->resolution; j++) {
      coord::tile tile_pos{i + this->ne_least.ne * 16, j + this->se_least.se * 16};
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


void Minimap::draw() {
  this->generate_background();

  GLfloat left = (GLfloat)this->hudpos.x;
  GLfloat right = (GLfloat)(this->hudpos.x + this->size.x);
  GLfloat bottom = (GLfloat)(this->hudpos.y);
  GLfloat top = (GLfloat)(this->hudpos.y + this->size.y);
  /* GLfloat ymid = (bottom + top) / 2; */
  /* GLfloat xmid = (left + right) / 2; */

  GLfloat vdata[] {
    // vertices
		/* left, ymid, */
		/* xmid, top, */
		/* right, ymid, */
		/* xmid, bottom, */
    left, bottom,
    left, top,
    right, top,
    right, bottom,
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
	glBindBuffer(GL_ARRAY_BUFFER, this->vertbuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vdata), vdata, GL_STATIC_DRAW);
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


  ///////////////////////////
  // VIEW SPACE ON MINIMAP //
  ///////////////////////////

/*   // minimap side in pixels */
/*   /1* GLfloat ydist = top - bottom; *1/ */
/*   /1* GLfloat xdist = right - left; *1/ */
/*   /1* GLint minimap_side = sqrt(ydist*ydist+xdist*xdist); *1/ */

/*   /1* int ppmt_x = xdist / this->resolution; *1/ */
/*   /1* int ppmt_y = ydist / this->resolution; *1/ */
/*   /1* log::log(MSG(dbg) << "PIXELS PER MINIMAP_TILE: " << ppmt_x << ", " << ppmt_y); *1/ */

/*   /1* coord::window win_pos  = *1/ */ 
/*   /1* coord::window win_pos = this->engine->engine_coord_data->camgame_window; *1/ */

/*   // center of window in phys */
/*   coord::phys3 win_pos = this->engine->engine_coord_data->camgame_phys; */
/*     // window size */
/*   coord::window win_size = this->engine->engine_coord_data->window_size; */  
/*   /1* coord::tile camtiles{win_pos.ne / coord::settings::phys_per_tile, win_pos.se / coord::settings::phys_per_tile}; *1/ */
/*   coord::tile win_pos_tile = this->engine->engine_coord_data->camgame_phys.to_tile3().to_tile(); */
/*   coord::phys3_delta phys_frac = this->engine->engine_coord_data->camgame_phys.get_fraction(); */
  
/*   float phys_frac_se = 0.0f; */
/*   if (phys_frac.se != 0) */
/*     phys_frac_se = coord::settings::phys_per_tile / phys_frac.se; */

/*   float phys_frac_ne = 0.0f; */ 
/*   if (phys_frac.ne != 0) */
/*     phys_frac_ne = coord::settings::phys_per_tile / phys_frac.ne; */
/*   else */
/*     log::log(MSG(dbg) << "It IS zero!"); */

/*   /1* coord::camhud_delta camera_rel_pos = tile_to_minimap_position(camtiles, ppmt_x, ppmt_y); *1/ */

/*   coord::tile_delta relative_to_west{(this->ne_least.ne * 16 + win_pos_tile.ne), (this->se_least.se * 16 + win_pos_tile.se)}; */
/*   coord::camhud camera_rel_pos{relative_to_west.se * (top - bottom) / this->resolution, relative_to_west.ne * (left - right) / this->resolution}; */

/*   /1* coord::tile_delta west_to_center{this->ne_least.ne * 16, this->se_least.se * 16}; *1/ */

/*   /1* coord::gamehud minimap_view{camtiles} *1/ */

/*   /1* log::log(MSG(dbg) << "Center Camera Tile: " << camtiles.ne << ", " << camtiles.se); *1/ */
/*   log::log(MSG(dbg) << "Camera Rel Pos: " << camera_rel_pos.x << ", " << camera_rel_pos.y); */
/*   log::log(MSG(dbg) << "Window Tile Position: " << win_pos_tile.ne << "." << phys_frac_ne << ", " << win_pos_tile.se << "." << phys_frac_se); */
/*   log::log(MSG(dbg) << "Relative to west: " << relative_to_west.ne << ", " << relative_to_west.se); */
/*   /1* log::log(MSG(dbg) << "Minimap Position: " << minimap_pos.x << ", " << minimap_pos.y); *1/ */

/*   glPointSize(5); */
/*   glColor3f(1.0, 1.0, 1.0); */
/*   glBegin(GL_POINTS); { */
/*     glVertex3i(camera_rel_pos.x, camera_rel_pos.y, 0); */
/*   } */
/*   glEnd(); */   
}


} // namespace openage
