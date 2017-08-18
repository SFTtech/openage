// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "gui_textured_points_shader.h"

#include <QQuickItem>

namespace openage {
namespace gui {

TexturedPointsMaterial::TexturedPointsMaterial(const std::tuple<std::string, std::string> &shaders)
	:
	QSGSimpleMaterial<Icon>{PtrShaderCreateFunc{}},
	shaders{shaders} {
}

QSGMaterialShader* TexturedPointsMaterial::createShader() const {
	return new TexturedPointsShader(this->shaders);
}

TexturedPointsMaterial *TexturedPointsShader::createMaterial(const std::tuple<std::string, std::string> &shaders) {
	return new TexturedPointsMaterial(shaders);
}

TexturedPointsShader::TexturedPointsShader(const std::tuple<std::string, std::string> &shaders)
	:
	QSGSimpleMaterialShader<Icon>{},
	shaders{shaders},
	id_tex{},
	id_columns{},
	id_spacing{} {
}

const char* TexturedPointsShader::vertexShader() const {
	return std::get<0>(this->shaders).c_str();
}

const char* TexturedPointsShader::fragmentShader() const {
	return std::get<1>(this->shaders).c_str();
}

QList<QByteArray> TexturedPointsShader::attributes() const {
	return QList<QByteArray>{} << "vertex" << "cell_id" << "color";
}

void TexturedPointsShader::updateState(const Icon *icon, const Icon*) {
	assert(icon->marker_atlas);
	assert(icon->marker_atlas->textureProvider());
	assert(icon->marker_atlas->textureProvider()->texture());
	icon->marker_atlas->textureProvider()->texture()->bind();
	this->program()->setUniformValue(this->id_columns, icon->columns);
	this->program()->setUniformValue(this->id_spacing, icon->spacing);
}

void TexturedPointsShader::resolveUniforms() {
	this->id_tex = this->program()->uniformLocation("tex");
	this->program()->setUniformValue(id_tex, 0);
	this->id_columns = this->program()->uniformLocation("columns");
	this->id_spacing = this->program()->uniformLocation("spacing");
}

}} // namespace openage::gui
