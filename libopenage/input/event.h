// Copyright 2015-2023 the openage authors. See copying.md for legal info.

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
 * highest level classes of input
 */
enum class event_class {
	KEYBOARD,
	MOUSE,
	WHEEL,
	GUI,
};


/**
 * Input event, as triggered by some input device like
 * mouse, kezb, joystick, tablet, microwave or dildo.
 * Some modifier keys may also be pressed during the event.
 */
class Event {
public:
	Event(event_class cl);
	Event(event_class cl,
	      coord::input mouse_position,
	      coord::input_delta mouse_motion);

	~Event() = default;

	event_class get_class() const;

	virtual int hash() const = 0;
	virtual bool operator==(const Event &other) const = 0;

private:
	event_class cl;
	coord::input mouse_position{0, 0};
	coord::input_delta mouse_motion{0, 0};
};


class KeyEvent : public Event {
public:
	KeyEvent(const QKeyCombination key);

	~KeyEvent() = default;

	int hash() const override;
	bool operator==(const Event &other) const override;

private:
	QKeyCombination key;
};


class MouseEvent : public Event {
public:
	MouseEvent(const QMouseEvent &ev);

	~MouseEvent() = default;

	int hash() const override;
	bool operator==(const Event &other) const override;

private:
	Qt::MouseButtons buttons;
	Qt::KeyboardModifiers mods;
};


class WheelEvent : public Event {
public:
	WheelEvent(const QWheelEvent &ev);

	~WheelEvent() = default;

	int hash() const override;
	bool operator==(const Event &other) const override;

private:
	int angle_delta;

	Qt::MouseButtons buttons;
	Qt::KeyboardModifiers mods;
};


struct event_hash {
	int operator()(const Event &e) const;
};


} // namespace input
} // namespace openage
