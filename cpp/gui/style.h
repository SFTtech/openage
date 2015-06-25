// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_GUI_STYLE_H_
#define OPENAGE_GUI_STYLE_H_

#include "../assetmanager.h"
#include "forward.h"

namespace openage {
namespace gui {

class Style {
public:
	Style(AssetManager *asset_manager) : asset_manager(asset_manager) {}

	std::unique_ptr<LabelButton> create_button() const;

	std::unique_ptr<Button> create_image_button(const char *texture, int base_subid) const;

protected:
	AssetManager *asset_manager;
};

} // namespace gui
} // namespace openage

#endif
