// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "gui_texture_handle.h"

#include <numeric>

#include "../../../texture.h"

namespace openage {
namespace gui {

SizedTextureHandle::SizedTextureHandle()
	:
	TextureHandle{nullptr, 0},
	size{} {
}

SizedTextureHandle::SizedTextureHandle(const TextureHandle &handle, const QSize &size)
	:
	TextureHandle(handle),
	size{size} {
}

bool isAtlasTexture(const TextureHandle &texture_handle) {
	return texture_handle.subid >= 0 && texture_handle.texture->get_subtexture_count() > 1;
}

QSize textureSize(const SizedTextureHandle &texture_handle) {
	return texture_handle.size;
}

QSize native_size(const TextureHandle &texture_handle) {
	auto tex = texture_handle.texture;

	if (isAtlasTexture(texture_handle)) {
		auto sub = tex->get_subtexture(texture_handle.subid);
		return QSize(sub->w, sub->h);
	} else {
		return QSize(tex->w, tex->h);
	}
}

QSize aspect_fit_size(const TextureHandle &texture_handle, const QSize &requested_size) {
	const QSize size = native_size(texture_handle);

	if (requested_size.isValid()) {
		const QSize bounding_size(requested_size.width() > 0 ? requested_size.width() : size.width(), requested_size.height() > 0 ? requested_size.height() : size.height());

		// If requested_size.isEmpty() then the caller don't care how big one or two dimensions can grow.
		return size.scaled(bounding_size, requested_size.isEmpty() && (requested_size.width() > size.width() || requested_size.height() > size.height()) ? Qt::KeepAspectRatioByExpanding : Qt::KeepAspectRatio);
	} else {
		return size;
	}
}

}} // namespace openage::gui
