// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include <cmath>

#include "../coord/tile.h"
#include "../coord/tile3.h"
#include "../engine.h"
#include "../terrain/terrain.h"

#include "action.h"
#include "command.h"
#include "producer.h"
#include "selection.h"
#include "unit.h"

namespace openage {

UnitSelection::UnitSelection()
	:
	selection_type{selection_type_t::nothing},
	drag_active{false},
	font_size{12} {
}

bool UnitSelection::on_drawhud() {

	// the drag selection box
	if (drag_active) {
		coord::camhud s = start.to_window().to_camhud();
		coord::camhud e = end.to_window().to_camhud();
		glLineWidth(1);
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINE_LOOP); {
			glVertex3f(s.x, s.y, 0);
			glVertex3f(e.x, s.y, 0);
			glVertex3f(e.x, e.y, 0);
			glVertex3f(s.x, e.y, 0);
		}
		glEnd();
	}

	// draw hp bars for each selected unit
	glLineWidth(3);
	for (auto u : this->units) {
		if (u.second.is_valid()) {
			Unit *unit_ptr = u.second.get();
			if (unit_ptr->location && unit_ptr->has_attribute(attr_type::hitpoints)) {
				auto &hp = unit_ptr->get_attribute<attr_type::hitpoints>();
				float percent = static_cast<float>(hp.current) / static_cast<float>(hp.max);
				int mid = percent * 28.0f - 14.0f;

				coord::phys3 &pos_phys3 = unit_ptr->location->pos.draw;
				coord::camhud pos = pos_phys3.to_camgame().to_window().to_camhud();
				glColor3f(0.0, 1.0, 0.0);
				glBegin(GL_LINES); {
					glVertex3f(pos.x - 14, pos.y + 60, 0);
					glVertex3f(pos.x + mid, pos.y + 60, 0);
				}
				glEnd();
				glColor3f(1.0, 0.0, 0.0);
				glBegin(GL_LINES); {
					glVertex3f(pos.x + mid, pos.y + 60, 0);
					glVertex3f(pos.x + 14, pos.y + 60, 0);
				}
				glEnd();
			}
		}
	}
	glColor3f(1.0, 1.0, 1.0); // reset

	// display details of single selected unit
	if (this->units.size() == 1) {
		auto &ref = this->units.begin()->second;
		if (ref.is_valid()) {
			this->show_attributes(ref.get());
		}
	}

	// ui graphics 3404 and 3405
	return true;
}

void UnitSelection::drag_begin(coord::camgame pos) {
	this->start = pos;
	this->end = pos;
	this->drag_active = true;
}

void UnitSelection::drag_update(coord::camgame pos) {
	if (!this->drag_active) {
		this->drag_begin(pos);
	}
	this->end = pos;
}

void UnitSelection::drag_release(const Player &player, Terrain *terrain, bool append) {
	if (this->start == this->end) {
		this->select_point(player, terrain, this->start, append);
	}
	else {
		this->select_space(player, terrain, this->start, this->end, append);
	}
	this->drag_active = false;
}

void UnitSelection::clear() {
	for (auto u : this->units) {
		if (u.second.is_valid()) {
			u.second.get()->selected = false;
		}
	}
	this->units.clear();
	this->selection_type = selection_type_t::nothing;
}

void UnitSelection::toggle_unit(const Player &player, Unit *u, bool append) {
	if (this->units.count(u->id) == 0) {
		this->add_unit(player, u, append);
	} else {
		this->remove_unit(u);
	}
}

void UnitSelection::add_unit(const Player &player, Unit *u, bool append) {
	// Only select resources and units with hitpoints > 0
	if (u->has_attribute(attr_type::resource) ||
	   (u->has_attribute(attr_type::hitpoints) && u->get_attribute<attr_type::hitpoints>().current > 0)) {

		selection_type_t unit_type = get_unit_selection_type(player, u);
		int unit_type_i = static_cast<int>(unit_type);
		int selection_type_i = static_cast<int>(this->selection_type);

		if (unit_type_i > selection_type_i) {
			// Don't select this unit as it has too low priority
			return;
		}

		if (unit_type_i < selection_type_i) {
			// Upgrade selection to a higher priority selection
			this->clear();
			this->selection_type = unit_type;
		}

		// Can't select multiple enemies at once
		if (not (unit_type == selection_type_t::own_units ||
		        (unit_type == selection_type_t::own_buildings && append))) {

			this->clear();
			this->selection_type = unit_type; // Clear resets selection_type
		}

		// Finally, add the unit to the selection
		u->selected = true;
		this->units[u->id] = u->get_ref();
	}
}

void UnitSelection::remove_unit(Unit *u) {
	u->selected = false;
	this->units.erase(u->id);

	if (this->units.empty()) {
		this->selection_type = selection_type_t::nothing;
	}
}

void UnitSelection::kill_unit(const Player &player) {
	if (this->units.empty()) {
		return;
	}

	UnitReference &ref = this->units.begin()->second;
	if (!ref.is_valid()) {
		this->units.erase(this->units.begin());

		if (this->units.empty()) {
			this->selection_type = selection_type_t::nothing;
		}
	} else {
		Unit *u = ref.get();

		// Check color: you can only kill your own units
		if (u->is_own_unit(player)) {
			this->remove_unit(u);
			u->delete_unit();
		}
	}
}

