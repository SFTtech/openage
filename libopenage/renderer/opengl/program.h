// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_OPENGL_PROGRAM_H_
#define OPENAGE_RENDERER_OPENGL_PROGRAM_H_

#include <epoxy/gl.h>
#include <vector>

#include "shader.h"

namespace openage {
namespace renderer {
namespace opengl {

class Program : public RawProgram {
public:
	/**
	 * A program is created from shader sources.
	 */
	Program(const ProgramSource &source);

	virtual ~Program();

	/**
	 * Activate the program on the GPU.
	 */
	virtual void use() override;

	/**
	 * Return the opengl handle id for a given pipeline uniform variable.
	 */
	GLint get_uniform_id(const char *name);

	/**
	 * Return the opengl handle id for a given uniform buffer object name.
	 */
	GLint get_uniformbuffer_id(const char *name);

	/**
	 * Return the opengl handle id for a given vertex attribute name.
	 */
	GLint get_attribute_id(const char *name);

	/**
	 * Set vertex attribute with given name to have a custom id.
	 */
	void set_attribute_id(const char *name, GLuint id);

	/**
	 * Query OpenGL which of the vertex attributes are actually active
	 * and haven't been optimized out by the compiler.
	 */
	virtual void dump_attributes() override;

protected:
	/**
	 * add a shader to be included in this program.
	 */
	void attach_shader(const opengl::Shader &s);

	/**
	 * Combine all attached shaders and link them
	 * to a usable program.
	 */
	void link();

	/**
	 * Don't use the program any more.
	 * Return to static pipeline configuration.
	 */
	void stopusing();

	/**
	 * The OpenGL handle id for this program.
	 */
	GLuint id;

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

	/**
	 * Get information about the program.
	 */
	GLint get_info(GLenum pname);
};


}}} // openage::renderer::opengl

#endif
