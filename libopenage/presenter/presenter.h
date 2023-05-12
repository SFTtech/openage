// Copyright 2019-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <vector>

#include "util/path.h"

namespace qtgui {
class GuiApplication;
}

namespace openage {

namespace event {
class TimeLoop;
}

namespace gamestate {
class GameSimulation;
}

namespace input {
class InputManager;
}

namespace renderer {
class RenderPass;
class Renderer;
class Texture2d;
class ShaderProgram;
class Window;

namespace camera {
class Camera;
class CameraManager;
} // namespace camera

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

namespace world {
class WorldRenderer;
}

namespace resources {
class AssetManager;
}

} // namespace renderer

namespace presenter {

class Presenter {
public:
	/**
	 * Create a new presenter.
	 *
	 * @param path Root directory path.
	 * @param simulation Game simulation. Can be set later with \p set_engine()
	 * @param time_loop Time loop which controls simulation time. Can be set later with \p set_time_loop()
	 */
	Presenter(const util::Path &path,
	          const std::shared_ptr<gamestate::GameSimulation> &simulation = nullptr,
	          const std::shared_ptr<event::TimeLoop> &time_loop = nullptr);

	~Presenter() = default;

	/**
	 * Start the presenter and initialize subsystems.
	 */
	void run();

	/**
	 * Set the game simulation controlled by this presenter.
	 *
	 * @param simulation Game simulation.
	 */
	void set_simulation(const std::shared_ptr<gamestate::GameSimulation> &simulation);

	/**
	 * Set the time loop controlled by this presenter.
	 *
	 * @param time_loop Time loop.
	 */
	void set_time_loop(const std::shared_ptr<event::TimeLoop> &time_loop);

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
	 * Initialize the GUI.
	 */
	void init_gui();

	/**
	 * Initialize the input management.
	 */
	void init_input();

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
	 * Camera for viewing things.
	 */
	std::shared_ptr<renderer::camera::Camera> camera;

	/**
	 * Qt-based GUI for interface.
	 */
	std::shared_ptr<renderer::gui::GUI> gui;

	/**
	 * Camera manager for camera controls.
	 */
	std::shared_ptr<renderer::camera::CameraManager> camera_manager;

	/**
	 * Graphics output for the map background.
	 */
	std::shared_ptr<renderer::skybox::SkyboxRenderer> skybox_renderer;

	/**
	 * Graphics output for terrain.
	 */
	std::shared_ptr<renderer::terrain::TerrainRenderer> terrain_renderer;

	/**
	 * Graphics output for units/buildings.
	 */
	std::shared_ptr<renderer::world::WorldRenderer> world_renderer;

	/**
	 * Final graphics output to the window screen.
	 */
	std::shared_ptr<renderer::screen::ScreenRenderer> screen_renderer;

	/**
	 * Manager for loading/storing asset resources.
	 */
	std::shared_ptr<renderer::resources::AssetManager> asset_manager;

	/**
	 * Render passes in the openage renderer.
	 */
	std::vector<std::shared_ptr<renderer::RenderPass>> render_passes;

	/**
	 * Game simulation.
	 */
	std::shared_ptr<gamestate::GameSimulation> simulation;

	/**
	 * Time loop.
	 */
	std::shared_ptr<event::TimeLoop> time_loop;

	/**
	 * Input manager.
	 */
	std::shared_ptr<input::InputManager> input_manager;
};

} // namespace presenter
} // namespace openage
