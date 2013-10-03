#include "shader.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>

#include "../../log/log.h"
#include "../../util/error.h"
#include "../../util/filetools.h"
#include "../../util/strings.h"

namespace openage {
namespace engine {
namespace shader {

const char *type_to_string(GLenum type) {
	switch (type) {
	case GL_VERTEX_SHADER:
		return "vertex";
	case GL_FRAGMENT_SHADER:
		return "fragment";
	case GL_GEOMETRY_SHADER:
		return "geometry";
	default:
		return "unknown";
	}
}

Shader::Shader(GLenum type, const char *source) {
	//create shader
	id = glCreateShader(type);

	//load shader source
	glShaderSource(id, 1, &source, NULL);

	//compile shader source
	glCompileShader(id);

	//check compiliation result
	GLint status;
	glGetShaderiv(id, GL_COMPILE_STATUS, &status);

	if (status != GL_TRUE) {
		GLint loglen;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &loglen);

		char *infolog = (char *) malloc(loglen);
		glGetShaderInfoLog(id, loglen, NULL, infolog);

		util::Error e("Failed to compile %s shader\n%s", type_to_string(type), infolog);
		free(infolog);
		glDeleteShader(id);

		throw e;
	}
}

Shader::~Shader() {
	glDeleteShader(id);
}

} //namespace shader
} //namespace engine
} //namespace openage
