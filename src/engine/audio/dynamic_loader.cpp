#include "dynamic_loader.h"

#include "../log.h"
#include "../util/error.h"

namespace engine {
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
			throw Error{"Not supported for format: %d", format};
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
	auto op_file = open_opus_file();

	channels = op_channel_count(op_file.get(), -1);
	auto length_factor = 2 / channels;

	length = op_pcm_total(op_file.get(), -1) * length_factor;
	log::msg("create dynamic opus loader: len=%d, chan=%d", length, channels);
}

uint32_t OpusDynamicLoader::get_length() {
	return length;
}

pcm_data_t OpusDynamicLoader::load_chunk(uint32_t position, uint32_t num_samples) {
	// if the requested position is greater than the resource's length, there is
	// no chunk left to load
	if (position > length) {
		return std::make_tuple(nullptr, 0);
	}

	// open opus file
	auto op_file = open_opus_file();

	// allocate the chunk's buffer
	std::unique_ptr<int16_t[]> chunk{new int16_t[num_samples]};

	// seek to the requested position
	int64_t pcm_position = static_cast<int64_t>(position * channels / 2);

	int op_ret = op_pcm_seek(op_file.get(), pcm_position);
	if (op_ret < 0) {
		throw Error{"Could not seek in %s: %d", path.c_str(), op_ret};
	}

	// read a chunk from the requested position
	int read_num_samples = num_samples / 2 * channels;
	int read_count = 0;
	while (read_count <= read_num_samples) {
		op_ret = op_read(op_file.get(), chunk.get() + read_count, read_num_samples - read_count,
				nullptr);
		if (op_ret < 0) {
			throw Error{"Could not read from %s: %d", path.c_str(), op_ret};
		} else if (op_ret == 0) {
			break;
		}

		read_count += op_ret * channels;
	}

	// convert to stereo
	if (channels == 1) {
		for(int i = read_count-1; i >= 0; i--) {
			auto value = chunk[i];
			chunk[i*2+1] = value;
			chunk[i*2] = value;
		}
	}
	
	log::msg("LOADED: file=%lu, stereo=%lu", read_count, read_count * 2 / channels);
	return std::make_tuple(std::move(chunk), read_count * 2 / channels);
}

opus_file_t OpusDynamicLoader::open_opus_file() {
	int op_err;
	opus_file_t op_file{op_open_file(path.c_str(), &op_err), opus_deleter};
	if (op_err != 0) {
		throw Error{"Could not open: %s", path.c_str()};
	}
	return std::move(op_file);
}

}
}
