// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "gui_cvar_list_model.h"

namespace openage {
namespace cvar {
class CVarManager;
}
namespace gui {
class CVarManagerLink;
}}

namespace qtsdl {
template<>
struct Wrap<openage::cvar::CVarManager> {
	using Type = openage::gui::CVarManagerLink;
};

template<>
struct Unwrap<openage::gui::CVarManagerLink> {
	using Type = openage::cvar::CVarManager;
};

}

namespace openage {
namespace gui {

class CVarManagerLink : public GuiCVarListModel {
	Q_OBJECT

public:
	explicit CVarManagerLink(cvar::CVarManager &cvar_manager);
	virtual ~CVarManagerLink();

	template<typename U>
	U* get() const {
		return &this->core;
	}

private:
	explicit CVarManagerLink(cvar::CVarManager &cvar_manager, int);

	cvar::CVarManager &core;
};

}} // namespace openage::gui
