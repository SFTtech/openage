// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_GUI_CHECKBOX_H_
#define OPENAGE_GUI_CHECKBOX_H_

#include "button.h"

namespace openage {
namespace gui {

class CheckBox : public Button {
public:
	CheckBox();

	bool is_checked() {
		return checked;
	}

	std::function<void(bool)> on_toggle; // should be some kind of signal

protected:
	virtual void set_checked(bool new_checked);

	bool checked = false;
};

} // namespace gui
} // namespace openage

#endif

