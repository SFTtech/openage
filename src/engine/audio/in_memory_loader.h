#ifndef _ENGINE_AUDIO_IN_MEMORY_LOADER_H_
#define _ENGINE_AUDIO_IN_MEMORY_LOADER_H_

#include <memory>
#include <tuple>

#include "format.h"

namespace engine {
namespace audio {

//
using pcm_data_t = std::tuple<std::unique_ptr<int16_t[]>,uint32_t>;

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
	InMemoryLoader(const std::string &path);
	virtual ~InMemoryLoader() = default;

	/**
	 * Returns a tuple of the the loaded pcm data and it's length.
	 */
	virtual pcm_data_t get_resource() = 0;

	/**
	 * Create a InMemoryLoader instance that support the given format.
	 * @param path the resource's location in the filesystem
	 * @param format the resource's format
	 */
	static std::unique_ptr<InMemoryLoader> create(const std::string &path,
			format_t format);
};

/**
 * A OpusInMemoryLoader load's opus files.
 */
class OpusInMemoryLoader : public InMemoryLoader {
public:
	OpusInMemoryLoader(const std::string &path);
	virtual ~OpusInMemoryLoader() = default;

	virtual pcm_data_t get_resource();
};
	
}
}

#endif
