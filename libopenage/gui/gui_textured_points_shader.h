// Copyright 2016-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <cassert>

#include <QSGSimpleMaterialShader>
#include <QSGTextureProvider>

QT_FORWARD_DECLARE_CLASS(QQuickItem)

namespace openage {
namespace gui {

struct Icon
{
	QQuickItem *marker_atlas;
	float columns;
	float spacing;
};

class TexturedPointsMaterial : public QSGSimpleMaterial<Icon>
{
public:
	TexturedPointsMaterial(const std::tuple<std::string, std::string> &shaders);

	virtual QSGMaterialShader *createShader() const override;

private:
	const std::tuple<std::string, std::string> shaders;
};

/**
 * Draws ColoredTexturedPoint2D arrays.
 *
 * Uses icons found in the square-grid atlas by the ColoredTexturedPoint2D::cell_id.
 * Applies color from ColoredTexturedPoint2D regardless of the color in the atlas.
 */
class TexturedPointsShader : public QSGSimpleMaterialShader<Icon>
{
public:
	static TexturedPointsMaterial *createMaterial(const std::tuple<std::string, std::string> &shaders);

	TexturedPointsShader(const std::tuple<std::string, std::string> &shaders);

	virtual const char* vertexShader() const override;
	virtual const char* fragmentShader() const override;

	virtual QList<QByteArray> attributes() const override;

	virtual void updateState(const Icon *icon, const Icon*) override;
	virtual void resolveUniforms() override;

private:
	const std::tuple<std::string, std::string> shaders;
	int id_tex;
	int id_columns;
	int id_spacing;
};

}} // namespace openage::gui
