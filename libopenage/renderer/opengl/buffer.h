// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_OPENGL_BUFFER_H_
#define OPENAGE_RENDERER_OPENGL_BUFFER_H_

#include "../buffer.h"

#include <epoxy/gl.h>
#include <memory>

namespace openage {
namespace renderer {
namespace opengl {

/**
 * OpenGL data buffer.
 */
class Buffer : public renderer::Buffer {
public:
	Buffer(renderer::Context *ctx, size_t size=0);
	virtual ~Buffer();

	Buffer(const Buffer &other) = delete;
	Buffer(Buffer &&other) = delete;
	Buffer &operator =(const Buffer &other) = delete;
	Buffer &operator =(Buffer &&other) = delete;

	/**
	 * Uploads the current state of the buffer to the GPU.
	 */
	void upload(bind_target target, usage usage) override;

	/**
	 * Bind this buffer to the specified slot.
	 */
	void bind(bind_target target) const override;

	/**
	 * Fetch the OpenGL specific buffer slot identification.
	 */
	static GLenum get_target(bind_target target);

	/**
	 * Fetch the OpenGL specific buffer usage prediction id.
	 */
	static GLenum get_usage(usage usage);

protected:
	GLuint id;
};

}}} // openage::renderer::opengl

#endif
