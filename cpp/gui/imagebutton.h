// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_GUI_IMAGEBUTTON_H_
#define OPENAGE_GUI_IMAGEBUTTON_H_

#include "../texture.h"
#include "button.h"
#include "forward.h"

namespace openage {
namespace gui {

class ImageButton : public Button {
public:
	ImageButton();

	void auto_size();

	void set_textures(const Texture *texture, int normal_subid, int pressed_subid = -1, int hovered_subid = -1) {
		this->normal_texture = texture;
		this->normal_subid = normal_subid;
		this->pressed_texture = (pressed_subid == -1) ? nullptr : texture;
		this->pressed_subid = pressed_subid;
		this->hovered_texture = (hovered_subid == -1) ? nullptr : texture;
		this->hovered_subid = hovered_subid;
		update_texture();
	}

	void set_normal_texture(const Texture *normal_texture, int normal_subid) {
		this->normal_texture = normal_texture;
		this->normal_subid = normal_subid;
		update_texture();
	}

	void set_pressed_texture(const Texture *pressed_texture, int pressed_subid) {
		this->pressed_texture = pressed_texture;
		this->pressed_subid = pressed_subid;
		update_texture();
	}

	void set_hovered_texture(const Texture *hovered_texture, int hovered_subid) {
		this->hovered_texture = hovered_texture;
		this->hovered_subid = hovered_subid;
		update_texture();
	}

protected:
	virtual void set_state(State new_state) override;

	void update_texture();

	const Texture *normal_texture = nullptr;
	int normal_subid = 0;
	const Texture *pressed_texture = nullptr;
	int pressed_subid;
	const Texture *hovered_texture = nullptr;
	int hovered_subid;

	Image *image;
};

} // namespace gui
} // namespace openage

#endif

