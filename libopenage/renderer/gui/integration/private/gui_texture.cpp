// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#include <array>

#include <QTransform>

#include "renderer/gui/integration/private/gui_make_standalone_subtexture.h"
#include "renderer/gui/integration/private/gui_texture.h"


namespace openage::renderer::gui {

GuiTexture::GuiTexture(const SizedTextureHandle &texture_handle) :
	QSGTexture{},
	texture_handle(texture_handle) {
}

GuiTexture::~GuiTexture() = default;

void GuiTexture::bind() {
	glBindTexture(GL_TEXTURE_2D, this->textureId());
}

qint64 GuiTexture::comparisonKey() const {
	// TODO: Qt5 What does this do??????
	return 0;
}

bool GuiTexture::hasAlphaChannel() const {
	// assume 32bit textures
	return true;
}

bool GuiTexture::hasMipmaps() const {
	return false;
}

bool GuiTexture::isAtlasTexture() const {
	return openage::renderer::gui::isAtlasTexture(this->texture_handle);
}

namespace {
GLuint create_compatible_texture(GLuint texture_id, GLsizei w, GLsizei h) {
	glBindTexture(GL_TEXTURE_2D, texture_id);

	GLint min_filter;
	GLint mag_filter;
	GLint iformat;

	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &min_filter);
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &mag_filter);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &iformat);

	GLuint new_texture_id;
	glGenTextures(1, &new_texture_id);
	glBindTexture(GL_TEXTURE_2D, new_texture_id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);

	glTexImage2D(GL_TEXTURE_2D, 0, iformat, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	glBindTexture(GL_TEXTURE_2D, 0);

	return new_texture_id;
}
} // namespace

QSGTexture *GuiTexture::removedFromAtlas(QRhiResourceUpdateBatch *resourceUpdates /* = nullptr */) const {
	if (this->isAtlasTexture()) {
		return this->standalone.get();
	}

	return nullptr;
}

QRectF GuiTexture::normalizedTextureSubRect() const {
	return QSGTexture::normalizedTextureSubRect();
}

int GuiTexture::textureId() const {
	return 0;
}

QSize GuiTexture::textureSize() const {
	return openage::renderer::gui::textureSize(this->texture_handle);
}

} // namespace openage::renderer::gui
