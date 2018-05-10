// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "simple_object.h"


namespace openage {
namespace renderer {
namespace opengl {

/// Represents an OpenGL buffer of memory
/// allocated on the GPU.
class GlIndexBuffer final : public GlSimpleObject {
public:
	/// Creates an empty buffer of the specified size.
	GlIndexBuffer(size_t size, GLenum usage = GL_STATIC_DRAW);
    
    /// Creates a buffer of the specified size and fills it with the given data.
	GlIndexBuffer(const GLuint *data, size_t size, GLenum usage = GL_STATIC_DRAW);


	void upload_data(const GLuint *data, size_t offset, size_t size);

	/// The size in bytes of this buffer.
	size_t get_size() const;


	/// Bind this buffer to the specified GL target.
	void bind() const;

    void unbind() const;

private:
	/// The size in bytes of this buffer.
	size_t size;

	/// The GL usage hint for this buffer.
	GLenum usage;
};

}}} // openage::renderer::opengl
