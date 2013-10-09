
#include <SDL2/SDL.h>


int main() {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window *window = SDL_CreateWindow("openage", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);

	SDL_DestroyWindow(window);
	SDL_Quit();
}
