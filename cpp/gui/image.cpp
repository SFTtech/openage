// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "image.h"

#include "drawer.h"

namespace openage {
namespace gui {

void Image::set_texture(const Texture *texture, int subid) {
	this->texture = texture;
	this->subid = subid;
}

void Image::draw(const Drawer &drawer) const {
	if (!texture) return;

	Drawer dr{drawer};
	dr.clip(left(), top(), width(), height());

	int subw, subh;
	texture->get_subtexture_size(subid, &subw, &subh);
	
	for (auto x = left(); x < right(); x += subw) {
		for (auto y = top(); y < bottom(); y += subh) {
			dr.draw_texture(texture, subid, x, y);
		}
	}
}

} // namespace gui
} // namespace openage
