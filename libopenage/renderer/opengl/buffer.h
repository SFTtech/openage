// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <experimental/optional>

#include <epoxy/gl.h>


namespace openage {
namespace renderer {
namespace opengl {

/// OpenGL data buffer on the GPU.
class GlBuffer {
public:
	/// Creates an empty buffer of the specified size.
	GlBuffer(size_t size, GLenum usage = GL_STATIC_DRAW);

	/// Creates a buffer of the specified size and fills it with the given data.
	GlBuffer(const uint8_t *data, size_t size, GLenum usage = GL_STATIC_DRAW);

	~GlBuffer();

	/// Moving is supported.
	GlBuffer &operator =(GlBuffer&&);
	GlBuffer(GlBuffer&&);

	// TODO support copies
	GlBuffer(const GlBuffer&) = delete;
	GlBuffer &operator =(const GlBuffer&) = delete;

	/// The size in bytes of this buffer.
	size_t get_size() const;

	/// Uploads `size` bytes of new data starting at `offset`.
	/// `offset + size` has to be less than or equal to `get_size()`.
	void upload_data(const uint8_t *data, size_t offset, size_t size);

	/// Bind this buffer to the specified GL target.
	void bind(GLenum target) const;

private:
	/// The OpenGL handle to this buffer. Can be empty if the object was moved out of.
	std::experimental::optional<GLuint> id;

	/// The size in bytes of this buffer.
	size_t size;

	/// The GL usage hint for this buffer.
	GLenum usage;
};

}}} // openage::renderer::opengl
