#include "texture.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <stdio.h>

#include "../log/log.h"
#include "../util/error.h"

namespace openage {
namespace engine {

namespace teamcolor_shader {

shader::Shader *vert;
shader::Shader *frag;
shader::Program *program;
GLint player_id_var, alpha_marker_var, player_color_var;

} //namespace teamcolor_shader

Texture::Texture(const char *filename, bool player_colored) {
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

	this->subtexture_count = 1;

	this->subtextures = new struct subtexture[this->subtexture_count];

	for (int i = 0; i < this->subtexture_count; i++) {
		struct subtexture subtext;
		//TODO parse subtexture specification file
		subtext.x = 0;
		subtext.y = 0;
		subtext.w = this->w;
		subtext.h = this->h;

		subtext.cx = 50;
		subtext.cy = 50;
		this->subtextures[i] = subtext;
	}

	//TODO: get metadata from texture description file
	this->centered = true;
}

Texture::~Texture() {
	glDeleteTextures(1, &this->id);
}

void Texture::draw(int x, int y, unsigned player, bool mirrored, int subid) {

	if (this->use_player_color_tinting) {
		teamcolor_shader::program->use();
		glUniform1i(teamcolor_shader::player_id_var, player);
	}

	glColor3f(1, 1, 1);

	glBindTexture(GL_TEXTURE_2D, this->id);
	glEnable(GL_TEXTURE_2D);

	struct subtexture tx;

	if (subid <= this->subtexture_count && subid >= 0) {
		tx = this->subtextures[subid];
	}
	else {
		throw util::Error("requested unknown subtexture %d", subid);
	}

	int left, right, top, bottom;

	//TODO do we have pixel-accuracy (rounding errors if w%2 == 1...)?

	left    = x      - tx.cx;
	right   = left   + tx.w;
	top     = y      - tx.cy;
	bottom  = top    + tx.h;

	if (mirrored) {
		int tmp = right;
		right = left;
		left = tmp;
	}

	float txl, txr, txh, txb;
	txl = (tx.x)/this->w;
	txr = (tx.x + tx.w)/this->w;
	txh = (tx.y + tx.h)/this->h;
	txb = (tx.y)/this->h;

	glBegin(GL_QUADS); { //stückbreite/textbreite
		glTexCoord2f(txl, txh);
		glVertex3f(left, top, 0);

		glTexCoord2f(txr, txh);
		glVertex3f(right, top, 0);

		glTexCoord2f(txr, txb);
		glVertex3f(right, bottom, 0);

		glTexCoord2f(txl, txb);
		glVertex3f(left, bottom, 0);
	}
	glEnd();

	glDisable(GL_TEXTURE_2D);

	if (use_player_color_tinting) {
		teamcolor_shader::program->stopusing();
	}
}

} //namespace engine
} //namespace openage
