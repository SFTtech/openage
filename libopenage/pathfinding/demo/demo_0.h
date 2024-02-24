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
 * Show the pathfinding functionality of the path module:
 *     - Cost field
 *     - Integration field
 *     - Flow field
 *
 * Visualizes the pathfinding results using our rendering backend.
 *
 * @param path Path to the project rootdir.
 */
void path_demo_0(const util::Path &path);


class RenderManager {
public:
	RenderManager(const std::shared_ptr<renderer::gui::GuiApplicationWithLogger> &app,
	              const std::shared_ptr<renderer::Window> &window,
	              const util::Path &path);
	~RenderManager() = default;

	void run();

	void show_cost_field(const std::shared_ptr<path::CostField> &field);
	void show_integration_field(const std::shared_ptr<path::IntegrationField> &field);
	void show_flow_field(const std::shared_ptr<path::FlowField> &field);

private:
	void init_shaders();
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
	 * @param field Flow field to visualize.
	 */
	static renderer::resources::MeshData get_flow_field_mesh(const std::shared_ptr<path::FlowField> &field,
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


	const util::Path &path;

	std::shared_ptr<renderer::gui::GuiApplicationWithLogger> app;
	std::shared_ptr<renderer::Window> window;
	std::shared_ptr<renderer::Renderer> renderer;
	std::shared_ptr<renderer::camera::Camera> camera;

	std::shared_ptr<renderer::ShaderProgram> cost_shader;
	std::shared_ptr<renderer::ShaderProgram> integration_shader;
	std::shared_ptr<renderer::ShaderProgram> flow_shader;
	std::shared_ptr<renderer::ShaderProgram> grid_shader;
	std::shared_ptr<renderer::ShaderProgram> obj_shader;
	std::shared_ptr<renderer::ShaderProgram> display_shader;

	std::shared_ptr<renderer::RenderPass> background_pass;
	std::shared_ptr<renderer::RenderPass> field_pass;
	std::shared_ptr<renderer::RenderPass> grid_pass;
	std::shared_ptr<renderer::RenderPass> display_pass;
};

} // namespace tests
} // namespace path
} // namespace openage
