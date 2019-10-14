// Copyright 2017-2019 the openage authors. See copying.md for legal info.

#include "filelike.h"

namespace openage::util::filelike {


FileLike::FileLike() = default;

bool FileLike::is_python_native() const noexcept {
	return false;
}

} // openage::util::filelike
