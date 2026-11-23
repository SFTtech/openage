// Copyright 2017-2019 the openage authors. See copying.md for legal info.

#include "fslike.h"

#include "../path.h"


namespace openage::util::fslike {

FSLike::FSLike() = default;

Path FSLike::root() {
	return Path{{}, this->shared_from_this()};
}


std::pair<bool, Path> FSLike::resolve_r(const Path::subpath_t &subpath) {
	if (this->is_file(subpath) or this->is_dir(subpath)) {
		return std::make_pair(true, Path{subpath, this->shared_from_this()});
	}
	else {
		return std::make_pair(false, Path{});
	}
}


std::pair<bool, Path> FSLike::resolve_w(const Path::subpath_t &subpath) {
	if (this->writable(subpath)) {
		return std::make_pair(true, Path{subpath, this->shared_from_this()});
	}
	else {
		return std::make_pair(false, Path{});
	}
}


bool FSLike::is_python_native() const noexcept {
	return false;
}


} // openage::util::fslike
