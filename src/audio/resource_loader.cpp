#include "resource_loader.h"

#include <opusfile.h>

#include "../log.h"
#include "../util/error.h"

namespace openage {
namespace audio {

ResourceLoader::ResourceLoader(const std::string &path)
		:
		path{path} {
}

std::unique_ptr<ResourceLoader> ResourceLoader::create_resource_loader(
		const std::string &path, format_t format) {
	std::unique_ptr<ResourceLoader> loader;
	// switch format and return an appropriate loader
	switch (format) {
		case format_t::OPUS:
			loader.reset(new OpusLoader{path});
			break;
		default:
			throw util::Error{"Not supported for format: %d", format};
	}
	return std::move(loader);
}

OpusLoader::OpusLoader(const std::string &path)
		:
		ResourceLoader{path} {
}

// custom deleter for a OggOpusFile unique pointers
static auto opus_deleter = [](OggOpusFile *op_file) {
	if (op_file != nullptr) {
		op_free(op_file);
	}
};

pcm_data_t OpusLoader::get_resource() {
	int op_err;
	// open the opus file
	std::unique_ptr<OggOpusFile, decltype(opus_deleter)> op_file{
			op_open_file(path.c_str(), &op_err), opus_deleter};

	if (op_err != 0) {
		throw util::Error{"Could not open: %s", path.c_str()};
	}

	// determine number of channels and number of pcm samples
	auto op_channels = op_channel_count(op_file.get(), -1);
	auto pcm_length = op_pcm_total(op_file.get(), -1);
	log::dbg("Opus channels=%d, pcm_length=%lu", op_channels, pcm_length);

	// calculate pcm buffer size depending on the number of channels
	// if the opus file only had one channel, the pcm buffer size must be
	// doubled
	auto length_factor = 2 / op_channels;
	std::unique_ptr<int16_t[]> buffer{new int16_t[pcm_length * length_factor]};
	uint32_t length = pcm_length * length_factor;

	// read data from opus file
	int position = 0;
	int num_read = 0;
	while (true) {
		num_read = op_read(op_file.get(), buffer.get()+position,
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

	return make_tuple(std::move(buffer), length);
}

}
}
