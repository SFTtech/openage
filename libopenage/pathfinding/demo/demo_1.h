// Copyright 2024-2025 the openage authors. See copying.md for legal info.

#pragma once

#include "pathfinding/definitions.h"
#include "pathfinding/path.h"
#include "renderer/resources/mesh_data.h"
#include "util/path.h"


namespace openage {
namespace renderer {
class Renderer;
class RenderPass;
class ShaderProgram;
class Window;

namespace gui {
class GuiApplicationWithLogger;
} // namespace gui

} // namespace renderer

namespace path {
class Grid;

namespace tests {


/**
 * Show the functionality of the high-level pathfinder:
 *     - Grids
 *     - Sectors
 *     - Portals
 *
 * Visualizes the pathfinding results using our rendering backend.
 *
 * @param path Path to the project rootdir.
 */
void path_demo_1(const util::Path &path);


/**
 * Manages the graphical display of the pathfinding demo.
 */
class RenderManager1 {
public:
	/**
	 * Create a new render manager.
	 *
	 * @param app GUI application.
	 * @param window Window to render to.
	 * @param path Path to the project rootdir.
	 */
	RenderManager1(const std::shared_ptr<renderer::gui::GuiApplicationWithLogger> &app,
	               const std::shared_ptr<renderer::Window> &window,
	               const util::Path &path,
	               const std::shared_ptr<path::Grid> &grid);
	~RenderManager1() = default;

	/**
	 * Run the render loop.
	 */
	void run();

	/**
	 * Create renderables for the waypoint tiles of a path.
	 *
	 * @param path Path object.
	 */
	void create_waypoint_tiles(const Path &path);

private:
	/**
	 * Load the shader sources for the demo and create the shader programs.
	 */
	void init_shaders();

	/**
	 * Create the following render passes for the demo:
	 *   - Background pass: Mono-colored background object.
	 *   - Field pass; Renders the cost, integration and flow fields.
	 *   - Grid pass: Renders a grid on top of the cost field.
	 *   - Display pass: Draws the results of previous passes to the screen.
	 */
	void init_passes();

	/**
	 * Create a mesh for the grid.
	 *
	 * @param grid Pathing grid.
	 *
	 * @return Mesh data for the grid.
	 */
	static renderer::resources::MeshData get_grid_mesh(const std::shared_ptr<path::Grid> &grid);

	/**
	 * Create renderables for the impassible tiles in the grid.
	 *
	 * @param grid Pathing grid.
	 */
	void create_impassible_tiles(const std::shared_ptr<path::Grid> &grid);

	/**
	 * Create renderables for the portal tiles in the grid.
	 *
	 * @param grid Pathing grid.
	 */
	void create_portal_tiles(const std::shared_ptr<path::Grid> &grid);

	/**
	 * Path to the project rootdir.
	 */
	const util::Path &path;

	/**
	 * Pathing grid of the demo.
	 */
	std::shared_ptr<path::Grid> grid;

	/* Renderer objects */

	/**
	 * Qt GUI application.
	 */
	std::shared_ptr<renderer::gui::GuiApplicationWithLogger> app;

	/**
	 * openage window to render to.
	 */
	std::shared_ptr<renderer::Window> window;

	/**
	 * openage renderer instance.
	 */
	std::shared_ptr<renderer::Renderer> renderer;

	/* Shader programs */

	/**
	 * Shader program for rendering a grid.
	 */
	std::shared_ptr<renderer::ShaderProgram> grid_shader;

	/**
	 * Shader program for rendering 2D mono-colored objects.
	 */
	std::shared_ptr<renderer::ShaderProgram> obj_shader;

	/**
	 * Shader program for rendering the final display.
	 */
	std::shared_ptr<renderer::ShaderProgram> display_shader;

	/* Render passes */

	/**
	 * Background pass: Mono-colored background object.
	 */
	std::shared_ptr<renderer::RenderPass> background_pass;

	/**
	 * Field pass: Renders the cost field.
	 */
	std::shared_ptr<renderer::RenderPass> field_pass;

