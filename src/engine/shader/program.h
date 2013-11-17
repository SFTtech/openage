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
	Program();
	~Program();

	void attach_shader(Shader *s);

	void link();

	void use();
	void stopusing();

	GLint get_uniform_id(const char *name);
	GLint get_attribute_id(const char *name);

	GLuint get_id();

private:
	bool is_linked;
	GLuint id;

	void check(GLenum what_to_check);
	GLint get_info(GLenum pname);
	char *get_log();

	Shader *vert, *frag, *geom;
};


} //namespace shader
} //namespace engine
} //namespace openage

#endif //_ENGINE_SHADER_PROGRAM_H_
