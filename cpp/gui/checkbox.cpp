// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "checkbox.h"

namespace openage {
namespace gui {

CheckBox::CheckBox() {
	on_click([this]{
		set_checked(!checked); // toggle state
		if (on_toggle) on_toggle(checked);
	});
}

void CheckBox::set_checked(bool new_checked) {
	checked = new_checked;
}

} // namespace gui
} // namespace openage

