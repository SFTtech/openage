#ifndef _SHADER_H_
#define _SHADER_H_

#include <GL/gl.h>

namespace openage {
namespace engine {
namespace shader {

enum shadertype {
	shader_vertex, shader_fragment, shader_geometry
};

class Shader {
public:
	Shader(shadertype type, const char* sname);
	~Shader();

	shadertype type;
	GLuint id;


	void load(const char *source);
	void load_from_file(const char *filename);

	int check();

	void compile();

	void get_info(GLenum pname, GLint *destination);
	void get_log(char *destination, GLsizei maxlength);
	char *repr();

private:
	const char* name;
};

class Program {
public:
	Program(const char* program_name);
	~Program();

	Shader *vertex, *fragment;
	GLuint id;


	int attach_shader(Shader *s);

	int check(GLenum check_link_or_validate_status);

	int link();

	void get_info(GLenum pname, GLint *destination);
	void get_log(char *destination, GLsizei maxlength);
	char *repr();

	void use();
	void stopusing();

private:
	const char* name;
	bool hasvshader = false, hasfshader = false;
};


}
}
}
#endif
