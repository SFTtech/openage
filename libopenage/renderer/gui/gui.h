// Copyright 2015-2022 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <string>

#include "gui/guisys/public/gui_engine.h"
#include "gui/guisys/public/gui_event_queue.h"
#include "gui/guisys/public/gui_input.h"
#include "gui/guisys/public/gui_renderer.h"
#include "gui/guisys/public/gui_subtree.h"
#include "gui/integration/public/gui_application_with_logger.h"
#include "gui/integration/public/gui_game_spec_image_provider.h"
#include "renderer/shader_program.h"

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
 */
class GUI {
public:
	explicit GUI(std::shared_ptr<Window> window,
	             const util::Path &source,
	             const util::Path &rootdir,
	             const util::Path &assetdir,
	             std::shared_ptr<Renderer> renderer);
	virtual ~GUI() = default;

	void process_events();

private:
	openage::gui::GuiApplicationWithLogger application;
	qtsdl::GuiEventQueue render_updater;
	qtsdl::GuiRenderer gui_renderer;
	qtsdl::GuiEventQueue game_logic_updater;
	openage::gui::GuiGameSpecImageProvider image_provider_by_filename;
	openage::gui::GuiGameSpecImageProvider image_provider_by_graphic_id;
	openage::gui::GuiGameSpecImageProvider image_provider_by_terrain_id;
	qtsdl::GuiEngine engine;
	qtsdl::GuiSubtree subtree;
	qtsdl::GuiInput input;

	// useless?
	std::shared_ptr<ShaderProgram> textured_screen_quad_shader;
};

} // namespace gui
} // namespace renderer
} // namespace openage