// Copyright 2015-2019 the openage authors. See copying.md for legal info.

#include "gui_game_spec_image_provider_by_id_impl.h"

#include "../../../error/error.h"

#include "../../../gamestate/game_spec.h"
#include "gui_texture_factory.h"

namespace openage::gui {

GuiGameSpecImageProviderByIdImpl::GuiGameSpecImageProviderByIdImpl(qtsdl::GuiEventQueue *render_updater)
	:
	GuiGameSpecImageProviderImpl{render_updater} {
}

GuiGameSpecImageProviderByIdImpl::~GuiGameSpecImageProviderByIdImpl() = default;

TextureHandle GuiGameSpecImageProviderByIdImpl::get_texture_handle(const QString &id) {
	ENSURE(this->loaded_game_spec, "trying to actually get a texture from a non-loaded spec");

	auto ids = id.splitRef(".");

	if (ids.size() == 2) {
		bool id_ok = false, subid_ok = false;
		const int texture_id = ids[0].toInt(&id_ok);
		const int subid = ids[1].toInt(&subid_ok);

		auto tex = (id_ok and subid_ok)
		           ?
		           this->get_texture(texture_id)
		           :
		           nullptr;

		if (tex != nullptr and
		    subid < static_cast<int>(tex->get_subtexture_count())) {

			return TextureHandle{tex, subid};
		}
		else {
			return this->get_missing_texture();
		}
	} else {
		qWarning("Invalid texture id: 'image://%s/%s'. Example formatting: 'image://%s/7366.18'.", this->get_id(), qUtf8Printable(id), this->get_id());
		return this->get_missing_texture();
	}
}

} // namespace openage::gui
