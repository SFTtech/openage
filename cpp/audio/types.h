// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_AUDIO_TYPES_H_
#define OPENAGE_AUDIO_TYPES_H_

#include <functional>
#include <memory>
#include <vector>

#include <opus/opusfile.h>

namespace openage {
namespace audio {

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

/**
 * opus_file_t is a OggOpusFile pointer that is stored inside a unique_ptr and
 * uses a custom deleter.
 */
using opus_file_t = std::unique_ptr<OggOpusFile, std::function<void(OggOpusFile*)>>;

}
}

#endif
