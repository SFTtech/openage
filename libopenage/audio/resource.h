// Copyright 2014-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <string>
#include <tuple>

#include "category.h"
#include "format.h"
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
	 * Returns a pointer to the sample buffer at the given position and the
	 * number of int16_t values that are actually available. If the end of the
	 * resource is reached, 0 will be returned. If the resource is not ready
	 * yet, a nullptr with a length, different to zero, will be returned.
	 *
	 * @param position the current position in the resource @param data_length
	 *        the number of int16_t values that should be returned
	 * @param data_length the number of bytes of audio data that is requested
	 */
	virtual audio_chunk_t get_data(size_t position, size_t data_length) = 0;

	static std::shared_ptr<Resource> create_resource(category_t category, int id,
	                                                 const std::string &path,
	                                                 format_t format,
	                                                 loader_policy_t loader_policy);
};

}
}
