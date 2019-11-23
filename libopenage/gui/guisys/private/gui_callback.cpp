// Copyright 2015-2019 the openage authors. See copying.md for legal info.

#include "gui_callback.h"

#include <cassert>

namespace qtsdl {

namespace {
const int registration = qRegisterMetaType<std::function<void()>>("function");
}

GuiCallback::GuiCallback()
	:
	QObject{} {
	Q_UNUSED(registration);
}

GuiCallback::~GuiCallback() = default;

void GuiCallback::process(const std::function<void()> &f) {
	f();
}

} // namespace qtsdl
