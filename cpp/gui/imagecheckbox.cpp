// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "imagecheckbox.h"

#include "image.h"

namespace openage {
namespace gui {

ImageCheckBox::ImageCheckBox() {
	image = create<gui::Image>();
}

void ImageCheckBox::auto_size() {
	if (!normal_texture) return;

	int subw, subh;
	normal_texture->get_subtexture_size(normal_subid, &subw, &subh);

	set_left(FormAttachment::adjacent_to(this).offset_by(-subw));
	set_right(FormAttachment::adjacent_to(this).offset_by(subw));
	set_top(FormAttachment::adjacent_to(this).offset_by(-subh));
	set_bottom(FormAttachment::adjacent_to(this).offset_by(subh));
}

void ImageCheckBox::set_checked(bool new_checked) {
	CheckBox::set_checked(new_checked);

	update_texture();
}

void ImageCheckBox::update_texture() {
	if (checked) {
		image->set_texture(normal_texture, checked_subid);
	} else {
		image->set_texture(normal_texture, normal_subid);
	}
}

} // namespace gui
} // namespace openage
