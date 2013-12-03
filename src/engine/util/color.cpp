#include "color.h"

#include <GL/gl.h>

namespace engine {
namespace util {

void col::use() {
	//TODO use glColor4b
	glColor4f(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
}

} //namespace util
} //namespace engine
