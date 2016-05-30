// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include <QSGTexture>

#include "gui_texture_handle.h"

namespace openage {
namespace gui {

class GuiTexture : public QSGTexture {
	Q_OBJECT

public:
	explicit GuiTexture(const SizedTextureHandle &texture_handle);
	virtual ~GuiTexture();

private:
	virtual void bind() override;
	virtual bool hasAlphaChannel() const override;
	virtual bool hasMipmaps() const override;
	virtual bool isAtlasTexture() const override;
	virtual QSGTexture* removedFromAtlas() const override;
	virtual QRectF normalizedTextureSubRect() const override;
	virtual int textureId() const override;
	virtual QSize textureSize() const override;

	const SizedTextureHandle texture_handle;
	mutable std::unique_ptr<QSGTexture> standalone;
};

}} // namespace openage::gui
