#ifndef OPENAGE_SHADER_SHADER_H_8E8D13AF4A0945399210C1058E9C6544
#define OPENAGE_SHADER_SHADER_H_8E8D13AF4A0945399210C1058E9C6544

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

#endif //_SHADER_SHADER_H_
