// Copyright 2022-2024 the openage authors. See copying.md for legal info.

#include "render_stage.h"

#include "renderer/camera/camera.h"
#include "renderer/opengl/context.h"
#include "renderer/render_pass.h"
#include "renderer/render_target.h"
#include "renderer/renderer.h"
#include "renderer/resources/shader_source.h"
#include "renderer/resources/texture_info.h"
#include "renderer/shader_program.h"
#include "renderer/stages/terrain/chunk.h"
#include "renderer/stages/terrain/mesh.h"
#include "renderer/stages/terrain/model.h"
#include "renderer/window.h"
#include "time/clock.h"


namespace openage::renderer::terrain {

TerrainRenderStage::TerrainRenderStage(const std::shared_ptr<Window> &window,
                                       const std::shared_ptr<renderer::Renderer> &renderer,
                                       const std::shared_ptr<renderer::camera::Camera> &camera,
                                       const util::Path &shaderdir,
                                       const std::shared_ptr<renderer::resources::AssetManager> &asset_manager,
                                       const std::shared_ptr<time::Clock> &clock) :
	renderer{renderer},
	camera{camera},
	render_entity{nullptr},
	model{std::make_shared<TerrainRenderModel>(asset_manager)},
	clock{clock} {
	renderer::opengl::GlContext::check_error();

	auto size = window->get_size();
	this->initialize_render_pass(size[0], size[1], shaderdir);

	window->add_resize_callback([this](size_t width, size_t height, double /*scale*/) {
		this->resize(width, height);
	});

	this->model->set_camera(this->camera);

	log::log(INFO << "Created render stage 'Terrain'");
}

std::shared_ptr<renderer::RenderPass> TerrainRenderStage::get_render_pass() {
	return this->render_pass;
}

void TerrainRenderStage::add_render_entity(const std::shared_ptr<RenderEntity> entity,
                                           const util::Vector2s chunk_size,
                                           const coord::scene2_delta chunk_offset) {
	std::unique_lock lock{this->mutex};

	this->render_entity = entity;
	this->model->add_chunk(this->render_entity, chunk_size, chunk_offset);
	this->update();
}

void TerrainRenderStage::update() {
	this->model->fetch_updates();
	auto current_time = this->clock->get_real_time();
	for (auto &chunk : this->model->get_chunks()) {
		for (auto &mesh : chunk->get_meshes()) {
			if (mesh->requires_renderable()) [[unlikely]] { /*probably doesn't happen that often?*/
				// TODO: Update uniforms and geometry individually, depending on what changed
				// TODO: Update existing renderable instead of recreating it
				auto geometry = this->renderer->add_mesh_geometry(mesh->get_mesh());
				auto transform_unifs = this->display_shader->create_empty_input();

				Renderable display_obj{
					transform_unifs,
					geometry,
					true,
					true, // it's a 3D object, so we need depth testing
				};

				// TODO: Remove old renderable instead of clearing everything
				this->render_pass->add_renderables(std::move(display_obj));
				mesh->clear_requires_renderable();

				mesh->set_uniforms(transform_unifs);
			}
		}
	}
	this->model->update_uniforms(current_time);
}

void TerrainRenderStage::resize(size_t width, size_t height) {
	this->output_texture = renderer->add_texture(resources::Texture2dInfo(width, height, resources::pixel_format::rgba8));
	this->depth_texture = renderer->add_texture(resources::Texture2dInfo(width, height, resources::pixel_format::depth24));

	auto fbo = this->renderer->create_texture_target({this->output_texture, this->depth_texture});
	this->render_pass->set_target(fbo);
}

void TerrainRenderStage::initialize_render_pass(size_t width,
                                                size_t height,
                                                const util::Path &shaderdir) {
	auto vert_shader_file = (shaderdir / "terrain.vert.glsl").open();
	auto vert_shader_src = renderer::resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::vertex,
		vert_shader_file.read());
	vert_shader_file.close();

	auto frag_shader_file = (shaderdir / "terrain.frag.glsl").open();
	auto frag_shader_src = renderer::resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::fragment,
		frag_shader_file.read());
	frag_shader_file.close();

	this->output_texture = renderer->add_texture(resources::Texture2dInfo(width, height, resources::pixel_format::rgba8));
	this->depth_texture = renderer->add_texture(resources::Texture2dInfo(width, height, resources::pixel_format::depth24));

	this->display_shader = this->renderer->add_shader({vert_shader_src, frag_shader_src});
	this->display_shader->bind_uniform_buffer("camera", this->camera->get_uniform_buffer());

	auto fbo = this->renderer->create_texture_target({this->output_texture, this->depth_texture});
	this->render_pass = this->renderer->add_render_pass({}, fbo);
}

} // namespace openage::renderer::terrain
