// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "filelike.h"

namespace openage {
namespace util {
namespace filelike {


FileLike::FileLike() {}

bool FileLike::is_python_native() const noexcept {
	return false;
}

}}} // openage::util::filelike
