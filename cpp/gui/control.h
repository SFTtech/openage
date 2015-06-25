// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_GUI_CONTROL_H_
#define OPENAGE_GUI_CONTROL_H_

#include "forward.h"
#include "layout.h"

#include <cassert>
#include <cstdint>
#include <memory>
#include <tuple>
#include <vector>

namespace openage {
namespace gui {

using seconds_t = double;

class Control {
	friend class Layout;
public:
	Control();
	Control(const Control &other) = delete;
	Control(Control &&other) = delete;

	void operator =(const Control &other) = delete;
	void operator =(Control &&other) = delete;

	virtual ~Control();

	template<typename T>
	void set_layout_data(const T &layout_data) {
		set_layout_data(std::unique_ptr<LayoutData>{new T{layout_data}});
	}

	void set_layout_data(std::unique_ptr<LayoutData> &&layout_data);

	virtual bool contains(int x, int y) const {
		if (x < left()) return false;
		if (x >= right()) return false;
		if (y < top()) return false;
		if (y >= bottom()) return false;
		return true;
	}

	virtual void update(seconds_t /*t*/) {}

	virtual void draw(const Drawer &drawer) const = 0;

	int left() const { return layout_data->left(); }
	int right() const { return layout_data->right(); }
	int top() const { return layout_data->top(); }
	int bottom() const { return layout_data->bottom(); }
	int width() const { return right() - left(); }
	int height() const { return bottom() - top(); }

	virtual std::tuple<int, int> get_best_size() const = 0;

	virtual void mouse_left() { }
	virtual bool mouse_moved(int /*x*/, int /*y*/) { return false; }
	virtual bool mouse_pressed(std::uint8_t /*button*/) { return false; }
	virtual void mouse_released(std::uint8_t /*button*/) { }
	virtual bool mouse_scrolled(int /*relX*/, int /*relY*/) { return false; }

protected:
	std::unique_ptr<LayoutData> layout_data;
};

class MouseTracker {
public:
	void mouse_moved(int x, int y) { mX = x; mY = y; }

	void mouse_left() { mX = MOUSE_OUT; }

	bool has_mouse() const { return mX != MOUSE_OUT; }

	int x() const { assert(has_mouse()); return mX; }

	int y() const { assert(has_mouse()); return mY; }

private:
	static constexpr int MOUSE_OUT = std::numeric_limits<int>::min();
	int mX = MOUSE_OUT, mY;
};

} // namespace gui
} // namespace openage

#endif
