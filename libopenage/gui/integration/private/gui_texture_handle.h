// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <QSize>

namespace openage {

class Texture;

namespace gui {

struct TextureHandle {
	openage::Texture *texture;
	int subid;
};

struct SizedTextureHandle : TextureHandle {
	SizedTextureHandle();
	SizedTextureHandle(const TextureHandle &handle, const QSize &size);
	QSize size;
};

bool isAtlasTexture(const TextureHandle &texture_handle);
QSize textureSize(const SizedTextureHandle &texture_handle);

QSize native_size(const TextureHandle &texture_handle);

/**
 * The sourceSize property of the Image QML element ends up here for checking.
 *
 * @param requested_size sourceSize if used in Image QML element.
 * @return if !requested_size.isValid() or requested_size.isNull() then native size, otherwise fit in the requested_size (zero component in requested_size means unbounded).
 */
QSize aspect_fit_size(const TextureHandle &texture_handle, const QSize &requested_size);

}} // namespace openage::gui
