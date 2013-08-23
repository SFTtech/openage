// main source for openage


#include <stdio.h>
#include <SDL2/SDL.h>
#include <GL/gl.h>

/*!
  entry function for openage.

  you may not have expected it, but this is actually the main function.
 */
int main() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL initialization: %s\n", SDL_GetError());
		return 1;
	}

	SDL_GLContext glcontext;

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	SDL_Window *window;
	window = SDL_CreateWindow("openage", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);

	if (window == NULL) {
		printf("SDL window creation: %s\n", SDL_GetError());
		return 1;
	}

	glcontext = SDL_GL_CreateContext(window);

	//vsync on
	SDL_GL_SetSwapInterval(1);

	unsigned frames = 0;
	unsigned starttime = SDL_GetTicks();

	//main loop
	while (true) {
		SDL_Event e;
		if (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				break;
			}
		}

		glClearColor ( 1.0, 0.0, 0.0, 1.0 );
		glClear ( GL_COLOR_BUFFER_BIT );
		//swap buffer
		SDL_GL_SwapWindow(window);
		//SDL_Delay(2000);

		glClearColor ( 0.0, 1.0, 0.0, 1.0 );
		glClear ( GL_COLOR_BUFFER_BIT );
		SDL_GL_SwapWindow(window);

		glClearColor ( 0.0, 0.0, 1.0, 1.0 );
		glClear ( GL_COLOR_BUFFER_BIT );
		SDL_GL_SwapWindow(window);

		frames += 1;
	}

	unsigned fps = (1000 * frames) / (SDL_GetTicks() - starttime);
	printf("fps: %d\n", fps);

	SDL_GL_DeleteContext(glcontext);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
