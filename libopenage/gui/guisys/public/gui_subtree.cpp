// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include "gui_subtree.h"

#include "../private/gui_subtree_impl.h"

namespace qtsdl {

GuiSubtree::GuiSubtree(GuiRenderer *renderer,
                       GuiEventQueue *game_logic_updater,
                       GuiEngine *engine,
                       const std::string &source,
                       const std::string &rootdir)
	:
	impl{std::make_unique<GuiSubtreeImpl>(
		renderer,
		game_logic_updater,
		engine,
		QString::fromStdString(source),
		QString::fromStdString(rootdir)
	)} {}

GuiSubtree::~GuiSubtree() {}

} // namespace qtsdl
