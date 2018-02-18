// Copyright 2015-2018 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <string>

#include "integration/public/gui_application_with_logger.h"
#include "integration/public/gui_game_spec_image_provider.h"
#include "guisys/public/gui_engine.h"
#include "guisys/public/gui_event_queue.h"
#include "guisys/public/gui_input.h"
#include "guisys/public/gui_renderer.h"
#include "guisys/public/gui_subtree.h"
#include "../handlers.h"


namespace qtsdl {
class GuiSingletonItemsInfo;
} // qtsdl

namespace openage {
namespace shader {
class Program;
} // shader

namespace gui {

class EngineQMLInfo;


/**
 * Main entry point for the openage Qt-based user interface.
 */
class GUI : public InputHandler, public ResizeHandler, public HudHandler {
public:
	explicit GUI(SDL_Window *window,
	             const std::string &source,
	             const std::string &rootdir,
	             EngineQMLInfo *info=nullptr);
	virtual ~GUI();

	void process_events();

private:
	virtual bool on_resize(coord::viewport_delta new_size) override;
	virtual bool on_input(SDL_Event *event) override;
	virtual bool on_drawhud() override;

	GLint tex_loc;
	GLuint screen_quad_vbo;

	GuiApplicationWithLogger application;
	qtsdl::GuiEventQueue render_updater;
	qtsdl::GuiRenderer renderer;
	qtsdl::GuiEventQueue game_logic_updater;
	GuiGameSpecImageProvider image_provider_by_filename;
	GuiGameSpecImageProvider image_provider_by_graphic_id;
	GuiGameSpecImageProvider image_provider_by_terrain_id;
	qtsdl::GuiEngine engine;
	qtsdl::GuiSubtree subtree;
	qtsdl::GuiInput input;

	// needs to be deallocated before the GuiRenderer
	// it accesses opengl api functions which require a
	// current context:
	std::unique_ptr<shader::Program> textured_screen_quad_shader;
};

}} // namespace openage::gui
