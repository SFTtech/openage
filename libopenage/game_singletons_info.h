// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <string>

#include "gui/guisys/public/gui_singleton_items_info.h"

namespace openage {

class Engine;

namespace gui {

/**
 * Accessible during creation of any singleton QML item.
 */
struct GameSingletonsInfo : qtsdl::GuiSingletonItemsInfo {
	GameSingletonsInfo(Engine *engine, const std::string &data_dir);

	Engine *engine;
	std::string data_dir;
};

}} // namespace openage::gui
