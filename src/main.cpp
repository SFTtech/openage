// main source for openage


#include <stdio.h>
#include <SDL2/SDL.h>

/*!
  entry function for openage.

  you may not have expected it, but this is actually the main function.

 */
int main() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL initialization: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Window *window;
	window = SDL_CreateWindow("openage", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_OPENGL);

	if (window == NULL) {
		printf("SDL window creation: %s\n", SDL_GetError());
		return 1;
	}

	int loopc = 0;

	//main loop
	while (true) {
		SDL_Event e;
		if (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				break;
			}
		}


		printf("loop...\n");
		loopc += 1;
	}

	SDL_DestroyWindow(window);

	printf("number of loop iterations: %d\n", loopc);
	printf("runtime: %d\n", SDL_GetTicks());

	SDL_Quit();
	return 0;
}
