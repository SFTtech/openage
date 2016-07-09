// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#include "gui_texture_from_img_data.h"

#include "../../../texture.h"

#include "gui_texture.h"

namespace openage {
namespace gui {

std::tuple<std::unique_ptr<GuiTexture>, QSizeF> gui_texture_from_img_data(std::unique_ptr<uint32_t[]> &&pixels, int size, int spacing) {
	return std::make_tuple(std::make_unique<GuiTexture>(std::make_unique<Texture>(size, size, std::move(pixels)), QSize{size, size}), QSizeF(size - spacing, size - spacing));
}

}} // namespace openage::gui
