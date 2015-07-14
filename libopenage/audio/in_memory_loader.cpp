// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include "in_memory_loader.h"

#include "opus_in_memory_loader.h"
#include "../error/error.h"

namespace openage {
namespace audio {


InMemoryLoader::InMemoryLoader(const std::string &path)
	:
	path{path} {}


std::unique_ptr<InMemoryLoader> InMemoryLoader::create(const std::string &path,
                                                       format_t format) {

	std::unique_ptr<InMemoryLoader> loader;

	// switch format and return an appropriate loader
	switch (format) {
	case format_t::OPUS:
		loader.reset(new OpusInMemoryLoader{path});
		break;
	default:
		throw Error{MSG(err) << "Not supported for format: " << format};
	}

	return loader;
}

}} // namespace openage::audio
