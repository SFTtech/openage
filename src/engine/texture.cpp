#include "texture.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>

#include "../log/log.h"

namespace openage {
namespace engine {

Texture::Texture(const char *filename) {
	SDL_Surface *surface;
	GLuint textureid;
	int mode;

	surface = IMG_Load(filename);

	if (!surface) {
		log::warn("failed to load texture from '%s': %s", filename, IMG_GetError());
		//TODO exception
		return;
	}
	else {
		log::msg("loaded texture from '%s'", filename);
	}

	//glTexImage2D format determination
	if (surface->format->BytesPerPixel == 3) { //RGB 24bit
		mode = GL_RGB;
	}
	else if (surface->format->BytesPerPixel == 4) { //RGBA 32bit
		mode = GL_RGBA;
	}
	else {
		//TODO exception
		return;
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

	this->gl_id = textureid;
}

Texture::~Texture() {
	//TODO free OpenGL ressource...
}

void Texture::draw(int x, int y) {
	glBindTexture(GL_TEXTURE_2D, gl_id);
	glEnable(GL_TEXTURE_2D);

	glBegin(GL_QUADS);

	glTexCoord2i(0, 1);
	glVertex3f(x, y, 0);

	glTexCoord2i(1, 1);
	glVertex3f(x + w, y, 0);

	glTexCoord2i(1, 0);
	glVertex3f(x + w, y + h, 0);

	glTexCoord2i(0, 0);
	glVertex3f(x, y + h, 0);

	glEnd();

	glDisable(GL_TEXTURE_2D);
}

} //namespace engine
} //namespace openage
