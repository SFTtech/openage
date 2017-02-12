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


}}} // openage::util::fslike
