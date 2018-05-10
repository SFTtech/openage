// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <QtGui>
#include <QSGTexture>

namespace openage {
namespace gui {

class GuiStandaloneSubtexture : public QSGTexture {
	Q_OBJECT

public:
	explicit GuiStandaloneSubtexture(GLuint id, const QSize &size);
	virtual ~GuiStandaloneSubtexture();

private:
	virtual void bind() override;
	virtual bool hasAlphaChannel() const override;
	virtual bool hasMipmaps() const override;
	virtual bool isAtlasTexture() const override;
	virtual int textureId() const override;
	virtual QSize textureSize() const override;

	const GLuint id;
	const QSize size;
};

}} // namespace openage::gui
