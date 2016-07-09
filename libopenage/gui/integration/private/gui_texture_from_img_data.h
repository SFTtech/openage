// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <tuple>
#include <memory>
#include <cstdint>

#include <QRect>

namespace openage {
namespace gui {

class GuiTexture;

/**
 * Convert a square image data to a texture that can be used in the gui.
 * @param pixels image data
 * @param size length of the edge of the texture
 * @param spacing bottom/right spacing in it in pixels
 * @return texture that can be used in the gui and the square with spacing cut off, its size in pixels
 */
std::tuple<std::unique_ptr<GuiTexture>, QSizeF> gui_texture_from_img_data(std::unique_ptr<uint32_t[]> &&pixels, int size, int spacing);

}} // namespace openage::gui
