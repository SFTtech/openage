// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <string>


namespace qtsdl {

class GuiRenderer;
class GuiEventQueue;
class GuiEngine;
class GuiSubtreeImpl;


/**
 * A root item that loads its code from source url.
 *
 * rootdir is the qml file root folder which is watched for changes.
 */
class GuiSubtree {
public:
	explicit GuiSubtree(GuiRenderer *renderer,
	                    GuiEventQueue *game_logic_updater,
	                    GuiEngine *engine,
	                    const std::string &source,
	                    const std::string &rootdir);

	~GuiSubtree();

private:
	friend class GuiSubtreeImpl;
	std::unique_ptr<GuiSubtreeImpl> impl;
};

} // namespace qtsdl
