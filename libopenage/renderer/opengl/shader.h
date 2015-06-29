// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_OPENGL_SHADER_H_
#define OPENAGE_RENDERER_OPENGL_SHADER_H_

#include <epoxy/gl.h>

namespace openage {
namespace renderer {
namespace opengl {

class Shader {
public:
	Shader(GLenum type, const char *source);

	Shader(const Shader &copy) = delete;
	Shader(Shader &&other) = delete;
	Shader &operator=(const Shader &copy) = delete;
	Shader &operator=(Shader &&other) = delete;

	virtual ~Shader();

	/**
	 * Return the shader type name.
	 */
	virtual const char *typestring() = 0;

	GLuint id;
	GLenum type;
};


class VertexShader : public Shader {
public:
	VertexShader(const char *source);
	virtual ~VertexShader() {};

	virtual const char *typestring() {
		return "vertex shader";
	}
};


class FragmentShader : public Shader {
public:
	FragmentShader(const char *source);
	virtual ~FragmentShader() {};

	virtual const char *typestring() {
		return "fragment shader";
	}
};


class GeometryShader : public Shader {
public:
	GeometryShader(const char *source);
	virtual ~GeometryShader() {};

	virtual const char *typestring() {
		return "geometry shader";
	}
};


}}} // openage::renderer::opengl

#endif
