// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include "gui_game_spec_image_provider_by_id_impl.h"

namespace openage {
namespace gui {

/**
 * Exposes game textures to the Qt by their id.
 *
 * Numeric id has a form of <texture-id>.<subid>.
 */
class GuiGameSpecImageProviderByGraphicIdImpl : public GuiGameSpecImageProviderByIdImpl {
public:
	explicit GuiGameSpecImageProviderByGraphicIdImpl(qtsdl::GuiEventQueue *render_updater);
	virtual ~GuiGameSpecImageProviderByGraphicIdImpl();

	static const char* id();

private:
	virtual const char* get_id() const override;
	virtual openage::Texture* get_texture(int texture_id) override;
};

}} // namespace openage::gui
