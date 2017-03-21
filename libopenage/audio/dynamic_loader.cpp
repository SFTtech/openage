// Copyright 2014-2017 the openage authors. See copying.md for legal info.

#include "dynamic_loader.h"

#include "error.h"
#include "opus_dynamic_loader.h"

namespace openage {
namespace audio {


DynamicLoader::DynamicLoader(const util::Path &path)
	:
	path{path} {}


std::unique_ptr<DynamicLoader> DynamicLoader::create(const util::Path &path,
                                                     format_t format) {
	std::unique_ptr<DynamicLoader> loader;

	switch (format) {
	case format_t::OPUS:
		loader = std::make_unique<OpusDynamicLoader>(path);
		break;

	default:
		throw audio::Error{
			ERR << "No dynamic audio loader for format supported: " << format
		};
	}
	return loader;
}


}} // openage::audio
