// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "drag_select.h"

#include <eigen3/Eigen/Dense>

#include "coord/phys.h"
#include "coord/pixel.h"
#include "coord/scene.h"
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

	Eigen::Matrix4f id_matrix = Eigen::Matrix4f::Identity();
	Eigen::Matrix4f cam_matrix = params.get("camera_matrix", id_matrix);
	Eigen::Vector2f drag_start = params.get("drag_start", Eigen::Vector2f{0, 0});
	Eigen::Vector2f drag_end = params.get("drag_end", Eigen::Vector2f{0, 0});

	// Boundaries of the rectangle
	float top = std::max(drag_start.y(), drag_end.y());
	float bottom = std::min(drag_start.y(), drag_end.y());
	float left = std::min(drag_start.x(), drag_end.x());
	float right = std::max(drag_start.x(), drag_end.x());

	log::log(SPAM << "Drag select rectangle (NDC):");
	log::log(SPAM << "\tTop: " << top);
	log::log(SPAM << "\tBottom: " << bottom);
	log::log(SPAM << "\tLeft: " << left);
	log::log(SPAM << "\tRight: " << right);

	std::vector<entity_id_t> selected;
	for (auto &entity : gstate->get_game_entities()) {
		if (not entity.second->has_component(component::component_t::SELECTABLE)) {
			// skip entities that are not selectable
			continue;
		}

		// Check if the entity is owned by the controlled player
		// TODO: Check this using Selectable diplomatic property
		auto owner = std::dynamic_pointer_cast<component::Ownership>(
			entity.second->get_component(component::component_t::OWNERSHIP));
		if (owner->get_owners().get(time) != controlled_id) {
			// only select entities of the controlled player
			continue;
		}

		// Get the position of the entity in the viewport
		auto pos = std::dynamic_pointer_cast<component::Position>(
			entity.second->get_component(component::component_t::POSITION));
		auto current_pos = pos->get_positions().get(time);
		auto world_pos = current_pos.to_scene3().to_world_space();
		Eigen::Vector4f clip_pos = cam_matrix * Eigen::Vector4f{world_pos.x(), world_pos.y(), world_pos.z(), 1};

		// Check if the entity is in the rectangle
		if (clip_pos.x() > left
		    and clip_pos.x() < right
		    and clip_pos.y() > bottom
		    and clip_pos.y() < top) {
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
