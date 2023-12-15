// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include <QSGTexture>

#include "gui_texture_handle.h"

namespace openage::renderer::gui {

class GuiTexture : public QSGTexture {
	Q_OBJECT

public:
	explicit GuiTexture(const SizedTextureHandle &texture_handle);
	virtual ~GuiTexture();

private:
	virtual qint64 comparisonKey() const override;
	virtual bool hasAlphaChannel() const override;
	virtual bool hasMipmaps() const override;
	virtual bool isAtlasTexture() const override;
	virtual QSGTexture *removedFromAtlas(QRhiResourceUpdateBatch *resourceUpdates = nullptr) const override;
	virtual QRectF normalizedTextureSubRect() const override;
	virtual QSize textureSize() const override;

	// TODO: Leftover from Qt5
	void bind();
	int textureId() const;

	const SizedTextureHandle texture_handle;
	mutable std::unique_ptr<QSGTexture> standalone;
};

} // namespace openage::renderer::gui
