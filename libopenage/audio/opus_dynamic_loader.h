// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_AUDIO_OPUS_DYNAMIC_LOADER_H_
#define OPENAGE_AUDIO_OPUS_DYNAMIC_LOADER_H_

#include <mutex>
#include <string>

#include "dynamic_loader.h"
#include "types.h"

namespace openage {
namespace audio {

/**
 * A OpusDynamicLoader load's opus encoded data.
 */
class OpusDynamicLoader : public DynamicLoader {
private:
	/** The source file. */
	opus_file_t source;
	/** The resource's length in int16_t values. */
	size_t length;
	/** The resource's pcm channels. */
	int channels;

public:
	/**
	 * Creates a new OpusDynamicLoader.
	 * @param path the resource's location in the filesystem
	 */
	OpusDynamicLoader(const std::string &path);
	virtual ~OpusDynamicLoader() = default;

	size_t load_chunk(int16_t *chunk_buffer, size_t offset,
	                  size_t chunk_size) override;

private:
	/**
	 * Opens a opus file. Its location is specified by the path stored in the
	 * DynamicLoader.
	 */
	opus_file_t open_opus_file();
};

}
}

#endif
