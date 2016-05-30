// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include "gui_game_spec_image_provider_impl.h"

namespace openage {
namespace gui {

/**
 * Exposes game textures to the Qt by their file name.
 *
 * Id has a form of <filename>.<subid> where <subid> is an integer.
 */
class GuiGameSpecImageProviderByFilenameImpl : public GuiGameSpecImageProviderImpl {
public:
	explicit GuiGameSpecImageProviderByFilenameImpl(qtsdl::GuiEventQueue *render_updater);
	virtual ~GuiGameSpecImageProviderByFilenameImpl();

	static const char* id();

private:
	virtual const char* get_id() const override;
	virtual TextureHandle get_texture_handle(const QString &id) override;
};

}} // namespace openage::gui
