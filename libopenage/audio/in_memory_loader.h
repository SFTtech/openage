// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_AUDIO_IN_MEMORY_LOADER_H_
#define OPENAGE_AUDIO_IN_MEMORY_LOADER_H_

#include <memory>
#include <string>
#include <tuple>

#include "format.h"
#include "types.h"

namespace openage {
namespace audio {

/**
 * A InMemoryLoader loads a audio file into memory and converts it into 16 bit
 * signed integer pcm data.
 */
class InMemoryLoader {
protected:
	/*
	 * The resource's location in the filesystem.
	 */
	std::string path;

public:
	/**
	 * Initializes a new InMemoryLoader.
	 * @param path the resource's location in the filesystem
	 */
	InMemoryLoader(const std::string &path);
	virtual ~InMemoryLoader() = default;

	/**
	 * Returns the resource as pcm data buffer.
	 */
	virtual pcm_data_t get_resource() = 0;

	/**
	 * Create a InMemoryLoader instance that supports the given format.
	 * @param path the resource's location in the filesystem
	 * @param format the resource's audio format
	 */
	static std::unique_ptr<InMemoryLoader> create(const std::string &path,
	                                              format_t format);
};

}
}

#endif
