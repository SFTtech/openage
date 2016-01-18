// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "gui_game_spec_image_provider_by_filename_impl.h"

#include "../../../error/error.h"

#include "../../../gamestate/game_spec.h"

namespace openage {
namespace gui {

GuiGameSpecImageProviderByFilenameImpl::GuiGameSpecImageProviderByFilenameImpl(qtsdl::GuiEventQueue *render_updater)
	:
	GuiGameSpecImageProviderImpl{render_updater} {
}

GuiGameSpecImageProviderByFilenameImpl::~GuiGameSpecImageProviderByFilenameImpl() {
}

const char* GuiGameSpecImageProviderByFilenameImpl::id() {
	return "by-filename";
}

const char* GuiGameSpecImageProviderByFilenameImpl::get_id() const {
	return GuiGameSpecImageProviderByFilenameImpl::id();
}

TextureHandle GuiGameSpecImageProviderByFilenameImpl::get_texture_handle(const QString &id) {
	ENSURE(this->loaded_game_spec, "trying to actually get a texture from a non-loaded spec");

	auto filename = id.section(".", 0, -2);
	auto subid_str = id.section(".", -1, -1);

	bool ok = false;
	const int subid = subid_str.toInt(&ok);

	if (!filename.isEmpty() && ok) {
		auto tex = this->loaded_game_spec->get_texture(filename.toStdString());
		return tex ? TextureHandle{tex, subid} : this->get_missing_texture();
	} else {
		qWarning("Invalid texture id: 'image://%s/%s'. Example formatting: 'image://%s/myfile.png.18'.", this->get_id(), qUtf8Printable(id), this->get_id());
		return this->get_missing_texture();
	}
}

}} // namespace openage::gui
