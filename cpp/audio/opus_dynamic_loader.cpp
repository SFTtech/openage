// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include "opus_dynamic_loader.h"

#include <thread>

#include <opusfile.h>

#include "../log.h"
#include "../util/error.h"

namespace openage {
namespace audio {

// custom deleter for OggOpusFile unique pointers
static auto opus_deleter = [](OggOpusFile *op_file) {
	if (op_file != nullptr) {
		op_free(op_file);
	}
};

OpusDynamicLoader::OpusDynamicLoader(const std::string &path)
		:
		DynamicLoader{path},
		source{std::move(open_opus_file())} {
	// read channels from the opus file
	channels = op_channel_count(source.get(), -1);

	int64_t pcm_length = op_pcm_total(source.get(), -1);
	if (pcm_length < 0) {
		throw util::Error{"Could not seek in %s: %ld", path.c_str(), pcm_length};
	}

	length = static_cast<size_t>(pcm_length) * 2;
	log::msg("Create dynamic opus loader: length=%lu, channels=%d",
			length, channels);
}

size_t OpusDynamicLoader::load_chunk(int16_t *chunk_buffer, size_t offset,
		size_t chunk_size) {
	// if the requested offset is greater than the resource's length, there is
	// no chunk left to load
	if (offset > length) {
		return 0;
	}

	// seek to the requested offset, the seek offset is given in samples
	// while the requested offset is given in int16_t values, so the division
	// by 2 is necessary
	int64_t pcm_offset = static_cast<int64_t>(offset / 2);

	// synchronize all accesses to the opus file, as multiple threads can use
	// this audio loader to concurrently load audio
	std::unique_lock<std::mutex> lock{this->mutex};
	int op_ret = op_pcm_seek(source.get(), pcm_offset);
	if (op_ret < 0) {
		throw util::Error{"Could not seek in %s: %d", path.c_str(), op_ret};
	}

	// read a chunk from the requested offset
	// if the opus file is a mono source, we read chunk_size / 2 values and
	// convert it to stereo directly
	// if the opus file is a stereo source, we read chunk_size directly
	int read_num_values = chunk_size / 2 * channels;
	int read_count = 0;
	// loop as long as there are samples left to read
	while (read_count <= read_num_values) {
		int samples_read = op_read(
			source.get(), chunk_buffer + read_count,
			read_num_values - read_count, nullptr
		);

		// an error occured
		if (samples_read < 0) {
			throw util::Error{"Could not read from %s: %d", path.c_str(), samples_read};
		}
		// end of the resource
		else if (samples_read == 0) {
			break;
		}

		// increase read_count by the number of int16_t values that have been
		// read
		read_count += samples_read * channels;
	}
	lock.unlock();

	// convert to stereo
	if (channels == 1) {
		for(int i = read_count-1; i >= 0; i--) {
			auto value = chunk_buffer[i];
			chunk_buffer[i*2+1] = value;
			chunk_buffer[i*2] = value;
		}
	}

	log::msg("DYNLOAD: file=%d all=%d", read_count, read_count * 2 / channels);
	return read_count * 2 / channels;
}

opus_file_t OpusDynamicLoader::open_opus_file() {
	int op_err;
	opus_file_t op_file{op_open_file(path.c_str(), &op_err), opus_deleter};
	if (op_err != 0) {
		throw util::Error{"Could not open: %s", path.c_str()};
	}
	return std::move(op_file);
}

}
}
