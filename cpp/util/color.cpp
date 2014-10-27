#include "color.h"

#include <GL/glew.h>
#include "../crossplatform/opengl.h"

namespace openage {
namespace util {

col::col(gamedata::palette_color c) {
	this->r = c.r;
	this->g = c.g;
	this->b = c.b;
	this->a = c.a;
}

void col::use() {
	//TODO use glColor4b
	glColor4f(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
}

void col::use(float alpha) {
	glColor4f(r / 255.f, g / 255.f, b / 255.f, alpha);
}

} //namespace util
} //namespace openage
