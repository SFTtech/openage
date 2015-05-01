// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "control.h"

#include "layout.h"

namespace openage {
namespace gui {

constexpr int MouseTracker::MOUSE_OUT;

Control::Control() = default;

Control::~Control() = default;

void Control::set_layout_data(std::unique_ptr<LayoutData> &&layout_data) {
	this->layout_data = std::move(layout_data);
}

} // namespace gui
} // namespace openage
