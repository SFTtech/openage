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
extern GLint size, orig, color;
} // namespace minimap_shader
namespace texture_shader {
extern shader::Program *program;
}


class Minimap {
public:
  openage::Engine *engine;  
  std::shared_ptr<Terrain> terrain;
  coord::camhud_delta size;
  coord::camhud hudpos;
  
  Minimap(openage::Engine *engine, std::shared_ptr<Terrain> terrain, coord::camhud_delta size,
          coord::camhud hudpos);

  // Draw a simple minimap
  void draw();
  void generate_background();
  void update();
  /* coord::camhud_delta tile_to_minimap_position(coord::tile tile_pos, int ppmt_x, int ppmt_y); */
  coord::camhud from_phys(coord::phys3 position);

  // Draw 

private:
  GLfloat left, right, bottom, top, center_vertical, center_horizontal;
  coord::chunk north, east, south, west;
  coord::phys3 old_camgame_phys;
  GLuint tervertbuf;
  GLuint tertex; // Minimap terrain as texture
  GLuint viewvertbuf;
  double ratio_horizontal;
  double ratio_vertical;
  int resolution;
};


} // namespace openage

#endif
