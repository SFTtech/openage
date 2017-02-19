// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "cvar_manager_link.h"

#include <QtQml>

#include "../cvar/cvar.h"

namespace openage {
namespace gui {

namespace
{
const int registration = qmlRegisterUncreatableType<CVarManagerLink>("yay.sfttech.openage", 1, 0, "CVarManager", "CVarManager is a part of the Engine and can not be created from QML.");
}

CVarManagerLink::CVarManagerLink(cvar::CVarManager &cvar_manager)
	:
	GuiCVarListModel{},
	core{cvar_manager} {
	Q_UNUSED(registration);

	/**
	 * making possible to get cvar_manager QML object from cvar_manager
	 */
	cvar_manager.gui_link = this;

	this->set_cvar_manager(&cvar_manager);
}

CVarManagerLink::~CVarManagerLink() {
}

}} // namespace openage::gui
