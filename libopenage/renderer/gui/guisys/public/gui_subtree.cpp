// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#include "gui_subtree.h"

#include "renderer/gui/guisys/private/gui_subtree_impl.h"

namespace qtgui {

GuiSubtree::GuiSubtree(std::shared_ptr<GuiRenderer> renderer,
                       std::shared_ptr<GuiQmlEngine> engine,
                       const std::string &source,
                       const std::string &rootdir) :
	impl{std::make_unique<GuiSubtreeImpl>(
		renderer,
		engine,
		QString::fromStdString(source),
		QString::fromStdString(rootdir))} {}

GuiSubtree::~GuiSubtree() = default;

} // namespace qtgui
