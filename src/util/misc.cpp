#include "misc.h"

#include <stdlib.h>


namespace openage {
namespace util {

int random_range(int lower, int upper) {
	return (rand() - lower) % (upper+1);
}

} //namespace util
} //namespace openage
