// Copyright 2022-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cmath>
#include <cstddef>
#include <memory>

#include <eigen3/Eigen/Dense>

#include "coord/pixel.h"
#include "coord/scene.h"
#include "util/vector.h"

#include "frustum.h"

namespace openage::renderer {
class Renderer;
class UniformBuffer;

namespace camera {

/**
 * Camera direction (= where it looks at).
 * Uses a dimetric perspective like in AoE with the (fixed) angles
 *    yaw   = -135 degrees
 *    pitch = -30 degrees
 */
static const Eigen::Vector3f cam_direction{
	-1 * (std::sqrt(6.f) / 4),
	-0.5f,
	-1 * (std::sqrt(6.f) / 4),
};

static const float near_distance = 0.01f;
static const float far_distance = 100.0f;

/**
 * Camera for selecting what part of the ingame world is displayed.
 *
 * TODO: Vulkan version.
 */
class Camera {
public:
	/**
	 * Create a new camera for the renderer.
	 *
	 * The camera uses default values. Its centered on the origin of the scene (0.0f, 0.0f, 0.0f)
	 * and has a zoom level of 1.0f.
	 *
	 * @param viewport_size Initial viewport size of the camera (width x height).
	 */
	Camera(const std::shared_ptr<Renderer> &renderer,
	       util::Vector2s viewport_size);

	/**
	 * Create a new camera for the renderer.
	 *
	 * @param viewport_size Viewport size of the camera (width x height).
	 * @param scene_pos Position of the camera in the scene.
	 * @param zoom Zoom level of the camera (defaults to 1.0f).
	 * @param max_zoom_out Maximum zoom out level (defaults to 64.0f).
	 * @param default_zoom_ratio Default zoom level calibration (defaults to (1.0f / 49)).
	 * @param frustum_culling Is frustum culling enables (defaults to false).
	 */
	Camera(const std::shared_ptr<Renderer> &renderer,
	       util::Vector2s viewport_size,
	       Eigen::Vector3f scene_pos,
	       float zoom = 1.0f,
	       float max_zoom_out = 64.0f,
	       float default_zoom_ratio = 1.0f / 49,
		   bool frustum_culling = false);
	~Camera() = default;

	/**
	 * Move the camera so that the center of its viewpoint points
	 * to the given position in the 3D scene.
	 *
	 * @param scene_pos Position in the 3D scene that the camera should center on.
	 */
	void look_at_scene(Eigen::Vector3f scene_pos);

	/**
	 * Move the camera so that the center of its viewpoint points
	 * to the given ingame coordinates.
	 *
	 * @param scene_pos Position of the ingame coordinates that the camera should center on.
	 */
	void look_at_coord(coord::scene3 coord_pos);

	/**
	 * Move the camera position in the direction of a given vector.
	 *
	 * @param scene_pos New 3D position of the camera in the scene.
	 */
	void move_to(Eigen::Vector3f scene_pos);

	/**
	 * Move the camera position in the direction of a given vector.
	 *
	 * @param direction Direction vector. Added to the current position.
	 * @param delta Delta for controlling the amount by which the camera is moved. The
	 *              value is multiplied with the directional vector before its applied to
	 *              the positional vector.
	 */
	void move_rel(Eigen::Vector3f direction, float delta = 1.0f);

	/**
	 * Set the zoom level of the camera. Values smaller than 1.0f let the
	 * camera zoom in, values greater than 1.0f let the camera zoom out.
	 *
	 * The max zoom in value is 0.05f. Passing values lower than that
	 * will just set the zoom level to max zoom.
	 *
	 * For incremental zooming, use the \p zoom_in() and \p zoom_out()
	 * methods.
	 *
	 * @param zoom New zoom level.
	 */
	void set_zoom(float zoom);

	/**
	 * Zoom into the scene.
	 *
	 * Decreases the current zoom level.
	 *
	 * @param zoom_delta How far the camera should zoom in.
	 */
	void zoom_in(float zoom_delta);

	/**
	 * Zoom out of the scene.
	 *
	 * Increases the current zoom level.
	 *
	 * @param zoom_delta How far the camera should zoom out.
	 */
	void zoom_out(float zoom_delta);

