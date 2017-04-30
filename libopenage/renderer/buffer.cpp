// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "buffer.h"

namespace openage {
namespace renderer {

Buffer::Buffer(/*Context *ctx,*/ size_t size)
	:
	//context{ctx},
	allocd{size},
	on_gpu{false} {

	if (size > 0) {
		this->create(size);
	}
}

size_t Buffer::size() const {
	return this->allocd;
}

char *Buffer::get(bool mark_dirty) {
	if (mark_dirty) {
		this->mark_dirty();
	}
	return this->buffer.get();
}

void Buffer::mark_dirty() {
	this->on_gpu = false;
}

void Buffer::create(size_t size) {
	this->buffer = std::make_unique<char[]>(size);
	this->allocd = size;
}

}} // openage::renderer
