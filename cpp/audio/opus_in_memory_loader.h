// Copyright 2014-2014 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_AUDIO_OPUS_IN_MEMORY_LOADER_H_
#define OPENAGE_AUDIO_OPUS_IN_MEMORY_LOADER_H_

#include <string>

#include "in_memory_loader.h"
#include "types.h"

namespace openage {
namespace audio {

/**
 * A OpusInMemoryLoader load's opus encoded data from opus files. A opus file is
 * loaded completely at once and the loaded data will be kept in memory.
 */
class OpusInMemoryLoader : public InMemoryLoader {
public:
	/**
	 * Creates a new OpusInMemoryLoader.
	 * @param path the resource's location in the filesystem
	 */
	OpusInMemoryLoader(const std::string &path);
	virtual ~OpusInMemoryLoader() = default;

	virtual pcm_data_t get_resource();
};

}
}

#endif
