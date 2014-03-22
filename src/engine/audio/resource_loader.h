#ifndef _ENGINE_AUDIO_RESOURCE_LOADER_H_
#define _ENGINE_AUDIO_RESOURCE_LOADER_H_

#include <memory>
#include <tuple>

namespace engine {
namespace audio {

//
using pcm_data_t = std::tuple<std::unique_ptr<int16_t[]>,uint32_t>;

/**
 * A ResourceLoader loads a audio file into memory and converts it into 16 bit
 * signed integer pcm data.
 */
class ResourceLoader {
protected:
	/*
	 * The resource's location in the filesystem.
	 */
	std::string path;

public:
	ResourceLoader(const std::string &path);
	virtual ~ResourceLoader() = default;

	/**
	 * Returns a tuple of the the loaded pcm data and it's length.
	 */
	virtual pcm_data_t get_resource() = 0;

	/**
	 * Create a ResourceLoader instance that support the given format.
	 * @param path the resource's location in the filesystem
	 * @param format the resource's format
	 */
	static std::unique_ptr<ResourceLoader> create_resource_loader(
			const std::string &path, int format);
};

/**
 * A OpusLoader load's opus files.
 */
class OpusLoader : public ResourceLoader {
public:
	OpusLoader(const std::string &path);
	virtual ~OpusLoader() = default;

	virtual pcm_data_t get_resource();
};
	
}
}

#endif
