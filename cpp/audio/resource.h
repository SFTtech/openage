// Copyright 2014-2014 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_AUDIO_RESOURCE_H_
#define OPENAGE_AUDIO_RESOURCE_H_

#include <atomic>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include <SDL2/SDL.h>

#include "category.h"
#include "dynamic_loader.h"
#include "format.h"
#include "../job/job_manager.h"
#include "loader_policy.h"
#include "types.h"

namespace openage {
namespace audio {

/**
 * A Resource contains 16 bit signed integer pcm data, that can be played by
 * sounds. Each Resource has an unique id within it's category. The category
 * specifies the type of sound, e. g. game sounds, background music, etc.
 * The id is a unique integer value.
 */
class Resource {
private:
	/**
	 * The resource's category.
	 */
	category_t category;

	/**
	 * The resource's id.
	 */
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

	/**
	 * Tells the resource, that it will be used by a sound object, so it can
	 * preload some pcm samples.
	 */
	virtual void use();

	/**
	 * Tells the resource, that one sound object does not use this resource any
	 * longer.
	 */
	virtual void stop_using();

	/**
	 * TODO fix docReturns a pointer to the sample buffer at the given position and the
	 * number of samples that are actually available. If the end of the resource
	 * is reached, 0 will be returned. If the resource is not ready yet, a
	 * nullptr with a length, different to zero, will be returned.
	 * @param position the current position in the resource
	 * @param num_samples the number of samples that should be returned
	 */
	virtual std::tuple<const int16_t*,uint32_t> get_data(uint32_t position,
			uint32_t data_length) = 0;

	static std::shared_ptr<Resource> create_resource(
		category_t category,
		int id, const std::string &path, format_t format,
		loader_policy_t loader_policy
	);
};

/**
 * A InMemoryResource loads the whole pcm data into memory and keeps it there.
 */
class InMemoryResource : public Resource {
private:
	/** The resource's internal buffer. */
	pcm_data_t buffer;

public:
	InMemoryResource(
		category_t category, int id,
		const std::string &path,
		format_t format=format_t::OPUS
	);
	virtual ~InMemoryResource() = default;

	virtual std::tuple<const int16_t*,uint32_t> get_data(
		uint32_t position,
		uint32_t data_length
	);
};

constexpr uint32_t CHUNK_SIZE = 96000;

class DynamicResource : public Resource {
private:
	std::atomic_int use_count;

	std::vector<pcm_chunk_t> chunks;
	int num_chunks;
	uint32_t length;

	std::unordered_map<int,job::Job<pcm_chunk_t>> running_jobs;

	std::unique_ptr<DynamicLoader> loader;

public:
	DynamicResource(
		category_t category, int id, const std::string &path,
		format_t format=format_t::OPUS
	);
	virtual ~DynamicResource() = default;

	virtual void use();
	virtual void stop_using();

	virtual std::tuple<const int16_t*,uint32_t> get_data(
		uint32_t position,
		uint32_t data_length
	);

private:
	void load_chunk(int chunk_index);
};

}
}

#endif
