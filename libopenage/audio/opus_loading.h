// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>
#include <memory>
#include <opus/opusfile.h>
#include <string>

#include "../util/path.h"


namespace openage {
namespace audio {

/**
 * opusfile handle storage.
 */
struct opus_file_t {
	/**
	 * The opusfile handle, with a custom deleter that frees
	 * the opusfile memory.
	 */
	std::unique_ptr<OggOpusFile, std::function<void(OggOpusFile*)>> handle;

	/**
	 * File used to supply the data.
	 * This is unused if the file is a real file.
	 */
	std::unique_ptr<util::File> file;
};


/**
 * Opens a opus file. Its location is specified by the path stored in the
 * DynamicLoader.
 */
opus_file_t open_opus_file(const util::Path &path);

}} // openage::audio
