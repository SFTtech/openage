// Copyright 2013-2019 the openage authors. See copying.md for legal info.

#include "program.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "../error/error.h"
#include "../log/log.h"
#include "../util/compiler.h"
#include "../util/file.h"
#include "../util/strings.h"

#include "shader.h"

namespace openage::shader {

Program::Program() : is_linked(false), vert(nullptr), frag(nullptr), geom(nullptr) {
	this->id = glCreateProgram();
}

Program::Program(Shader *s0, Shader *s1) : Program{} {
	this->attach_shader(s0);
	this->attach_shader(s1);
}

Program::~Program() {
	glDeleteProgram(this->id);
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
	this->post_link_hook();

	if (this->vert != nullptr) {
		glDetachShader(this->id, this->vert->id);
	}
	if (this->frag != nullptr) {
		glDetachShader(this->id, this->frag->id);
	}
	if (this->geom != nullptr) {
		glDetachShader(this->id, this->geom->id);
	}
}

/**
 * checks a given status for this program.
 *
 * @param what_to_check GL_LINK_STATUS GL_VALIDATE_STATUS GL_COMPILE_STATUS
 */
void Program::check(GLenum what_to_check) {
	GLint status;
	glGetProgramiv(this->id, what_to_check, &status);

	if (status != GL_TRUE) {
		GLint loglen;
		glGetProgramiv(this->id, GL_INFO_LOG_LENGTH, &loglen);
		char *infolog = new char[loglen];
		glGetProgramInfoLog(this->id, loglen, nullptr, infolog);

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

		auto errormsg = MSG(err);
		errormsg << "Program " << what_str << " failed\n" << infolog;
		delete[] infolog;

		throw Error(errormsg);
	}
}

void Program::use() {
	glUseProgram(this->id);
}

void Program::stopusing() {
	glUseProgram(static_cast<GLuint>(0));
}

GLint Program::get_uniform_id(const char *name) {
	return glGetUniformLocation(this->id, name);
}

GLint Program::get_attribute_id(const char *name) {
	if (unlikely(!this->is_linked)) {
		throw Error(MSG(err) <<
			"Attribute " << name <<
			" was queried before program was linked!");
	}

	GLint aid = glGetAttribLocation(this->id, name);

	if (unlikely(aid == -1)) {
		this->dump_active_attributes();
		throw Error(MSG(err) <<
			"Attribute " << name << " queried but not found or active"
			" (pwnt by the compiler).");
	}

	return aid;
}

void Program::set_attribute_id(const char *name, GLuint id) {
	if (!this->is_linked) {
		glBindAttribLocation(this->id, id, name);
	}
	else {
		//TODO: maybe enable overwriting, but after that relink the program
		throw Error(MSG(err) << "assigned attribute " << name << " = " << id
		                     << " after program was linked!");
	}
}

void Program::dump_active_attributes() {
	auto msg = MSG(warn);
	msg << "Dumping shader program active attribute list:";

	GLint num_attribs;
	glGetProgramiv(this->id, GL_ACTIVE_ATTRIBUTES, &num_attribs);

	GLint attrib_max_length;
	glGetProgramiv(this->id, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &attrib_max_length);

	for (int i = 0; i < num_attribs; i++) {
		GLsizei attrib_length;
		GLint attrib_size;
		GLenum attrib_type;
		char *attrib_name = new char[attrib_max_length];
		glGetActiveAttrib(this->id, i, attrib_max_length, &attrib_length,
		                  &attrib_size, &attrib_type, attrib_name);

		msg << "\n" <<
			"-> attribute " << attrib_name << ": "
			" : type=" << attrib_type << ", size=" << attrib_size;
		delete[] attrib_name;
	}
}


void Program::post_link_hook() {
	this->pos_id  = this->get_attribute_id("vertex_position");
	this->mvpm_id = this->get_uniform_id("mvp_matrix");
}

} // openage::shader
