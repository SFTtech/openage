// Copyright 2015-2022 the openage authors. See copying.md for legal info.

#include "gui_application_with_logger.h"

#include "../../guisys/private/gui_application_impl.h"
#include "../../integration/private/gui_log.h"

namespace openage::renderer::gui {

namespace {
std::shared_ptr<qtgui::GuiApplicationImpl> create() {
	qInstallMessageHandler(gui_log);
	return qtgui::GuiApplicationImpl::get();
}
}

GuiApplicationWithLogger::GuiApplicationWithLogger()
	:
	GuiApplication{create()} {
}

GuiApplicationWithLogger::~GuiApplicationWithLogger() = default;

} // namespace openage::renderer::gui