	/**
	 * Grid pass: Renders a grid on top of the cost field.
	 */
	std::shared_ptr<renderer::RenderPass> grid_pass;

	/**
	 * Waypoint pass: Renders the path and its waypoints.
	 */
	std::shared_ptr<renderer::RenderPass> waypoint_pass;

	/**
	 * Display pass: Draws the results of previous passes to the screen.
	 */
	std::shared_ptr<renderer::RenderPass> display_pass;
};

// Cost for the sectors in the grid
// taken from Figure 23.1 in "Crowd Pathfinding and Steering Using Flow Field Tiles"
const std::vector<std::vector<cost_t>> SECTORS_COST = {
	{
		// clang-format off
          1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
          1, 255, 255, 255, 255,   1,   1,   1,   1,   1,
          1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
          1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
          1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
          1,   1,   1,   1, 255, 255, 255, 255,   1,   1,
          1,   1,   1, 255, 255, 255, 255, 255,   1,   1,
          1,   1, 255, 255, 255, 255, 255,   1,   1,   1,
          1,   1, 255,   1,   1,   1,   1,   1,   1,   1,
          1,   1, 255,   1,   1,   1,   1,   1,   1,   1,
		// clang-format on
	},
	{
		// clang-format off
          1,   1, 255, 255, 255, 255, 255, 255, 255, 255,
          1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
          1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
          1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
          1,   1,   1, 255,   1,   1, 255,   1,   1,   1,
          1,   1,   1, 255,   1, 255, 255,   1,   1,   1,
          1,   1,   1, 255, 255, 255, 255, 255, 255, 255,
          1,   1,   1,   1, 255, 255, 255,   1,   1,   1,
          1,   1,   1,   1, 255, 255,   1,   1,   1,   1,
          1,   1,   1, 255,   1,   1,   1,   1,   1,   1,
		// clang-format on
	},
	{
		// clang-format off
        255, 255,   1,   1,   1,   1,   1, 255, 255, 255,
          1,   1,   1,   1,   1,   1,   1,   1, 255, 255,
          1,   1,   1,   1,   1,   1,   1,   1, 255, 255,
          1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
          1,   1,   1,   1, 255, 255, 255,   1,   1,   1,
          1,   1,   1,   1,   1, 255, 255, 255, 255, 255,
          1,   1,   1,   1, 255, 255, 255, 255, 255,   1,
          1,   1,   1,   1, 255,   1,   1,   1,   1,   1,
          1,   1,   1,   1, 255,   1,   1,   1,   1, 255,
          1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
		// clang-format on
	},
	{
		// clang-format off
        255, 255,   1,   1,   1,   1,   1,   1,   1,   1,
        255, 255,   1,   1,   1,   1,   1,   1,   1,   1,
        255, 255,   1,   1,   1,   1,   1,   1,   1,   1,
          1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
          1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
          1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
          1,   1,   1, 255,   1,   1,   1,   1,   1,   1,
          1,   1, 255, 255,   1,   1,   1,   1,   1,   1,
        255, 255, 255, 255,   1,   1,   1,   1,   1,   1,
          1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
		// clang-format on
	},
	{
		// clang-format off
          1,   1, 255,   1,   1,   1,   1,   1,   1,   1,
          1,   1, 255,   1,   1,   1,   1,   1, 255,   1,
          1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
          1,   1,   1,   1, 255,   1,   1,   1,   1,   1,
          1,   1,   1, 255, 255,   1,   1,   1,   1,   1,
          1,   1, 255,   1, 255,   1,   1,   1,   1,   1,
          1,   1,   1,   1, 255,   1,   1,   1,   1,   1,
          1,   1,   1,   1, 255, 255, 255, 255, 255, 255,
          1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
          1,   1,   1,   1,   1,   1,   1, 255, 255, 255,
		// clang-format on
	},
	{
		// clang-format off
          1,   1,   1, 255,   1,   1,   1,   1,   1, 255,
          1,   1,   1, 255,   1, 255, 255, 255, 255, 255,
          1,   1,   1, 255,   1,   1,   1,   1, 255, 255,
          1,   1,   1, 255,   1,   1,   1,   1, 255, 255,
          1, 255, 255,   1,   1,   1,   1,   1, 255, 255,
          1,   1, 255,   1,   1, 255,   1,   1,   1,   1,
          1,   1, 255,   1,   1, 255,   1,   1,   1,   1,
        255, 255, 255, 255, 255, 255,   1,   1,   1,   1,
          1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
        255,   1,   1,   1,   1,   1,   1,   1,   1,   1,
		// clang-format on
	},
	{
		// clang-format off
        255,   1,   1,   1,   1,   1,   1,   1,   1,   1,
        255,   1,   1,   1,   1,   1,   1,   1,   1,   1,
        255,   1,   1,   1,   1,   1,   1,   1, 255, 255,
        255,   1,   1,   1,   1,   1,   1,   1,   1,   1,
        255,   1,   1,   1,   1,   1, 255,   1,   1,   1,
        255,   1,   1,   1,   1,   1, 255,   1,   1,   1,
        255,   1,   1,   1,   1,   1, 255,   1,   1,   1,
        255,   1,   1,   1,   1,   1, 255, 255, 255, 255,
        255,   1,   1,   1, 255, 255, 255, 255, 255,   1,
        255,   1,   1,   1,   1,   1,   1,   1,   1,   1,
		// clang-format on
	},
	{
		// clang-format off
          1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
          1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
        255, 255,   1,   1,   1,   1,   1,   1,   1,   1,
          1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
          1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
          1,   1,   1, 255, 255, 255, 255, 255,   1,   1,
          1,   1,   1,   1,   1,   1,   1, 255,   1,   1,
          1,   1,   1,   1,   1,   1,   1, 255,   1,   1,
          1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
          1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
		// clang-format on
	},
	{
		// clang-format off
          1,   1, 255,   1,   1,   1,   1, 255, 255, 255,
          1,   1, 255,   1,   1,   1, 255, 255,   1,   1,
          1,   1,   1,   1,   1, 255, 255, 255,   1,   1,
          1,   1,   1,   1, 255,   1,   1,   1,   1,   1,
          1,   1,   1,   1, 255,   1,   1,   1,   1,   1,
          1,   1,   1,   1,   1,   1,   1,   1,   1, 255,
          1,   1,   1,   1,   1,   1,   1,   1, 255,   1,
          1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
          1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
          1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
		// clang-format on
	},
	{
		// clang-format off
        255,   1,   1,   1,   1,   1,   1,   1,   1,   1,
          1,   1,   1, 255,   1,   1,   1,   1,   1,   1,
          1,   1, 255, 255, 255, 255, 255, 255,   1,   1,
          1, 255, 255, 255,   1,   1,   1,   1,   1,   1,
        255, 255, 255, 255,   1,   1,   1,   1,   1,   1,
        255,   1,   1,   1,   1,   1,   1,   1,   1, 255,
          1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
          1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
          1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
          1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
		// clang-format on
	},
	{
		// clang-format off
        255,   1,   1,   1,   1,   1,   1,   1,   1,   1,
          1,   1,   1, 255, 255, 255, 255, 255,   1,   1,
          1,   1, 255, 255,   1,   1,   1, 255,   1,   1,
          1, 255, 255,   1,   1,   1,   1,   1,   1,   1,
        255, 255,   1,   1,   1,   1,   1,   1,   1,   1,
        255, 255,   1,   1,   1, 255, 255, 255,   1,   1,
          1,   1,   1,   1,   1,   1, 255,   1,   1,   1,
          1,   1,   1, 255, 255, 255, 255,   1,   1,   1,
          1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
          1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
		// clang-format on
	},
	{
		// clang-format off
          1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
          1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
          1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
          1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
          1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
          1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
          1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
          1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
          1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
          1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
		// clang-format on
	}};

} // namespace tests
} // namespace path
} // namespace openage
