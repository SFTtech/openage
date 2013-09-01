#include "shader.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>

#include "../../log/log.h"
#include "../../util/error.h"
#include "../../util/filetools.h"

namespace openage {
namespace engine {
namespace shader {

Shader::Shader(shadertype type, const char *name) : type(type), name(name) {
	switch(type) {
	case shader_vertex:
		id = glCreateShader(GL_VERTEX_SHADER);
		break;
	case shader_fragment:
		id = glCreateShader(GL_FRAGMENT_SHADER);
		break;
	case shader_geometry:
		id = glCreateShader(GL_GEOMETRY_SHADER);
		break;
	default:
		throw util::Error("Unknown shader ID: %d", type);
	}
}

Shader::~Shader() {
	glDeleteShader(this->id);
}

void Shader::load(const char *source) {
	//copy the given source code to the opengl shader.
	glShaderSource(this->id, 1, (const char **) &source, NULL);
}

void Shader::load_from_file(const char *filename) {
	char *srctext = util::read_whole_file(filename);
	//load the source into the opengl shader
	this->load(srctext);
	//this copies the source code, so we can free it now.
	free(srctext);
}

void Shader::compile() {
	glCompileShader(this->id);
}

int Shader::check() {
	GLenum what_to_check = GL_COMPILE_STATUS;
	GLint status;

	this->get_info(what_to_check, &status);
	bool failed = (status == GL_FALSE);
	bool succeded = (status == GL_TRUE);

	char *whattext;
	if (what_to_check == GL_LINK_STATUS) {
		whattext = (char*) "link";
	} else if (what_to_check == GL_VALIDATE_STATUS) {
		whattext = (char*) "validat";
	} else if (what_to_check == GL_COMPILE_STATUS) {
		whattext = (char*) "compil";
	} else {
		log::err("don't know what to check for in %s: %d", this->repr(), what_to_check);
		return 1;
	}

	// get length of compilation log
	this->get_info(GL_INFO_LOG_LENGTH, &status);

	if (status > 0) {
		char* infolog = new char[status];

		// populate reserved text with compilation log
		this->get_log(infolog, status);

		if (succeded) {
			log::msg("%s was %sed successfully:\n%s", this->repr(), whattext, infolog);
			delete[] infolog;
			return 0;
		} else if (failed) {
			log::err("failed %sing %s:\n%s", whattext, this->repr(), infolog);
			delete[] infolog;
			return 1;
		} else {
			log::err("%s %sing status unknown. log: %s", this->repr(), whattext, infolog);
			delete[] infolog;
			return 1;
		}

	} else {
		log::err("empty program info log of %s", this->repr());
		return 1;
	}
}

void Shader::get_info(GLenum pname, GLint *params) {
	glGetShaderiv(this->id, pname, params);
}

void Shader::get_log(char *destination, GLsizei maxlength) {
	glGetShaderInfoLog(this->id, maxlength, NULL, destination);
}

const char *Shader::repr() {
	std::string repr;

	switch (this->type) {
	case shader_vertex:
		repr = "vertex";
		break;
	case shader_fragment:
		repr = "fragment";
		break;
	case shader_geometry:
		repr = "geometry";
		break;
	default:
		repr = "unknown";
		break;
	}

	repr += " shader: ";
	repr += this->name;

	return repr.c_str();
}

} //namespace shader
} //namespace engine
} //namespace openage
