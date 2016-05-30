// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "../public/gui_subtree.h"

#include <QString>

#include "../private/gui_subtree_impl.h"

namespace qtsdl {

GuiSubtree::GuiSubtree(GuiRenderer *renderer, GuiEventQueue *game_logic_updater, GuiEngine *engine, const std::string &source, const std::vector<std::string> &search_paths)
	:
	impl{std::make_unique<GuiSubtreeImpl>(renderer, game_logic_updater, engine, QString::fromStdString(source), search_paths)} {
}

GuiSubtree::~GuiSubtree() {
}

} // namespace qtsdl
