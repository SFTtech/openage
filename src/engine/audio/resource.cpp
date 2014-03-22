#include "resource.h"

#include "resource_loader.h"
#include "../util/error.h"
#include "../log.h"

namespace engine {
namespace audio {

InMemoryResource::InMemoryResource(const std::string &path, int format) {
	auto loader = ResourceLoader::create_resource_loader(path, format);
	auto resource = loader->get_resource();
	buffer = std::move(std::get<0>(resource));
	length = std::get<1>(resource);
}

uint32_t InMemoryResource::get_length() const {
	return length;
}

std::tuple<const int16_t*,uint32_t> InMemoryResource::get_samples(uint32_t position,
		uint32_t num_samples) {
	if (position >= length) {
		return std::make_tuple(nullptr, 0);
	}

	const int16_t *buf_pos = buffer.get() + position;
	if (num_samples > length - position) {
		return std::make_tuple(buf_pos, length - position);
	} else {
		return std::make_tuple(buf_pos, num_samples);
	}
}

}
}
