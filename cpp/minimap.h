// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_MINIMAP_H_
#define OPENAGE_MINIMAP_H_

#include "epoxy/gl.h"

#include "shader/program.h"
#include "terrain/terrain.h"
#include "coord/camhud.h"
#include "unit/unit_container.h"
#include "unit/unit.h"

namespace openage {
namespace minimap_shader {
extern shader::Program *program;
extern GLint size, orig, color;
} // namespace minimap_shader
namespace texture_shader {
extern shader::Program *program;
}


class Minimap {
private:
  Engine *engine;  
  UnitContainer *container;
  std::shared_ptr<Terrain> terrain;
  coord::camhud_delta size;
  coord::camhud hudpos;

public:  
  Minimap(Engine *engine, UnitContainer *container, std::shared_ptr<Terrain> terrain, coord::camhud_delta size,
          coord::camhud hudpos);

  // Draw a simple minimap
  void draw();
  void draw_unit(Unit *unit);
  void generate_background();
  void update();
  coord::camhud from_phys(coord::phys3 position);

private:
  GLfloat left, right, bottom, top, center_vertical, center_horizontal;
  coord::chunk north, east, south, west;
  coord::phys3 old_camgame_phys;
  GLuint tervertbuf;
  GLuint tertex; // Minimap terrain as texture
  GLuint viewvertbuf;
  GLuint unitvertbuf;
  double ratio_horizontal;
  double ratio_vertical;
  int resolution;
};


} // namespace openage

#endif
