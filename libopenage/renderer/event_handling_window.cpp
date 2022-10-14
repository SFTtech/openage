// Copyright 2022-2022 the openage authors. See copying.md for legal info.

#include "event_handling_window.h"

#include "log/log.h"

namespace openage::renderer {

EventHandlingQuickWindow::EventHandlingQuickWindow() :
	event_queue{} {
}

std::vector<QEvent *> EventHandlingQuickWindow::poll_events() {
	std::vector<QEvent *> events{this->event_queue};
	this->event_queue.clear();
	return events;
}

void EventHandlingQuickWindow::keyPressEvent(QKeyEvent *ev) {
	this->event_queue.push_back(ev);
}

void EventHandlingQuickWindow::keyReleaseEvent(QKeyEvent *ev) {
	this->event_queue.push_back(ev);
}

void EventHandlingQuickWindow::mouseDoubleClickEvent(QMouseEvent *ev) {
	this->event_queue.push_back(ev);
}

void EventHandlingQuickWindow::mousePressEvent(QMouseEvent *ev) {
	this->event_queue.push_back(ev);
}

void EventHandlingQuickWindow::mouseReleaseEvent(QMouseEvent *ev) {
	this->event_queue.push_back(ev);
}

void EventHandlingQuickWindow::resizeEvent(QResizeEvent *ev) {
	this->event_queue.push_back(ev);
}

} // namespace openage::renderer