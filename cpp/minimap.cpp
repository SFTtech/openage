// Copyright 2013-2015 the openage authors. See copying.md for legal info.
#include "minimap.h"

#include <SDL2/SDL.h>

#include "log/log.h"
#include "coord/window.h"


namespace openage {

namespace minimap_shader {
shader::Program *program;
} // namespace minimap_shader

Minimap::Minimap(Engine *engine, std::shared_ptr<Terrain> terrain, int size, coord::camhud hudpos)
  :
  engine{engine},
  terrain{terrain},
  size{size} {
  this->hudpos = hudpos;

  glGenBuffers(1, &this->vertbuf);
}

void Minimap::draw() {
	// this array will be uploaded to the GPU.
	// it contains all dynamic vertex data (position, tex coordinates, mask coordinates)

  int i = 0;
  /* for (coord::chunk chunk_pos : this->terrain->used_chunks()) { */
  /*   log::log(MSG(dbg) << "Chunk " << i); */
  /*   TerrainChunk *chunk = this->terrain->get_chunk(chunk_pos); */
  /*   if (this->terrain->get_chunk(chunk_pos) != nullptr) { */
          
  /*   } */
  /*   log::log(MSG(dbg) << "TERRAIN ID[0] = " << this->terrain->get_chunk(chunk_pos)->data[0].terrain_id); */
  /* } */
  

  GLfloat left = (GLfloat)hudpos.x;
  GLfloat right = (GLfloat)(hudpos.x + this->size);
  GLfloat bottom = (GLfloat)(hudpos.y);
  GLfloat top = (GLfloat)(hudpos.y + this->size);

  GLfloat vdata[] {
		left, bottom,
		left, top,
		right, top,
		right, bottom 
  };

  int *pos_id;
  
  minimap_shader::program->use();
  pos_id = &minimap_shader::program->pos_id;

	// store vertex buffer data, TODO: prepare this sometime earlier.
	glBindBuffer(GL_ARRAY_BUFFER, this->vertbuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vdata), vdata, GL_STATIC_DRAW);
  glEnableVertexAttribArray(*pos_id);
  glVertexAttribPointer(*pos_id, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);

	// draw the vertex array
	glDrawArrays(GL_QUADS, 0, 4);

	// unbind the current buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
  minimap_shader::program->stopusing();
}

} // namespace openage
