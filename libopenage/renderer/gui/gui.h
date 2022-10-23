// Copyright 2015-2022 the openage authors. See copying.md for legal info.

#pragma once

#include "renderer/texture.h"
#include <memory>
#include <string>

#include "gui/guisys/public/gui_engine.h"
#include "gui/guisys/public/gui_event_queue.h"
#include "gui/guisys/public/gui_input.h"
#include "gui/guisys/public/gui_subtree.h"
#include "renderer/gui/integration/public/gui_application_with_logger.h"
#include "gui/integration/public/gui_game_spec_image_provider.h"
#include "renderer/gui/guisys/public/gui_renderer.h"
#include "renderer/shader_program.h"

namespace openage {
namespace util {
class Path;
}

namespace renderer {
class RenderPass;
class Renderer;
class Window;

namespace gui {

class QMLInfo;

/**
 * Main entry point for the openage Qt-based user interface.
 */
class GUI {
public:
	explicit GUI(std::shared_ptr<qtgui::GuiApplication> app,
	             std::shared_ptr<Window> window,
	             const util::Path &source,
	             const util::Path &rootdir,
	             const util::Path &assetdir,
	             const std::shared_ptr<Renderer> &renderer,
	             QMLInfo *info = nullptr);
	virtual ~GUI() = default;

	std::shared_ptr<renderer::RenderPass> get_render_pass();

	void process_events();
	bool drawhud();

private:
	void initialize_render_pass(size_t width,
	                            size_t height,
	                            const util::Path &shaderdir);

	/**
	 * Update GUI texture size and propagate the change to GUI render pass.
	 */
	void resize(size_t width, size_t height);

	std::shared_ptr<qtgui::GuiApplication> application;
	qtsdl::GuiEventQueue render_updater;
	qtgui::GuiRenderer gui_renderer;
	// qtsdl::GuiEventQueue game_logic_updater;
	// openage::gui::GuiGameSpecImageProvider image_provider_by_filename;
	// qtsdl::GuiEngine engine;
	// qtsdl::GuiSubtree subtree;
	// qtsdl::GuiInput input;

	/**
	 * The renderer displaying this GUI.
	 * We use it to fetch new textures n shit.
	 */
	std::shared_ptr<Renderer> renderer;

	/**
	 * Where the GUI is rendered into.
	 */
	std::shared_ptr<renderer::Texture2d> texture;

	/**
	 * How the GUI is rendered.
	 */
	std::shared_ptr<renderer::RenderPass> render_pass;

	// useless?
	std::shared_ptr<ShaderProgram> textured_screen_quad_shader;
};

} // namespace gui
} // namespace renderer
} // namespace openage
