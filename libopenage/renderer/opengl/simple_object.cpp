// Copyright 2015-2018 the openage authors. See copying.md for legal info.

#include "simple_object.h"

#include <utility>


namespace openage {
namespace renderer {
namespace opengl {

GlSimpleObject::GlSimpleObject(std::function<void(GLuint)> delete_fun)
	: delete_fun(std::move(delete_fun)) {}

GlSimpleObject::GlSimpleObject(GlSimpleObject&& other)
	: handle(other.handle)
	, delete_fun(std::move(other.delete_fun)) {
	other.handle = {};
}

GlSimpleObject &GlSimpleObject::operator =(GlSimpleObject&& other) {
	if (this->handle) {
		this->delete_fun(*this->handle);
	}

	this->handle = other.handle;
	this->delete_fun = std::move(other.delete_fun);
	other.handle = {};

	return *this;
}

GlSimpleObject::~GlSimpleObject() {
	if (this->handle) {
		this->delete_fun(*this->handle);
	}
}

GLuint GlSimpleObject::get_handle() const {
	return *this->handle;
}

}}} // openage::renderer::opengl
