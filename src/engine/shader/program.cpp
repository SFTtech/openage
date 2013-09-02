#include "program.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>

#include "../../log/log.h"
#include "../../util/filetools.h"
#include "../../util/strings.h"
#include "../../util/error.h"

namespace openage {
namespace engine {
namespace shader {

Program::Program(const char *name) : name(name) {
	this->id = glCreateProgram();
}

Program::~Program() {
	glDeleteProgram(this->id);
}

void Program::attach_shader(Shader *s) {
	glAttachShader(this->id, s->id);

	if (s->type == shader_fragment) {
		this->hasfshader = true;
	} else if (s->type == shader_vertex) {
		this->hasvshader = true;
	}
}

void Program::link() {
	if ( !hasfshader || !hasvshader) {
		throw util::Error("Program %s is missing a vertex and/or fragment shader and can not be linked", this->name);
	}

	glLinkProgram(this->id);
	this->check(GL_LINK_STATUS);
	glValidateProgram(this->id);
	this->check(GL_VALIDATE_STATUS);
}

void Program::check(GLenum what_to_check) {
	GLint status = get_info(what_to_check); //GL_FALSE or GL_TRUE
	if (status != GL_TRUE) {
		char *infolog = this->get_log();

		const char *what_str;
		switch(what_to_check) {
		case GL_LINK_STATUS:
			what_str = "linking";
			break;
		case GL_VALIDATE_STATUS:
			what_str = "validation";
			break;
		case GL_COMPILE_STATUS:
			what_str = "compiliation";
			break;
		default:
			what_str = "<unknown task>";
			break;
		}

		log::dbg("GL_TRUE == %d", GL_TRUE);
		log::dbg("GL_FALSE == %d", GL_FALSE);

		throw util::Error("Program %s %s failed with status %d\n%s", name, what_str, status, infolog);
		//TODO memory leak here (infolog can not be freed after throwing Error)
		//(solve by using cppstrings everywhere?)
	}
}

GLint Program::get_info(GLenum pname) {
	GLint result;
	glGetProgramiv(this->id, pname, &result);
	return result;
}

char *Program::get_log() {
	GLint loglen = this->get_info(GL_INFO_LOG_LENGTH);
	char *result = (char *) malloc(loglen);
	glGetProgramInfoLog(this->id, loglen, NULL, result);
	return result;
}

void Program::use() {
	if (glIsProgram(this->id) == GL_TRUE) {
		glUseProgram(this->id);
	} else {
		throw util::Error("Could not use program %s", this->name);
	}
}

void Program::stopusing() {
	glUseProgram((GLuint) 0);
}

GLint Program::get_uniform_id(const char *name) {
	return glGetUniformLocation(this->id, name);
}

} //namespace shader
} //namespace engine
} //namespace openage
