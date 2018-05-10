// Copyright 2014-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <string>

#include "format.h"
#include "types.h"
#include "../util/path.h"


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
	util::Path path;

public:
	/**
	 * Initializes a new DynamicLoader.
	 * @param path the resource's location in the filesystem
	 */
	DynamicLoader(const util::Path &path);
	virtual ~DynamicLoader() = default;

	/**
	 * Loads a chunk of stereo pcm data from the resource. The chunk of data
	 * begins at the given offset from the beginning of the resource. The actual
	 * read number of int16_t values is returned.
	 * TODO: Returns zero if the end of resource is reached.
	 *
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
	static std::unique_ptr<DynamicLoader> create(const util::Path &path,
	                                             format_t format);
};

}} // openage::audio
