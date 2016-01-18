// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "gui_make_standalone_subtexture.h"
#include "gui_standalone_subtexture.h"

namespace openage {
namespace gui {

std::unique_ptr<QSGTexture> make_standalone_subtexture(GLuint id, const QSize &size) {
	return std::make_unique<GuiStandaloneSubtexture>(id, size);
}

}} // namespace openage::gui
