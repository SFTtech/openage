// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include "in_memory_loader.h"

#include "opus_in_memory_loader.h"
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

}
}
