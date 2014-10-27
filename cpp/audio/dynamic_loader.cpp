#include "dynamic_loader.h"

#include "../log.h"
#include "../util/error.h"

#include <cstring>

namespace openage {
namespace audio {

DynamicLoader::DynamicLoader(const std::string &path)
		:
		path{path} {
}

std::unique_ptr<DynamicLoader> DynamicLoader::create(const std::string &path,
		format_t format) {
	std::unique_ptr<DynamicLoader> loader;
	switch (format) {
		case format_t::OPUS:
			loader.reset(new OpusDynamicLoader{path});
			break;
		default:
			throw util::Error{"Not supported for format: %d", format};
	}
	return std::move(loader);
}

// custom deleter for OggOpusFile unique pointers
static auto opus_deleter = [](OggOpusFile *op_file) {
	if (op_file != nullptr) {
		op_free(op_file);
	}
};


OpusDynamicLoader::OpusDynamicLoader(const std::string &path)
		:
		DynamicLoader{path} {
	// open opus file
	auto op_file = open_opus_file();

	// read channels from the opus file
	channels = op_channel_count(op_file.get(), -1);

	length = op_pcm_total(op_file.get(), -1) * 2;
	log::msg("Create dynamic opus loader: len=%d, chan=%d", length, channels);
}

uint32_t OpusDynamicLoader::get_length() {
	return length;
}

pcm_chunk_t OpusDynamicLoader::load_chunk(uint32_t offset, uint32_t chunk_size) {
	// if the requested offset is greater than the resource's length, there is
	// no chunk left to load
	if (offset > length) {
		return {};
	}

	// open opus file
	auto op_file = open_opus_file();

	// allocate the chunk's buffer
	pcm_chunk_t chunk;
	chunk.reserve(chunk_size);
	// initialize chunks with zeroes
	std::memset(&chunk.front(), 0, chunk_size*sizeof(int16_t));

	// seek to the requested offset, the seek offset is given in samples
	// while the requested offset is given in int16_t values, so the division
	// by 2 is necessary
	int64_t pcm_offset = static_cast<int64_t>(offset / 2);
	int op_ret = op_pcm_seek(op_file.get(), pcm_offset);
	if (op_ret < 0) {
		throw util::Error{"Could not seek in %s: %d", path.c_str(), op_ret};
	}

	// read a chunk from the requested offset
	// if the opus file is a mono source, we read chunk_size / 2 values and
	// convert it to stereo directly
	// if the opus file is a stereo source, we read chunk_size directly
	int read_num_values = chunk_size / 2 * channels;
	int read_count = 0;
	int samples_read;
	// loop as long as there are samples left to read
	while (read_count <= read_num_values) {
		samples_read = op_read(
			op_file.get(), &chunk.front() + read_count,
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

	// convert to stereo
	if (channels == 1) {
		for(int i = read_count-1; i >= 0; i--) {
			auto value = chunk[i];
			chunk[i*2+1] = value;
			chunk[i*2] = value;
		}
	}

	log::msg("DYNLOAD: file=%d all=%d", read_count, read_count * 2 / channels);
	return std::move(chunk);
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
