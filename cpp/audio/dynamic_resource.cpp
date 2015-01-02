// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "dynamic_resource.h"

#include "../engine.h"
#include "../log.h"

namespace openage {
namespace audio {

chunk_info_t::chunk_info_t(int state, size_t resource_index, int16_t *buffer)
		:
		state{state},
		resource_index{resource_index},
		actual_size{0},
		buffer{buffer} {
}

DynamicResource::DynamicResource(category_t category, int id,
		const std::string &path, format_t format, int preload_threshold,
		size_t chunk_size, size_t max_chunks)
		:
		Resource{category, id},
		path{path},
		format{format},
		preload_threshold{preload_threshold},
		chunk_size{chunk_size},
		max_chunks{max_chunks},
		use_count{0},
		chunk_buffer_size{chunk_size * max_chunks} {
}

void DynamicResource::use() {
	log::msg("DYNRES: now in use");
	log::msg("CS=%lu, MX=%lu, CBS=%lu", chunk_size, max_chunks, chunk_buffer_size);
	// if the resource is new in use
	if ((this->use_count++) == 0) {
		// create loader
		this->loader = DynamicLoader::create(this->path, this->format);
		// allocate chunk buffer
		this->chunk_buffer.reset(new int16_t[this->chunk_buffer_size]);
		// create chunk information
		for (size_t i = 0; i < this->max_chunks; i++) {
			int16_t *buffer = this->chunk_buffer.get() + i * this->chunk_size;
			this->chunk_infos.push(std::make_shared<chunk_info_t>(
					chunk_info_t::UNUSED, 0, buffer));
		}
	}
}

void DynamicResource::stop_using() {
	log::msg("DYNRES: no longer in use");
	// if the resource is not used anymore
	if ((--this->use_count) == 0) {
		// clear chunk information
		this->chunk_infos.clear();
		// delete the internal chunk buffer
		this->chunk_buffer.reset(nullptr);
	}
	//
	this->chunk_mapping.clear();
}

std::tuple<const int16_t*,size_t> DynamicResource::get_data(
		size_t position, size_t data_length) {
	size_t resource_chunk_index = position / this->chunk_size;
	size_t chunk_offset = position % this->chunk_size;

	auto chunk_map_it = this->chunk_mapping.find(resource_chunk_index);
	// the chunk is either in loading state or ready
	if (chunk_map_it != std::end(this->chunk_mapping)) {
		auto chunk_info = chunk_map_it->second;
		int16_t *chunk = chunk_info->buffer + chunk_offset;
		// switch chunk's current state
		switch (chunk_info->state.load()) {
		case chunk_info_t::UNUSED:
			this->chunk_mapping.erase(chunk_map_it);
			return std::make_tuple(nullptr, 0);
		case chunk_info_t::LOADING:
			// signal that resource is not ready yet
			return std::make_tuple(nullptr, 1);
		case chunk_info_t::READY:
			this->start_preloading(resource_chunk_index);
			// calculate actual data length
			if (chunk_info->actual_size - chunk_offset >= data_length) {
				return std::make_tuple(chunk, data_length);
			} else {
				return std::make_tuple(chunk,
						chunk_info->actual_size - chunk_offset);
			}
		}
	}

	auto chunk_info = this->chunk_infos.front();
	this->chunk_infos.pop();

	this->chunk_mapping.insert({resource_chunk_index, chunk_info});
	size_t resource_chunk_offset = resource_chunk_index * this->chunk_size;
	this->start_loading(chunk_info, resource_chunk_offset);
	this->start_preloading(resource_chunk_index);

	// the chunk is currently not loaded, so start a background job
	return std::make_tuple(nullptr, 1);
}

void DynamicResource::start_preloading(size_t resource_chunk_index) {
	size_t resource_chunk_offset = resource_chunk_index * this->chunk_size;

	for (int i = 1; i < this->preload_threshold; i++) {
		resource_chunk_index += 1;
		resource_chunk_offset += this->chunk_size;

		auto chunk_map_it = this->chunk_mapping.find(resource_chunk_index);
		// the chunk is either in loading state or ready
		if (chunk_map_it != std::end(this->chunk_mapping)) {
			//log::msg("ALREADY_THERE: %lu", resource_chunk_index);
			auto chunk_info = chunk_map_it->second;
			// TODO atomic?
			if (chunk_info->state.load() == chunk_info_t::UNUSED) {
				this->chunk_mapping.erase(chunk_map_it);
				// there is a chunk that is the end of the stream, so abort here
				break;
			}
		// there is no entry, so start loading
		} else {
			//log::msg("UP_TO_LOAD: %lu", resource_chunk_index);
			auto local_chunk_info = this->chunk_infos.front();
			this->chunk_infos.pop();

			this->chunk_mapping.insert({resource_chunk_index, local_chunk_info});
			this->start_loading(local_chunk_info, resource_chunk_offset);
		}
	}
}

void DynamicResource::start_loading(
		std::shared_ptr<chunk_info_t> chunk_info,
		size_t resource_chunk_offset) {
	chunk_info->state.store(chunk_info_t::LOADING);
	auto load_function = [this,chunk_info,resource_chunk_offset]() -> int {
		int16_t *buffer = chunk_info->buffer;
		size_t loaded = this->loader->load_chunk(buffer, resource_chunk_offset, this->chunk_size);
		//log::msg("LOADED=%lu, ROFF=%lu", loaded, resource_chunk_offset);
		if (loaded == 0) {
			chunk_info->state.store(chunk_info_t::UNUSED);
			this->chunk_infos.push(chunk_info);
		} else {
			chunk_info->state.store(chunk_info_t::READY);
			chunk_info->actual_size = loaded;
			this->chunk_infos.push(chunk_info);
		}

		return 0;
	};

	Engine &e = Engine::get();
	auto job = e.get_job_manager()->enqueue<int>(load_function);
}

}
}
