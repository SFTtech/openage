// Copyright 2021-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <memory>
#include <vector>

namespace openage::renderer::resources {

class AngleInfo;
class FrameTiming;

enum class display_mode {
	OFF,  // Only show first frame
	ONCE, // Play loop once
	LOOP  // Loop indefinitely
};

/**
 * Contains information about a 2D animation layer. The layer defines the display
 * mode, overall position and timing of frames. It also contains angle
 * definitions for choosing frames based on a game object's rotation.
 */
class LayerInfo {
public:
	/**
	 * Create a 2D Layer Info.
	 *
	 * @param mode Determines how often the layer loops its frames.
	 * @param position Absolute position of the layer on the screen.
	 * @param time_per_frame Time that each frame is displayed in seconds.
	 * @param replay_delay Additional time (in seconds) to display the last frame in a loop.
	 * @param angles Angle information. Angles should be ordered clockwise, starting with the front-facing angle.
	 */
	LayerInfo(std::vector<std::shared_ptr<AngleInfo>> &angles,
	          const display_mode mode = display_mode::OFF,
	          const size_t position = 0,
	          const float time_per_frame = 0.0,
	          const float replay_delay = 0.0);

	~LayerInfo() = default;

	/**
	 * Get the display mode of the layer.
	 *
	 * @return A display mode setting.
	 */
	display_mode get_display_mode() const;

	/**
	 * Get the absolute position of the layer.
	 *
	 * @return Position of the layer.
	 */
	size_t get_position() const;

	/**
	 * Get the time that each frame is displayed.
	 *
	 * @return Time each frame is displayed in seconds.
	 */
	float get_time_per_frame() const;

	/**
	 * Get the additional display tme for the last frame in the loop.
	 *
	 * @return Additional time to display the last frame in seconds.
	 */
	float get_replay_delay() const;

	/**
	 * Get number of angles in the layer.
	 *
	 * @return Number of angles.
	 */
	size_t get_angle_count() const;

	/**
	 * Get the angle information of the angle with the specified index.
	 *
	 * @param idx Index of the angle.
	 *
	 * @return An angle information object.
	 */
	const std::shared_ptr<AngleInfo> &get_angle(size_t idx) const;

	/**
	 * Get the angle information of the angle matching the specified direction.
	 *
	 * @param direction Direction in degrees.
	 *
	 * @return An angle information object.
	 */
	const std::shared_ptr<AngleInfo> &get_direction_angle(float direction) const;

	/**
	 * Get the frame timing information of this layer.
	 *
	 * @return Frame timing information.
	 */
	const std::shared_ptr<FrameTiming> &get_frame_timing() const;

private:
	/**
	 * Display mode of the contained frames.
	 */
	display_mode mode;

	/**
	 * Absolute position of the layer.
	 */
	size_t position;

	/**
	 * Time each frame is displayed (in seconds).
	 */
	float time_per_frame;

	/**
	 * Time the last frame in a loop is additionally displayed (in seconds).
	 */
	float replay_delay;

	/**
	 * Angle information.
	 */
	std::vector<std::shared_ptr<AngleInfo>> angles;

	/**
	 * Frame timing in the animated sequence.
	 */
	std::shared_ptr<FrameTiming> frame_timing;
};

} // namespace openage::renderer::resources
