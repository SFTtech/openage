// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "container.h"
#include "drawer.h"
#include "layout.h"

namespace openage {
namespace gui {

ContainerBase::ContainerBase() = default;

ContainerBase::~ContainerBase() = default;

void ContainerBase::update(seconds_t t) {
	if (needs_layout) {
		needs_layout = false;
		auto w = width();
		auto h = height();
		layout->layout(*this);
		if (mt.has_mouse()) {
			mouse_moved(mt.x(), mt.y());
		}
	}

	for (auto &c : controls) {
		c->update(t);
	}
}

void ContainerBase::draw(const Drawer &drawer) const {
	Drawer dr = drawer;
	dr.translate(left(), top());
	for (auto &c : controls) {
		c->draw(dr);
	}
}

bool ContainerBase::contains(int x, int y) const {
	if (!Control::contains(x, y)) return false;
	x -= left(); y -= top();
	for (auto &c : controls) {
		if (c->contains(x, y)) {
			return true;
		}
	}
	return false;
}

std::tuple<int, int> ContainerBase::get_best_size() const {
	return layout->get_best_size(controls);
}

void ContainerBase::mouse_left() {
	assert(mouse_capture_count == 0);
	mt.mouse_left();
	if (control_under_mouse) {
		control_under_mouse->mouse_left();
		control_under_mouse = nullptr;
	}
	mouse_left_bg();
}

bool ContainerBase::mouse_moved(int x, int y) {
	mt.mouse_moved(x, y);
	x -= left(); y -= top();
	if (mouse_capture_count > 0) {
		if (control_under_mouse) {
			control_under_mouse->mouse_moved(x, y);
		}
		else {
			mouse_moved_bg(mt.x(), mt.y());
		}
		return true; // always consume event
	}

	if (control_under_mouse && !control_under_mouse->contains(x, y)) {
		control_under_mouse->mouse_left();
		control_under_mouse = nullptr;
	}

	if (!control_under_mouse) {
		control_under_mouse = control_at(x, y);
	}

	if (control_under_mouse && control_under_mouse->mouse_moved(x, y)) {
		mouse_left_bg();
		return true;
	}

	return mouse_moved_bg(mt.x(), mt.y());
}

bool ContainerBase::mouse_pressed(std::uint8_t button) {
	if (mouse_capture_count > 0) {
		if (control_under_mouse) {
			control_under_mouse->mouse_pressed(button);
		}
		else {
			mouse_pressed_bg(button);
		}
		mouse_capture_count += 1;
		return true; // always consume event
	}
	if (control_under_mouse) {
		if (control_under_mouse->mouse_pressed(button)) {
			mouse_capture_count += 1;
			return true;
		}
		control_under_mouse->mouse_left();
		control_under_mouse = nullptr;
	}

	if (mouse_pressed_bg(button)) {
		mouse_capture_count += 1;
		return true;
	}
	return false;
}

void ContainerBase::mouse_released(std::uint8_t button) {
	assert(mouse_capture_count > 0);
	if (control_under_mouse) {
		control_under_mouse->mouse_released(button);
	}
	else {
		mouse_released_bg(button);
	}

	mouse_capture_count -= 1;
	if (mouse_capture_count == 0) {
		if (!contains(mt.x(), mt.y())) {
			mouse_left();
			return;
		}

		mouse_moved(mt.x(), mt.y());
	}
}

bool ContainerBase::mouse_scrolled(int relX, int relY) {
	if (mouse_capture_count > 0) {
		if (control_under_mouse) {
			control_under_mouse->mouse_scrolled(relX, relY);
		}
		else {
			mouse_scrolled_bg(relX, relY);
		}
		return true; // always consume event
	}
	if (control_under_mouse && control_under_mouse->mouse_scrolled(relX, relY)) {
		return true;
	}
	return mouse_scrolled_bg(relX, relY);
}

Control *ContainerBase::control_at(int x, int y) const {
	for (auto &c : controls) {
		if (c->contains(x, y)) {
			return c.get();
		}
	}
	return nullptr;
}
	
void ContainerBase::set_layout(std::unique_ptr<Layout> &&layout) {
	this->layout = std::move(layout);
	needs_layout = true;
}

} // namespace gui
} // namespace openage
