// Copyright 2014-2019 the openage authors. See copying.md for legal info.

#include "opus_dynamic_loader.h"

#include <opus/opusfile.h>

#include "error.h"
#include "../log/log.h"

namespace openage::audio {


OpusDynamicLoader::OpusDynamicLoader(const util::Path &path)
	:
	DynamicLoader{path},
	source{open_opus_file(path)} {

	// read channels from the opus file
	channels = op_channel_count(this->source.handle.get(), -1);

	int64_t pcm_length = op_pcm_total(this->source.handle.get(), -1);
	if (pcm_length < 0) {
		throw audio::Error{
			ERR << "Could not seek in "
			    << path << ": " << pcm_length};
	}

	length = static_cast<size_t>(pcm_length) * 2;
}


size_t OpusDynamicLoader::load_chunk(int16_t *chunk_buffer,
                                     size_t offset,
                                     size_t chunk_size) {

	// if the requested offset is greater than the resource's length, there is
	// no chunk left to load
	if (offset > this->length) {
		return 0;
	}

	// seek to the requested offset, the seek offset is given in samples
	// while the requested offset is given in int16_t values, so the division
	// by 2 is necessary
	auto pcm_offset = static_cast<int64_t>(offset / 2);

	int op_ret = op_pcm_seek(this->source.handle.get(), pcm_offset);
	if (op_ret < 0) {
		throw audio::Error{
			ERR << "Could not seek in " << this->path << ": " << op_ret
		};
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
			this->source.handle.get(),
			chunk_buffer + read_count,
			read_num_values - read_count,
			nullptr
		);

		// an error occurred
		if (samples_read < 0) {
			throw audio::Error{
				ERR << "Could not read from "
				    << this->path << ": " << samples_read
			};
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
		for (int i = read_count-1; i >= 0; i--) {
			auto value = chunk_buffer[i];
			chunk_buffer[i*2+1] = value;
			chunk_buffer[i*2] = value;
		}
	}

	return (read_count * 2) / channels;
}

} // openage::audio
