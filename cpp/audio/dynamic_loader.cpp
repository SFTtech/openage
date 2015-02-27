// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include "dynamic_loader.h"

#include "opus_dynamic_loader.h"
#include "../util/error.h"

namespace openage {
namespace audio {


DynamicLoader::DynamicLoader(const std::string &path)
	:
	path{path} {
}


std::unique_ptr<DynamicLoader> DynamicLoader::create(const std::string &path,
                                                     format_t format) {
	std::unique_ptr<DynamicLoader> loader;
	switch (format) {
	case format_t::OPUS:
		loader.reset(new OpusDynamicLoader{path});
		break;
	default:
		throw util::Error{MSG(err) <<
			"No dynamic audio loader for format supported: " << format};
	}
	return std::move(loader);
}


}
}
