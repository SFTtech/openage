// Copyright 2016-2023 the openage authors. See copying.md for legal info.

#include "text_to_event.h"

#include <QKeySequence>

#include "error/error.h"
#include "log/log.h"


namespace openage::input {

Event text_to_event(const std::string &event_str) {
	auto qkey_sequence = QKeySequence::fromString(QString::fromStdString(event_str));

	if (qkey_sequence.isEmpty()) {
		throw Error{MSG(err) << "Invalid event string: key sequence is empty"};
	}

	auto qkey_combination = qkey_sequence[0];
	auto event = Event(event_class::KEYBOARD,
	                   qkey_combination.key(),
	                   qkey_combination.keyboardModifiers(),
	                   QEvent::KeyRelease // TODO: configurable?
	);

	return event;
}

} // namespace openage::input
