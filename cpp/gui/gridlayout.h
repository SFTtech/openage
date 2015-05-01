// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_GUI_GRIDLAYOUT_H_
#define OPENAGE_GUI_GRIDLAYOUT_H_

#include "forward.h"
#include "layout.h"

#include <array>

namespace openage {
namespace gui {

class GridLayoutData : public LayoutData {
	friend class GridLayout;
public:
	static GridLayoutData left() {
		return GridLayoutData{0, 1, 1, 1};
	}

	static GridLayoutData right() {
		return GridLayoutData{0, 1, 1, 1};
	}

	static GridLayoutData center() {
		return GridLayoutData{1, 1, 1, 1};
	}

	static GridLayoutData at(int x, int y) {
		return GridLayoutData{x, y, 1, 1};
	}

	GridLayoutData span(int w, int h) {
		GridLayoutData retVal = *this;
		retVal.w = w;
		retVal.h = h;
		return retVal;
	}

protected:
	GridLayoutData() = default;

	GridLayoutData(int x, int y, int w, int h) :
			x(x), y(y), w(w), h(h) {
		// empty
	}

	int x, y, w, h;
};

class GridLayout : public Layout {
public:
	virtual void layout(const std::vector<std::unique_ptr<Control>> &controls, int container_w, int container_h) override;
	
	virtual std::tuple<int, int> get_best_size(const std::vector<std::unique_ptr<Control>> &controls) override;
protected:
};

} // namespace gui
} // namespace openage

#endif


