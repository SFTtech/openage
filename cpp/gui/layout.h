// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_GUI_LAYOUT_H_
#define OPENAGE_GUI_LAYOUT_H_

#include "forward.h"

#include <limits>
#include <memory>
#include <vector>

namespace openage {
namespace gui {

class Layout {
public:
	void layout(ContainerBase &container);
	virtual void layout(const std::vector<std::unique_ptr<Control>> &controls, int container_w, int container_h) = 0;

	virtual std::tuple<int, int> get_best_size(const std::vector<std::unique_ptr<Control>> &controls) = 0;
protected:
	LayoutData *get_layout_data(const Control *control);

	int last_width = 0;
	int last_height = 0;
};

class LayoutData {
public:
	virtual ~LayoutData() = default;

	int left() const { return position[0]; }
	int right() const { return position[1]; }
	int top() const { return position[2]; }
	int bottom() const { return position[3]; }

	void invalidate() {
		position[3] = INVALID_POS;
	}

protected:
	static constexpr int INVALID_POS = std::numeric_limits<int>::min();
	static constexpr int EVALUATING_POS = INVALID_POS + 1;

	std::array<int, 4> position = {{0, 0, 0, INVALID_POS}};
};

} // namespace gui
} // namespace openage

#endif
