// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#include "../../config.h"
#if WITH_OPENGL

#include "shader.h"

#include "../../error/error.h"
#include "../../log/log.h"
#include "../../util/compiler.h"
#include "../../util/file.h"
#include "../../util/strings.h"

namespace openage {
namespace renderer {
namespace opengl {

Shader::Shader(const ShaderSource &source) {
	// assign gl shader type
	switch (source.get_type()) {
	case shader_type::vertex:
		this->type = GL_VERTEX_SHADER;
		break;

	case shader_type::fragment:
		this->type = GL_FRAGMENT_SHADER;
		break;

	case shader_type::geometry:
		this->type = GL_GEOMETRY_SHADER;
		break;

	case shader_type::tesselation_control:
		this->type = GL_TESS_CONTROL_SHADER;
		break;

	case shader_type::tesselation_evaluation:
		this->type = GL_TESS_EVALUATION_SHADER;
		break;

	default:
		throw Error{MSG(err) << "Unknown shader type requested."};
	}

	// create shader from source code!
	this->create_from_source(source.get_source());
}

Shader::Shader(GLenum type, const char *source)
	:
	type{type} {

	// create the shader!
	this->create_from_source(source);
}

void Shader::create_from_source(const char *source) {
	// allocate shader in opengl
	this->id = glCreateShader(this->type);

	if (unlikely(this->id == 0)) {
		throw Error{MSG(err) << "no gl shader created! wtf?", true};
	}

	// load shader source
	glShaderSource(this->id, 1, &source, NULL);

	// compile shader source
	glCompileShader(this->id);

	// check compiliation result
	GLint status;
	glGetShaderiv(this->id, GL_COMPILE_STATUS, &status);

	if (status != GL_TRUE) {
		GLint loglen;
		glGetShaderiv(this->id, GL_INFO_LOG_LENGTH, &loglen);

		auto infolog = std::make_unique<char[]>(loglen);
		glGetShaderInfoLog(this->id, loglen, NULL, infolog.get());

		auto errmsg = MSG(err);
		errmsg << "Failed to compile shader:\n" << infolog;

		glDeleteShader(this->id);

		throw Error{errmsg, true};
	}
}

Shader::~Shader() {
	glDeleteShader(this->id);
}

const char *Shader::typestring() {
	switch (this->type) {
	case GL_VERTEX_SHADER:
		return "vertex shader";
	case GL_FRAGMENT_SHADER:
		return "fragment shader";
	case GL_GEOMETRY_SHADER:
		return "geometry shader";
	case GL_TESS_CONTROL_SHADER:
		return "tesselation control shader";
	case GL_TESS_EVALUATION_SHADER:
		return "tesselation evaluation shader";
	default:
		return "unknown shader type";
	}
}


VertexShader::VertexShader(const char *source)
	:
	Shader{GL_VERTEX_SHADER, source} {}


FragmentShader::FragmentShader(const char *source)
	:
	Shader{GL_FRAGMENT_SHADER, source} {}


GeometryShader::GeometryShader(const char *source)
	:
	Shader{GL_GEOMETRY_SHADER, source} {}


}}} // openage::renderer::opengl

#endif // WITH_OPENGL
