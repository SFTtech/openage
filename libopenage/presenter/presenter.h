// Copyright 2019-2022 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <vector>

#include "util/path.h"

namespace qtgui {
class GuiApplication;
}

namespace openage {

namespace renderer {
class RenderPass;
class Renderer;
class Texture2d;
class ShaderProgram;
class Window;

namespace gui {
class GUI;
}

namespace terrain {
class TerrainRenderer;
}

} // namespace renderer

namespace presenter {

class Presenter {
public:
	Presenter(const util::Path &path);

	/**
	 * Start the presenter and initialize subsystems.
	 *
	 */
	void run();

	/**
	 * Initialize the Qt application managing the graphical views. Required
	 * for creating windows.
	 *
	 * @returns Pointer to openage's application wrapper,
	 */
	static std::shared_ptr<qtgui::GuiApplication> init_window_system();

protected:
	/**
	 * Initialize all graphics subsystems of the presenter, i.e.
	 *     - window creation
	 *     - main renderer
	 *     - component renderers (Terrain, Game Entities, GUI)
	 */
	void init_graphics();

	/**
	 * Initialize the terrain renderer.
	 */
	void init_terrain_renderer();

	/**
	 * Initialize the GUI.
	 */
	void init_gui();

	/**
	 * Initialize the final render pass that renders the results of all previous
	 * render passes to the window screen.
	 */
	void init_render_pass();
	// void init_audio();

	/**
	 * Render all configured render passes in sequence.
	 */
	void render();

	/**
	 * Update the render pass uniforms. This must be done if
	 * the output textures of the render passes change, e.g. after
	 * a resize action.
	 */
	void update_render_pass_unifs();

	// TODO: remove and move into our config/settings system
	util::Path root_dir;

	// graphis components
	/**
	 * Windowing GUI Application wrapper.
	 */
	std::shared_ptr<qtgui::GuiApplication> gui_app;

	/**
	 * Display window.
	 */
	std::shared_ptr<renderer::Window> window;

	/**
	 * openage's graphics renderer.
	 */
	std::shared_ptr<renderer::Renderer> renderer;

	/**
	 * Qt-based GUI for interface.
	 */
	std::shared_ptr<renderer::gui::GUI> gui;

	/**
	 * Graphics output for terrain.
	 */
	std::shared_ptr<renderer::terrain::TerrainRenderer> terrain_renderer;

	/**
	 * Render passes in the openage renderer.
	 */
	std::vector<std::shared_ptr<renderer::RenderPass>> render_passes;

	/**
	 * Results of the individual render stages.
	 */
	std::vector<std::shared_ptr<renderer::Texture2d>> pass_outputs;

	/**
	 * Shader for rendering to the window.
	 */
	std::shared_ptr<renderer::ShaderProgram> display_shader;
};

} // namespace presenter
} // namespace openage
