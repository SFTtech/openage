// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_GUI_BORDER_H_
#define OPENAGE_GUI_BORDER_H_

#include "../texture.h"
#include "container.h"
#include "forward.h"

#include <array>

namespace openage {
namespace gui {

class Border : public ContainerBase {
public:
	Border();

	void set_texture(const Texture *texture, int base_subid);

	Container *get_contents() {
		return contents;
	}

protected:
	std::array<Image *, 9> slices;
	Container *contents;
};

} // namespace gui
} // namespace openage

#endif



