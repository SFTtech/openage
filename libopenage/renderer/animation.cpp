// Copyright 2021-2021 the openage authors. See copying.md for legal info.

#include "animation.h"

namespace openage::renderer {

Animation2d::Animation2d(const resources::Animation2dInfo &info) :
	info{info} {}

const resources::Animation2dInfo &Animation2d::get_info() const {
	return this->info;
}

} // namespace openage::renderer
