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


// TODO: Remove
struct [[deprecated]] camgame_delta : CoordXYRelative<pixel_t, camgame, camgame_delta> {
	using CoordXYRelative<pixel_t, camgame, camgame_delta>::CoordXYRelative;

	/**
	 * There are infinite solutions to this conversion problem because
	 * a 2D coordinate is converted into a 3D coordinate.
	 *
	 * The user needs to manually give the 'up' value of the phys3 result.
	 */
	phys3_delta to_phys3(const CoordManager &mgr, phys_t up = phys_t::zero()) const;
	viewport_delta to_viewport() const;
};


// TODO: Remove
struct [[deprecated]] camgame : CoordXYAbsolute<pixel_t, camgame, camgame_delta> {
	using CoordXYAbsolute<pixel_t, camgame, camgame_delta>::CoordXYAbsolute;

	/**
	 * See the comments for camgame_delta::to_phys3.
	 *
	 * TODO: Once we have terrain elevation, 'up' will not mean the absolute
	 *       elevation, but instead the returned phys3 coordinate will be
	 *       the intersection between the camgame line and the 3d terrain +
	 *       up altitude.
	 */
	viewport to_viewport(const CoordManager &mgr) const;
	camhud to_camhud(const CoordManager &mgr) const;
	phys3 to_phys3(const CoordManager &mgr, phys_t up = phys_t::zero()) const;
	tile to_tile(const CoordManager &mgr, phys_t up = phys_t::zero()) const;
};


struct camhud_delta : CoordXYRelative<pixel_t, camhud, camhud_delta> {
	using CoordXYRelative<pixel_t, camhud, camhud_delta>::CoordXYRelative;

	// coordinate conversions
	viewport_delta to_viewport() const;
};


struct camhud : CoordXYAbsolute<pixel_t, camhud, camhud_delta> {
	using CoordXYAbsolute<pixel_t, camhud, camhud_delta>::CoordXYAbsolute;

	// coordinate conversions
	viewport to_viewport(const CoordManager &mgr) const;
};


struct viewport_delta : CoordXYRelative<pixel_t, viewport, viewport_delta> {
	using CoordXYRelative<pixel_t, viewport, viewport_delta>::CoordXYRelative;

	// coordinate conversions
	camhud_delta to_camhud() const {
		return camhud_delta{this->x, this->y};
	}

	// TODO: Remove
	[[deprecated]] constexpr camgame_delta to_camgame() const {
		return camgame_delta{this->x, this->y};
	}
	[[deprecated]] phys3_delta to_phys3(const CoordManager &mgr, phys_t up) const;
};


struct viewport : CoordXYAbsolute<pixel_t, viewport, viewport_delta> {
	using CoordXYAbsolute<pixel_t, viewport, viewport_delta>::CoordXYAbsolute;

	// coordinate conversions
	camhud to_camhud(const CoordManager &mgr) const;

	// renderer conversions
	Eigen::Vector2f to_ndc_space(const std::shared_ptr<renderer::camera::Camera> &camera) const;

	// TODO: Remove
	[[deprecated]] camgame to_camgame(const CoordManager &mgr) const;
	[[deprecated]] phys3 to_phys3(const CoordManager &mgr, phys_t up = phys_t::zero()) const;
	[[deprecated]] tile to_tile(const CoordManager &mgr, phys_t up = phys_t::zero()) const;
};


struct input_delta : CoordXYRelative<pixel_t, input, input_delta> {
	using CoordXYRelative<pixel_t, input, input_delta>::CoordXYRelative;

	// coordinate conversions
	viewport_delta to_viewport(const CoordManager &mgr) const;

	// TODO: Remove
	[[deprecated]] camgame_delta to_camgame(const CoordManager &mgr) const;
	[[deprecated]] phys3_delta to_phys3(const CoordManager &mgr, phys_t up = phys_t::zero()) const;
};


struct input : CoordXYAbsolute<pixel_t, input, input_delta> {
	using CoordXYAbsolute<pixel_t, input, input_delta>::CoordXYAbsolute;

	// coordinate conversions
	viewport to_viewport(const CoordManager &mgr) const;
	phys3 to_phys3(const std::shared_ptr<renderer::camera::Camera> &camera) const;
	scene3 to_scene3(const std::shared_ptr<renderer::camera::Camera> &camera) const;

	// TODO: Remove
	[[deprecated]] phys3 to_phys3(const CoordManager &mgr, phys_t up = phys_t::zero()) const;
	[[deprecated]] camgame to_camgame(const CoordManager &mgr) const;
};


} // namespace coord
} // namespace openage
