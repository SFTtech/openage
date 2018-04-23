// Copyright 2015-2018 the openage authors. See copying.md for legal info.

#include "gui_application_impl.h"

#include <locale>
#include <cassert>

#include <QtGlobal>
#include <QtDebug>

namespace qtsdl {

std::weak_ptr<GuiApplicationImpl> GuiApplicationImpl::instance;

std::shared_ptr<GuiApplicationImpl> GuiApplicationImpl::get() {
	std::shared_ptr<GuiApplicationImpl> candidate = GuiApplicationImpl::instance.lock();

	assert(!candidate || std::this_thread::get_id() == candidate->owner);

	return candidate ? candidate : std::shared_ptr<GuiApplicationImpl>{new GuiApplicationImpl};
}

GuiApplicationImpl::~GuiApplicationImpl() {
	assert(std::this_thread::get_id() == this->owner);
}

void GuiApplicationImpl::processEvents() {
	assert(std::this_thread::get_id() == this->owner);
#ifndef __APPLE__
	this->app.processEvents();
#endif
}

namespace {
	int argc = 1;
	char arg[] = "qtsdl";
	char *argv = &arg[0];
}

GuiApplicationImpl::GuiApplicationImpl()
	:
#ifndef NDEBUG
	owner{std::this_thread::get_id()},
#endif
	app{argc, &argv}
{
	// Set locale back to POSIX for the decimal point parsing (see qcoreapplication.html#locale-settings).
	std::locale::global(std::locale().combine<std::numpunct<char>>(std::locale::classic()));

	qInfo() << "Compiled with Qt" << QT_VERSION_STR << "and run with Qt" << qVersion();
}

} // namespace qtsdl
