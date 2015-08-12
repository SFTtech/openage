// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_MINIMAP_H_
#define OPENAGE_MINIMAP_H_

#include "epoxy/gl.h"
#include <vector>
#include <memory>

#include "handlers.h"

#include "shader/program.h"
#include "coord/camhud.h"
#include "coord/chunk.h"
#include "gamedata/color.gen.h"


namespace openage {
namespace minimap_shader {
extern shader::Program *program;
extern GLint size, orig, color;
} // namespace minimap_shader
namespace texture_shader {
extern shader::Program *program;
}

class Terrain;
class Unit;
class UnitContainer;

/**
 * Implements a minimap, as a HudHandler.
 *
 * Supports dynamic terrain sizes, changing terrain, and displays units.
 */
class Minimap: public HudHandler {
public:  
	/**
	 * Creates the minimap.
	 */
	Minimap(UnitContainer *container, const std::shared_ptr<Terrain> &terrain, coord::camhud_delta size,
          coord::camhud hudpos);
	~Minimap();

	bool on_drawhud() override;
	void draw_unit(Unit *unit);
	void generate_background();

	/**
	 * Updates various variables crucial to the function of the minimap, including
	 * the minimap resolution and the minimap corner position.
	 */
  void auto_mapping();
	void set_mapping(coord::chunk, int resolution);

	/**
	 * Converts a phys3 coordinate to a camhud coordinate, that corresponds to the
	 * given position on the minimap.
	 *
	 * @param coord: The phys3 coordinate to be converted.
	 * @returns The given coordinate position in minimap camhud.
	 */
	coord::camhud from_phys(coord::phys3 coord);

	/**
	 * Converts a camhud coordinate to a phys3 coordinate, assuming that the 
	 * camhud coordinate refers to a position on the minimap.
	 *
	 * @param coord: The minimap camhud coordinate to be converted.
	 * @returns The given coordinate position in phys3.
	 */
	coord::phys3 to_phys(coord::camhud coord);

	/**
	 * Check if a camhud coordinate is within the drawing area of the minimap.
	 *
	 * @param coord: The camhud coordinate to be checked.
	 * @returns True if the coordinate is within the minimap.
	 */
	bool is_within(coord::camhud coord);

private:
	UnitContainer *container;
	const std::shared_ptr<Terrain> &terrain;
	coord::camhud_delta size;
	coord::camhud hudpos;
	std::vector<gamedata::palette_color> palette, player_palette;

	GLint left, right, bottom, top, center_vertical, center_horizontal;
	coord::chunk north, east, south, west;
	GLuint terrain_vertbuf;
	GLuint terrain_tex; 
	GLuint view_vertbuf;
	GLuint unit_vertbuf;

	/**
	 * The ratio of distance between eastern and western minimap corners
	 * (NOT terrain corners!) in camhud game to camhud minimap.
	 */ 
	float ratio_horizontal; 

	/**
	 * The ratio of distance between nortern and southern minimap corners
	 * (NOT terrain corners!) in camhud game to camhud minimap.
	 */
	float ratio_vertical;

	/**
	 * The minimap side length in tiles
	 */
	int resolution;
};


} // namespace openage

#endif
