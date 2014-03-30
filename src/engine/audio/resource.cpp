#include "resource.h"

#include "in_memory_loader.h"
#include "../util/error.h"
#include "../log.h"

namespace engine {
namespace audio {

Resource::Resource(category_t category, int id)
		:
		category{category},
		id{id} {
}

category_t Resource::get_category() const {
	return category;
}

int Resource::get_id() const {
	return id;
}

std::shared_ptr<Resource> Resource::create_resource(category_t category,
		int id, const std::string &path, format_t format,
		loader_policy_t loader_policy) {
	switch (loader_policy) {
		case loader_policy_t::IN_MEMORY:
			return std::make_shared<InMemoryResource>(category, id, path,
					format);
		case loader_policy_t::DYNAMIC:
		default:
			throw Error{"Unsupported loader policy"};
	}
}

InMemoryResource::InMemoryResource(category_t category, int id,
		const std::string &path, format_t format)
		:
		Resource{category, id}  {
	auto loader = InMemoryLoader::create(path, format);
	auto resource = loader->get_resource();
	buffer = std::move(std::get<0>(resource));
	length = std::get<1>(resource);
}

uint32_t InMemoryResource::get_length() const {
	return length;
}

std::tuple<const int16_t*,uint32_t> InMemoryResource::get_samples(uint32_t position,
		uint32_t num_samples) {
	// if the resource's end has been reached
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
