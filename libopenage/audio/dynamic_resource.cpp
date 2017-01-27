// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include "dynamic_resource.h"


#include "audio_manager.h"
#include "../engine.h"
#include "../job/job_manager.h"
#include "../log/log.h"

namespace openage {
namespace audio {

chunk_info_t::chunk_info_t(chunk_info_t::state_t state,
                           size_t buffer_size)
	:
	state{state},
	actual_size{0},
	buffer{std::make_unique<int16_t[]>(buffer_size)} {}


DynamicResource::DynamicResource(AudioManager *manager,
                                 category_t category,
                                 int id,
                                 const std::string &path,
                                 format_t format,
                                 int preload_threshold,
                                 size_t chunk_size,
                                 size_t max_chunks)
	:
	Resource{manager, category, id},
	path{path},
	format{format},
	preload_threshold{preload_threshold},
	chunk_size{chunk_size},
	max_chunks{max_chunks},
	use_count{0} {}

void DynamicResource::use() {
	// if the resource is new in use
	if ((this->use_count++) == 0) {
		// create loader
		this->loader = DynamicLoader::create(this->path, this->format);

		// create chunk information
		for (size_t i = 0; i < this->max_chunks; i++) {
			this->chunk_infos.push(std::make_shared<chunk_info_t>(
				chunk_info_t::state_t::UNUSED,
				this->chunk_size)
			);
		}

		// create loading job group
		this->loading_job_group = this->manager->get_job_manager()->create_job_group();
	}
}

void DynamicResource::stop_using() {
	// if the resource is not used anymore
	if ((--this->use_count) == 0) {
		// clear the chunk_mapping
		this->chunk_mapping.clear();
		// clear chunk information
		this->chunk_infos.clear();
	}
}

audio_chunk_t DynamicResource::get_data(size_t position, size_t data_length) {
	size_t resource_chunk_index = position / this->chunk_size;
	size_t chunk_offset = position % this->chunk_size;

	auto chunk_map_it = this->chunk_mapping.find(resource_chunk_index);
	// the chunk is either in loading state or ready
	if (chunk_map_it != std::end(this->chunk_mapping)) {
		auto chunk_info = chunk_map_it->second;
		int16_t *chunk = chunk_info->buffer.get() + chunk_offset;
		// switch chunk's current state
		switch (chunk_info->state.load()) {
		case chunk_info_t::state_t::UNUSED:
			this->chunk_mapping.erase(chunk_map_it);
			return {nullptr, 0};
		case chunk_info_t::state_t::LOADING:
			// signal that resource is not ready yet
			return {nullptr, 1};
		case chunk_info_t::state_t::READY:
			this->start_preloading(resource_chunk_index);
			// calculate actual data length
			if (chunk_info->actual_size - chunk_offset >= data_length) {
				return {chunk, data_length};
			} else {
				return {chunk, chunk_info->actual_size - chunk_offset};
			}
		}
	}

	// the chunk is currently not loaded, so start a background job
	// take the next available chunk
	auto chunk_info = this->chunk_infos.front();
	this->chunk_infos.pop();

	// map it to the corresponding resource offset
	this->chunk_mapping.insert({resource_chunk_index, chunk_info});
	size_t resource_chunk_offset = resource_chunk_index * this->chunk_size;

	// and start loading
	this->start_loading(chunk_info, resource_chunk_offset);
	this->start_preloading(resource_chunk_index);

	return {nullptr, 1};
}


void DynamicResource::start_preloading(size_t resource_chunk_index) {
	size_t resource_chunk_offset = resource_chunk_index * this->chunk_size;

	for (int i = 1; i < this->preload_threshold; i++) {
		resource_chunk_index += 1;
		resource_chunk_offset += this->chunk_size;

		auto chunk_map_it = this->chunk_mapping.find(resource_chunk_index);
		// the chunk is either in loading state or ready
		if (chunk_map_it != std::end(this->chunk_mapping)) {
			auto chunk_info = chunk_map_it->second;
			if (chunk_info->state.load() == chunk_info_t::state_t::UNUSED) {
				this->chunk_mapping.erase(chunk_map_it);
				// there is a chunk that is the end of the stream, so abort here
				break;
			}
		// there is no entry, so start loading
		} else {
			auto local_chunk_info = this->chunk_infos.front();
			this->chunk_infos.pop();

			this->chunk_mapping.insert({resource_chunk_index, local_chunk_info});
			this->start_loading(local_chunk_info, resource_chunk_offset);
		}
	}
}


void DynamicResource::start_loading(std::shared_ptr<chunk_info_t> chunk_info,
                                    size_t resource_chunk_offset) {
	chunk_info->state.store(chunk_info_t::state_t::LOADING);

	auto loading_function = [this,chunk_info,resource_chunk_offset]() -> int {
		int16_t *buffer = chunk_info->buffer.get();
		size_t loaded = this->loader->load_chunk(buffer, resource_chunk_offset, this->chunk_size);
		if (loaded == 0) {
			chunk_info->state.store(chunk_info_t::state_t::UNUSED);
			this->chunk_infos.push(chunk_info);
		} else {
			chunk_info->state.store(chunk_info_t::state_t::READY);
			chunk_info->actual_size = loaded;
			this->chunk_infos.push(chunk_info);
		}
		// as the job manager currently does not support executing void
		// functions, we return zero
		return 0;
	};
	this->loading_job_group.enqueue<int>(loading_function);
}


}} // namespace openage::audio
