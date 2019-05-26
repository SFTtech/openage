// Copyright 2013-2019 the openage authors. See copying.md for legal info.

#include "color.h"

#include <epoxy/gl.h>

namespace openage {
namespace util {

col::col(gamedata::palette_color c) {
	this->r = c.r;
	this->g = c.g;
	this->b = c.b;
	this->a = c.a;
}

void col::use() {
	//\todo use glColor4b
	glColor4f(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
}

void col::use(float alpha) {
	glColor4f(r / 255.f, g / 255.f, b / 255.f, alpha);
}

}} // openage::util
