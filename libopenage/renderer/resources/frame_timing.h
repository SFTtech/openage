// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <vector>

#include "time/time.h"


namespace openage::renderer::resources {

/**
 * Storage for the timing of a sequence of frames.
 *
 * Optimized for fast read access.
 */
class FrameTiming {
public:
	using keyframe_t = time::time_t;

	/**
	 * Create a new frame timing sequence.
	 *
	 * @param total_length Total length of the sequence (in seconds).
	 * @param keyframes Time of each frame (in seconds). Expected to be sorted.
	 */
	FrameTiming(const time::time_t &total_length,
	            const std::vector<keyframe_t> &&keyframes);

	~FrameTiming() = default;

	/**
	 * Set the total length of the frame timing sequence.
	 *
	 * @param total_length Total length of the sequence (in seconds).
	 */
	void set_total_length(const time::time_t &total_length);

	/**
	 * Insert a new keyframe into the sequence.
	 *
	 * Insertion complexity is linear (O(n)), so it should be avoided during rendering
	 * when possible. Prefer to create several alternative sequence instead if the
	 * timing needs to be changed.
	 *
	 * @param time Time of the new keyframe (in seconds).
	 */
	void insert(const time::time_t &time);

	/**
	 * Get the index of the frame in the sequence that should be displayed at the
	 * given time (closest frame with t <= time).
	 *
	 * @param time Time in the sequence (in seconds).
	 * @return Frame index in the sequence.
	 */
	size_t get_frame(const time::time_t &time) const;

	/**
	 * Get the index of the frame in the sequence that should be displayed at a specified
	 * time. This method is intended for looping frame sequences.
	 *
	 * Sequence time is determined from the modulus of the time difference between
	 * \p current (current simulation time) and \p start (time when the sequence
	 * was first started):
	 *
	 * time = (current - start) % total_length
	 *
	 * @param current Current simulation time (in seconds).
	 * @param start Start time of the sequence (in seconds).
	 * @return Frame index in the sequence.
	 */
	size_t get_frame(const time::time_t &current, const time::time_t &start) const;

	/**
	 * Get the number of frames in the sequence.
	 *
	 * @return Number of frames.
	 */
	size_t size() const;

private:
	/**
	 * Search for the closest frame index with t <= time.
	 *
	 * Uses binary search to find the frame index. Complexity is therefore
	 * O(log_2(this->keyframes.size())).
	 *
	 * @param time Time in the sequence (in seconds).
	 * @return Frame index in the sequence.
	 */
	size_t search_frame(const time::time_t &time) const;

	/**
	 * Time of each frame in the sequence relative to the sequence start (in seconds).
	 */
	std::vector<keyframe_t> keyframes;

	/**
	 * Total length of the sequence (in seconds).
	 */
	time::time_t total_length;
};

} // namespace openage::renderer::resources
