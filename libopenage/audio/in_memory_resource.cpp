// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include "in_memory_resource.h"

#include "in_memory_loader.h"

namespace openage {
namespace audio {

InMemoryResource::InMemoryResource(category_t category,
                                   int id,
                                   const std::string &path,
                                   format_t format)
	:
	Resource{category, id} {

	auto loader = InMemoryLoader::create(path, format);
	buffer = loader->get_resource();
}

audio_chunk_t InMemoryResource::get_data(size_t position,
                                         size_t data_length) {
	// if the resource's end has been reached
	size_t length = buffer.size();
	if (position >= length) {
		return {nullptr, 0};
	}

	const int16_t *buf_pos = &buffer[position];
	if (data_length > length - position) {
		return {buf_pos, length - position};
	} else {
		return {buf_pos, data_length};
	}
}

}
}
