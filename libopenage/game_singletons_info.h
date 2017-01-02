// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <string>

#include "gui/guisys/public/gui_singleton_items_info.h"


namespace openage {

class Engine;

namespace gui {

/**
 * This container is attached to the QML engine.
 *
 * It allows that one can access the members in the qml engine context then.
 * That also means the members accessible during creation of any singleton QML item.
 *
 * This struct is used to link the openage Engine with QML in engine_link.cpp.
 */
struct GameSingletonsInfo : qtsdl::GuiSingletonItemsInfo {
	GameSingletonsInfo(Engine *engine, const std::string &data_dir);

	/**
	 * The openage engine, so it can be "used" in QML as a "QML Singleton".
	 */
	Engine *engine;

	/**
	 * Search path for finding assets n stuff.
	 */
	std::string data_dir;
};

}} // openage::gui
