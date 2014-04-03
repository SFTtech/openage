#ifndef _ENGINE_AUDIO_TYPES_H_
#define _ENGINE_AUDIO_TYPES_H_

namespace engine {
namespace audio {

using pcm_data_t = std::tuple<std::unique_ptr<int16_t[]>,uint32_t>;

}
}

#endif
