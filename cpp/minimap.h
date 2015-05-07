// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_MINIMAP_H_
#define OPENAGE_MINIMAP_H_

#include "epoxy/gl.h"

#include "terrain/terrain.h"
#include "shader/program.h"
#include "shader/shader.h"

namespace openage {
namespace minimap_shader {
extern shader::Program *program;
}

class Minimap {
public:
  Minimap();

  // Draw a simple minimap
  void draw();

private:
  GLuint vertbuf;
};

}

#endif
