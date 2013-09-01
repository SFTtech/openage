#ifndef _ENGINE_SHADER_PROGRAM_H_
#define _ENGINE_SHADER_PROGRAM_H_

#include <GL/glew.h>
#include <GL/gl.h>

#include "shader.h"

namespace openage {
namespace engine {
namespace shader {

class Program {
public:
	Program(const char *name);
	~Program();

	Shader *vertex, *fragment;
	GLuint id;


	int attach_shader(Shader *s);

	int link();

	int check(GLenum check_link_or_validate_status);

	void get_info(GLenum pname, GLint *destination);
	void get_log(char *destination, GLsizei maxlength);
	const char *repr();

	void use();
	void stopusing();

	GLint get_uniform_id(const char *name);

private:
	const char *name;
	bool hasvshader = false, hasfshader = false;
};


} //namespace shader
} //namespace engine
} //namespace openage

#endif //_ENGINE_SHADER_PROGRAM_H_
