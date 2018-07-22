// Copyright 2015-2018 the openage authors. See copying.md for legal info.

#pragma once

#include "simple_object.h"


namespace openage {
namespace renderer {
namespace opengl {

/// Represents an OpenGL buffer of memory
/// allocated on the GPU.
class GlBuffer final : public GlSimpleObject {
public:
	/// Creates an empty buffer of the specified size.
	/// Binds the GL_COPY_WRITE_BUFFER target.
	GlBuffer(size_t size, GLenum usage = GL_STATIC_DRAW);

	/// Creates a buffer of the specified size and fills it with the given data.
	/// Binds the GL_COPY_WRITE_BUFFER target.
	GlBuffer(const uint8_t *data, size_t size, GLenum usage = GL_STATIC_DRAW);

	/// The size in bytes of this buffer.
	size_t get_size() const;

	/// Uploads `size` bytes of new data starting at `offset`.
	/// `offset + size` has to be less than or equal to `get_size()`.
	/// Binds the GL_COPY_WRITE_BUFFER target.
	void upload_data(const uint8_t *data, size_t offset, size_t size);

	/// Bind this buffer to the specified GL target.
	void bind(GLenum target) const;

private:
	/// The size in bytes of this buffer.
	size_t size;

	/// The GL usage hint for this buffer.
	GLenum usage;
};

}}} // openage::renderer::opengl
