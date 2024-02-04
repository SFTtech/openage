// Copyright 2015-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <QKeyCombination>
#include <QMouseEvent>
#include <QWheelEvent>

#include "coord/pixel.h"

namespace openage {
namespace input {

/**
 * Input classes.
 */
enum class event_class {
	// catch-all for all classes of events
	ANY,

	// main classes for the different input devices
	KEYBOARD,
	MOUSE,
	WHEEL,
	GUI,

	// keyboard subclasses
	ALPHA,    // abc
	DIGIT,    // 123
	PRINT,    // remaining printable chars
	NONPRINT, // tab, return, backspace, delete
	OTHER,    // arrows, home, end

	// mouse subclasses
	MOUSE_BUTTON,
	MOUSE_BUTTON_DBL,
	MOUSE_MOVE,
};

struct event_class_hash {
	int operator()(const event_class &s) const;
};

/**
 * map event class to parent class
 */
static std::unordered_map<event_class, event_class, event_class_hash> event_class_rel{
	{event_class::KEYBOARD, event_class::ANY},
	{event_class::MOUSE, event_class::ANY},
	{event_class::WHEEL, event_class::ANY},
	{event_class::GUI, event_class::ANY},

	{event_class::ALPHA, event_class::KEYBOARD},
	{event_class::DIGIT, event_class::KEYBOARD},
	{event_class::PRINT, event_class::KEYBOARD},
	{event_class::NONPRINT, event_class::KEYBOARD},
	{event_class::OTHER, event_class::KEYBOARD},

	{event_class::MOUSE_BUTTON, event_class::MOUSE},
	{event_class::MOUSE_BUTTON_DBL, event_class::MOUSE},
	{event_class::MOUSE_MOVE, event_class::MOUSE},
};

/**
 * Key/button identifiers.
 */
using code_t = int;

/**
 * Base event that contains event class and key/button identifier.
 */
class ClassCode {
public:
	ClassCode() = default;
	ClassCode(event_class cl, code_t code);
	~ClassCode() = default;

	/**
	 * Get all event classes that are covered by this class code,
	 * ordered from most specific to most generic.
	 *
	 * @return Event classes.
	 */
	std::vector<event_class> get_classes() const;

	/**
	 * Check whether this class code is covered by a given event class.
	 *
	 * @param b Event class.
	 *
	 * @return true if the class code's event class is a descendant of or equal to \p other, else false.
	 */
	bool is_subclass(const event_class &other) const;

	/**
	 * Check whether two events are equal.
	 *
	 * Events are equal if their class, code, modset, and state are matching.
	 */
	bool operator==(const ClassCode &other) const;

	/**
	 * Event class.
	 */
	event_class cl;

	/**
	 * Identifier of the key/button that was pressed. It should be unique
	 * for the given event class.
	 */
	code_t code;
};


struct class_code_hash {
	int operator()(const ClassCode &cc) const;
};


/**
 * Modifiers and states.
 */
using modset_t = int;
using state_t = int;

/**
 * Input event, as triggered by some input device like
 * mouse, kezb, joystick, tablet, microwave or dildo.
 * Some modifier keys may also be pressed during the event.
 */
class Event {
public:
	/**
	 * Create a new input event from a window event.
	 *
	 * @param ev Qt input event.
	 */
	Event(const QEvent &ev);

	/**
	 * Create a new input event from custom values.
	 *
	 * This can be used to create the bindings for the input contexts.
	 *
	 * @param cl Event class.
	 * @param code Key/button code.
	 * @param mod Keyboard modifiers bitset.
	 * @param state Key/button state.
	 */
	Event(event_class cl,
	      code_t code,
	      modset_t mod,
	      state_t state);

	~Event() = default;

	/**
	 * Get the associated Qt input event from the window manager.
	 *
	 * This may return \p nullptr if this event was not created from a
	 * Qt input event.
	 *
	 * @return Qt input event, or \p nullptr.
	 */
	const std::shared_ptr<QEvent> &get_event() const;

	/**
	 * Check whether two events are equal.
	 *
	 * Events are equal if their class, code, modset, and state are matching.
	 */
	bool operator==(const Event &other) const;

	/**
	 * Get loggable debug info about the event.
	 */
	std::string info() const;

	/**
	 * Class code.
	 */
	ClassCode cc;

	/**
	 * Keyboard modifiers (CTRL, ALT, SHIFT, META) that were active when
	 * the key/button was pressed.
	 */
	modset_t mod_code;

	/**
	 * Key/button state (pressed, released, double click, ...).
	 */
	state_t state;

private:
	/**
	 * Associated Qt event from the window manager. May be \p nullptr
	 * if this event is not generated from a window event.
	 */
	const std::shared_ptr<QEvent> event;
};


struct event_hash {
	int operator()(const Event &e) const;
};

using event_flags_t = std::unordered_map<std::string, std::string>;

/**
 * Contains information about a triggered event and other
 * input meta information.
 */
struct event_arguments {
	// Triggering event
	const Event e;

	// Mouse position
	const coord::input mouse;
	const coord::input_delta motion;

	// additional settings
	const event_flags_t flags = {};
};


} // namespace input
} // namespace openage
