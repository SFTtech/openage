// Copyright 2017-2023 the openage authors. See copying.md for legal info.

#pragma once

#include "gui/guisys/public/gui_singleton_items_info.h"
#include "util/path.h"

namespace openage {

namespace engine {
class GameSimulation;
}

namespace presenter {
class Presenter;
}

namespace renderer {
namespace gui {

class QMLInfo : public qtsdl::GuiSingletonItemsInfo {
public:
	QMLInfo(engine::GameSimulation *engine, const util::Path &asset_dir);

	/**
	 * The openage engine, so it can be "used" in QML as a "QML Singleton".
	 * With this pointer, all of QML can find back to the engine.
	 */
	engine::GameSimulation *engine;

	/**
	 * The openage display.
	 */
	presenter::Presenter *display;

	/**
	 * Search path for finding assets n stuff.
	 */
	util::Path asset_dir;
};


} // namespace gui
} // namespace renderer
} // namespace openage
