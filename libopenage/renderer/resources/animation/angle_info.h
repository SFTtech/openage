// Copyright 2021-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <memory>
#include <vector>

namespace openage::renderer::resources {

class FrameInfo;

/**
 * Describe whether/how the frame is mirrored
 */
enum class flip_type {
	NONE,   // do not flip
	FLIP_X, // flip across x axis
	FLIP_Y  // flip across y axis
};

/**
 * Contains information about a 2D animation angle. It defines the
 * angle in degrees and frame information about the frames displayed
 * at this angle.
 */
class AngleInfo {
public:
	/**
	 * Create a 2D Angle Info.
	 *
	 * @param angle_start Rotation in degress at which the frames are displayed.
	 * @param frames Frame information.
	 * @param mirror_from Mirror frames from another angle instead of using uniquely
	 *                    defined frames.
	 */
	AngleInfo(const float angle_start,
	          std::vector<std::shared_ptr<FrameInfo>> &frames,
	          std::shared_ptr<AngleInfo> mirror_from = nullptr,
	          flip_type mirror_type = flip_type::NONE);

	AngleInfo() = default;
	~AngleInfo() = default;

	/**
	 * Get the rotation in degrees at which the angle starts.
	 *
	 * @return A float containing the starting rotation in degrees.
	 */
	float get_angle_start() const;

	/**
	 * Check if the angle is mirrored from another angle.
	 *
	 * @return true if a mirrored angle has been defined, else false.
	 */
	bool is_mirrored() const;

	/**
	 * Get the mirror direction of the angle.
	 *
	 * @return const flip_type&
	 */
	const flip_type &get_mirror_type();

	/**
	 * Get the mirrored angle information.
	 *
	 * @return An angle information object.
	 */
	const std::shared_ptr<AngleInfo> &get_mirrored_angle() const;

	/**
	 * Get number of frames for the angle.
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
	const std::shared_ptr<FrameInfo> &get_frame(size_t idx) const;

private:
	/**
	 * Starting rotation in degress at which the frames are displayed.
	 */
	float angle_start;

	/**
	 * Frame information.
	 */
	std::vector<std::shared_ptr<FrameInfo>> frames;

	/**
	 * Mirrored angle information.
	 */
	std::shared_ptr<AngleInfo> mirror_from = nullptr;

	/**
	 * How the frame should be displayed if its mirrored.
	 */
	flip_type mirror_type = flip_type::NONE;
};

} // namespace openage::renderer::resources
