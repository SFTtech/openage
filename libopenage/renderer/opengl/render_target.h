// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "../renderer.h"
#include "texture.h"
#include "../../error/error.h"


namespace openage {
namespace renderer {
namespace opengl {

enum class gl_render_target_t {
	window,
	texture,
	// TODO MRT
};

class GlRenderTarget : public RenderTarget {
public:
	/// Construct a render target that renders into the framebuffer (the screen).
	GlRenderTarget()
		: type(gl_render_target_t::window)
		, handle(0) {}

	// TODO the validity of this object is contingent
	// on its texture existing. use shared_ptr?
	GlRenderTarget(std::vector<const GlTexture*> textures) {
		glGenFramebuffers(1, &this->handle);
		glBindFramebuffer(GL_FRAMEBUFFER, this->handle);

		for (size_t i = 0; i < textures.size(); i++) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textures[i]->get_handle(), 0);
		}

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			throw Error(MSG(err) << "Could not create OpenGL framebuffer.");
		}
	}

	~GlRenderTarget() {
		if (type == gl_render_target_t::texture) {
			glDeleteFramebuffers(1, &this->handle);
		}
	}

	void bind_read() const {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, this->handle);
	}

	void bind_write() const {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->handle);
	}

private:
	gl_render_target_t type;

	GLuint handle;
};

}}}
