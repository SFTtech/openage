// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_OPENGL_VERTEX_STATE_H_
#define OPENAGE_RENDERER_OPENGL_VERTEX_STATE_H_

#include <epoxy/gl.h>

#include "../vertex_buffer.h"
#include "../vertex_state.h"

namespace openage {
namespace renderer {
namespace opengl {

/**
 * OpenGL-specific vertex array object management.
 * This wraps a vertex array object (vao).
 */
class VertexState : public renderer::VertexState {
public:
	VertexState(renderer::Context *ctx);
	virtual ~VertexState();

	/**
	 * Do attribute buffer section assignments.
	 * This sets pointers to the given buffer.
	 */
	void attach_buffer(const VertexBuffer &buf) override;

	/**
	 * Remove attribute buffer section assignments.
	 */
	void detach_buffer(const VertexBuffer &buf) override;

	/**
	 * Make this vertex array object the current one.
	 */
	void bind() const override;

	/**
	 * Convert the buffer section data type to the OpenGL enum.
	 */
	static GLenum get_attribute_type(vertex_attribute_type type);

protected:
	/**
	 * Internal OpenGL handle id.
	 */
	GLuint id;
};

}}} // openage::renderer::opengl

#endif
