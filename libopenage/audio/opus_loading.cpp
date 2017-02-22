// Copyright 2014-2017 the openage authors. See copying.md for legal info.

#include "opus_loading.h"

#include "error.h"
#include "../log/log.h"

namespace openage {
namespace audio {

// custom deleter for OggOpusFile unique pointers
static auto opus_deleter = [](OggOpusFile *op_file) {
	if (op_file != nullptr) {
		op_free(op_file);
	}
};


opus_file_t open_opus_file(const util::Path &path) {
	int op_err;

	if (not path.is_file()) {
		throw audio::Error{
			ERR << "Audio file not found: " << path
		};
	}

	// TODO: if there is not native path (e.g. inside an archive)
	//       this will fail hard.
	auto native_path = path.get_native_path();

	opus_file_t op_file{
		op_open_file(native_path.c_str(), &op_err),
		opus_deleter
	};

	if (op_err != 0) {
		throw audio::Error{
			ERR
			<< "Could not open opus file: "
			<< path << " = '" << native_path << "'"
		};
	}
	return op_file;
}

}} // openage::audio
