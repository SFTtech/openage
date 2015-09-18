// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_OPENGL_PROGRAM_H_
#define OPENAGE_RENDERER_OPENGL_PROGRAM_H_

#include <epoxy/gl.h>
#include <unordered_map>
#include <vector>

#include "../program.h"
#include "shader.h"

namespace openage {
namespace renderer {

class Context;

namespace opengl {

class Program : public renderer::Program {
public:
	/**
	 * A program is created from shader sources.
	 */
	Program(renderer::Context *context, const ProgramSource &source);
	virtual ~Program();

	/**
	 * Activate the program on the GPU.
	 */
	void use() override;

	/**
	 * Return the opengl handle id for a given pipeline uniform variable.
	 */
	GLint get_uniform_id(const char *name);

	/**
	 * Return the opengl handle id for a given uniform buffer object name.
	 */
	GLint get_uniformbuffer_id(const char *name);

	/**
	 * Return the opengl layout id for a given vertex attribute name.
	 */
	int get_attribute_id(const char *name) override;

	/**
	 * Set vertex attribute with given name to have a custom id.
	 */
	void set_attribute_id(const char *name, GLuint id);

	/**
	 * Query OpenGL which of the vertex attributes are actually active
	 * and haven't been optimized out by the compiler.
	 */
	void dump_attributes() override;

	/* ====== */
	// shader variables

	/**
	 * Upload the vertex buffer object to the gpu.
	 */
	void set_vertex_buffer(const VertexBuffer &buf) override;

	/**
	 * Set a 3 dimensional float vector
	 */
	void set_uniform_3f(const char *name, const util::Vector<3> &value) override;

	/**
	 * Set a single integer value
	 */
	void set_uniform_1i(const char *name, const int &value) override;

	/**
	 * Set 2d texture data.
	 */
	void set_uniform_2dtexture(const char *name, const renderer::Texture &value) override;


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
	 * Uniform id cache.
	 *
	 * Maps uniform variable name to the handle id.
	 */
	std::unordered_map<const char *, GLint> uniforms;

	/**
	 * checks a given status for this program.
	 *
	 * @param what_to_check can be one of GL_LINK_STATUS
	 *                      GL_VALIDATE_STATUS GL_COMPILE_STATUS
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
