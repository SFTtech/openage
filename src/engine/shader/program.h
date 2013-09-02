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


	void attach_shader(Shader *s);

	void link();

	void check(GLenum what_to_check);

	GLint get_info(GLenum pname);
	char *get_log();

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
