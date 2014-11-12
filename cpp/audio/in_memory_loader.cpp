// Copyright 2014-2014 the openage authors. See copying.md for legal info.

#include "in_memory_loader.h"

#include <cinttypes>

#include <opusfile.h>

#include "../log.h"
#include "../util/error.h"

namespace openage {
namespace audio {

InMemoryLoader::InMemoryLoader(const std::string &path)
		:
		path{path} {
}

std::unique_ptr<InMemoryLoader> InMemoryLoader::create(const std::string &path,
		format_t format) {
	std::unique_ptr<InMemoryLoader> loader;
	// switch format and return an appropriate loader
	switch (format) {
	case format_t::OPUS:
		loader.reset(new OpusInMemoryLoader{path});
		break;
	default:
		throw util::Error{"Not supported for format: %d", format};
	}
	return std::move(loader);
}

OpusInMemoryLoader::OpusInMemoryLoader(const std::string &path)
		:
		InMemoryLoader{path} {
}

// custom deleter for OggOpusFile unique pointers
static auto opus_deleter = [](OggOpusFile *op_file) {
	if (op_file != nullptr) {
		op_free(op_file);
	}
};

pcm_data_t OpusInMemoryLoader::get_resource() {
	int op_err;
	// open the opus file
	opus_file_t op_file{op_open_file(path.c_str(), &op_err), opus_deleter};

	if (op_err != 0) {
		throw util::Error{"Could not open: %s", path.c_str()};
	}

	// determine number of channels and number of pcm samples
	auto op_channels = op_channel_count(op_file.get(), -1);
	auto pcm_length = op_pcm_total(op_file.get(), -1);
	log::dbg("Opus channels=%d, pcm_length=%" PRIuPTR, op_channels, static_cast<uintptr_t>(pcm_length));

	// calculate pcm buffer size depending on the number of channels
	// if the opus file only had one channel, the pcm buffer size must be
	// doubled
	uint32_t length = pcm_length * 2;
	pcm_data_t buffer(static_cast<size_t>(length), 0);

	// read data from opus file
	int position = 0;
	while (true) {
		int num_read = op_read(op_file.get(), &buffer.front()+position,
				pcm_length-position, nullptr);
		if (num_read < 0) {
			throw util::Error{"Failed to read from opus file: errorcode=%d", num_read};
		} else if(num_read == 0) {
			break;
		}

		position += num_read * op_channels;
	}

	// convert from mono to stereo
	if (op_channels == 1) {
		for(int i = pcm_length-1; i >= 0; i--) {
			auto value = buffer[i];
			buffer[i*2+1] = value;
			buffer[i*2] = value;
		}
	}

	return std::move(buffer);
}

}
}
