// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "border.h"

#include "image.h"

namespace openage {
namespace gui {

Border::Border() {
	for (auto &img : slices) {
		img = create<Image>();
	}
	contents = create<Container>();
}

void Border::set_texture(const Texture *texture, int base_subid) {
	for (size_t i = 0; i < slices.size(); ++i) {
		slices[i]->set_texture(texture, base_subid + i);
	}

#if 0
	slices[0]->set_left(FormAttachment::left());
	slices[3]->set_left(FormAttachment::left());
	slices[6]->set_left(FormAttachment::left());
	
	slices[2]->set_right(FormAttachment::right());
	slices[5]->set_right(FormAttachment::right());
	slices[8]->set_right(FormAttachment::right());
	
	slices[0]->set_top(FormAttachment::top());
	slices[1]->set_top(FormAttachment::top());
	slices[2]->set_top(FormAttachment::top());

	slices[6]->set_bottom(FormAttachment::bottom());
	slices[7]->set_bottom(FormAttachment::bottom());
	slices[8]->set_bottom(FormAttachment::bottom());
	
	slices[1]->set_left(FormAttachment::adjacent_to(slices[0]));
	slices[4]->set_left(FormAttachment::adjacent_to(slices[3]));
	slices[7]->set_left(FormAttachment::adjacent_to(slices[6]));
	
	slices[1]->set_right(FormAttachment::adjacent_to(slices[2]));
	slices[4]->set_right(FormAttachment::adjacent_to(slices[5]));
	slices[7]->set_right(FormAttachment::adjacent_to(slices[8]));

	slices[3]->set_top(FormAttachment::adjacent_to(slices[0]));
	slices[4]->set_top(FormAttachment::adjacent_to(slices[1]));
	slices[5]->set_top(FormAttachment::adjacent_to(slices[2]));

	slices[3]->set_bottom(FormAttachment::adjacent_to(slices[6]));
	slices[4]->set_bottom(FormAttachment::adjacent_to(slices[7]));
	slices[5]->set_bottom(FormAttachment::adjacent_to(slices[8]));
#endif
}

} // namespace gui
} // namespace openage

