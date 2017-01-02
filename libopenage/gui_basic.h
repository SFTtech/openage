// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <string>
#include <memory>

#include "gui/integration/public/gui_application_with_logger.h"
#include "gui/guisys/public/gui_engine.h"
#include "gui/guisys/public/gui_event_queue.h"
#include "gui/guisys/public/gui_input.h"
#include "gui/guisys/public/gui_renderer.h"
#include "gui/guisys/public/gui_subtree.h"
#include "handlers.h"

#include <vector>

#include "gui/integration/public/gui_game_spec_image_provider.h"

namespace openage {
namespace shader {
class Program;
}} // namespace openage::shader

namespace qtsdl {
class GuiSingletonItemsInfo;
} // namespace qtsdl

namespace openage {
namespace gui {

/**
 * Smallest gui configuration.
 */
class GuiBasic : public InputHandler, public ResizeHandler, public HudHandler {
public:
	explicit GuiBasic(SDL_Window *window, const std::string &source, qtsdl::GuiSingletonItemsInfo *singleton_items_info=nullptr, const std::vector<std::string> &search_paths=std::vector<std::string>{});
	virtual ~GuiBasic();

	void process_events();

private:
	virtual bool on_resize(coord::window new_size) override;
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