	/**
	 * Resize the camera viewport.
	 *
	 * @param width New width of the viewport.
	 * @param height New height of the viewport.
	 */
	void resize(size_t width, size_t height);

	/**
	 * Get the current zoom level of the camera.
	 *
	 * @return Zoom level.
	 */
	float get_zoom() const;

	/**
	 * Get the view matrix for this camera.
	 *
	 * @return Camera view matrix.
	 */
	const Eigen::Matrix4f &get_view_matrix();

	/**
	 * Get the projection matrix for this camera.
	 *
	 * @return Camera projection matrix.
	 */
	const Eigen::Matrix4f &get_projection_matrix();

	/**
	 * Get the size of the camera viewport.
	 *
	 * @return Viewport size as a 2D vector (x, y).
	 */
	const util::Vector2s &get_viewport_size() const;

	/**
	 * Get the corresponding 3D position of a 2D input coordinate in the viewport.
	 * The position is on the plane created by the camera's orthographic projection.
	 *
	 * This may be used to get the 3D position of a mouse click and subsequent
	 * ray casting calculations.
	 *
	 * @param coord 2D input coordinate in the viewport.
	 *
	 * @return Position of the input in the 3D scene.
	 */
	Eigen::Vector3f get_input_pos(const coord::input &coord) const;

	/**
	 * Get the uniform buffer for this camera.
	 *
	 * @return Uniform buffer.
	 */
	const std::shared_ptr<renderer::UniformBuffer> &get_uniform_buffer() const;

	bool using_frustum_culling() const;

	bool is_in_frustum(Eigen::Vector3f pos) const;

private:
	/**
	 * Position in the 3D scene.
	 */
	Eigen::Vector3f scene_pos;

	/**
	 * Size of the camera viewport.
	 */
	util::Vector2s viewport_size;

	/**
	 * Zoom level.
	 *
	 * 0.0f < z < 1.0f => zoom in
	 * z = 1.0f        => default view
	 * z > 1.0f        => zoom out
	 */
	float zoom;

	/**
	 * Maximum possible zoom in level.
	 *
	 * Has to be above 0.0f, otherwise we get zero division errors.
	 */
	static constexpr float MAX_ZOOM_IN = 0.005f;

	/**
	 * Maximum possible zoom out level.
	 *
	 * This can be set per camera.
	 */
	float max_zoom_out;

	/**
	 * Modifier that controls what the default zoom level (zoom = 1.0f)
	 * looks like. Essentially, this value is also a zoom that is pre-applied
	 * before other calculations.
	 *
	 * This value is important for calibrating the default zoom to match
	 * the pixel size of assets. For example, terrain in AoE2 has a height
	 * of 49 pixels, while in the renderer scene the height is always 1.0.
	 * Setting \p default_zoom_ratio to 1 / 49 ensure that the default zoom
	 * level matches the pixel ration of the original game.
	 */
	float default_zoom_ratio;

	/**
	 * Flag set when the camera is moved.
	 *
	 * If true, the view matrix needs to be recalculated.
	 */
	bool moved;

	/**
	 * Flag set when the camera zoom is changed.
	 *
	 * If true, the projection matrix needs to be recalculated.
	 */
	bool zoom_changed;

	/**
	 * Flag set when the camera viewport is resized.
	 *
	 * If true, the projection matrix needs to be recalculated.
	 */
	bool viewport_changed;

	/**
	 * Current view matrix for the camera.
	 *
	 * Cached because it may be requested many times.
	 */
	Eigen::Matrix4f view;

	/**
	 * Current projection matrix for the camera.
	 *
	 * Cached because it may be requested many times.
	 */
	Eigen::Matrix4f proj;

	/**
	 * Uniform buffer for the camera matrices.
	 */
	std::shared_ptr<renderer::UniformBuffer> uniform_buffer;

	/**
	 * Is frustum culling enabled? If true, perform frustum culling.
	 * If false, all frustum checks will return true
	 */
	bool frustum_culling;

	/**
	 * The frustum used to cull objects
	 * Will be recalculated regardless of whether frustum culling is enabled
	 */
	Frustum frustum;
};

} // namespace camera
} // namespace openage::renderer
