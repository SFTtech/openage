// Copyright 2018-2018 the openage authors. See copying.md for legal info.

#include <cstdlib>
#include <epoxy/gl.h>
#include <functional>
#include <unordered_map>
#include <memory>
#include <eigen3/Eigen/Dense>

#include <nyan/nyan.h>

#include "../../util/math_constants.h"
#include "../../log/log.h"
#include "../../error/error.h"
#include "../../renderer/resources/shader_source.h"
#include "../../renderer/resources/texture_data.h"
#include "../../renderer/resources/mesh_data.h"
#include "../../renderer/texture.h"
#include "../../renderer/shader_program.h"
#include "../../renderer/util.h"
#include "../../renderer/geometry.h"
#include "../../renderer/opengl/window.h"


namespace openage::main::tests::pong {


void main(const util::Path& path) {

	std::shared_ptr<nyan::Database> db = nyan::Database::create();

	db->load(
		"test.nyan",
		[&path] (const std::string &filename) {
			// TODO: nyan should provide a file api that we can inherit from
			// then we can natively interface to the openage file abstraction
			// and do not need to read the whole file here.
			// ideally, the returned file lazily accesses all data through openage::util::File.
			return std::make_shared<nyan::File>(
				filename,
				(path / ("assets/nyan/" + filename)).open_r().read()
			);
		}
	);

	std::shared_ptr<nyan::View> root = db->new_view();
	nyan::Object test = root->get("test.Test");
	log::log(INFO << "nyan read test: " << *test.get<nyan::Int>("member"));

	renderer::opengl::GlWindow window("openage engine test", 800, 600);

	auto renderer = window.make_renderer();

	auto vshader_src = renderer::resources::ShaderSource(
		renderer::resources::shader_lang_t::glsl,
		renderer::resources::shader_stage_t::vertex,
		R"s(
#version 330

layout(location=0) in vec2 position;
uniform mat4 pos;
uniform mat4 proj;

void main() {
	gl_Position = proj * pos * vec4(position, 0.0, 1.0);
}
)s");

	auto fshader_src = renderer::resources::ShaderSource(
		renderer::resources::shader_lang_t::glsl,
		renderer::resources::shader_stage_t::fragment,
		R"s(
#version 330

out vec4 col;

void main() {
	col = vec4(1.0, 0, 0, 1.0);
}
)s");

	auto shader = renderer->add_shader( { vshader_src, fshader_src } );

	auto pos1 = Eigen::Affine3f::Identity();
	pos1.prescale(Eigen::Vector3f(200.0f, 200.0f, 1.0f));
	//pos1.prerotate(Eigen::AngleAxisf(45.0f * math::PI / 180.0f, Eigen::Vector3f::UnitZ()));
	pos1.pretranslate(Eigen::Vector3f(400.0f, 400.0f, 0.0f));

	auto pos_in = shader->new_uniform_input(
		"pos", pos1.matrix()
	);

	auto proj_in = shader->new_uniform_input(
		"proj", Eigen::Affine3f::Identity().matrix()
	);

	auto quad = renderer->add_mesh_geometry(renderer::resources::MeshData::make_quad());
	renderer::Renderable obj1 {
		pos_in.get(),
		quad.get(),
		true,
		true,
	};

	renderer::Renderable projsetup {
		proj_in.get(),
		nullptr,
	};

	renderer::RenderPass pass {
		{ projsetup, obj1 },
		renderer->get_display_target(),
	};


	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0, 1.0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	window.add_resize_callback(
		[&] (size_t w, size_t h) {
			Eigen::Matrix4f proj_matrix = renderer::util::ortho_matrix_f(0.0f, w,
			                                                             0.0f, h,
			                                                             0.0f, 1.0f);

			shader->update_uniform_input(proj_in.get(), "proj", proj_matrix);
		}
	);

	while (!window.should_close()) {
		renderer->render(pass);
		window.update();
		renderer::opengl::GlContext::check_error();
	}
}


} // openage::main::tests::pong
