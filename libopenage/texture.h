// Copyright 2013-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <epoxy/gl.h>
#include <vector>
#include <memory>

#include "gamedata/texture.gen.h"
#include "coord/camgame.h"
#include "coord/camhud.h"
#include "coord/tile.h"
#include "coord/tile3.h"
#include "shader/program.h"
#include "shader/shader.h"
#include "util/path.h"


class SDL_Surface;
namespace openage {

namespace util {
class Path;
}

namespace texture_shader {
extern shader::Program *program;
extern GLint texture, tex_coord;
} // namespace texture_shader

namespace teamcolor_shader {
extern shader::Program *program;
extern GLint texture, tex_coord;
extern GLint player_id_var, alpha_marker_var, player_color_var;
} // namespace teamcolor_shader

namespace alphamask_shader {
extern shader::Program *program;
extern GLint base_texture, mask_texture, base_coord, mask_coord, show_mask;
} // namespace alphamask_shader

// bitmasks for shader modes
constexpr int PLAYERCOLORED = 1 << 0;
constexpr int ALPHAMASKED   = 1 << 1;

/**
 * enables transfer of data to opengl
 */
struct gl_texture_buffer {
	GLuint id, vertbuf;

	// this requires loading on the main thread
	bool transferred;
	int texture_format_in;
	int texture_format_out;
	std::unique_ptr<uint32_t[]> data;
};


/**
 * A texture for rendering graphically.
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
	 * Create a texture from a rgba8 array.
	 * It will have w * h * 32bit storage.
	 */
	Texture(int width, int height, std::unique_ptr<uint32_t[]> data);

	/**
	 * Create a texture from a existing image file.
	 * For supported image file types, see the SDL_Image initialization in the engine.
	 */
	Texture(const util::Path &filename,
	        bool use_metafile = false,
	        SDL_Surface *surface = nullptr);
	~Texture();

	void draw(coord::camhud pos, unsigned int mode=0, bool mirrored=false, int subid=0, unsigned player=0) const;
	void draw(coord::camgame pos, unsigned int mode=0, bool mirrored=false, int subid=0, unsigned player=0) const;
	void draw(coord::tile pos, unsigned int mode, int subid, Texture *alpha_texture=nullptr, int alpha_subid=-1) const;
	void draw(coord::pixel_t x, coord::pixel_t y, unsigned int mode, bool mirrored, int subid, unsigned player, Texture *alpha_texture, int alpha_subid) const;

	/**
	 * Reload the image file. Used for inotify refreshing.
	 */
	void reload();

	/**
	 * Get the subtexture coordinates by its id.
	 */
	const gamedata::subtexture *get_subtexture(uint64_t subid) const;

	/**
	 * @return the number of available subtextures
	 */
	size_t get_subtexture_count() const;

	/**
	 * Fetch the size of the given subtexture.
	 * @param subid: index of the requested subtexture
	 * @param w: the subtexture width
	 * @param h: the subtexture height
	 */
	void get_subtexture_size(uint64_t subid, int *w, int *h) const;

	/**
	 * get atlas subtexture coordinates.
	 *
	 * left, right, top and bottom bounds as coordinates
	 * these pick the requested area out of the big texture.
	 * returned as floats in range 0.0 to 1.0
	 */
	void get_subtexture_coordinates(uint64_t subid, float *txl, float *txr, float *txt, float *txb) const;
	void get_subtexture_coordinates(const gamedata::subtexture *subtex, float *txl, float *txr, float *txt, float *txb) const;

	/**
	 * fixes the hotspots of all subtextures to (x,y).
	 * this is a temporary workaround; such fixes should actually be done in the
	 * convert script.
	 */
	void fix_hotspots(unsigned x, unsigned y);

	/**
	 * activates the influence of a given alpha mask to this texture.
	 */
	void activate_alphamask(Texture *mask, uint64_t subid);

	/**
	 * disable a previously activated alpha mask.
	 */
	void disable_alphamask();

	/**
	 * returns the opengl texture id of this texture.
	 */
	GLuint get_texture_id() const;

private:
	std::unique_ptr<gl_texture_buffer> buffer;
	std::vector<gamedata::subtexture> subtextures;
	bool use_metafile;

	util::Path filename;

	void load(SDL_Surface *surface);

	/**
	 * The texture loadin must occur on the thread that manages the gl context.
	 */
	void load_in_glthread() const;
	GLuint make_gl_texture(int iformat, int oformat, int w, int h, void *) const;
	void unload();

};

} // namespace openage
