// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_MINIMAP_H_
#define OPENAGE_MINIMAP_H_

#include "epoxy/gl.h"

#include "shader/program.h"
#include "terrain/terrain.h"
#include "coord/camhud.h"

namespace openage {
namespace minimap_shader {
extern shader::Program *program;
} // namespace minimap_shader

class Minimap {
public:
  openage::Engine *engine;  
  std::shared_ptr<Terrain> terrain;
  int size;
  coord::camhud hudpos;
  
  Minimap(openage::Engine *engine, std::shared_ptr<Terrain> terrain, int size,
      coord::camhud hudpos);

  // Draw a simple minimap
  void draw();

private:
  GLuint vertbuf;
};

} // namespace openage

#endif
