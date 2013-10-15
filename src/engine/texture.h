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

/**
one sprite.

this struct stores information about what position and size
one sprite included in the "texture atlas" has.
*/
struct subtexture {
	/** x,y starting coordinates and width/height of the subtexture */
	unsigned int x, y, w, h;

	/** hotspot coordinates. */
	unsigned int cx, cy;
};


/**
a texture for rendering graphically.

You may believe it or not, but this class represents a single texture,
which can be drawn on the screen.

The class supports subtextures, so that one big texture can contain
several small images. These are the ones actually to be rendered.
*/
class Texture {
public:
	int w;
	int h;

	bool centered;

	Texture(const char *filename, bool player_colored=true);
	~Texture();

	void draw(int x, int y, unsigned player, bool mirrored=false, int subid=0);

private:
	GLuint id;
	struct subtexture *subtextures;
	int subtexture_count;
	bool use_player_color_tinting;
};

} //namespace engine
} //namespace openage

#endif //_ENGINE_TEXTURE_H_
