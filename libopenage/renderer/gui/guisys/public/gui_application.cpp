// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#include <utility>

#include "gui_application.h"

#include "../private/gui_application_impl.h"

namespace qtgui {

GuiApplication::GuiApplication() :
	application{GuiApplicationImpl::get()} {
}

GuiApplication::GuiApplication(std::shared_ptr<GuiApplicationImpl> application) :
	application{application} {
}

GuiApplication::~GuiApplication() = default;

void GuiApplication::process_events() {
	this->application->processEvents();
}

} // namespace qtgui
