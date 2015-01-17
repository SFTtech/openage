// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_AUDIO_DYNAMIC_RESOURCE_H_
#define OPENAGE_AUDIO_DYNAMIC_RESOURCE_H_

#include <atomic>
#include <memory>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "category.h"
#include "dynamic_loader.h"
#include "format.h"
#include "resource.h"
#include "types.h"
#include "../datastructure/concurrent_queue.h"
#include "../job/job.h"
#include "../job/job_group.h"

namespace openage {
namespace audio {

struct chunk_info_t {
	enum {
		/** The chunk is currently unused. */
		UNUSED,

		/** The chunk is currently loading. */
		LOADING,

		/** The chunk is loaded and ready to be used. */
		READY
	};

	/** The chunk's current state. */
	std::atomic_int state;

	/** The chunk's actual size. */
	size_t actual_size;

	/** The chunk's buffer. */
	std::unique_ptr<int16_t[]> buffer;

	chunk_info_t(int state, size_t buffer_size);
	~chunk_info_t() = default;
};

struct loading_info_t {
	/** The chunk into which audio data should be loaded. */
	std::shared_ptr<chunk_info_t> chunk_info;

	/**
	 * The offset of the audio data, that should be loaded, within the
	 * resource.
	 */
	size_t resource_chunk_offset;
};

class DynamicResource : public Resource {
public:
	/**
	 * The number of chunks that have to be loaded, before a sound actually
	 * starts playing.
	 */
	static const int DEFAULT_PRELOAD_THRESHOLD = 10;

	/** The default used chunk size in bytes (100ms). */
	static const size_t DEFAULT_CHUNK_SIZE = 9600*2;

	/** The default number of chunks, that can be loaded at the same time. */
	static const size_t DEFAULT_MAX_CHUNKS = 100;

private:
	/** The resource's path. */
	std::string path;

	/** The resource's audio format. */
	format_t format;

	/** The number of chunks that should be preloaded. */
	int preload_threshold;

	/** The size of one audio chunk in bytes. */
	size_t chunk_size;

	/** The number of chunks that are used to store audio data. */
	size_t max_chunks;

	/** The number of sounds that currently use this resource. */
	std::atomic_int use_count;

	/** The background audio loader. */
	std::unique_ptr<DynamicLoader> loader;

	/** Queue of audio chunks, that should be used next for loading audio data. */
	openage::datastructure::ConcurrentQueue<std::shared_ptr<chunk_info_t>> chunk_infos;

	/** Resource chunk index to chunk mapping. */
	std::unordered_map<size_t,std::shared_ptr<chunk_info_t>> chunk_mapping;

	/** The background loading job group. */
	openage::job::JobGroup loading_job_group;

public:
	DynamicResource(category_t category, int id, const std::string &path,
			format_t format=format_t::OPUS,
			int preload_threshold=DEFAULT_PRELOAD_THRESHOLD,
			size_t chunk_size=DEFAULT_CHUNK_SIZE,
			size_t max_chunks=DEFAULT_MAX_CHUNKS);
	virtual ~DynamicResource() = default;

	virtual void use();
	virtual void stop_using();

	virtual std::tuple<const int16_t*,size_t> get_data(size_t position,
			size_t data_length);

private:
	void start_preloading(size_t resource_chunk_index);

	void start_loading(std::shared_ptr<chunk_info_t> chunk_info,
			size_t resource_chunk_offset);
};

}
}

#endif
