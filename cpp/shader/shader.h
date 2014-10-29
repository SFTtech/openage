#ifndef OPENAGE_SHADER_SHADER_H_
#define OPENAGE_SHADER_SHADER_H_

#include <GL/glew.h>
#include "../crossplatform/opengl.h"

namespace openage {
namespace shader {

const char *type_to_string(GLenum type);

class Shader {
public:
	Shader(GLenum type, const char *source);
	~Shader();

	GLuint id;
	GLenum type;
};

} //namespace shader
} //namespace openage

#endif
