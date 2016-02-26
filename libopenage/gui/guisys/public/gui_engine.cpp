// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "../public/gui_engine.h"

#include "../private/gui_engine_impl.h"

namespace qtsdl {

GuiEngine::GuiEngine(GuiRenderer *renderer, const std::vector<GuiImageProvider*> &image_providers, GuiSingletonItemsInfo *singleton_items_info)
	:
	impl{std::make_unique<GuiEngineImpl>(renderer, image_providers, singleton_items_info)} {
}

GuiEngine::~GuiEngine() {
}

} // namespace qtsdl
