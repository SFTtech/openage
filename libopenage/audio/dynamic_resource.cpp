// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include "dynamic_resource.h"


#include "audio_manager.h"
#include "../engine.h"
#include "../job/job_manager.h"
#include "../log/log.h"
#include "../job/job_manager.h"

namespace openage {
namespace audio {

chunk_info_t::chunk_info_t(chunk_info_t::state_t state,
                           size_t buffer_size)
	:
	state{state},
	size{0},
	buffer{std::make_unique<int16_t[]>(buffer_size)} {}


DynamicResource::DynamicResource(AudioManager *manager,
                                 category_t category,
                                 int id,
                                 const util::Path &path,
                                 format_t format,
                                 int preload_amount,
                                 size_t chunk_size,
                                 size_t max_chunks)
	:
	Resource{manager, category, id},
	path{path},
	format{format},
	preload_amount{preload_amount},
	chunk_size{chunk_size},
	max_chunks{max_chunks},
	use_count{0} {}

void DynamicResource::use() {
	// if the resource is new in use
	bool initialize_use = (this->use_count == 0);
	use_count += 1;

	if (initialize_use) {
		// create loading job group
		this->loading_job_group = this->manager->get_job_manager()->create_job_group();

		// create the fixed amount of chunk loading buffers
		for (size_t i = 0; i < this->max_chunks; i++) {
			this->decay_queue.push(
				std::make_shared<chunk_info_t>(
					chunk_info_t::state_t::EMPTY,
					this->chunk_size
				)
			);
		}

		// create loader
		this->loader = DynamicLoader::create(this->path, this->format);
	}
}

void DynamicResource::stop_using() {
	// if the resource is not used anymore
	if ((--this->use_count) == 0) {
		// clear the chunks
		this->chunks.clear();
		// clear chunk information
		this->decay_queue.clear();
	}
}

audio_chunk_t DynamicResource::get_data(size_t position, size_t data_length) {
	size_t resource_chunk_index = position / this->chunk_size;
	size_t chunk_offset = position % this->chunk_size;

	// check if the audio chunk is currently loading or already ready
	auto chunk_map_it = this->chunks.find(resource_chunk_index);
	if (chunk_map_it != std::end(this->chunks)) {
		auto chunk_info = chunk_map_it->second;
		int16_t *data = chunk_info->buffer.get() + chunk_offset;

		switch (chunk_info->state.load()) {
		case chunk_info_t::state_t::EMPTY:
			this->chunks.erase(chunk_map_it);
			// signal end of stream
			return {nullptr, 0};

		case chunk_info_t::state_t::LOADING:
			// signal that resource is not ready yet
			return {nullptr, 1};

		case chunk_info_t::state_t::READY:
			// start to preload the next chunks!
			this->start_preloading(resource_chunk_index);

			// calculate actual data length
			if (chunk_info->size - chunk_offset >= data_length) {
				return {data, data_length};
			} else {
				return {data, chunk_info->size - chunk_offset};
			}
		}
	}

	// guard the .empty(), .front() and .pop() calls
	auto chunk_info_lock = this->decay_queue.lock();

	// we don't have next data to load!
	if (this->decay_queue.empty()) {
		throw Error{ERR << "chunk decay queue ran dry, resource is dying."};
	}

	// obtain a chunk info element to be used and overwritten
	auto chunk_info = this->decay_queue.front();
	this->decay_queue.pop();

	chunk_info_lock.unlock();

	// map it to the corresponding resource offset
	this->chunks.insert({resource_chunk_index, chunk_info});

	// start a background job to get the data of that chunk
	size_t resource_chunk_offset = resource_chunk_index * this->chunk_size;
	this->load_chunk_async(chunk_info, resource_chunk_offset);

	// and also load more chunks starting at the current chunk index
	this->start_preloading(resource_chunk_index);

	return {nullptr, 1};
}


void DynamicResource::start_preloading(size_t resource_chunk_index) {
	size_t resource_chunk_offset = resource_chunk_index * this->chunk_size;

	// from the current index, enqueue the load of `threshold` more chunks
	for (int i = 1; i < this->preload_amount; i++) {
		resource_chunk_index += 1;
		resource_chunk_offset += this->chunk_size;

		auto chunk_map_it = this->chunks.find(resource_chunk_index);

		// the chunk is either in loading state or ready
		// we do this to no longer preload if the stream is at its end.
		if (chunk_map_it != std::end(this->chunks)) {
			auto chunk_info = chunk_map_it->second;
			if (chunk_info->state.load() == chunk_info_t::state_t::EMPTY) {
				this->chunks.erase(chunk_map_it);

				// we encountered the end of stream!
				break;
			}
			// else, the chunk is known already and loading or even loaded,
			// no need to trigger the load.
		}
		else {
			// chunk is unknown yet, so let's load it.

			auto decay_queue_lock = this->decay_queue.lock();
			// obtain the least-recently-used chunk to overwrite.
			auto local_chunk_info = this->decay_queue.front();
			this->decay_queue.pop();
			decay_queue_lock.unlock();

			this->chunks.insert({resource_chunk_index, local_chunk_info});
			this->load_chunk_async(local_chunk_info, resource_chunk_offset);
		}
	}
}


void DynamicResource::load_chunk_async(std::shared_ptr<chunk_info_t> chunk_info,
                                       size_t resource_chunk_offset) {
	chunk_info->state.store(chunk_info_t::state_t::LOADING);

	// the `chunk_info` will stay out of the `decay_queue`
	// until it was loaded. So it's possible the queue runs dry.

	ENSURE(this->loader, "tried to load chunk without audio loader!");

	auto loading_function = [this,chunk_info,resource_chunk_offset]() -> int {
		int16_t *buffer = chunk_info->buffer.get();
		size_t loaded = this->loader->load_chunk(buffer, resource_chunk_offset, this->chunk_size);
		if (loaded == 0) {
			chunk_info->state.store(chunk_info_t::state_t::EMPTY);
			this->decay_queue.push(chunk_info);
		} else {
			chunk_info->state.store(chunk_info_t::state_t::READY);
			chunk_info->size = loaded;
			this->decay_queue.push(chunk_info);
		}
		// as the job manager currently does not support executing void
		// functions, we return zero
		return 0;
	};
	this->loading_job_group.enqueue<int>(loading_function);
}


}} // namespace openage::audio
