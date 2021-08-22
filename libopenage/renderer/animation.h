// Copyright 2021-2021 the openage authors. See copying.md for legal info.

#pragma once

#include <vector>

#include "resources/animation_info.h"

namespace openage::renderer {

class Animation2d {
public:
	Animation2d() = default;
	~Animation2d() = default;

	/**
	 * Get the animation information.
	 *
	 * @return Animation information.
	 */
	const resources::Animation2dInfo &get_info() const;

protected:
	/**
	 * Creates a 2D animation.
	 */
	Animation2d(const resources::Animation2dInfo &info);

	/**
	 * Information about the animation layers, angles and frames.
	 */
	resources::Animation2dInfo info;
};

} // namespace openage::renderer
