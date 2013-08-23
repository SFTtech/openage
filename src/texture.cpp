#include <SDL2/SDL.h>
#include <stdio.h>
#include <unistd.h> //for getcwd

#include "texture.h"


GLuint load_texture(const char *filename, int *width, int *height) {

	SDL_Surface *surface;
	GLuint textureid;
	int mode;


	printf("generating texture %s\n", filename);

	surface = SDL_LoadBMP(filename);

	if (!surface) {
		printf("failed generating texture %s\n", filename);
		return 0;
	}
	else {
		printf("created texture %s successfully\n", filename);
	}

	// glTexImage2D format determination
	if (surface->format->BytesPerPixel == 3) { // RGB 24bit
		mode = GL_RGB;
	}
	else if (surface->format->BytesPerPixel == 4) { // RGBA 32bit
		mode = GL_RGBA;
	}
	else {
		SDL_FreeSurface(surface);
		return 0;
	}

	*width=surface->w;
	*height=surface->h;

	glGenTextures(1, &textureid);

	glBindTexture(GL_TEXTURE_2D, textureid);

	// sdl surface -> opengl texture
	glTexImage2D(GL_TEXTURE_2D, 0, mode, surface->w, surface->h, 0, mode, GL_UNSIGNED_BYTE, surface->pixels);

	// later drawing settings
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	SDL_FreeSurface(surface);

	return textureid;
}

void draw_texture(int x, int y, GLuint textureid, int width, int height) {

	glBindTexture(GL_TEXTURE_2D, textureid);
	glEnable(GL_TEXTURE_2D);

	glBegin(GL_QUADS);

	glTexCoord2i(0, 0);
	glVertex3f(x, y, 0);

	glTexCoord2i(1, 0);
	glVertex3f(x+width, y, 0);

	glTexCoord2i(1, 1);
	glVertex3f(x+width, y+height, 0);

	glTexCoord2i(0, 1);
	glVertex3f(x, y+height, 0);

	glEnd();

	glDisable(GL_TEXTURE_2D);
}
