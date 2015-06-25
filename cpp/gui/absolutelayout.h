// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_GUI_ABSOLUTELAYOUT_H_
#define OPENAGE_GUI_ABSOLUTELAYOUT_H_

#include "layout.h"

namespace openage {
namespace gui {

class AbsoluteLayoutData : public LayoutData {
public:
	void set_position(int x, int y, int w, int h) {
		position = {{x, x + w, y, y + h}};
	}
};

class AbsoluteLayout : public Layout {
protected:
	virtual void layout(const std::vector<std::unique_ptr<Control>> &controls, int container_w, int container_h) {
		(void)controls;
		(void)container_w;
		(void)container_h;
	}
};

} // namespace gui
} // namespace openage

#endif

