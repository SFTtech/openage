// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include "util/path.h"


namespace openage {
namespace renderer {
class RenderPass;
class Renderer;
class ShaderProgram;
class Window;

namespace camera {
class Camera;
} // namespace camera

namespace gui {
class GuiApplicationWithLogger;
} // namespace gui

namespace resources {
class MeshData;
} // namespace resources
} // namespace renderer

namespace path {
class CostField;
class IntegrationField;
class FlowField;

namespace tests {

/**
 * Show the functionality of the different flowfield types:
 *     - Cost field
 *     - Integration field
 *     - Flow field
 *
 * Visualizes the pathfinding results using our rendering backend.
 *
 * @param path Path to the project rootdir.
 */
void path_demo_0(const util::Path &path);


/**
 * Manages the graphical display of the pathfinding demo.
 */
class RenderManager0 {
public:
	enum class field_t {
		COST,
		INTEGRATION,
		FLOW
	};

	/**
	 * Create a new render manager.
	 *
	 * @param app GUI application.
	 * @param window Window to render to.
	 * @param path Path to the project rootdir.
	 */
	RenderManager0(const std::shared_ptr<renderer::gui::GuiApplicationWithLogger> &app,
	               const std::shared_ptr<renderer::Window> &window,
	               const util::Path &path);
	~RenderManager0() = default;

	/**
	 * Run the render loop.
	 */
	void run();

	/**
	 * Draw a cost field to the screen.
	 *
	 * @param field Cost field.
	 */
	void show_cost_field(const std::shared_ptr<path::CostField> &field);

	/**
	 * Draw an integration field to the screen.
	 *
	 * @param field Integration field.
	 */
	void show_integration_field(const std::shared_ptr<path::IntegrationField> &field);

	/**
	 * Draw a flow field to the screen.
	 *
	 * @param flow_field Flow field.
	 * @param int_field Integration field.
	 */
	void show_flow_field(const std::shared_ptr<path::FlowField> &flow_field,
	                     const std::shared_ptr<path::IntegrationField> &int_field);

	/**
	 * Draw the steering vectors of a flow field to the screen.
	 *
	 * @param field Flow field.
	 */
	void show_vectors(const std::shared_ptr<path::FlowField> &field);

	/**
	 * Hide drawn steering vectors.
	 */
	void hide_vectors();

	/**
	 * Get the cell coordinates at a given screen position.
	 *
	 * @param x X coordinate.
	 * @param y Y coordinate.
	 */
	std::pair<int, int> select_tile(double x, double y);

private:
	/**
	 * Load the shader sources for the demo and create the shader programs.
	 */
	void init_shaders();

	/**
	 * Create the following render passes for the demo:
	 *   - Background pass: Mono-colored background object.
	 *   - Field pass; Renders the cost, integration and flow fields.
	 *   - Vector pass: Renders the steering vectors of a flow field.
	 *   - Grid pass: Renders a grid on top of the fields.
	 *   - Display pass: Draws the results of previous passes to the screen.
	 */
	void init_passes();


	/**
	 * Create a mesh for the cost field.
	 *
	 * @param field Cost field to visualize.
	 * @param resolution Determines the number of subdivisions per grid cell. The number of
	 *                   quads per cell is resolution^2. (default = 2)
	 *
	 * @return Mesh data for the cost field.
	 */
	static renderer::resources::MeshData get_cost_field_mesh(const std::shared_ptr<path::CostField> &field,
	                                                         size_t resolution = 2);

	/**
	 * Create a mesh for the integration field.
	 *
	 * @param field Integration field to visualize.
	 * @param resolution Determines the number of subdivisions per grid cell. The number of
	 *                   quads per cell is resolution^2. (default = 2)
	 *
	 * @return Mesh data for the integration field.
	 */
	static renderer::resources::MeshData get_integration_field_mesh(const std::shared_ptr<path::IntegrationField> &field,
	                                                                size_t resolution = 2);

	/**
	 * Create a mesh for the flow field.
	 *
	 * @param flow_field Flow field to visualize.
	 * @param int_field Integration field.
	 */
	static renderer::resources::MeshData get_flow_field_mesh(const std::shared_ptr<path::FlowField> &flow_field,
	                                                         const std::shared_ptr<path::IntegrationField> &int_field,
	                                                         size_t resolution = 2);

	/**
	 * Create a mesh for an arrow.
	 *
	 * @return Mesh data for an arrow.
	 */
	static renderer::resources::MeshData get_arrow_mesh();

	/**
	 * Create a mesh for the grid.
	 *
	 * @param side_length Length of the grid's side.
	 *
	 * @return Mesh data for the grid.
	 */
	static renderer::resources::MeshData get_grid_mesh(size_t side_length);

	/**
	 * Path to the project rootdir.
	 */
	const util::Path &path;

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

	/**
	 * Camera to view the scene.
	 */
	std::shared_ptr<renderer::camera::Camera> camera;

	/* Shader programs */

	/**
	 * Shader program for rendering a cost field.
	 */
	std::shared_ptr<renderer::ShaderProgram> cost_shader;

	/**
	 * Shader program for rendering a integration field.
	 */
	std::shared_ptr<renderer::ShaderProgram> integration_shader;

	/**
	 * Shader program for rendering a flow field.
	 */
	std::shared_ptr<renderer::ShaderProgram> flow_shader;

	/**
	 * Shader program for rendering steering vectors.
	 */
	std::shared_ptr<renderer::ShaderProgram> vector_shader;

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
	 * Field pass: Renders the cost, integration and flow fields.
	 */
	std::shared_ptr<renderer::RenderPass> field_pass;

	/**
	 * Vector pass: Renders the steering vectors of a flow field.
	 */
	std::shared_ptr<renderer::RenderPass> vector_pass;

	/**
	 * Grid pass: Renders a grid on top of the fields.
	 */
	std::shared_ptr<renderer::RenderPass> grid_pass;

	/**
	 * Display pass: Draws the results of previous passes to the screen.
	 */
	std::shared_ptr<renderer::RenderPass> display_pass;
};

} // namespace tests
} // namespace path
} // namespace openage
