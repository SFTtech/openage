// Copyright 2016-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <eigen3/Eigen/Dense>

#include "coord_xy.gen.h"
#include "declarations.h"

namespace openage {

namespace renderer::camera {
class Camera;
}

namespace coord {


/*
 * Pixel-aligned coordinate systems.
 * See doc/code/coordinate-systems.md for more information.
 */

struct camhud_delta : CoordXYRelative<pixel_t, camhud, camhud_delta> {
	using CoordXYRelative<pixel_t, camhud, camhud_delta>::CoordXYRelative;

	// coordinate conversions
	viewport_delta to_viewport() const;
};


struct camhud : CoordXYAbsolute<pixel_t, camhud, camhud_delta> {
	using CoordXYAbsolute<pixel_t, camhud, camhud_delta>::CoordXYAbsolute;

	// coordinate conversions
	viewport to_viewport() const;
};


struct viewport_delta : CoordXYRelative<pixel_t, viewport, viewport_delta> {
	using CoordXYRelative<pixel_t, viewport, viewport_delta>::CoordXYRelative;

	// coordinate conversions
	camhud_delta to_camhud() const {
		return camhud_delta{this->x, this->y};
	}
};


struct viewport : CoordXYAbsolute<pixel_t, viewport, viewport_delta> {
	using CoordXYAbsolute<pixel_t, viewport, viewport_delta>::CoordXYAbsolute;

	// coordinate conversions
	camhud to_camhud() const;

	// renderer conversions
	Eigen::Vector2f to_ndc_space(const std::shared_ptr<renderer::camera::Camera> &camera) const;
};


struct input_delta : CoordXYRelative<pixel_t, input, input_delta> {
	using CoordXYRelative<pixel_t, input, input_delta>::CoordXYRelative;

	// coordinate conversions
	viewport_delta to_viewport(const std::shared_ptr<renderer::camera::Camera> &camera) const;
};


struct input : CoordXYAbsolute<pixel_t, input, input_delta> {
	using CoordXYAbsolute<pixel_t, input, input_delta>::CoordXYAbsolute;

	// coordinate conversions
	viewport to_viewport(const std::shared_ptr<renderer::camera::Camera> &camera) const;
	phys3 to_phys3(const std::shared_ptr<renderer::camera::Camera> &camera) const;
	scene3 to_scene3(const std::shared_ptr<renderer::camera::Camera> &camera) const;
};


} // namespace coord
} // namespace openage
