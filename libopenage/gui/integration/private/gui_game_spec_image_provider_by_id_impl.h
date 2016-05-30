// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include "gui_game_spec_image_provider_impl.h"

namespace openage {
namespace gui {

/**
 * Base for providers that expose textures to the Qt by their id.
 *
 * Numeric id has a form of <texture-id>.<subid>.
 */
class GuiGameSpecImageProviderByIdImpl : public GuiGameSpecImageProviderImpl {
public:
	explicit GuiGameSpecImageProviderByIdImpl(qtsdl::GuiEventQueue *render_updater);
	virtual ~GuiGameSpecImageProviderByIdImpl();

private:
	virtual TextureHandle get_texture_handle(const QString &id) override;
	virtual openage::Texture* get_texture(int texture_id) = 0;
};

}} // namespace openage::gui
