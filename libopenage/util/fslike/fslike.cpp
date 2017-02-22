// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "fslike.h"

#include "../path.h"


namespace openage {
namespace util {
namespace fslike {

FSLike::FSLike() {}

Path FSLike::root() {
	return Path{this->shared_from_this(), {}};
}


std::pair<bool, Path> FSLike::resolve_r(const Path::parts_t &parts) {
	if (this->is_file(parts) or this->is_dir(parts)) {
		return std::make_pair(true, Path{this->shared_from_this(), parts});
	}
	else {
		return std::make_pair(false, Path{});
	}
}


std::pair<bool, Path> FSLike::resolve_w(const Path::parts_t &parts) {
	if (this->writable(parts)) {
		return std::make_pair(true, Path{this->shared_from_this(), parts});
	}
	else {
		return std::make_pair(false, Path{});
	}
}


}}} // openage::util::fslike
