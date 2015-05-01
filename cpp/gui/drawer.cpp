// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "drawer.h"

#include "../engine.h"

namespace openage {
namespace gui {

Drawer::Drawer(int w, int h) :
		scale_x{1.0f}, scale_y{-1.0f},
		translate_x{0.0f}, translate_y{static_cast<float>(-h)} {
	(void)w;
}

void Drawer::draw_texture(const Texture *texture, int subid, float gui_x, float gui_y) const {
	int width, height;
	texture->get_subtexture_size(subid, &width, &height);

	int left, right, bottom, top;
	std::tie(left, top) = to_screen_coord(gui_x, gui_y);
	std::tie(right, bottom) = to_screen_coord(gui_x + width, gui_y + height);

	int orig_left = left;
	int orig_top = top;

	std::tie(left, top) = clip_coord(std::make_tuple(left, top));
	std::tie(right, bottom) = clip_coord(std::make_tuple(right, bottom));

	width = right - left;
	if (width <= 0) return;

	height = top - bottom;
	if (height <= 0) return;

	texture->draw_clipped(left, bottom,
	                      left - orig_left, top - orig_top,
	                      width, height,
	                      subid);
}

void Drawer::draw_text(const std::string &text, int size, float gui_x, float gui_y) const {
	// ignores clipping for now
	int left, top;
	std::tie(left, top) = to_screen_coord(gui_x, gui_y);
	Engine::get().render_text({left, top - size + 2}, size, "%s", text.c_str());
}

} // namespace gui
} // namespace openage
