// Copyright 2014-2017 the openage authors. See copying.md for legal info.

#include "opus_in_memory_loader.h"

#include <string>

#include <opusfile.h>

#include "error.h"
#include "../log/log.h"


namespace openage {
namespace audio {

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
		throw audio::Error{MSG(err) << "Could not open: " << path};
	}

	auto op_channels = op_channel_count(op_file.get(), -1);
	auto pcm_length = op_pcm_total(op_file.get(), -1);
	// the stream is not seekable
	if (pcm_length < 0) {
		throw audio::Error{MSG(err) << "Opus file is not seekable"};
	}

	// calculate pcm buffer size depending on the number of channels
	// if the opus file only had one channel, the pcm buffer size must be
	// doubled
	size_t length = static_cast<size_t>(pcm_length) * 2;
	pcm_data_t buffer(length, 0);

	// read data from opus file
	int position = 0;
	while (true) {
		int samples_read = op_read(op_file.get(), &buffer.front()+position,
				length-position, nullptr);
		if (samples_read < 0) {
			throw audio::Error{MSG(err) << "Failed to read from opus file: errorcode=" << samples_read};
		} else if (samples_read == 0) {
			break;
		}

		position += samples_read * op_channels;
	}

	// convert from mono to stereo
	if (op_channels == 1) {
		for (int i = pcm_length-1; i >= 0; i--) {
			auto value = buffer[i];
			buffer[i*2+1] = value;
			buffer[i*2] = value;
		}
	}

	return buffer;
}

}} // openage::audio
