// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "drag_select.h"

#include "coord/phys.h"
#include "curve/discrete.h"
#include "gamestate/component/internal/ownership.h"
#include "gamestate/component/internal/position.h"
#include "gamestate/game_entity.h"
#include "gamestate/game_state.h"
#include "gamestate/types.h"


namespace openage::gamestate::event {

DragSelectHandler::DragSelectHandler() :
	OnceEventHandler{"game.drag_select"} {}

void DragSelectHandler::setup_event(const std::shared_ptr<openage::event::Event> & /* event */,
                                    const std::shared_ptr<openage::event::State> & /* state */) {
	// TODO
}

void DragSelectHandler::invoke(openage::event::EventLoop & /* loop */,
                               const std::shared_ptr<openage::event::EventEntity> & /* target */,
                               const std::shared_ptr<openage::event::State> &state,
                               const time::time_t &time,
                               const param_map &params) {
	auto gstate = std::dynamic_pointer_cast<openage::gamestate::GameState>(state);

	size_t controlled_id = params.get("controlled", 0);

	// Start and end of the drag select rectangle
	coord::phys3 start = params.get("drag_start", coord::phys3{0, 0, 0});
	coord::phys3 end = params.get("drag_end", coord::phys3{0, 0, 0});
	coord::phys3 corner0 = params.get("drag_corner0", coord::phys3{0, 0, 0});
	coord::phys3 corner1 = params.get("drag_corner1", coord::phys3{0, 0, 0});

	// Check which coordinate is the top left and which is the bottom right
	coord::phys3 top_left = coord::phys3{0, 0, 0};
	coord::phys3 bottom_right = coord::phys3{0, 0, 0};
	if (start.se < end.se) {
		top_left = start;
		bottom_right = end;
	}
	else {
		top_left = end;
		bottom_right = start;
	}

	// Get the other two corners of the rectangle
	coord::phys3 top_right = coord::phys3{0, 0, 0};
	coord::phys3 bottom_left = coord::phys3{0, 0, 0};
	if (corner0.ne < corner1.ne) {
		top_right = corner1;
		bottom_left = corner0;
	}
	else {
		top_right = corner0;
		bottom_left = corner1;
	}

	log::log(DBG << "Drag select rectangle:");
	log::log(DBG << "\tTop left: " << top_left);
	log::log(DBG << "\tTop right: " << top_right);
	log::log(DBG << "\tBottom left: " << bottom_left);
	log::log(DBG << "\tBottom right: " << bottom_right);

	// Check which entities are in the rectangle
	std::vector<entity_id_t> selected;
	for (auto entity : gstate->get_game_entities()) {
		auto owner = std::dynamic_pointer_cast<component::Ownership>(
			entity.second->get_component(component::component_t::OWNERSHIP));
		if (owner->get_owners().get(time) != controlled_id) {
			// only select entities of the controlled player
			continue;
		}

		auto pos = std::dynamic_pointer_cast<component::Position>(
			entity.second->get_component(component::component_t::POSITION));
		auto current_pos = pos->get_positions().get(time);
		if (current_pos.ne <= top_right.ne
		    and current_pos.ne >= bottom_left.ne
		    and current_pos.se <= bottom_right.se
		    and current_pos.se >= top_left.se) {
			// check if the entity is in the rectangle
			selected.push_back(entity.first);
		}
	}

	// Select the units
	auto select_cb = params.get("select_cb",
	                            std::function<void(const std::vector<entity_id_t> ids)>{
									[](const std::vector<entity_id_t> /* ids */) {}});
	select_cb(selected);
}

time::time_t DragSelectHandler::predict_invoke_time(const std::shared_ptr<openage::event::EventEntity> & /* target */,
                                                    const std::shared_ptr<openage::event::State> & /* state */,
                                                    const time::time_t &at) {
	return at;
}


} // namespace openage::gamestate::event
