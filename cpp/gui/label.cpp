// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "label.h"

#include "drawer.h"

namespace openage {
namespace gui {

Label::Label() = default;

void Label::set_text(const std::string &text) {
	this->text = text;
}

void Label::set_horizontal_alignment(HorizontalAlignment horizontal_alignment) {
	this->horizontal_alignment = horizontal_alignment;
}

void Label::draw(const Drawer &drawer) const {
	if (text.empty()) return;

	assert(height() == default_height());

	Drawer dr{drawer};
	dr.clip(left(), top(), width(), height());
	dr.draw_text(text, 20, left(), top());
}

} // namespace gui
} // namespace openage
