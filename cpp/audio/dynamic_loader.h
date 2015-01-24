// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_AUDIO_DYNAMIC_LOADER_H_
#define OPENAGE_AUDIO_DYNAMIC_LOADER_H_

#include <memory>
#include <string>

#include "format.h"
#include "types.h"

namespace openage {
namespace audio {

/**
 * A DynamicLoader loads pcm chunks without loading the whole resource. A chunk
 * is a int16_t buffer with a fixed size that contains 16 bit signed integer
 * pcm data.
 */
class DynamicLoader {
protected:
	/**
	 * the resource's location in the filesystem
	 */
	std::string path;

public:
	/**
	 * Initializes a new DynamicLoader.
	 * @param path the resource's location in the filesystem
	 */
	DynamicLoader(const std::string &path);
	virtual ~DynamicLoader() = default;

	/**
	 * Loads a chunk of stereo pcm data from the resource. The chunk of data
	 * begins at the given offset from the beginning of the resource. The actual
	 * read number of int16_t values is returned. TODO Returns zero if the end
	 * of resource is reached.
	 * @param chunk_buffer the buffer to save the chunk into
	 * @param offset the offset from the resource's beginning
	 * @param chunk_size the number of int16_t values that fit in one chunk
	 */
	virtual size_t load_chunk(int16_t *chunk_buffer, size_t offset,
	                          size_t chunk_size) = 0;

	/**
	 * Creates a DynamicLoader instance that supports the given format.
	 * @param path the resource's location in the filesystem
	 * @param format the resource's audio format
	 */
	static std::unique_ptr<DynamicLoader> create(const std::string &path,
	                                             format_t format);
};

}
}

#endif
