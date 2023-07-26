// Copyright 2022-2023 the openage authors. See copying.md for legal info.

#include "window_event_handler.h"

#include <QEvent>

#include "log/log.h"


namespace openage::renderer {


bool WindowEventHandler::eventFilter(QObject * /* obj */, QEvent *event) {
	auto add_to_queue = [this, event]() {
		this->event_queue.push_back(std::shared_ptr<QEvent>(event->clone()));
	};

	switch (event->type()) {
	case QEvent::KeyPress:
	case QEvent::KeyRelease:
	case QEvent::MouseButtonDblClick:
	case QEvent::MouseButtonPress:
	case QEvent::MouseButtonRelease:
	case QEvent::MouseMove:
	case QEvent::Resize:
	case QEvent::Wheel:
		add_to_queue();
		// pass on the event
		return false;
	case QEvent::Close:
		add_to_queue();
		// filter out the event
		return true;

	default:
		break;
	}

	// true: event is filtered = ignored
	// false: event is forwarded to its registered qt handler
	return false;
}


std::vector<std::shared_ptr<QEvent>> WindowEventHandler::pop_events() {
	auto ret = this->event_queue;
	this->event_queue.clear();
	return ret;
}


} // namespace openage::renderer
