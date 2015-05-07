// Copyright 2013-2015 the openage authors. See copying.md for legal info.
//
#include "minimap.h"

#include <SDL2/SDL.h>

#include "log/log.h"


namespace openage {

namespace minimap_shader {
shader::Program *program;
} // namespace minimap_shader

Minimap::Minimap() {
  glGenBuffers(1, &this->vertbuf);
}

void Minimap::draw() {
  log::log(MSG(info) << "attempting to draw minimap");
	// this array will be uploaded to the GPU.
	// it contains all dynamic vertex data (position, tex coordinates, mask coordinates)
	GLfloat vdata[] {
    0.0f, 0.0f,
    1000.0f, 0.0f,
    1000.0f, 1000.0f
	};
  minimap_shader::program->use();

	// store vertex buffer data, TODO: prepare this sometime earlier.
	glBindBuffer(GL_ARRAY_BUFFER, this->vertbuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vdata), vdata, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
  glEnableVertexAttribArray(0);

	// draw the vertex array
	glDrawArrays(GL_TRIANGLES, 0, 3);

	// unbind the current buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
  minimap_shader::program->stopusing();
}

} // namespace openage
