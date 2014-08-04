#define GEN_IMPL_TERM_CPP
#include "term.h"

#include "camhud.h"
#include "../console/console.h"

namespace openage {
namespace coord {

camhud term::to_camhud() {
	camhud result;
	result.x = console::bottomleft.x + x * console::charsize.x; 
	result.y = console::topright.y   - y * console::charsize.y; 
	return result;
}

} //namespace coord
} //namespace openage
