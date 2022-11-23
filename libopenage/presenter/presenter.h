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

namespace screen {
class ScreenRenderer;
}

namespace skybox {
class SkyboxRenderer;
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
	void init_final_render_pass();

	// void init_audio();

	/**
	 * Render all configured render passes in sequence.
	 */
	void render();

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
	 * Graphics output for the map background.
	 */
	std::shared_ptr<renderer::skybox::SkyboxRenderer> skybox_renderer;

	/**
	 * Graphics output for terrain.
	 */
	std::shared_ptr<renderer::terrain::TerrainRenderer> terrain_renderer;

	/**
	 * Final graphics output to the window screen.
	 */
	std::shared_ptr<renderer::screen::ScreenRenderer> screen_renderer;

	/**
	 * Render passes in the openage renderer.
	 */
	std::vector<std::shared_ptr<renderer::RenderPass>> render_passes;
};

} // namespace presenter
} // namespace openage
