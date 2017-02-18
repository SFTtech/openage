// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <QObject>

#include <string>

namespace qtsdl {
class QmlEngineWithSingletonItemsInfo;
}

namespace openage {
namespace cvar {
class CVarManager;
} // namespace cvar

namespace gui {

class GuiGlobalFont : public QObject {
	Q_OBJECT

public:
	explicit GuiGlobalFont(qtsdl::QmlEngineWithSingletonItemsInfo *engine_with_singleton_items_info, cvar::CVarManager &cvar_manager);

signals:
	void reload_gui_manually(QPrivateSignal);
	void assign_font(const QFont& font, QPrivateSignal);

private slots:
	void on_assign_font(const QFont &font);

private:
	void set_font_size(const std::string &font_size);
};

}} // namespace openage::gui
