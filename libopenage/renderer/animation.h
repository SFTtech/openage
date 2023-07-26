// Copyright 2021-2023 the openage authors. See copying.md for legal info.

#pragma once

#include "renderer/resources/animation/animation_info.h"

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
