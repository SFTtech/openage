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
 * opus_file_t is a OggOpusFile pointer that is stored inside a unique_ptr and
 * uses a custom deleter.
 */
using opus_file_t = std::unique_ptr<OggOpusFile, std::function<void(OggOpusFile*)>>;


/**
 * Opens a opus file. Its location is specified by the path stored in the
 * DynamicLoader.
 */
opus_file_t open_opus_file(const util::Path &path);

}} // openage::audio
