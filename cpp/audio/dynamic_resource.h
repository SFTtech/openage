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
	int16_t *buffer;

	chunk_info_t(int state, int16_t *buffer);
	~chunk_info_t() = default;
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
	std::string path;
	format_t format;
	int preload_threshold;
	size_t chunk_size;
	size_t max_chunks;

	std::atomic_int use_count;
	std::unique_ptr<DynamicLoader> loader;

	size_t chunk_buffer_size;
	openage::datastructure::ConcurrentQueue<std::shared_ptr<chunk_info_t>> chunk_infos;
	std::unique_ptr<int16_t[]> chunk_buffer;

	/** Resource chunk index to chunk mapping. */
	std::unordered_map<size_t,std::shared_ptr<chunk_info_t>> chunk_mapping;

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
