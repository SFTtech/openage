// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include <cmath>

#include "../coord/tile.h"
#include "../coord/tile3.h"
#include "../terrain/terrain.h"
#include "selection.h"
#include "unit.h"

namespace openage {

UnitSelection::UnitSelection()
	:
	drag_active{false} {
	modifiers.set();
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
			if (unit_ptr->has_attribute(attr_type::hitpoints)) {
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

	// ui graphics 3404 and 3405

	return true;
}

void UnitSelection::drag_begin(coord::camgame pos) {
	this->start = pos;
	this->end = pos;
	drag_active = true;
}

void UnitSelection::drag_update(coord::camgame pos) {
	this->end = pos;
}

void UnitSelection::drag_release(Terrain *terrain, bool append) {
	if (this->start == this->end) {
		this->select_point(terrain, this->start, append);
	}
	else {
		this->select_space(terrain, this->start, this->end, append);
		drag_active = false;
	}
}

void UnitSelection::clear() {
	for (auto u : this->units) {
		if (u.second.is_valid()) {
			u.second.get()->selected = false;
		}
	}
	this->units.clear();

	// reset modifiers
	modifiers.set();
}

void UnitSelection::toggle_unit(Unit *u) {
	if (this->units.count(u->id) > 0) {
		u->selected = false;
		this->units.erase(u->id);
	}
	else {
		u->selected = true;
		this->units[u->id] = u->get_ref();
	}
}

void UnitSelection::select_point(Terrain *terrain, coord::camgame p, bool append) {
	if (!append) {
		this->clear();
	}

	// find the tile
	coord::phys3 p_phys3 = p.to_phys3();
	coord::tile p_tile = p_phys3.to_tile3().to_tile();
	TileContent *tc = terrain->get_data(p_tile);
	if (!tc || tc->obj.empty()) {
		return;
	}

	// at least one object is on the tile
	// find the one nearest to the click
	coord::phys_t nearest_dist = (1 << 16);
	Unit *nearest_unit = nullptr;
	for (auto o : tc->obj) {
		coord::camgame upos = o->pos.draw.to_camgame();
		coord::phys_t dx = p.x - upos.x;
		coord::phys_t dy = p.y - upos.y;
		coord::phys_t dist = std::hypot(dx, dy);
		if (dist < nearest_dist) { //  && this->units.count(o->unit->id) == 0
			nearest_dist = dist;
			nearest_unit = o->unit;
		}
	}

	if (nearest_unit) {
		this->toggle_unit(nearest_unit);
	}
}

void UnitSelection::select_space(Terrain *terrain, coord::camgame p1, coord::camgame p2, bool append) {
	if (!append) {
		this->clear();
	}

	coord::camgame min;
	coord::camgame max;
	min.x = std::min(p1.x, p2.x);
	min.y = std::min(p1.y, p2.y);
	max.x = std::max(p1.x, p2.x);
	max.y = std::max(p1.y, p2.y);

	// look at each tile in the range and find all unique units
	std::unordered_set<Unit *> boxed_units;
	for (coord::tile check_pos : tiles_in_range(p1, p2)) {
		TileContent *tc = terrain->get_data(check_pos);
		if (tc) {
			// find objects within selection box
			for (auto o : tc->obj) {
				coord::camgame pos = o->pos.draw.to_camgame();
				if ((min.x < pos.x && pos.x < max.x) &&
				     (min.y < pos.y && pos.y < max.y)) {
					boxed_units.insert(o->unit);
				}
			}
		}
	}

	// toggle each unit in the box
	for (Unit *u : boxed_units) {
		this->toggle_unit(u);
	}
}

void UnitSelection::all_invoke(Command &) {
	// TODO: use commands to control units
}


void UnitSelection::set_ability(ability_type ability) {
	modifiers = 0;
	modifiers[ability] = true;
}

void UnitSelection::all_invoke(ability_type ability, uint arg) {
	for (auto u : this->units) {
		if (u.second.is_valid()) {
			u.second.get()->invoke(ability, arg);
		}
	}
}

void UnitSelection::all_target(Terrain *terrain, coord::phys3 target) {
	// get object currently standing at the clicked position

	coord::tile tile = target.to_tile3().to_tile();
	TileContent *tc = terrain->get_data(tile);
	if (!tc) {
		return;
	}

	if (tc->obj.empty()) {
		for (auto u : this->units) {
			if (u.second.is_valid()) {
				u.second.get()->target(target, modifiers);
			}
		}
	}
	else {
		Unit *obj = tc->obj[0]->unit;
		for (auto u : this->units) {
			if (u.second.is_valid()) {
				// try target unit
				if (!u.second.get()->target(obj, modifiers)) {
					u.second.get()->target(target, modifiers);
				}
			}
		}
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
	log::dbg("box min %ld %ld, max %ld %ld", min.ne, min.se, max.ne, max.se);


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
