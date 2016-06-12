// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <vector>

#include "../gamedata/unit.gen.h"
#include "../gamedata/color.gen.h"

namespace openage {

class UnitContainer;

namespace gui {

struct ColoredTexturedPoint2D {
	float x, y;

	/**
	 * Which cell of the marker atlas to use for this minimap marker.
	 */
	float cell_id;

	unsigned char r, g, b, a;
};

/**
 * @param placed_units units in the game
 * @param markers_types which marker type each cell of the marker atlas represents
 * @param player_palette all color shades of the players
 * @return array of markers that is drawable by the TexturedPointsShader
 */
std::vector<ColoredTexturedPoint2D> get_unit_marker_points(UnitContainer &placed_units, const std::vector<gamedata::minimap_modes> &markers_types, const std::vector<gamedata::palette_color> &player_palette);

}} // namespace openage::gui
