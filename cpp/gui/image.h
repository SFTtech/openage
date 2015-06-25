// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_GUI_IMAGE_H_
#define OPENAGE_GUI_IMAGE_H_

#include "control.h"

#include "../texture.h"

namespace openage {
namespace gui {

class Image : public Control {
public:
	void set_texture(const Texture *texture, int subid=0);

	void draw(const Drawer &drawer) const override;

	virtual std::tuple<int, int> get_best_size() const override {
		std::tuple<int, int> retVal;
		texture->get_subtexture_size(subid, &std::get<0>(retVal), &std::get<1>(retVal));
		return retVal;
	}

protected:
	const Texture *texture;
	int subid;
};

} // namespace gui
} // namespace openage

#endif
