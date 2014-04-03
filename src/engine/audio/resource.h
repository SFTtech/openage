#ifndef _ENGINE_AUDIO_RESOURCE_H_
#define _ENGINE_AUDIO_RESOURCE_H_

#include <atomic>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include <SDL.h>

#include "category.h"
#include "dynamic_loader.h"
#include "format.h"
#include "loader_policy.h"

namespace engine {
namespace audio {

/**
 * A resource contains pcm data, that can be played by sounds.
 */
class Resource {
private:
	category_t category;
	int id;

public:
	Resource(category_t category, int id);
	virtual ~Resource() = default;

	Resource(const Resource&) = delete;
	Resource &operator=(const Resource&) = delete;

	Resource(Resource&&) = delete;
	Resource &operator=(Resource&&) = delete;

	virtual category_t get_category() const;
	virtual int get_id() const;

	virtual void use();
	virtual void stop_using();

	/**
	 * Returns the resource's length in int16_t values.
	 */
	virtual uint32_t get_length() const = 0;

	/**
	 * Returns a pointer to the sample buffer at the given position and the
	 * number of samples that are actually available. If the end of the resource
	 * is reached, 0 will be returned. If the resource is not ready yet, a
	 * nullptr will be returned.
	 * @param position the current position in the resource
	 * @param num_samples the number of int16_t that should be returned
	 */
	virtual std::tuple<const int16_t*,uint32_t> get_samples(uint32_t position,
			uint32_t num_samples) = 0;

	static std::shared_ptr<Resource> create_resource(category_t category,
			int id, const std::string &path, format_t format,
			loader_policy_t loader_policy);
};

/**
 * A InMemoryResource loads the whole pcm data into memory and keeps it there.
 */
class InMemoryResource : public Resource {
private:
	/*
	 * The resource's internal buffer.
	 */
	std::unique_ptr<int16_t[]> buffer;
	/**
	 * The buffer's length.
	 */
	uint32_t length;

public:
	InMemoryResource(category_t category, int id, const std::string &path,
			format_t format = format_t::OPUS);
	virtual ~InMemoryResource() = default;

	virtual uint32_t get_length() const;	

	virtual std::tuple<const int16_t*,uint32_t> get_samples(uint32_t position,
			uint32_t num_samples);
};

constexpr uint32_t CHUNK_SIZE = 12000;

class DynamicResource : public Resource {
private:
	std::atomic_int use_count;

	std::vector<std::unique_ptr<int16_t[]>> chunks;
	uint32_t num_chunks;
	uint32_t length;

	std::unique_ptr<DynamicLoader> loader;

public:
	DynamicResource(category_t category, int id, const std::string &path,
			format_t format = format_t::OPUS);
	virtual ~DynamicResource() = default;

	virtual void use();
	virtual void stop_using();

	virtual uint32_t get_length() const;	

	virtual std::tuple<const int16_t*,uint32_t> get_samples(uint32_t position,
			uint32_t num_samples);
};

}
}

#endif
