// Copyright 2014-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <string>

#include "format.h"
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
	InMemoryResource(AudioManager *manager,
	                 category_t category, int id, const std::string &path,
	                 format_t format=format_t::OPUS);
	virtual ~InMemoryResource() = default;

	void use() override;
	void stop_using() override;

	audio_chunk_t get_data(size_t position, size_t data_length) override;
};

}
}
