#ifndef _INPUT_H_
#define _INPUT_H_

#include <SDL2/SDL.h>
#include <unordered_map>
#include <vector>

#include "handlers.h"

namespace openage {

/**
 * the core engine input handler.
 * tracks key states, invokes window resize handlers.
 */
class CoreInputHandler : public InputHandler {
public:
	CoreInputHandler();
	~CoreInputHandler();

	/**
	 * query stored pressing stat for a key.
	 * @return true when the key is pressed, false else.
	 */
	bool is_key_down(SDL_Keycode k);

	/**
	 * process the input event.
	 * @param event: captured sdl input event to handle
	 * @return whether other handlers may process `event`
	 */
	virtual bool on_input(SDL_Event *event);

	/**
	 * register a handler object for window resize.
	 * it is run when the window size changes.
	 */
	void register_resize_action(ResizeHandler *handler);


private:
	/**
	 * key to is_down map.
	 * stores a mapping between keycodes and its pressing state.
	 * a true value means the key is currently pressed,
	 * false indicates the key is untouched.
	 */
	std::unordered_map<SDL_Keycode, bool> key_states;

	/**
	 * list of handlers that are executed upon a resize event.
	 */
	std::vector<ResizeHandler *> on_resize;
};

} //namespace openage

#endif //_INPUT_H_
