// Copyright 2015-2019 the openage authors. See copying.md for legal info.

#include "../public/gui_image_provider.h"

#include "../private/gui_image_provider_impl.h"

namespace qtsdl {

GuiImageProvider::GuiImageProvider(std::unique_ptr<GuiImageProviderImpl> impl)
	:
	impl{impl.release(), std::default_delete<GuiImageProviderImpl>()} {
}

GuiImageProvider::~GuiImageProvider() = default;

} // namespace qtsdl
