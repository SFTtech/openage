// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <QKeyCombination>

namespace openage {
namespace input {

/**
 * highest level classes of input
 */
enum class event_class {
	KEYBOARD,
	MOUSE,
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

	~Event() = default;

	virtual int hash() const = 0;
	virtual bool operator==(const Event &other) const = 0;

	event_class cl;
};


class KeyEvent : public Event {
public:
	KeyEvent(QKeyCombination key);

	~KeyEvent() = default;

	int hash() const override;
	bool operator==(const Event &other) const override;

private:
	QKeyCombination key;
};


struct event_hash {
	int operator()(const Event &e) const;
};


} // namespace input
} // namespace openage
