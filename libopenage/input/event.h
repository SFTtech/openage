// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_KEYBINDS_KEY_H_
#define OPENAGE_KEYBINDS_KEY_H_

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <SDL2/SDL.h>

namespace openage {
namespace input {

/**
 * highest level classes of input
 */
enum class event_class {
	ANY,
	KEYBOARD,
	ALPHA,
	NUMERIC,
	SYMBOL,
	OTHER,
	MOUSE,
	MOUSE_BUTTON,
	MOUSE_WHEEL,
	MOUSE_MOTION
};


struct event_class_hash {
	int operator()(const event_class &s) const;
};


/**
 * each event type mapped to parent type
 */
static std::unordered_map<event_class, event_class, event_class_hash> event_base {
	{event_class::KEYBOARD, event_class::ANY},
	{event_class::ALPHA, event_class::KEYBOARD},
	{event_class::NUMERIC, event_class::KEYBOARD},
	{event_class::SYMBOL, event_class::KEYBOARD},
	{event_class::OTHER, event_class::KEYBOARD},
	{event_class::MOUSE, event_class::ANY},
	{event_class::MOUSE_BUTTON, event_class::MOUSE},
	{event_class::MOUSE_WHEEL, event_class::MOUSE},
	{event_class::MOUSE_MOTION, event_class::MOUSE},
};

/**
 * mods set on an event
 */
enum class modifier {
	CTRL,
	ALT,
	SHIFT
};


struct modifier_hash {
	int operator()(const modifier &s) const;
};

/**
 * types used by events
 */
using code_t = int;
using modset_t = std::unordered_set<modifier, modifier_hash>;

/**
 * a set sdl event types
 * TODO to char function
 */
struct class_code_t {
public:
	class_code_t(event_class cl, code_t code);

	/**
	 * classes ordered with most specific first
	 */
	std::vector<event_class> get_classes() const;
	bool has_class(const event_class &c) const;

	const event_class eclass;
	const code_t code;
};

bool operator ==(class_code_t a, class_code_t b);

struct class_code_hash {
	int operator()(const class_code_t &k) const;
};


/**
 * event with a mod
 */
class Event {
public:
	Event(event_class cl, code_t code, modset_t mod);

	/**
	 * logable debug info
	 */
	std::string info() const;

	bool operator ==(const Event &other) const;

	const class_code_t cc;
	const modset_t mod;
};

struct event_hash {
	int operator()(const Event &e) const;
};

using event_set_t = std::unordered_set<Event, event_hash>;


// SDL mapping functions

modset_t sdl_mod(SDL_Keymod mod);
Event sdl_key(SDL_Keycode code);
Event sdl_key(SDL_Keycode code, SDL_Keymod mod);
Event sdl_mouse(int button);
Event sdl_wheel(int direction);


} //namespace input
} //namespace openage

#endif
