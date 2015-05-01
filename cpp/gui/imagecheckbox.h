// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_GUI_IMAGECHECKBOX_H_
#define OPENAGE_GUI_IMAGECHECKBOX_H_

#include "../texture.h"
#include "checkbox.h"
#include "forward.h"

namespace openage {
namespace gui {

class ImageCheckBox : public CheckBox {
public:
	ImageCheckBox();

	void auto_size();

	void set_textures(const Texture *texture, int normal_subid, int checked_subid = -1) {
		this->normal_texture = texture;
		this->normal_subid = normal_subid;
		this->checked_subid = checked_subid;
		update_texture();
	}

protected:
	virtual void set_checked(bool new_checked) override;

	void update_texture();

	const Texture *normal_texture = nullptr;
	int normal_subid = 0;
	int checked_subid = -1;

	Image *image;
};

} // namespace gui
} // namespace openage

#endif


