// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <memory>
#include <vector>


namespace openage::renderer::resources {
class TerrainFrameInfo;
class FrameTiming;

enum class terrain_display_mode {
	OFF, // Only show first frame
	LOOP // Loop indefinitely
};

/**
 * Contains information about a 2D terrain layer. The layer defines the display
 * mode, overall position and timing of frames.
 */
class TerrainLayerInfo {
public:
	/**
	 * Create a 2D Terrain Layer Info.
	 *
	 * @param frames Frame information.
	 * @param mode Determines how often the layer loops its frames.
	 * @param position Absolute position of the layer on the screen.
	 * @param time_per_frame Time that each frame is displayed in seconds.
	 * @param replay_delay Additional time (in seconds) to display the last frame in a loop.
	 */
	TerrainLayerInfo(const std::vector<std::shared_ptr<TerrainFrameInfo>> &frames,
	                 const terrain_display_mode mode = terrain_display_mode::OFF,
	                 const size_t position = 0,
	                 const float time_per_frame = 0.0,
	                 const float replay_delay = 0.0);

	~TerrainLayerInfo() = default;

	/**
	 * Get the display mode of the layer.
	 *
	 * @return Display mode setting.
	 */
	terrain_display_mode get_display_mode() const;

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
	 * Get number of frames for the layer.
	 *
	 * @return Number of frames.
	 */
	size_t get_frame_count() const;

	/**
	 * Get the frame information of the frame with the specified index.
	 *
	 * @param idx Index of the frame.
	 *
	 * @return A frame information object.
	 */
	const std::shared_ptr<TerrainFrameInfo> &get_frame(size_t idx) const;

	/**
	 * Get the frame timing information of this layer.
	 *
	 * @return Curve associating time with frame indices.
	 */
	const std::shared_ptr<FrameTiming> &get_frame_timing() const;


private:
	/**
	 * Display mode of the contained frames.
	 */
	terrain_display_mode mode;

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
	 * Frame information.
	 */
	std::vector<std::shared_ptr<TerrainFrameInfo>> frames;

	/**
	 * Frame timing in the animated sequence.
	 */
	std::shared_ptr<FrameTiming> frame_timing;
};

} // namespace openage::renderer::resources
