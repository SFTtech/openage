// Copyright 2017-2023 the openage authors. See copying.md for legal info.

#pragma once

#include "../presenter/legacy/legacy.h"
#include "../util/path.h"
#include "guisys/public/gui_singleton_items_info.h"

namespace openage {
class LegacyEngine;

namespace presenter {
class LegacyDisplay;
}

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
	EngineQMLInfo(LegacyEngine *engine, const util::Path &asset_dir);

	/**
	 * The openage engine, so it can be "used" in QML as a "QML Singleton".
	 * With this pointer, all of QML can find back to the engine.
	 */
	LegacyEngine *engine;

	/**
	 * The openage display.
	 */
	presenter::LegacyDisplay *display;

	/**
	 * Search path for finding assets n stuff.
	 */
	util::Path asset_dir;
};


} // namespace gui
} // namespace openage
