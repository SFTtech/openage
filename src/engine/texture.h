#ifndef _ENGINE_TEXTURE_H_
#define _ENGINE_TEXTURE_H_

#include <GL/glew.h>
#include <GL/gl.h>

#include "coordinates.h"
#include "shader/shader.h"
#include "shader/program.h"

namespace openage {
namespace engine {

namespace shared_shaders {
extern shader::Shader *maptexture;
} //namespace shared_shaders

namespace teamcolor_shader {
extern shader::Shader *frag;
extern shader::Program *program;
extern GLint player_id_var, alpha_marker_var, player_color_var;
} //namespace teamcolor_shader

namespace alphamask_shader {
extern shader::Shader *frag;
extern shader::Program *program;
extern GLint base_texture, mask_texture;
} //namespace alphamask_shader


constexpr int PLAYERCOLORED = 1 << 0;
constexpr int ALPHAMASKED   = 1 << 1;


/**
one sprite.

this struct stores information about what position and size
one sprite included in the "texture atlas" has.
*/
struct subtexture {
	/** x,y starting coordinates and width/height of the subtexture */
	int x, y, w, h;

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

	/**
	terrain atlas dimensions (floor(sqrt(subtexture count)))
	relevant for determining terrain subtexture ids
	*/
	size_t atlas_dimensions;

	Texture(const char *filename, bool use_metafile = false, unsigned int mode = 0);
	~Texture();

	void draw(coord::phys pos, bool mirrored = false, int subid = 0, unsigned player = 0);
	void draw(int x, int y, bool mirrored = false, int subid = 0, unsigned player = 0);
	int get_subtexture_count();
	void get_subtexture_size(int subid, int *w, int *h);

	/**
	fixes the hotspots of all subtextures to (x,y).
	this is a temporary workaround; such fixes should actually be done in the
	convert script.
	*/
	void fix_hotspots(unsigned x, unsigned y);

	/**
	activates the influence of a given alpha mask to this texture.
	*/
	void activate_alphamask(Texture *mask, int subid);

	/**
	disable a previously activated alpha mask.
	*/
	void disable_alphamask();

	/**
	returns the opengl texture id of this texture.
	*/
	GLuint get_texture_id();

private:
	GLuint id;
	struct subtexture *subtextures;
	int subtexture_count;
	bool use_player_color_tinting;
	bool use_alpha_masking;

	bool alpha_mask_active;
	Texture *alpha_texture;
	int alpha_subid;
};

} //namespace engine
} //namespace openage

#endif //_ENGINE_TEXTURE_H_
