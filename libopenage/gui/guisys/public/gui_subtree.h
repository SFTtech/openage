// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <vector>
#include <string>
#include <memory>

namespace qtsdl {

class GuiRenderer;
class GuiEventQueue;
class GuiEngine;
class GuiSubtreeImpl;

/**
 * A root item that loads its code from source url.
 */
class GuiSubtree {
public:
	explicit GuiSubtree(GuiRenderer *renderer, GuiEventQueue *game_logic_updater, GuiEngine *engine, const std::string &source, const std::vector<std::string> &search_paths=std::vector<std::string>{});
	~GuiSubtree();

private:
	friend class GuiSubtreeImpl;
	std::unique_ptr<GuiSubtreeImpl> impl;
};

} // namespace qtsdl
