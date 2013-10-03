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

Texture::Texture(const char *filename) {
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

	this->centered = true;
}

Texture::~Texture() {
	//TODO free OpenGL ressource...
}

void Texture::draw(int x, int y, unsigned player, bool mirrored) {
	teamcolor_shader::program->use();
	glColor3f(1, 1, 1);
	glUniform1i(teamcolor_shader::player_id_var, player);

	glBindTexture(GL_TEXTURE_2D, this->id);
	glEnable(GL_TEXTURE_2D);

	int left, right, top, bottom;

	//TODO do we have pixel-accuracy (rounding errors if w%2 == 1...)?
	left = x;
	top = y;

	if (centered) {
		left -= w / 2;
		top -= h / 2;
	}

	right = left + w;
	bottom = top + h;

	if (mirrored) {
		int tmp = right;
		right = left;
		left = tmp;
	}


	glBegin(GL_QUADS); {
		glTexCoord2i(0, 1);
		glVertex3f(left, top, 0);

		glTexCoord2i(1, 1);
		glVertex3f(right, top, 0);

		glTexCoord2i(1, 0);
		glVertex3f(right, bottom, 0);

		glTexCoord2i(0, 0);
		glVertex3f(left, bottom, 0);
	}
	glEnd();

	glDisable(GL_TEXTURE_2D);
	teamcolor_shader::program->stopusing();
}

} //namespace engine
} //namespace openage
