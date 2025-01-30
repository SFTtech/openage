// Copyright 2019-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <vector>

#include "renderer/window.h"
#include "util/path.h"


namespace qtgui {
class GuiApplication;
}

namespace openage {

namespace gamestate {
class GameSimulation;
}

namespace input {
class InputManager;
}

namespace time {
class TimeLoop;
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

namespace hud {
class HudRenderStage;
}

namespace screen {
class ScreenRenderStage;
}

namespace skybox {
class SkyboxRenderStage;
}

namespace terrain {
class TerrainRenderStage;
}

namespace world {
class WorldRenderStage;
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
	          const std::shared_ptr<time::TimeLoop> &time_loop = nullptr);

	~Presenter() = default;

	/**
	 * Start the presenter and initialize subsystems.
	 *
	 * @param window_settings The settings to customize the display window (e.g. size, display mode, vsync).
	 */
	void run(const renderer::window_settings window_settings = {});

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
	void set_time_loop(const std::shared_ptr<time::TimeLoop> &time_loop);

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
	void init_graphics(const renderer::window_settings &window_settings = {});

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
	std::shared_ptr<renderer::skybox::SkyboxRenderStage> skybox_renderer;

	/**
	 * Graphics output for terrain.
	 */
	std::shared_ptr<renderer::terrain::TerrainRenderStage> terrain_renderer;

	/**
	 * Graphics output for units/buildings.
	 */
	std::shared_ptr<renderer::world::WorldRenderStage> world_renderer;

	/**
	 * Graphics output for the HUD.
	 */
	std::shared_ptr<renderer::hud::HudRenderStage> hud_renderer;

	/**
	 * Final graphics output to the window screen.
	 */
	std::shared_ptr<renderer::screen::ScreenRenderStage> screen_renderer;

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
	std::shared_ptr<time::TimeLoop> time_loop;

	/**
	 * Input manager.
	 */
	std::shared_ptr<input::InputManager> input_manager;
};

} // namespace presenter
} // namespace openage
