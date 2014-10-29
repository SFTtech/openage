#ifndef OPENAGE_TEXTURE_H_
#define OPENAGE_TEXTURE_H_

#include <GL/glew.h>
#include "crossplatform/opengl.h"
#include <vector>

#include "gamedata/texture.gen.h"
#include "coord/camgame.h"
#include "coord/camhud.h"
#include "coord/tile.h"
#include "coord/tile3.h"
#include "shader/program.h"
#include "shader/shader.h"
#include "util/file.h"

namespace openage {

namespace texture_shader {
extern shader::Program *program;
extern GLint texture, tex_coord;
} //namespace texture_shader

namespace teamcolor_shader {
extern shader::Program *program;
extern GLint texture, tex_coord;
extern GLint player_id_var, alpha_marker_var, player_color_var;
} //namespace teamcolor_shader

namespace alphamask_shader {
extern shader::Program *program;
extern GLint base_texture, mask_texture, base_coord, mask_coord, show_mask;
} //namespace alphamask_shader

// bitmasks for shader modes
constexpr int PLAYERCOLORED = 1 << 0;
constexpr int ALPHAMASKED   = 1 << 1;


/**
 * a texture for rendering graphically.
 *
 * You may believe it or not, but this class represents a single texture,
 * which can be drawn on the screen.
 *
 * The class supports subtextures, so that one big texture can contain
 * several small images. These are the ones actually to be rendered.
 */
class Texture {
public:
	int w;
	int h;

	/**
	 * terrain atlas dimensions (floor(sqrt(subtexture count)))
	 * relevant for determining terrain subtexture ids
	 */
	size_t atlas_dimensions;

	Texture(std::string filename, bool use_metafile = false);
	~Texture();

	void draw(coord::camhud pos, unsigned int mode = 0, bool mirrored = false, int subid = 0, unsigned player = 0);
	void draw(coord::camgame pos, unsigned int mode = 0, bool mirrored = false, int subid = 0, unsigned player = 0);
	void draw(coord::tile pos, unsigned int mode, int subid, Texture *alpha_texture = nullptr, int alpha_subid = -1);
	void draw(coord::pixel_t x, coord::pixel_t y, unsigned int mode, bool mirrored, int subid, unsigned player, Texture *alpha_texture, int alpha_subid);

	void reload();

	struct gamedata::subtexture *get_subtexture(int subid);
	int get_subtexture_count();
	void get_subtexture_size(int subid, int *w, int *h);

	/**
	 * get atlas subtexture coordinates.
	 *
	 * left, right, top and bottom bounds as coordinates
	 * these pick the requested area out of the big texture.
	 * returned as floats in range 0.0 to 1.0
	 */
	void get_subtexture_coordinates(int subid, float *txl, float *txr, float *txt, float *txb);
	void get_subtexture_coordinates(struct gamedata::subtexture *subtex, float *txl, float *txr, float *txt, float *txb);

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
	GLuint id, vertbuf;
	std::vector<gamedata::subtexture>subtextures;
	size_t subtexture_count;
	bool use_metafile;

	std::string filename;

	void load();
	void unload();
};

} //namespace openage

#endif
