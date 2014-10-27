#ifndef _SHADER_PROGRAM_H_
#define _SHADER_PROGRAM_H_

#include <GL/glew.h>
#include "../crossplatform/opengl.h"

#include "shader.h"

namespace openage {
namespace shader {

class Program {
public:
	GLuint id;
	GLint pos_id, mvpm_id;

	Program();
	Program(Shader *s0, Shader *s1);
	~Program();

	void attach_shader(Shader *s);

	void link();

	void use();
	void stopusing();

	GLint get_uniform_id(const char *name);
	GLint get_attribute_id(const char *name);

	void set_attribute_id(const char *name, GLuint id);

	void dump_active_attributes();

private:
	bool is_linked;
	Shader *vert, *frag, *geom;

	void check(GLenum what_to_check);
	GLint get_info(GLenum pname);
	char *get_log();
	void post_link_hook();
};


} //namespace shader
} //namespace openage

#endif //_SHADER_PROGRAM_H_
