// Copyright 2022-2024 the openage authors. See copying.md for legal info.

#include "render_entity.h"

#include <functional>
#include <mutex>

#include "renderer/definitions.h"


namespace openage::renderer::world {

RenderEntity::RenderEntity() :
	renderer::RenderEntity{},
	ref_id{0},
	position{nullptr, 0, "", nullptr, SCENE_ORIGIN},
	angle{nullptr, 0, "", nullptr, 0},
	animation_path{nullptr, 0} {
}

void RenderEntity::update(const uint32_t ref_id,
                          const curve::Continuous<coord::phys3> &position,
                          const curve::Segmented<coord::phys_angle_t> &angle,
                          const std::string animation_path,
                          const time::time_t time) {
	std::unique_lock lock{this->mutex};

	// Sync the data curves using the earliest time of last update and time
	auto sync_time = std::min(this->last_update, time);

	this->ref_id = ref_id;
	std::function<coord::scene3(const coord::phys3 &)> to_scene3 = [](const coord::phys3 &pos) {
		return pos.to_scene3();
	};
	this->position.sync(position,
	                    std::function<coord::scene3(const coord::phys3 &)>([](const coord::phys3 &pos) {
							return pos.to_scene3();
						}),
	                    sync_time);
	this->angle.sync(angle, sync_time);
	this->animation_path.set_last(time, animation_path);

	// Record time of last update
	this->last_update = time;

	// Record when the render update should fetch data from the entity
	this->fetch_time = std::min(this->fetch_time, time);

	this->changed = true;
}

void RenderEntity::update(const uint32_t ref_id,
                          const coord::phys3 position,
                          const std::string animation_path,
                          const time::time_t time) {
	std::unique_lock lock{this->mutex};

	this->ref_id = ref_id;
	this->position.set_last(time, position.to_scene3());
	this->animation_path.set_last(time, animation_path);

	this->last_update = time;
	this->fetch_time = std::min(this->fetch_time, time);

	this->changed = true;
}

uint32_t RenderEntity::get_id() {
	return this->ref_id;
}

const curve::Continuous<coord::scene3> &RenderEntity::get_position() {
	return this->position;
}

const curve::Segmented<coord::phys_angle_t> &RenderEntity::get_angle() {
	return this->angle;
}

const curve::Discrete<std::string> &RenderEntity::get_animation_path() {
	return this->animation_path;
}

} // namespace openage::renderer::world
