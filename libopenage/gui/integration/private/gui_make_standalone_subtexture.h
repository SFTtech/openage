// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include <QSGTexture>

#include <epoxy/gl.h>

namespace openage {
namespace gui {

/*
 * Reason for this is to resolve epoxy header clash that occur
 * when a header with QObject-derived (so, it's moc-ed) class
 * needs GL headers. Automoc can arrange headers in any order,
 * so the GL can be included through Qt first, because guisys
 * files aren't allowed to include epoxy at all, for example.
 */
std::unique_ptr<QSGTexture> make_standalone_subtexture(GLuint id, const QSize &size);

}} // namespace openage::gui
