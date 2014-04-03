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

void Resource::use() {
}

void Resource::stop_using() {
}

std::shared_ptr<Resource> Resource::create_resource(category_t category,
		int id, const std::string &path, format_t format,
		loader_policy_t loader_policy) {
	switch (loader_policy) {
		case loader_policy_t::IN_MEMORY:
			return std::make_shared<InMemoryResource>(category, id, path,
					format);
		case loader_policy_t::DYNAMIC:
			return std::make_shared<DynamicResource>(category, id, path,
					format);
		default:
			throw Error{"Unsupported loader policy"};
	}
}

// in memory resource

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

std::tuple<const int16_t*,uint32_t> InMemoryResource::get_samples(
		uint32_t position, uint32_t num_samples) {
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

// dynamic resource

DynamicResource::DynamicResource(category_t category, int id,
		const std::string &path, format_t format)
		:
		Resource{category, id},
		use_count{0} {
	loader = DynamicLoader::create(path, format);
	length = loader->get_length();
	num_chunks = length / CHUNK_SIZE;
	if (length % CHUNK_SIZE != 0) {
		num_chunks++;
	}

	log::msg("DYNRES: len=%d, chunks=%d", length, num_chunks);
}

void DynamicResource::use() {
	log::msg("DYNRES: now in use");
	// if use count was zero
	if ((use_count++) == 0) {
		// initialize chunks
		chunks.reserve(num_chunks);
		for (int i = 0; i < num_chunks; i++) {
			chunks.emplace_back(nullptr);
		}
	}
}

void DynamicResource::stop_using() {
	log::msg("DYNRES: no longer in use");
	// if use count is now zero
	if ((--use_count) == 0) {
		// delete all chunks
		chunks.clear();
		chunks.shrink_to_fit();
	}
}

uint32_t DynamicResource::get_length() const {
	return length;
}

std::tuple<const int16_t*,uint32_t> DynamicResource::get_samples(
		uint32_t position, uint32_t num_samples) {
	log::msg("DYNRES: request pos=%lu, num=%lu", position, num_samples);
	auto chunk_index = position / CHUNK_SIZE;
	auto chunk_offset = position % CHUNK_SIZE;
	if (chunk_index >= num_chunks) {
		return std::make_tuple(nullptr, 0);
	}
	log::msg(" --> chunk_index=%d, chunk_offset=%d", chunk_index, chunk_offset);
	if (!chunks[chunk_index]) {
		auto chunk = loader->load_chunk(chunk_index*CHUNK_SIZE, CHUNK_SIZE);
		chunks[chunk_index].swap(std::get<0>(chunk));
	}

	auto buf = chunks[chunk_index].get() + chunk_offset;
	if (CHUNK_SIZE - chunk_offset >= num_samples) {
		log::msg(" --> RET %d", num_samples);
		return std::make_tuple(buf, num_samples);
	} else {
		log::msg(" --> RET %d", CHUNK_SIZE - chunk_offset);
		return std::make_tuple(buf, CHUNK_SIZE - chunk_offset);
	}
}

}
}
