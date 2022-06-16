// Copyright 2015-2022 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <string>


namespace openage {
namespace util {
class Path;
}

namespace renderer {

class Renderer;
class Window;

namespace gui {


/**
 * Main entry point for the openage Qt-based user interface.
 *
 * Legacy variant for the "old" renderer.
 */
class GUI {
public:
	explicit GUI(std::shared_ptr<Window> window,
	             const util::Path &source,
	             const util::Path &rootdir,
	             const util::Path &assetdir,
	             std::shared_ptr<Renderer> renderer);
	virtual ~GUI();

	void process_events();

private:
	// openage::gui::GuiApplicationWithLogger application;
	// qtsdl::GuiEventQueue render_updater;
	// qtsdl::GuiRenderer renderer;
	// qtsdl::GuiEventQueue game_logic_updater;
	// openage::gui::GuiGameSpecImageProvider image_provider_by_filename;
	// openage::gui::GuiGameSpecImageProvider image_provider_by_graphic_id;
	// openage::gui::GuiGameSpecImageProvider image_provider_by_terrain_id;
	// qtsdl::GuiEngine engine;
	// qtsdl::GuiSubtree subtree;
	// qtsdl::GuiInput input;
};

} // namespace gui
} // namespace renderer
} // namespace openage