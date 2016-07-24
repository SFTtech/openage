// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#include "text_to_event.h"

#include <regex>
#include <stdexcept>

#include <SDL2/SDL.h>

#include "../error/error.h"
#include "../testing/testing.h"

namespace openage {
namespace input {

namespace {
const std::vector<int> modifiers{
	KMOD_LCTRL, KMOD_LSHIFT, KMOD_RCTRL, KMOD_RSHIFT, KMOD_LALT, KMOD_RALT, KMOD_LGUI, KMOD_RGUI, KMOD_CTRL, KMOD_SHIFT, KMOD_ALT, KMOD_GUI, KMOD_MODE, KMOD_CAPS, KMOD_NUM
};

const std::regex event_pattern{
	"(?:(?:(LCtrl)|(LShift)|(RCtrl)|(RShift)|(LAlt)|(RAlt)|(LGui)|(RGUI)|(Ctrl)|(Shift)|(Alt)|(Gui)|(AltGr)|(Caps)|(NumLck))[[:space:]]+)?" // modifier (optional)
	"([^[:space:]]+)" // key
	"(?:[[:space:]]+([^[:space:]]+))?" // parameter, like direction (optional)
	"[[:space:]]*"
};

void check_modifiers_once() {
	static bool checked = false;

	if (!checked) {
		checked = true;
		ENSURE(event_pattern.mark_count() == modifiers.size() + 2, "Groups in the input event regex pattern: one per key modifier, key itself and amount.");
	}
}

Event to_event(const std::string &event_type, const std::string &param, const int mod) {
	try {
		if (event_type == "MOUSE")
			return sdl_mouse(std::stoi(param), static_cast<SDL_Keymod>(mod));
		else if (event_type == "MOUSE_UP")
			return sdl_mouse_up_down(std::stoi(param), true, static_cast<SDL_Keymod>(mod));
		else if (event_type == "MOUSE_DOWN")
			return sdl_mouse_up_down(std::stoi(param), false, static_cast<SDL_Keymod>(mod));
	} catch (std::logic_error&) {
		throw Error(MSG(err) << "could not parse mouse button '" << param << "'!");
	}

	if (event_type == "WHEEL") {
		if (param == "1" || param == "UP") {
			return sdl_wheel(1, static_cast<SDL_Keymod>(mod));
		} else if (param == "-1" || param == "DOWN") {
			return sdl_wheel(-1, static_cast<SDL_Keymod>(mod));
		}

		throw Error(MSG(err) << "could not parse mouse wheel amount '" << param << "'!");
	}

	SDL_Keycode key_code = SDL_GetKeyFromName(event_type.c_str());
	if (key_code == SDLK_UNKNOWN) {
		throw Error(MSG(err) << "could not parse key '" << event_type << "'!");
	}

	if (!param.empty())
		log::log(MSG(warn) << "nothing expected after key name '" << event_type << "', but got '" << param << "'.");

	return sdl_key(key_code, static_cast<SDL_Keymod>(mod));
}

}

/**
 * Convert a string to an event, throw if the string is not a valid event.
 */
Event text_to_event(const std::string &event_str) {
	check_modifiers_once();
	ENSURE(event_str.find('\n'), "Input event string representation must be one line, got '" << event_str << "'.");

	int mod = 0;
	std::smatch event_elements;

	if (std::regex_match(event_str, event_elements, event_pattern)) {
		/**
		 * First element is the entire match, so search from the second.
		 */
		auto groups_it = std::begin(event_elements) + 1;

		auto first_non_empty = std::find_if(groups_it, std::end(event_elements), [] (const std::ssub_match &element) {
			return element.length();
		});

		ENSURE(first_non_empty != std::end(event_elements), "Nothing captured from string representation of event '"<< event_str <<"': regex pattern is broken.");

		auto index_first_non_empty = std::distance(groups_it, first_non_empty);

		if (index_first_non_empty < std::distance(std::begin(modifiers), std::end(modifiers)))
			mod = modifiers[index_first_non_empty];

		auto event_type = groups_it[modifiers.size()].str();
		ENSURE(!event_type.empty(), "Empty group where key was expected in string representation of event '"<< event_str <<"': regex pattern is broken.");

		auto param = groups_it[modifiers.size() + 1].str();

		return to_event(event_type, param, mod);
	} else {
		throw Error(MSG(err) << "could not parse keybinding '" << event_str << "'!");
	}
}

namespace tests {
void parse_event_string() {
	text_to_event("q") == Event{event_class::ALPHA, SDL_GetKeyFromName("q"), modset_t{}} || TESTFAIL;
	text_to_event("Return") == Event{event_class::NONPRINT, SDL_GetKeyFromName("Return"), modset_t{}} || TESTFAIL;
	text_to_event("Ctrl p") == Event{event_class::ALPHA, SDL_GetKeyFromName("p"), sdl_mod(static_cast<SDL_Keymod>(KMOD_CTRL))} || TESTFAIL;
	text_to_event("Shift MOUSE 1") == Event{event_class::MOUSE_BUTTON, 1, sdl_mod(static_cast<SDL_Keymod>(KMOD_SHIFT))} || TESTFAIL;
	text_to_event("MOUSE_UP 1") == Event{event_class::MOUSE_BUTTON_UP, 1, modset_t{}} || TESTFAIL;
	text_to_event("WHEEL -1") == Event{event_class::MOUSE_WHEEL, -1, modset_t{}} || TESTFAIL;
	TESTTHROWS(text_to_event(""));
	TESTTHROWS(text_to_event("WHEEL"));
	TESTTHROWS(text_to_event("MOUSE"));
	TESTTHROWS(text_to_event("MOUSE_DOWN"));
	TESTTHROWS(text_to_event("Blurb MOUSE 1"));
	TESTTHROWS(text_to_event("Shift MICKEY_MOUSE 1"));
	TESTTHROWS(text_to_event("WHEEL TEAR_OFF"));
}

} // tests

}} // openage::input
