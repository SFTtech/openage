// Copyright 2013-2015 the openage authors. See copying.md for legal info.
#include "minimap.h"

#include <SDL2/SDL.h>

#include "log/log.h"
#include "coord/window.h"
#include "coord/chunk.h"
#include <stdio.h>
#include "terrain/terrain.h"

namespace openage {
namespace minimap_shader {
shader::Program *program;
} // namespace texture_shader

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
/* KOH,28 */
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

  this->generate_background();
}


void Minimap::generate_background() {
  std::vector<coord::chunk> used = this->terrain->used_chunks();

  coord::chunk ne_least, ne_most, se_least, se_most;
  ne_least = ne_most = se_least = se_most = {0, 0};

  for (coord::chunk c : used) {
    if (c.ne < ne_least.ne) ne_least = c;
    if (c.se < se_least.se) se_least = c;
    if (c.ne > ne_most.ne) ne_most = c;
    if (c.se > se_most.se) se_most = c;
  }

  uint32_t ne_size = 1 + ne_most.ne - ne_least.ne;
  uint32_t se_size = 1 + se_most.se - se_least.se;

  int longest;
  bool ne_longest;
  if (ne_size > se_size) {
    longest = ne_size * 16;
    ne_longest = true;
  } else {
    longest = se_size * 16;
    ne_longest = false;
  }
  // Generate terrain texture
  unsigned char *pixels = new unsigned char[longest * longest * 3];
  for (int32_t i = 0; i < longest; i++){
    for (int32_t j = 0; j < longest; j++) {
      coord::tile tile_pos{i + ne_least.ne * 16, j + se_least.se * 16};
      TileContent *tile_data = this->terrain->get_data(tile_pos);

      if (tile_data == nullptr) {
        pixels[i * longest * 3 + j * 3 + 0] = 0;
        pixels[i * longest * 3 + j * 3 + 1] = 0;
        pixels[i * longest * 3 + j * 3 + 2] = 0;  
      } else {
        map_terrain_id(&pixels[i * longest * 3 + j * 3], tile_data->terrain_id);
      }
    }
  }

  glBindTexture(GL_TEXTURE_2D, this->tertex);

 	glTexImage2D(
		GL_TEXTURE_2D, 0,
		GL_RGB, longest, longest, 0,
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

  GLfloat vdata[] {
    // vertices
		left, (bottom + top) / 2,
		(left + right) / 2, top,
		right, (bottom + top) / 2,
		(left + right) / 2, bottom,
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
}


} // namespace openage
