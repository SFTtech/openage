#include "texture.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <math.h>
#include <stdio.h>

#include "../log/log.h"
#include "../util/error.h"
#include "../util/filetools.h"

namespace openage {
namespace engine {

namespace teamcolor_shader {

shader::Shader *vert;
shader::Shader *frag;
shader::Program *program;
GLint player_id_var, alpha_marker_var, player_color_var;

} //namespace teamcolor_shader

Texture::Texture(const char *filename, bool player_colored, bool use_metafile) {
	this->use_player_color_tinting = player_colored;

	SDL_Surface *surface;
	GLuint textureid;
	int mode;

	surface = IMG_Load(filename);

	if (!surface) {
		throw util::Error("Could not load texture from '%s': %s", filename, IMG_GetError());
	}
	else {
		log::dbg1("Loaded texture from '%s'", filename);
	}

	//glTexImage2D format determination
	switch (surface->format->BytesPerPixel) {
	case 3: //RGB 24 bit
		mode = GL_RGB;
		break;
	case 4: //RGBA 32 bit
		mode = GL_RGBA;
		break;
	default:
		throw util::Error("Unknown texture bit depth for '%s': %d bytes per pixel)", filename, surface->format->BytesPerPixel);
		break;
	}

	this->w = surface->w;
	this->h = surface->h;

	//generate 1 texture handle
	glGenTextures(1, &textureid);

	glBindTexture(GL_TEXTURE_2D, textureid);

	//sdl surface -> opengl texture
	glTexImage2D(GL_TEXTURE_2D, 0, mode, surface->w, surface->h, 0, mode, GL_UNSIGNED_BYTE, surface->pixels);

	//later drawing settings
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	SDL_FreeSurface(surface);

	this->id = textureid;

	if (use_metafile) {
		//change the suffix to .docx (lol)
		size_t m_len = strlen(filename) + 2;
		char *meta_filename = new char[m_len];
		strncpy(meta_filename, filename, m_len);

		meta_filename[m_len-5] = 'd';
		meta_filename[m_len-4] = 'o';
		meta_filename[m_len-3] = 'c';
		meta_filename[m_len-2] = 'x';
		meta_filename[m_len-1] = '\0';


		log::msg("loading meta file %s", meta_filename);

		//get subtexture information by meta file exported by script
		char *texture_meta_file = util::read_whole_file(meta_filename);

		delete[] meta_filename;

		char *tmeta_seeker = texture_meta_file;
		char *currentline = texture_meta_file;

		bool wanting_count = true;

		for(; *tmeta_seeker != '\0'; tmeta_seeker++) {

			if (*tmeta_seeker == '\n') {
				*tmeta_seeker = '\0';

				if (*currentline != '#') {
					//count, index, x, y, width, height, hotspotx, hotspoty
					uint n, idx, tx, ty, tw, th, hx, hy;

					if(sscanf(currentline, "n=%u", &n) == 1) {
						this->subtexture_count = n;
						this->subtextures = new struct subtexture[n];
						wanting_count = false;
					}
					else {
						if (wanting_count) {
							throw util::Error("texture meta line found, but no count set yet in %s", meta_filename);
						}
						else if(sscanf(currentline, "%u=%u,%u,%u,%u,%u,%u", &idx, &tx, &ty, &tw, &th, &hx, &hy)) {
							struct subtexture subtext;

							//lower left coordinates, origin
							subtext.x = tx;
							subtext.y = ty;

							//width and height from lower left origin
							subtext.w = tw;
							subtext.h = th;

							//hotspot/center coordinates
							subtext.cx = hx;
							subtext.cy = hy;

							this->subtextures[idx] = subtext;
						}
						else {
							throw util::Error("unknown line content reading texture meta file %s", meta_filename);
						}
					}
				}
				currentline = tmeta_seeker + 1;
			}
		}

		delete[] texture_meta_file;
	}
	else { //this texture does not contain subtextures
		struct subtexture subtext;

		subtext.x = 0;
		subtext.y = 0;
		subtext.w = this->w;
		subtext.h = this->h;
		subtext.cx = 0;
		subtext.cy = 0;

		this->subtexture_count = 1;
		this->subtextures = new struct subtexture[1];
		this->subtextures[0] = subtext;
	}

	//deprecated..
	this->centered = true;
}

Texture::~Texture() {
	delete[] this->subtextures;
	glDeleteTextures(1, &this->id);
}

void Texture::draw(int x, int y, bool mirrored, int subid, unsigned player) {

	if (this->use_player_color_tinting) {
		teamcolor_shader::program->use();
		glUniform1i(teamcolor_shader::player_id_var, player);
	}

	glColor3f(1, 1, 1);

	glBindTexture(GL_TEXTURE_2D, this->id);
	glEnable(GL_TEXTURE_2D);

	struct subtexture *tx;

	if (subid < this->subtexture_count && subid >= 0) {
		tx = &this->subtextures[subid];
	}
	else {
		throw util::Error("requested unknown subtexture %d", subid);
	}

	int left, right, top, bottom;

	//coordinates where the texture will be drawn on screen.
	bottom  = y      - tx->cy;
	top     = bottom + tx->h;

	if (!mirrored) {
		left  = x    - tx->cx;
		right = left + tx->w;
	} else {
		left  = x    + tx->cx;
		right = left - tx->w;
	}

	//subtexture coordinates
	//left, right, top and bottom bounds as coordinates
	//these pick the requested area out of the big texture.
	float txl, txr, txt, txb;
	txl = ((float)tx->x)           /this->w;
	txr = ((float)(tx->x + tx->w)) /this->w;
	txt = ((float)tx->y)           /this->h;
	txb = ((float)(tx->y + tx->h)) /this->h;

	//TODO:replate with vertex buffer/uniforms for vshader
	glBegin(GL_QUADS); {
		glTexCoord2f(txl, txt);
		glVertex3f(left, top, 0);

		glTexCoord2f(txl, txb);
		glVertex3f(left, bottom, 0);

		glTexCoord2f(txr, txb);
		glVertex3f(right, bottom, 0);

		glTexCoord2f(txr, txt);
		glVertex3f(right, top, 0);
	}
	glEnd();

	glDisable(GL_TEXTURE_2D);

	if (this->use_player_color_tinting) {
		teamcolor_shader::program->stopusing();
	}
}

int Texture::get_subtexture_count() {
	return this->subtexture_count;
}

void Texture::get_subtexture_size(int subid, int *w, int *h) {
	if (subid > this->get_subtexture_count() -1) {
		throw util::Error("Requested nonexistant subtexture %d", subid);
	}

	*w = this->subtextures[subid].w;
	*h = this->subtextures[subid].h;
}

/**
returns the terrain subtexture id for a given position.

this function returns always the right value, so that neighbor tiles
of the same terrain (like grass-grass) are matching (without blendomatic).
*/
int Texture::get_subtexture_id(int x, int y) {
	int mod = sqrt(this->subtexture_count);
	return ((x % mod) + (mod * (y % mod)));
}



} //namespace engine
} //namespace openage
