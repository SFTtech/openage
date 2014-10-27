#include "resource.h"

#include "in_memory_loader.h"
#include "../engine.h"
#include "../util/error.h"
#include "../log.h"

namespace openage {
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
			throw util::Error{"Unsupported loader policy"};
	}
}

// in memory resource

InMemoryResource::InMemoryResource(category_t category, int id,
		const std::string &path, format_t format)
		:
		Resource{category, id}  {
	auto loader = InMemoryLoader::create(path, format);
	buffer = loader->get_resource();
}

uint32_t InMemoryResource::get_length() const {
	return static_cast<uint32_t>(buffer.size());
}

std::tuple<const int16_t*,uint32_t> InMemoryResource::get_samples(
		uint32_t position, uint32_t num_samples) {
	// if the resource's end has been reached
	uint32_t length = static_cast<uint32_t>(buffer.size());
	if (position >= length) {
		return std::make_tuple(nullptr, 0);
	}

	const int16_t *buf_pos = &buffer[position];
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
			chunks.push_back(std::vector<int16_t>{});
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
		this->running_jobs.clear();
	}
}

uint32_t DynamicResource::get_length() const {
	return length;
}

std::tuple<const int16_t*,uint32_t> DynamicResource::get_samples(
		uint32_t position, uint32_t num_samples) {
	// TODO refactor implementation into single methods, probably change
	// preloading behavior
	log::msg("DYNRES: request pos=%u, num=%u", position, num_samples);
	// calculate chunk index and offset
	int chunk_index = position / CHUNK_SIZE;
	auto chunk_offset = position % CHUNK_SIZE;
	// the chunk is beyond the end of the resource, so the resource has finished
	if (chunk_index >= num_chunks) {
		return std::make_tuple(nullptr, 0);
	}
	log::msg(" --> chunk_index=%d, chunk_offset=%d", chunk_index, chunk_offset);

	// if chunk was not found, load it
	if (chunks[chunk_index].empty()) {
		auto job_it = this->running_jobs.find(chunk_index);
		if (job_it == std::end(this->running_jobs)) {
			load_chunk(chunk_index);
			return std::make_tuple(nullptr,1);
		} else {
			if (job_it->second.is_finished()) {
				auto result = job_it->second.get_result();
				chunks[chunk_index].swap(result);
				this->running_jobs.erase(job_it);
			} else {
				return std::make_tuple(nullptr,1);
			}
		}
	}

	// preload next chunk
	if (chunk_index < num_chunks-1 && chunks[chunk_index+1].empty()) {
		auto nindex = chunk_index+1;
		auto job_it = this->running_jobs.find(nindex);
		if (job_it == std::end(this->running_jobs)) {
			load_chunk(nindex);
		} else {
			if (job_it->second.is_finished()) {
				auto result = job_it->second.get_result();
				chunks[nindex].swap(result);
				this->running_jobs.erase(job_it);
			}
		}
	}

	// get chunk and calculate buffer
	auto buf = &chunks[chunk_index].front() + chunk_offset;
	if (CHUNK_SIZE - chunk_offset >= num_samples) {
		log::msg(" --> RET %d", num_samples);
		return std::make_tuple(buf, num_samples);
	} else {
		log::msg(" --> RET %d", CHUNK_SIZE - chunk_offset);
		return std::make_tuple(buf, CHUNK_SIZE - chunk_offset);
	}
}

void DynamicResource::load_chunk(int chunk_index) {
	auto load_function = [this,chunk_index]() -> std::vector<int16_t> {
		auto chunk = this->loader->load_chunk(chunk_index*CHUNK_SIZE, CHUNK_SIZE);
		return std::move(chunk);
	};

	Engine &e = Engine::get();

	auto job = e.get_job_manager()->enqueue<std::vector<int16_t>>(load_function);
	this->running_jobs.insert({chunk_index, job});
}

}
}
