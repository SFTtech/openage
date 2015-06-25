// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_GUI_DRAWER_H_
#define OPENAGE_GUI_DRAWER_H_

#include "../texture.h"

#include <array>
#include <cmath>
#include <limits>
#include <tuple>

namespace openage {
namespace gui {

class Drawer {
public:
	Drawer(int w, int h);

	void scale(float scale) {
		this->scale(scale, scale);
	}

	void scale(float scale_x, float scale_y) {
		this->scale_x *= scale_x;
		this->scale_y *= scale_y;
		this->translate_x /= scale_x;
		this->translate_y /= scale_y;
	}

	void translate(float translate_x, float translate_y) {
		this->translate_x += translate_x;
		this->translate_y += translate_y;
	}

	void clip(float x, float y, float w, float h) {
		std::tie(clipL, clipT) = clip_coord(to_screen_coord(x, y));
		std::tie(clipR, clipB) = clip_coord(to_screen_coord(x + w, y + h));
	}

	void draw_texture(const Texture *texture, int subid, float gui_x, float gui_y) const;

	void draw_text(const std::string &text, int size, float gui_x, float gui_y) const;

protected:
	std::tuple<int, int> to_screen_coord(float gui_x, float gui_y) const {
		int x = std::lround((gui_x + translate_x) * scale_x);
		int y = std::lround((gui_y + translate_y) * scale_y);
		return std::make_tuple(x, y);
	}

	std::tuple<int, int> clip_coord(std::tuple<int, int> hud_coord) const {
		auto &x = std::get<0>(hud_coord);
		auto &y = std::get<1>(hud_coord);

		if (x < clipL) x = clipL;
		else if (x > clipR) x = clipR;

		if (y < clipB) y = clipB;
		else if (y > clipT) y = clipT;

		return hud_coord;
	}

private:
	float scale_x, scale_y;
	float translate_x, translate_y;

	// uses the screen coordinate system
	int clipL = std::numeric_limits<int>::lowest();
	int clipR = std::numeric_limits<int>::max();
	int clipB = std::numeric_limits<int>::lowest();
	int clipT = std::numeric_limits<int>::max();
};

} // namespace gui
} // namespace openage

#endif
