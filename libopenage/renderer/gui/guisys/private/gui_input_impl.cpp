// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#include "gui_input_impl.h"

#include <QCoreApplication>
#include <QEvent>
#include <QQuickWindow>

#include "renderer/gui/guisys/public/gui_renderer.h"

namespace qtgui {

GuiInputImpl::GuiInputImpl(const std::shared_ptr<GuiRenderer> &renderer) :
	QObject{},
	window{renderer->get_window()} {
}


bool GuiInputImpl::process(const std::shared_ptr<QEvent> &event) {
	switch (event->type()) {
	case QEvent::MouseButtonPress:
	case QEvent::MouseButtonRelease:
	case QEvent::MouseMove:
	case QEvent::MouseButtonDblClick: {
		// Quote from the QQuickRenderControl Example:
		// Use the constructor taking position and globalPosition. That puts position into the
		// event's position and scenePosition, and globalPosition into the event's globalPosition. This way
		// the scenePosition in e is ignored and is replaced by position. This is necessary
		// because QQuickWindow thinks of itself as a top-level window always.
		auto mouse_event = std::dynamic_pointer_cast<QMouseEvent>(event);
		QMouseEvent ev{mouse_event->type(),
		               mouse_event->position(),
		               mouse_event->globalPosition(),
		               mouse_event->button(),
		               mouse_event->buttons(),
		               mouse_event->modifiers()};
		return QCoreApplication::instance()->sendEvent(this->window.get(), &ev);
	}
	case QEvent::KeyPress:
	case QEvent::KeyRelease:
		return QCoreApplication::instance()->sendEvent(this->window.get(), event.get());

	case QEvent::Wheel:
		return QCoreApplication::instance()->sendEvent(this->window.get(), event.get());

	default:
		break;
	}
	return false;
}

} // namespace qtgui
