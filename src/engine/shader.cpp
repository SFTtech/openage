#include <GL/glew.h>
#include <GL/gl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>

#include "shader.h"

#include "../log/log.h"
#include "../util/filetools.h"

using namespace openage;
using namespace engine;
using namespace shader;

//TODO: all this is fakking duplicated code which needs to be merged
//with a class hierarchy!


Shader::Shader(shadertype stype, const char* sname) {
	this->type = stype;
	this->name = sname;

	if (this->type == shader_vertex) {
		this->id = glCreateShader(GL_VERTEX_SHADER);
	}
	else if (type == shader_fragment) {
		this->id = glCreateShader(GL_FRAGMENT_SHADER);
	}
	else if (type == shader_geometry) {
		this->id = glCreateShader(GL_GEOMETRY_SHADER);
	}
	else {
		log::fatal("unknown shader type created: %d", type);
		exit(1);
	}
}

Shader::~Shader() {
	glDeleteShader(this->id);
}

void Shader::load(const char* source) {
	//copy the given source code to the opengl shader.
	glShaderSource(this->id, 1, (const char**) &source, NULL);
}

void Shader::load_from_file(const char* filename) {
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

	char* whattext;
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

char* Shader::repr() {
	std::string stype;
	if (this->type == shader_vertex) {
		stype = "vertexshader ";
	}
	else if (this->type == shader_fragment) {
		stype = "fragmentshader ";
	}
	else if (this->type == shader_geometry) {
		stype = "geometryshader ";
	}
	else {
		stype = "unknown shader ";
	}
	std::string ret = stype + std::string(this->name);

	return (char *) ret.c_str();
}




Program::Program(const char* pname) : name(pname) {
	this->id = glCreateProgram();
}

Program::~Program() {
	glDeleteProgram(this->id);
}

int Program::attach_shader(Shader *s) {
	glAttachShader(this->id, s->id);

	if (s->type == shader_fragment) {
		this->hasfshader = true;
	} else if (s->type == shader_vertex) {
		this->hasvshader = true;
	}

	return 0;
}

int Program::link() {
	if ( !hasfshader || !hasvshader) {
		log::err("program %s does not have vertex and fragment shader yet, cannot be linked.", this->name);
		return 1;
	}

	glLinkProgram(this->id);

	int err, err2;
	if ((err = this->check(GL_LINK_STATUS)) == 0) {
		glValidateProgram(this->id);
		if ((err2 = this->check(GL_VALIDATE_STATUS)) == 0) {
			return 0;
		} else {
			return 1;
		}
	} else {
		log::err("linking of program %s failed.", this->name);
		return 1;
	}
}

int Program::check(GLenum what_to_check) {
	GLint status;

	this->get_info(what_to_check, &status);
	bool failed = (status == GL_FALSE);
	bool succeded = (status == GL_TRUE);

	char* whattext;
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
			log::err("%s %sing status unknown. log:\n%s", this->repr(), whattext, infolog);
			delete[] infolog;
			return 1;
		}

	} else {
		log::err("empty program info log of %s", this->repr());
		return 1;
	}
}

void Program::get_info(GLenum pname, GLint *params) {
	glGetProgramiv(this->id, pname, params);
}

void Program::get_log(char *destination, GLsizei maxlength) {
	glGetProgramInfoLog(this->id, maxlength, NULL, destination);
}

char* Program::repr() {
	std::string ret = std::string("program ") + std::string(this->name);
	return (char *) ret.c_str();
}

void Program::use() {
	if (glIsProgram(this->id) == GL_TRUE) {
		glUseProgram(this->id);
	} else {
		log::err("error using a erroréús ", this->repr());
	}
}

void Program::stopusing() {
	glUseProgram((GLuint) 0);
}

GLint Program::get_uniform_id(const char *name) {
	return glGetUniformLocation(this->id, name);
}
