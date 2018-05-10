// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "gui_application_with_logger.h"

#include "../../guisys/private/gui_application_impl.h"
#include "../private/gui_log.h"

namespace openage {
namespace gui {

namespace {
std::shared_ptr<qtsdl::GuiApplicationImpl> create() {
	qInstallMessageHandler(gui_log);
	return qtsdl::GuiApplicationImpl::get();
}
}

GuiApplicationWithLogger::GuiApplicationWithLogger()
	:
	GuiApplication{create()} {
}

GuiApplicationWithLogger::~GuiApplicationWithLogger() {
}

}} // namespace openage::gui
