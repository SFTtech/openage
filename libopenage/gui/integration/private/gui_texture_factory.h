// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include <QQuickTextureFactory>

#include "gui_texture_handle.h"
#include "gui_filled_texture_handles.h"

namespace openage {
namespace gui {

class GuiGameSpecImageProviderImpl;

class GuiTextureFactory : public QQuickTextureFactory {
	Q_OBJECT

public:
	explicit GuiTextureFactory(GuiGameSpecImageProviderImpl *provider, const QString &id, const QSize &requested_size);
	virtual ~GuiTextureFactory();

	virtual QSGTexture* createTexture(QQuickWindow *window) const override;
	virtual int textureByteCount() const override;
	virtual QSize textureSize() const override;

private:
	SizedTextureHandle texture_handle;
	GuiFilledTextureHandleUser texture_handle_user;
};

}} // namespace openage::gui
