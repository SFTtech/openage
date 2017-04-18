// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include "renderable.h"

namespace openage {
namespace renderer {

std::vector<Renderable*> Renderable::RENDERABLES;
std::vector<uint16_t> Renderable::FREE_IDS;
std::mutex Renderable::ID_MUTEX;

Renderable::Renderable(UniformInput const *uniform_input, Geometry const* geometry, const bool alpha_blending, const bool depth_test,
		const bool writes_id, const function_t &on_click, const function_t &on_area_select)
	: id{this->free_id()}, unif_in{uniform_input}, geometry{geometry}, alpha_blending{alpha_blending}, depth_test{depth_test},
	  writes_id{writes_id}, on_click_callback{on_click}, on_area_selected_callback{on_area_select}
{
	std::lock_guard<std::mutex> lock(ID_MUTEX);
	RENDERABLES[this->id] = this;
}

Renderable::Renderable(const Renderable& renderable)
	: Renderable{
		renderable.unif_in, renderable.geometry, renderable.alpha_blending, renderable.depth_test, renderable.writes_id,
		renderable.on_click_callback, renderable.on_area_selected_callback
	}
{

}

Renderable& Renderable::operator=(const Renderable& renderable)
{
	this->unif_in = renderable.unif_in;
	this->geometry = renderable.geometry;
	this->alpha_blending = renderable.alpha_blending;
	this->depth_test = renderable.depth_test;
	this->writes_id = renderable.writes_id;
	this->on_click_callback = renderable.on_click_callback;
	this->on_area_selected_callback = renderable.on_area_selected_callback;

	return *this;
}

Renderable::~Renderable()
{
	std::lock_guard<std::mutex> lock(ID_MUTEX);
	RENDERABLES[this->id] = nullptr;
	FREE_IDS.push_back(this->id);
}

Renderable::id_t Renderable::free_id()
{
	std::lock_guard<std::mutex> lock(ID_MUTEX);
	if (FREE_IDS.size() > 0) {
		auto free_id = FREE_IDS.back();
		FREE_IDS.pop_back();
		return free_id;
	}

	// create new id by incrementing current highest id, and also allocationg the place for
	// the new renderable with this id in the RENDERABLES vector
	auto new_id = static_cast<id_t>(RENDERABLES.size());
	RENDERABLES.push_back(nullptr);
	return new_id;
}

Renderable* Renderable::get(const id_t id)
{
	return RENDERABLES[id];
}

}}
