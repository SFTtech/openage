#ifndef _ENGINE_SHADER_SHADER_H_
#define _ENGINE_SHADER_SHADER_H_

#include <GL/glew.h>
#include <GL/gl.h>

namespace openage {
namespace engine {
namespace shader {

enum shadertype {
	shader_vertex, shader_fragment, shader_geometry
};

class Shader {
public:
	Shader(shadertype type, const char *name);
	~Shader();

	shadertype type;
	GLuint id;


	void load(const char *source);
	void load_from_file(const char *filename);

	void compile();

	int check();

	void get_info(GLenum pname, GLint *destination);
	void get_log(char *destination, GLsizei maxlength);
	const char *repr();

private:
	const char *name;
};

} //namespace shader
} //namespace engine
} //namespace openage

#endif //_ENGINE_SHADER_SHADER_H_
