// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "../util/path.h"
#include "guisys/public/gui_singleton_items_info.h"


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
class EngineQMLInfo : public qtsdl::GuiSingletonItemsInfo {
public:
	EngineQMLInfo(Engine *engine, const util::Path &asset_dir);

	/**
	 * The openage engine, so it can be "used" in QML as a "QML Singleton".
	 * With this pointer, all of QML can find back to the engine.
	 */
	Engine *engine;

	/**
	 * Search path for finding assets n stuff.
	 */
	util::Path asset_dir;
};


}} // namespace openage::gui
