// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#include "gui_make_standalone_subtexture.h"

#include "renderer/gui/integration/private/gui_standalone_subtexture.h"


namespace openage::renderer::gui {

std::unique_ptr<QSGTexture> make_standalone_subtexture(GLuint id, const QSize &size) {
	return std::make_unique<GuiStandaloneSubtexture>(id, size);
}

} // namespace openage::renderer::gui
