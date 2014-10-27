#ifndef OPENAGE_ENGINE_AUDIO_TYPES_H_012E05C65438461A9E32B10D1997FC74
#define OPENAGE_ENGINE_AUDIO_TYPES_H_012E05C65438461A9E32B10D1997FC74

#include <opusfile.h>

namespace openage {
namespace audio {

/**
 * pcm_data_t is a int16_t pcm data buffer with a fixed length within a
 * unique_ptr. The buffer and it's length are stored in a tuple.
 * It is used to represent one complete audio resource's buffer.
 */
using pcm_data_t = std::tuple<std::unique_ptr<int16_t[]>,uint32_t>;

/**
 * pcm_chunk_t is a int16_t pcm data buffer with a fixed length within a
 * unique_ptr. Because the chunk size has a fixed value that is known to all
 * components that use pcm chunks, it must not be stored separately.
 */
using pcm_chunk_t = std::unique_ptr<int16_t[]>;

/**
 * opus_file_t is a OggOpusFile pointer that is stored inside a unique_ptr and
 * uses a custom deleter.
 */
using opus_file_t = std::unique_ptr<OggOpusFile,std::function<void(OggOpusFile*)>>;

}
}

#endif
