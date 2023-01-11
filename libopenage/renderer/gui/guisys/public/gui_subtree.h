// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <string>


namespace qtgui {

class GuiRenderer;
class GuiQmlEngine;
class GuiSubtreeImpl;


/**
 * A root item that loads its code from source url.
 *
 * rootdir is the qml file root folder which is watched for changes.
 */
class GuiSubtree {
public:
	explicit GuiSubtree(std::shared_ptr<GuiRenderer> renderer,
	                    std::shared_ptr<GuiQmlEngine> engine,
	                    const std::string &source,
	                    const std::string &rootdir);

	~GuiSubtree();

private:
	friend class GuiSubtreeImpl;
	std::unique_ptr<GuiSubtreeImpl> impl;
};

} // namespace qtgui
