#ifndef _ENGINE_TEXTURE_H_
#define _ENGINE_TEXTURE_H_

#include <GL/glew.h>
#include <GL/gl.h>

#include "shader/shader.h"
#include "shader/program.h"

namespace openage {
namespace engine {

namespace teamcolor_shader {

extern shader::Shader *vert;
extern shader::Shader *frag;
extern shader::Program *program;
extern GLint player_id_var, alpha_marker_var, player_color_var;

} //namespace teamcolor_shader

class Texture {
public:
	int w;
	int h;

	bool centered;

	Texture(const char *filename);
	~Texture();

	void draw(int x, int y, unsigned player, bool mirrored);

private:
	GLuint id;
};

} //namespace engine
} //namespace openage

#endif //_ENGINE_TEXTURE_H_
