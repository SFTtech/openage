// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "gui_global_font.h"

#include <QGuiApplication>
#include <QFont>

#include "../cvar/cvar.h"
#include "../log/log.h"

#include "guisys/link/qml_engine_with_singleton_items_info.h"

namespace openage {
namespace gui {

GuiGlobalFont::GuiGlobalFont(qtsdl::QmlEngineWithSingletonItemsInfo *engine_with_singleton_items_info, cvar::CVarManager &cvar_manager)
	:
	QObject{} {

	QObject::connect(this, &GuiGlobalFont::assign_font, this, &GuiGlobalFont::on_assign_font, Qt::QueuedConnection);
	QObject::connect(this, &GuiGlobalFont::reload_gui_manually, engine_with_singleton_items_info, &qtsdl::QmlEngineWithSingletonItemsInfo::reload_gui_manually, Qt::QueuedConnection);
	cvar_manager.create("FONT_SIZE", std::make_pair([] { return std::to_string(QGuiApplication::font().pointSize()); }, std::bind(&GuiGlobalFont::set_font_size, this, std::placeholders::_1)));
	// TODO: no load here...
}

void GuiGlobalFont::on_assign_font(const QFont &font) {
	QGuiApplication::setFont(font);
}

void GuiGlobalFont::set_font_size(const std::string &font_size) {
	try {
		size_t chars_used;
		const int size_int = std::stoi(font_size, &chars_used);

		if (chars_used == font_size.size() && size_int > 0) {
			auto font = QGuiApplication::font();

			if (font.pointSize() != size_int) {
				font.setPointSize(size_int);
				emit this->assign_font(font, QPrivateSignal{});

				emit this->reload_gui_manually(QPrivateSignal{});
			}
		}
	} catch(std::exception&) {
		log::log(MSG(err) << "Unable to parse font size: '" << font_size << "'");
	}
}

}} // namespace openage::gui
