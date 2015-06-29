// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_OPENGL_PROGRAM_H_
#define OPENAGE_RENDERER_OPENGL_PROGRAM_H_

#include <epoxy/gl.h>
#include <vector>

#include "shader.h"

namespace openage {
namespace renderer {
namespace opengl {

class Program {
public:
	GLuint id;

	Program();
	virtual ~Program();

	/**
	 * add a shader to be included in this program.
	 */
	void attach_shader(const opengl::Shader &s);

	/**
	 * combine all attached shaders and link them
	 * to a usable program.
	 */
	void link();

	/**
	 * activate the program on the gpu.
	 */
	void use();
	void stopusing();

	GLint get_uniform_id(const char *name);
	GLint get_uniformbuffer_id(const char *name);
	GLint get_attribute_id(const char *name);

	void set_attribute_id(const char *name, GLuint id);

	void dump_active_attributes();

private:
	/**
	 * True when this program was linked.
	 */
	bool is_linked;

	/**
	 * Shaders attached to this program.
	 */
	std::vector<GLuint> shader_ids;

	/**
	 * checks a given status for this program.
	 *
	 * @param what_to_check GL_LINK_STATUS GL_VALIDATE_STATUS GL_COMPILE_STATUS
	 */
	void check(GLenum what_to_check);

	/**
	 * check if this program was linked already.
	 */
	void check_is_linked(const char *info="");


	GLint get_info(GLenum pname);
	char *get_log();
	void post_link_hook();
};


}}} // openage::renderer::opengl

#endif
