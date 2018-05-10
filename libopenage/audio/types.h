// Copyright 2014-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>
#include <memory>
#include <vector>


namespace openage {
namespace audio {

/**
 * A piece of raw audio data.
 *
 * special values:
 *   (nullptr, *) no data in the chunk
 *   (*, 0) end of stream
 */
struct audio_chunk_t {
	const int16_t *data;
	size_t length;
};

/**
 * pcm_data_t is a vector consisting of signed 16 bit integer samples. It is
 * used to represent one complete audio resource's buffer.
 */
using pcm_data_t = std::vector<int16_t>;

/**
 * pcm_chunk_t is a vector consisting of signed 16 bit integer samples. It is
 * used to represent a chunk of a audio resource's buffer with a fixed size.
 */
using pcm_chunk_t = std::vector<int16_t>;

}} // openage::audio
