#include "program.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "../../log/log.h"
#include "../../util/filetools.h"
#include "../../util/strings.h"
#include "../../util/error.h"

namespace openage {
namespace engine {
namespace shader {

Program::Program() : is_linked(false), vert(nullptr), frag(nullptr), geom(nullptr) {
	this->id = glCreateProgram();
}

Program::Program(Shader *s0, Shader *s1) : Program{} {
	this->attach_shader(s0);
	this->attach_shader(s1);
}

Program::~Program() {
	glDeleteProgram(this->id);
	if (this->vert != nullptr) {
		delete this->vert;
	}
	if (this->frag != nullptr) {
		delete this->frag;
	}
	if (this->geom != nullptr) {
		delete this->geom;
	}
}

void Program::attach_shader(Shader *s) {
	switch (s->type) {
	case GL_VERTEX_SHADER:
		this->vert = s;
		break;
	case GL_FRAGMENT_SHADER:
		this->frag = s;
		break;
	case GL_GEOMETRY_SHADER:
		this->geom = s;
		break;
	}
	glAttachShader(this->id, s->id);
}

void Program::link() {
	glLinkProgram(this->id);
	this->check(GL_LINK_STATUS);
	glValidateProgram(this->id);
	this->check(GL_VALIDATE_STATUS);
	this->is_linked = true;
}

void Program::check(GLenum what_to_check) {
	GLint status;
	glGetProgramiv(this->id, what_to_check, &status);

	if (status != GL_TRUE) {
		GLint loglen;
		glGetProgramiv(this->id, GL_INFO_LOG_LENGTH, &loglen);
		char *infolog = new char[loglen];
		glGetProgramInfoLog(this->id, loglen, NULL, infolog);

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

		util::Error e("Program %s failed\n%s", what_str, infolog);
		delete[] infolog;
		throw e;
	}
}

void Program::use() {
	glUseProgram(this->id);
}

void Program::stopusing() {
	glUseProgram((GLuint) 0);
}

GLint Program::get_uniform_id(const char *name) {
	return glGetUniformLocation(this->id, name);
}

GLint Program::get_attribute_id(const char *name) {
	if (this->is_linked) {
		GLint aid = glGetAttribLocation(this->id, name);
		if (aid == -1) {
			throw util::Error("queried attribute '%s' not found.", name);
		} else {
			return aid;
		}
	}
	else {
		throw util::Error("queried attribute '%s' id before program was linked.", name);
	}
}

GLuint Program::get_id() {
	return this->id;
}

} //namespace shader
} //namespace engine
} //namespace openage