bool UnitSelection::contains_builders(const Player &player) {
	for (auto &it : units) {
		if (it.second.is_valid() &&
		    it.second.get()->get_ability(ability_type::build) &&
		    it.second.get()->is_own_unit(player)) {
			return true;
		}
	}
	return false;
}

void UnitSelection::select_point(const Player &player, Terrain *terrain, coord::camgame p, bool append) {
	if (!append) {
		this->clear();
	}

	if (!terrain) {
		log::log(MSG(warn) << "selection terrain not specified");
		return;
	}

	// find any object at selected point
	auto obj = terrain->obj_at_point(p.to_phys3());
	if (obj) {
		this->toggle_unit(player, &obj->unit);
	}
}

void UnitSelection::select_space(const Player &player, Terrain *terrain, coord::camgame p1, coord::camgame p2, bool append) {
	if (!append) {
		this->clear();
	}

	coord::camgame min;
	coord::camgame max;
	min.x = std::min(p1.x, p2.x);
	min.y = std::min(p1.y, p2.y);
	max.x = std::max(p1.x, p2.x);
	max.y = std::max(p1.y, p2.y);

	// look at each tile in the range and find all units
	for (coord::tile check_pos : tiles_in_range(p1, p2)) {
		TileContent *tc = terrain->get_data(check_pos);
		if (tc) {
			// find objects within selection box
			for (auto unit_location : tc->obj) {
				coord::camgame pos = unit_location->pos.draw.to_camgame();
				if ((min.x < pos.x && pos.x < max.x) &&
				     (min.y < pos.y && pos.y < max.y)) {
					this->add_unit(player, &unit_location->unit, append);
				}
			}
		}
	}
}

void UnitSelection::all_invoke(Command &cmd) {
	for (auto u : this->units) {
		if (u.second.is_valid() && u.second.get()->is_own_unit(cmd.player)) {

			// allow unit to find best use of the command
			// TODO: queue_cmd returns ability which allows playing of sound
			u.second.get()->queue_cmd(cmd);
		}
	}
}

void UnitSelection::show_attributes(Unit *u) {
	std::vector<std::string> lines;
	lines.push_back(u->top()->name());
	lines.push_back("type: "+std::to_string(u->unit_type->id()));

	if (u->has_attribute(attr_type::owner)) {
		auto &own_attr = u->get_attribute<attr_type::owner>();
		lines.push_back(own_attr.player.name);
	}
	if (u->has_attribute(attr_type::hitpoints)) {
		auto &hp_attr = u->get_attribute<attr_type::hitpoints>();
		lines.push_back("hitpoints: "+std::to_string(hp_attr.current)+"/"+std::to_string(hp_attr.max));
	}
	if (u->has_attribute(attr_type::resource)) {
		auto &res_attr = u->get_attribute<attr_type::resource>();
		lines.push_back("resource: "+std::to_string(res_attr.amount));
	}
	if (u->has_attribute(attr_type::building)) {
		auto &build_attr = u->get_attribute<attr_type::building>();
		lines.push_back("built: "+std::to_string(build_attr.completed));
	}
	if (u->has_attribute(attr_type::garrison)) {
		auto &garrison_attr = u->get_attribute<attr_type::garrison>();
		lines.push_back("garrison: "+std::to_string(garrison_attr.content.size()));
	}

	// render text
	Engine &engine = Engine::get();
	int vpos = 160;
	engine.render_text({0, vpos}, 20, "%s", u->unit_type->name().c_str());
	for (auto &s : lines) {
		vpos -= this->font_size;
		engine.render_text({0, vpos}, this->font_size, "%s", s.c_str());
	}
}

selection_type_t UnitSelection::get_unit_selection_type(const Player &player, Unit *u) {
	bool is_building = u->has_attribute(attr_type::building);

	// Check color
	if (u->is_own_unit(player)) {
		return is_building ? selection_type_t::own_buildings : selection_type_t::own_units;
	} else {
		return is_building ? selection_type_t::enemy_building : selection_type_t::enemy_unit;
	}
}

std::vector<coord::tile> tiles_in_range(coord::camgame p1, coord::camgame p2) {
	// the remaining corners
	coord::camgame p3 = coord::camgame{p1.x, p2.y};
	coord::camgame p4 = coord::camgame{p2.x, p1.y};
	coord::camgame pts[4] {p1, p2, p3, p4};

	// find the range of tiles covered
	coord::tile t1 = pts[0].to_phys3().to_tile3().to_tile();
	coord::tile min = t1;
	coord::tile max = t1;
	for (unsigned i = 1; i < 4; ++i) {
		coord::tile t = pts[i].to_phys3().to_tile3().to_tile();
		min.ne = std::min(min.ne, t.ne);
		min.se = std::min(min.se, t.se);
		max.ne = std::max(max.ne, t.ne);
		max.se = std::max(max.se, t.se);
	}

	// find all units in the boxed region
	std::vector<coord::tile> tiles;
	coord::tile check_pos = min;
	while (check_pos.ne <= max.ne) {
		while (check_pos.se <= max.se) {
			tiles.push_back(check_pos);
			check_pos.se += 1;
		}
		check_pos.se = min.se;
		check_pos.ne += 1;
	}
	return tiles;
}

} /* namespace openage */
