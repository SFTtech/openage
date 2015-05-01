// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "imagebutton.h"

#include "image.h"

namespace openage {
namespace gui {

ImageButton::ImageButton() {
	image = create<gui::Image>();
}

void ImageButton::auto_size() {
	if (!normal_texture) return;

	int subw, subh;
	normal_texture->get_subtexture_size(normal_subid, &subw, &subh);

	set_left(FormAttachment::adjacent_to(this).offset_by(-subw));
	set_right(FormAttachment::adjacent_to(this).offset_by(subw));
	set_top(FormAttachment::adjacent_to(this).offset_by(-subh));
	set_bottom(FormAttachment::adjacent_to(this).offset_by(subh));
}

void ImageButton::set_state(State new_state) {
	Button::set_state(new_state);

	update_texture();
}

void ImageButton::update_texture() {
	if (state == State::HOVERED && hovered_texture) {
		image->set_texture(hovered_texture, hovered_subid);
	}
	else if (state == State::DOWN && pressed_texture) {
		image->set_texture(pressed_texture, pressed_subid);
	}
	else {
		image->set_texture(normal_texture, normal_subid); // even if it's nullptr
	}
}

} // namespace gui
} // namespace openage
