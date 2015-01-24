// Copyright 2014-2014 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_AUDIO_IN_MEMORY_RESOURCE_H_
#define OPENAGE_AUDIO_IN_MEMORY_RESOURCE_H_

#include <string>
#include <tuple>

#include "format.h"
#include "loader_policy.h"
#include "resource.h"
#include "types.h"

namespace openage {
namespace audio {

/**
 * An InMemoryResource loads the whole pcm data into memory and keeps it there.
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

	virtual audio_chunk_t get_data(size_t position, size_t data_length);
};

}
}

#endif
