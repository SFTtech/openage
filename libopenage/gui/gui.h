// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <string>

#include "guisys/public/gui_engine.h"
#include "guisys/public/gui_event_queue.h"
#include "guisys/public/gui_input.h"
#include "guisys/public/gui_renderer.h"
#include "guisys/public/gui_subtree.h"
#include "integration/public/gui_application_with_logger.h"


namespace qtsdl {
class GuiSingletonItemsInfo;
} // namespace qtsdl

namespace openage {
namespace gui {

class EngineQMLInfo;


/**
 * Main entry point for the openage Qt-based user interface.
 *
 * Legacy variant for the "old" renderer.
 */
class GUI {
public:
	explicit GUI(SDL_Window *window,
	             const std::string &source,
	             const std::string &rootdir,
	             EngineQMLInfo *info = nullptr);
	virtual ~GUI();

	void process_events();

private:
	// virtual bool on_resize(coord::viewport_delta new_size) override;
	// virtual bool on_input(SDL_Event *event) override;
	// virtual bool on_drawhud() override;

	GuiApplicationWithLogger application;
	qtsdl::GuiEventQueue render_updater;
	qtsdl::GuiRenderer renderer;
	qtsdl::GuiEventQueue game_logic_updater;
	qtsdl::GuiEngine engine;
	qtsdl::GuiSubtree subtree;
	qtsdl::GuiInput input;
};

} // namespace gui
} // namespace openage
