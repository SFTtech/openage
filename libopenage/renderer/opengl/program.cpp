// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#include "../../config.h"
#if WITH_OPENGL

#include "program.h"

#include "texture.h"

#include "../../log/log.h"
#include "../../error/error.h"
#include "../../util/compiler.h"
#include "../../util/file.h"
#include "../../util/strings.h"

namespace openage {
namespace renderer {
namespace opengl {

Program::Program(renderer::Context *context, const ProgramSource &source)
	:
	renderer::Program{context},
	is_linked{false} {

	// tell OpenGL we wanna have a new pipeline program
	this->id = glCreateProgram();

	if (unlikely(this->id == 0)) {
		throw Error{MSG(err) << "no gl program created! wtf?", true};
	}

	// add all shader sources
	for (auto &shadersource : source.get_shaders()) {
		// create the shader from the source code
		Shader shader{*shadersource};

		// attach the shader to this program
		this->attach_shader(shader);
	}

	// link the sources together.
	this->link();
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
		throw Error{MSG(err) << "using program before it was linked!"};
	}

	glUseProgram(this->id);
}

void Program::stopusing() {
	glUseProgram((GLuint) 0);
}

void Program::check_is_linked(const char *info) {
	if (unlikely(not this->is_linked)) {
		throw Error{MSG(err) << info << " before program was linked!"};
	}
}

GLint Program::get_uniform_id(const char *name) {
	GLint loc;

	// check if the location is cached.
	auto it = this->uniforms.find(name);
	if (it == this->uniforms.end()) {
		this->check_is_linked("Uniform id requested");
		loc = glGetUniformLocation(this->id, name);

		// save to cache
		this->uniforms[name] = loc;
	} else {
		loc = it->second;
	}

	return loc;
}

GLint Program::get_uniformbuffer_id(const char *name) {
	this->check_is_linked("Uniform buffer requested");
	return glGetUniformBlockIndex(this->id, name);
}

GLint Program::get_attribute_id(const char *name) {
	this->check_is_linked("Vertex attribute requested");

	GLint aid = glGetAttribLocation(this->id, name);

	if (unlikely(aid == -1)) {
		this->dump_attributes();
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

void Program::dump_attributes() {
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


void Program::set_uniform_3f(const char *name, const util::Vector<3> &value) {
	this->use();
	GLint location = this->get_uniform_id(name);
	glUniform3f(location, value[0], value[1], value[2]);
}


void Program::set_uniform_1i(const char *name, const int &value) {
	this->use();
	GLint location = this->get_uniform_id(name);
	glUniform1i(location, value);
}


void Program::set_uniform_2dtexture(const char *name, const renderer::Texture &texture) {
	this->use();

	// set the sampler "value" to the texture slot id.
	GLint location = this->get_uniform_id(name);

	// TODO: use multiple slots!
	int slot = 0;
	glUniform1i(location, slot);
	texture.bind_to(slot);
}

}}} // openage::renderer::opengl

#endif // WITH_OPENGL
