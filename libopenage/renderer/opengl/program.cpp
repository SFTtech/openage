// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#include "program.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "../../log/log.h"
#include "../../error/error.h"
#include "../../util/compiler.h"
#include "../../util/file.h"
#include "../../util/strings.h"

namespace openage {
namespace renderer {
namespace opengl {

Program::Program()
	:
	is_linked{false} {

	this->id = glCreateProgram();
}

Program::~Program() {
	glDeleteProgram(this->id);
}

void Program::attach_shader(const Shader &shader) {
	if (unlikely(this->is_linked)) {
		throw Error{MSG(err) << "can't attach shader to linked program", true};
	}

	this->shader_ids.push_back(shader.id);
	glAttachShader(this->id, shader.id);
}

void Program::link() {
	if (unlikely(this->is_linked)) {
		throw Error{MSG(err) << "can't relink a program", true};
	}

	// link shaders to a program
	glLinkProgram(this->id);
	this->check(GL_LINK_STATUS);

	// check if program is usable
	glValidateProgram(this->id);
	this->check(GL_VALIDATE_STATUS);

	this->is_linked = true;
	this->post_link_hook();

	for (auto &id : this->shader_ids) {
		glDetachShader(this->id, id);
	}
}

void Program::check(GLenum what_to_check) {
	GLint status;
	glGetProgramiv(this->id, what_to_check, &status);

	if (status != GL_TRUE) {
		GLint loglen;
		glGetProgramiv(this->id, GL_INFO_LOG_LENGTH, &loglen);

		auto infolog = std::make_unique<char[]>(loglen);
		glGetProgramInfoLog(this->id, loglen, NULL, infolog.get());

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

		throw Error{MSG(err) << "Program " << what_str << " failed:\n" << infolog, true};
	}
}

void Program::use() {
	if (unlikely(not this->is_linked)) {
		throw Error{MSG(err) << "using none-linked program!", true};
	}
	glUseProgram(this->id);
}

void Program::stopusing() {
	glUseProgram((GLuint) 0);
}

void Program::check_is_linked(const char *info) {
	// just throw up when we're not linked yet.

	if (unlikely(not this->is_linked)) {
		throw Error{MSG(err) << info << " before program was linked!", true};
	}
}

GLint Program::get_uniform_id(const char *name) {
	this->check_is_linked("Uniform id requested");
	return glGetUniformLocation(this->id, name);
}

GLint Program::get_uniformbuffer_id(const char *name) {
	this->check_is_linked("Uniform buffer requested");
	return glGetUniformBlockIndex(this->id, name);
}

GLint Program::get_attribute_id(const char *name) {
	this->check_is_linked("Vertex attribute requested");

	GLint aid = glGetAttribLocation(this->id, name);

	if (unlikely(aid == -1)) {
		this->dump_active_attributes();
		throw Error{MSG(err) << "Attribute " << name
		                     << " queried but not found or active"
		                     << " (optimized out by the compiler?).", true};
	}

	return aid;
}

void Program::set_attribute_id(const char *name, GLuint id) {
	if (unlikely(this->is_linked)) {
		// TODO: maybe enable overwriting, but after that relink the program
		throw Error{MSG(err)
			<< "assigned attribute " << name << " = "
			<< id << " after program was linked!", true};
	}
	else {
		glBindAttribLocation(this->id, id, name);
	}
}

void Program::dump_active_attributes() {
	auto msg = MSG(info);
	msg << "Dumping shader program " << this->id << " active attribute list:";

	GLint num_attribs;
	glGetProgramiv(this->id, GL_ACTIVE_ATTRIBUTES, &num_attribs);

	GLint attrib_max_length;
	glGetProgramiv(this->id, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &attrib_max_length);

	for (int i = 0; i < num_attribs; i++) {
		GLsizei attrib_length;
		GLint attrib_size;
		GLenum attrib_type;
		auto attrib_name = std::make_unique<char[]>(attrib_max_length);

		glGetActiveAttrib(this->id, i, attrib_max_length, &attrib_length, &attrib_size, &attrib_type, attrib_name.get());

		msg << "\n -> attribute " << attrib_name
		    << ": type=" << attrib_type << ", size=" << attrib_size
		    << ", id=" << this->get_attribute_id(attrib_name.get());
	}

	log::log(msg);
}


void Program::post_link_hook() {}

}}} // openage::renderer::opengl
