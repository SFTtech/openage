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

Shader::Shader(shadertype type, const char *name) : type(type), name(name) {
	switch(this->type) {
	case shader_vertex:
		this->id = glCreateShader(GL_VERTEX_SHADER);
		break;
	case shader_fragment:
		this->id = glCreateShader(GL_FRAGMENT_SHADER);
		break;
	case shader_geometry:
		this->id = glCreateShader(GL_GEOMETRY_SHADER);
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
	delete[] srctext;
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

	const char *whattext;
	if (what_to_check == GL_LINK_STATUS) {
		whattext = "link";
	} else if (what_to_check == GL_VALIDATE_STATUS) {
		whattext = "validat";
	} else if (what_to_check == GL_COMPILE_STATUS) {
		whattext = "compil";
	} else {
		char *repr = this->repr();
		log::err("don't know what to check for in %s: %d", repr, what_to_check);
		delete[] repr;
		return 1;
	}

	// get length of compilation log
	this->get_info(GL_INFO_LOG_LENGTH, &status);

	if (status > 0) {
		char* infolog = new char[status];

		// populate reserved text with compilation log
		this->get_log(infolog, status);

		if (succeded) {
			char *repr = this->repr();
			log::msg("%s was %sed successfully:\n%s", repr, whattext, infolog);
			delete[] repr;
			delete[] infolog;
			return 0;
		} else if (failed) {
			char *repr = this->repr();
			log::err("failed %sing %s:\n%s", whattext, repr, infolog);
			delete[] repr;
			delete[] infolog;
			return 1;
		} else {
			char *repr = this->repr();
			log::err("%s %sing status unknown. log: %s", repr, whattext, infolog);
			delete[] repr;
			delete[] infolog;
			return 1;
		}

	} else {
		char *repr = this->repr();
		log::err("empty program info log of %s", repr);
		delete[] repr;
		return 1;
	}
}

void Shader::get_info(GLenum pname, GLint *params) {
	glGetShaderiv(this->id, pname, params);
}

void Shader::get_log(char *destination, GLsizei maxlength) {
	glGetShaderInfoLog(this->id, maxlength, NULL, destination);
}

char *Shader::repr() {
	const char *type_name;
	switch (this->type) {
	case shader_vertex:
		type_name = "vertex";
		break;
	case shader_fragment:
		type_name = "fragment";
		break;
	case shader_geometry:
		type_name = "geometry";
		break;
	default:
		type_name = "unknown";
		break;
	}

	return util::format("%s shader: %s", type_name, this->name);
}

} //namespace shader
} //namespace engine
} //namespace openage
