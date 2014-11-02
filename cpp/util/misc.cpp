// Copyright 2013-2014 the openage authors. See copying.md for legal info.

#include "misc.h"

#include <stdlib.h>

namespace openage {
namespace util {

int random_range(int lower, int upper) {
	return (rand() % (upper - lower)) + lower;
}

} //namespace util
} //namespace openage
