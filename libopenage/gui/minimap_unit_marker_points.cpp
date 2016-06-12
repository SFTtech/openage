// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#include "minimap_unit_marker_points.h"

#include <algorithm>
#include <iterator>
#include <array>

#include "../unit/unit.h"
#include "../gamestate/player.h"

namespace openage {
namespace gui {

std::vector<ColoredTexturedPoint2D> get_unit_marker_points(UnitContainer &placed_units, const std::vector<gamedata::minimap_modes> &markers_types, const std::vector<gamedata::palette_color> &player_palette) {
	std::vector<ColoredTexturedPoint2D> unit_marker_points;

	for (auto unit : placed_units.all_units()) {
		// ignore unselected resources
		if (!unit->selected && unit->has_attribute(attr_type::resource))
			continue;

		int shape = 0;

		if (unit->has_attribute(attr_type::map_draw_level)) {
			auto minimap_mode = unit->get_attribute<attr_type::map_draw_level>().map_draw_level;

			auto found_it = std::find(std::begin(markers_types), std::end(markers_types), minimap_mode);

			if (found_it != std::end(markers_types))
				shape = std::distance(std::begin(markers_types), found_it);
			else
				continue;
		} else {
			continue;
		}

		std::array<unsigned char, 3> rgb;

		// TODO: fix resource colors
		if (unit->selected) {
			rgb = {255, 255, 255};
		} else if (unit->has_attribute(attr_type::owner)) {
			int player_color_index = (unit->get_attribute<attr_type::owner>().player.color - 1) * 8 + 3;
			gamedata::palette_color player_color = player_palette[player_color_index];
			rgb = {player_color.r, player_color.g, player_color.b};
		} else {
			continue;
		}

		auto coord = unit->location.get()->pos.draw;

		unit_marker_points.push_back({float(coord.ne), float(coord.se), float(shape), rgb[0], rgb[1], rgb[2], 255});
	}

	return unit_marker_points;
}

}} // namespace openage::gui
