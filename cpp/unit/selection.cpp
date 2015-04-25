// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include <cmath>

#include "../coord/tile.h"
#include "../coord/tile3.h"
#include "../terrain/terrain.h"
#include "action.h"
#include "producer.h"
#include "selection.h"
#include "unit.h"

namespace openage {

UnitSelection::UnitSelection()
	:
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
}

void UnitSelection::toggle_unit(Unit *u) {
	if (this->units.count(u->id) > 0) {
		u->selected = false;
		this->units.erase(u->id);
	}
	else if (u->has_attribute(attr_type::hitpoints) && u->get_attribute<attr_type::hitpoints>().current > 0){
		u->selected = true;
		this->units[u->id] = u->get_ref();
	}
}

void UnitSelection::kill_unit() {
	if (!this->units.empty()) {
		auto it = this->units.begin();
		it->second.get()->delete_unit();
		this->units.erase(it);
	}
}

void UnitSelection::select_point(Terrain *terrain, coord::camgame p, bool append) {
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
		this->toggle_unit(&obj->unit);
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
			for (auto unit_location : tc->obj) {
				coord::camgame pos = unit_location->pos.draw.to_camgame();
				if ((min.x < pos.x && pos.x < max.x) &&
				     (min.y < pos.y && pos.y < max.y)) {
					boxed_units.insert(&unit_location->unit);
				}
			}
		}
	}

	// toggle each unit in the box
	for (Unit *u : boxed_units) {
		this->toggle_unit(u);
	}
}

void UnitSelection::all_invoke(Command &cmd) {
	for (auto u : this->units) {
		if (u.second.is_valid()) {

			// allow unit to find best use of the command
			u.second.get()->invoke(cmd, true);
		}
	}
}

Player *UnitSelection::owner() {
	for (auto u : this->units) {
		if (u.second.is_valid()) {
			auto unit_ptr = u.second.get();
			if (unit_ptr->has_attribute(attr_type::owner)) {
				return &unit_ptr->get_attribute<attr_type::owner>().player;
			}
		}
	}
	return nullptr;
}

void UnitSelection::show_attributes(Unit *u) {
	std::vector<std::string> lines;
	lines.push_back(u->top()->name());

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
	int vpos = 120;
	engine.render_text({0, vpos}, 20, u->unit_type->name().c_str());
	for (auto &s : lines) {
		vpos -= this->font_size;
		engine.render_text({0, vpos}, this->font_size, s.c_str());
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
