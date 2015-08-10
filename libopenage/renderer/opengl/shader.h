// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_OPENGL_SHADER_H_
#define OPENAGE_RENDERER_OPENGL_SHADER_H_

#include <epoxy/gl.h>

#include "../shader.h"

namespace openage {
namespace renderer {
namespace opengl {

class Shader {
public:
	/**
	 * Create an OpenGL shader from a source code string.
	 */
	Shader(GLenum type, const char *source);

	/**
	 * Create an OpenGL shader from a ShaderSource object.
	 */
	Shader(const ShaderSource &source);

	// don't allow copying as the shader has a context-bound opengl handle id
	Shader(const Shader &copy) = delete;
	Shader &operator=(const Shader &copy) = delete;

	// moving could be enabled i guess, but beware the destructor call.
	Shader(Shader &&other) = delete;
	Shader &operator=(Shader &&other) = delete;

	virtual ~Shader();

	/**
	 * Return the shader type name.
	 */
	const char *typestring();

	/**
	 * OpenGL handle id.
	 */
	GLuint id;

protected:
	/**
	 * actually create the shader.
	 */
	void create_from_source(const char *source);

	/**
	 * OpenGL shader type enum.
	 */
	GLenum type;
};


class VertexShader : public Shader {
public:
	VertexShader(const char *source);
	virtual ~VertexShader() {};
};


class FragmentShader : public Shader {
public:
	FragmentShader(const char *source);
	virtual ~FragmentShader() {};
};


class GeometryShader : public Shader {
public:
	GeometryShader(const char *source);
	virtual ~GeometryShader() {};
};


}}} // openage::renderer::opengl

#endif
