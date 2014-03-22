#ifndef _ENGINE_AUDIO_RESOURCE_LOADER_H_
#define _ENGINE_AUDIO_RESOURCE_LOADER_H_

#include <memory>
#include <tuple>

namespace engine {
namespace audio {

using resource_t = std::tuple<std::unique_ptr<int16_t[]>,uint32_t>;

class ResourceLoader {
protected:
	std::string path;

public:
	ResourceLoader(const std::string &path);
	virtual ~ResourceLoader() = default;

	ResourceLoader(const ResourceLoader&) = delete;
	ResourceLoader &operator=(const ResourceLoader&) = delete;

	ResourceLoader(ResourceLoader&&) = default;
	ResourceLoader &operator=(ResourceLoader&&) = default;

	virtual resource_t get_resource() = 0;

	static std::unique_ptr<ResourceLoader> create_resource_loader(
			const std::string &path, int format);
};

class OpusLoader : public ResourceLoader {
public:
	OpusLoader(const std::string &path);
	virtual ~OpusLoader() = default;

	virtual resource_t get_resource();
};
	
}
}

#endif
