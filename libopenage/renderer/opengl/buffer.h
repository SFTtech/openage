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
 * Context-specialized graphics system buffer.
 *
 * TODO: auto-dirty flagging when modified.
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
	void upload(bind_target target) const override;

	static GLenum get_target(bind_target target);

protected:
	GLuint id;
};

}}} // openage::renderer::opengl

#endif
