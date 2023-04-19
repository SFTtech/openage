// Copyright 2016-2018 the openage authors. See copying.md for legal info.

#include "pixel.h"

#include "coord/coordmanager.h"
#include "coord/phys.h"
#include "renderer/camera/camera.h"

namespace openage {
namespace coord {


phys3_delta camgame_delta::to_phys3(const CoordManager &mgr, phys_t up) const {
	// apply scaling factor; w/2 for x, h/2 for y
	phys_t x = phys_t::from_int(this->x) / static_cast<int64_t>(mgr.tile_size.x / 2);
	phys_t y = phys_t::from_int(this->y) / static_cast<int64_t>(mgr.tile_size.y / 2);

	// apply transformation matrix to 'scaled',
	// to get the relative phys3 position
	//
	// a camgame position represents a line in the 3D phys space
	// a camgame delta of 0 might actually correlate to an arbitrarily
	// large phys delta.
	// we select one specific point on that line by explicitly specifying the
	// 'up' value of the result.
	//
	// the transformation matrix is:
	//
	//  (ne) = (+0.5 +0.5 +0.5) ( x)
	//  (se) = (+0.5 -0.5 -0.5) ( y)
	//  (up) = (+0.0 +0.0 +1.0) (up)
	phys3_delta result;
	result.ne = (x + y + up) / 2L;
	result.se = (x - y - up) / 2L;
	result.up = (up);

	return result;
}


phys3 camgame::to_phys3(const CoordManager &mgr, phys_t up) const {
	return (*this - camgame{0, 0}).to_phys3(mgr, up) + mgr.camgame_phys;
}


viewport camgame::to_viewport(const CoordManager &mgr) const {
	// reverse of viewport::to_camgame
	return (*this - camgame{0, 0}).to_viewport() + mgr.camgame_viewport;
}


phys3 viewport::to_phys3(const CoordManager &mgr, phys_t up) const {
	return this->to_camgame(mgr).to_phys3(mgr, up);
}


camhud camgame::to_camhud(const CoordManager &mgr) const {
	return this->to_viewport(mgr).to_camhud(mgr);
}


viewport_delta camgame_delta::to_viewport() const {
	return viewport_delta{this->x, this->y};
}


phys3_delta viewport_delta::to_phys3(const CoordManager &mgr, phys_t up) const {
	return this->to_camgame().to_phys3(mgr, up);
}


camgame viewport::to_camgame(const CoordManager &mgr) const {
	// mgr.camgame_viewport describes the coordinates of camgame{0, 0} in the viewport system.
	return camgame{0, 0} + (*this - mgr.camgame_viewport).to_camgame();
}


tile viewport::to_tile(const CoordManager &mgr, phys_t up) const {
	return this->to_camgame(mgr).to_tile(mgr, up);
}


tile camgame::to_tile(const CoordManager &mgr, phys_t up) const {
	return this->to_phys3(mgr, up).to_tile();
}


camhud viewport::to_camhud(const CoordManager &mgr) const {
	return camhud{0, 0} + (*this - mgr.camhud_viewport).to_camhud();
}


viewport camhud::to_viewport(const CoordManager &mgr) const {
	// reverse of viewport::to_camhud
	return (*this - camhud{0, 0}).to_viewport() + mgr.camhud_viewport;
}


viewport_delta camhud_delta::to_viewport() const {
	return viewport_delta{this->x, this->y};
}


viewport_delta input_delta::to_viewport(const CoordManager &mgr) const {
	viewport_delta result;

	result.x = this->x;
	result.y = mgr.viewport_size.y - this->y;

	return result;
}


camgame_delta input_delta::to_camgame(const CoordManager &mgr) const {
	return this->to_viewport(mgr).to_camgame();
}


phys3_delta input_delta::to_phys3(const CoordManager &mgr, phys_t up) const {
	return this->to_viewport(mgr).to_camgame().to_phys3(mgr, up);
}


viewport input::to_viewport(const CoordManager &mgr) const {
	return viewport{0, 0} + (*this - input{0, 0}).to_viewport(mgr);
}


camgame input::to_camgame(const CoordManager &mgr) const {
	return this->to_viewport(mgr).to_camgame(mgr);
}


phys3 input::to_phys3(const CoordManager &mgr, phys_t up) const {
	return this->to_viewport(mgr).to_camgame(mgr).to_phys3(mgr, up);
}

phys3 input::to_phys3(const std::shared_ptr<renderer::camera::Camera> &camera) const {
	return this->to_scene3(camera).to_phys3();
}

scene3 input::to_scene3(const std::shared_ptr<renderer::camera::Camera> &camera) const {
	// reverse transformations from world space to screen space
	auto view_m = camera->get_view_matrix();
	auto proj_m = camera->get_projection_matrix();

	int32_t viewport_size_x = camera->get_viewport_size()[0];
	int32_t viewport_size_y = camera->get_viewport_size()[1];

	// Adjust projection matrix; otherwise the ray direction is wrong (too wide)
	// TODO: why? (probably because of orthographic projection)
	// this has (presumably) nothing to do with zoom/viewport size
	proj_m(0, 0) = proj_m(0, 0) * 20;
	proj_m(1, 1) = proj_m(1, 1) * 20;

	// screen space -> normalized device coordinates
	float mouse_ndc_x = (2.0f * this->x) / viewport_size_x - 1.0f;
	float mouse_ndc_y = 1.0f - (2.0f * this->y) / viewport_size_y;
	Eigen::Vector4f mouse_ndc = Eigen::Vector4f(
		mouse_ndc_x,
		mouse_ndc_y,
		-1.0f,
		1.0f);

	// normalized device coordinates -> eye coordinates
	Eigen::Vector4f eye = proj_m.inverse() * mouse_ndc;
	eye = Eigen::Vector4f(eye.x(), eye.y(), -1.0f, 0.0f);

	// eye coordinates -> world coordinates
	Eigen::Vector4f world_ray = view_m.inverse() * eye;
	Eigen::Vector3f input_ray = Eigen::Vector3f(world_ray.x(), world_ray.y(), world_ray.z());

	// direction ray for the input coordinates from camera position
	input_ray.normalize();

	// calculate intersection with the xz plane
	auto plane_pos = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
	auto plane_dir = Eigen::Vector3f(0.0f, 1.0f, 0.0f);

	auto cam_pos = camera->get_scene_pos();

	// calculate intersection point
	Eigen::Vector3f p_intersect = cam_pos + input_ray * ((plane_pos - cam_pos).dot(plane_dir) / input_ray.dot(plane_dir));

	return scene3(-p_intersect.z(), p_intersect.x(), 0.0f);
}

} // namespace coord
} // namespace openage
