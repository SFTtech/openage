// Copyright 2019-2019 the openage authors. See copying.md for legal info.

#include "gui.h"

#include <algorithm>
#include <vector>
#include <eigen3/Eigen/Dense>

#include "gamestate.h"
#include "../../log/log.h"
#include "../../renderer/opengl/shader.h"
#include "../../renderer/resources/shader_source.h"
#include "../../renderer/resources/texture_data.h"
#include "../../renderer/resources/mesh_data.h"
#include "../../renderer/texture.h"
#include "../../renderer/shader_program.h"
#include "../../renderer/util.h"
#include "../../renderer/geometry.h"
#include "../../renderer/opengl/window.h"


namespace openage::main::tests::pong {


const std::vector<PongEvent> &Gui::get_inputs(const std::shared_ptr<PongPlayer> &player) {
	this->input_cache.clear();

	PongEvent evnt;
	evnt.player = player->id();
	evnt.state = PongEvent::IDLE;

	std::vector<int> inputs;

	/*
	for (all inputs from SDL) {
		add key to inputs vector;
	}
	*/

	for (auto input : inputs) {
		switch (input) {
		case 0:
			evnt.state = PongEvent::DOWN;
			break;

		case 1:
			evnt.state = PongEvent::UP;
			break;

		case 27:
			exit(0);
			break;

		case 'r':
		case ' ':
			evnt.state = PongEvent::START;
			break;

		default:
			evnt.state = PongEvent::IDLE;
			break;
		}

		this->input_cache.push_back(evnt);
	}

	if (this->input_cache.empty()) {
		// store 'idle' input to cancel movement
		this->input_cache.push_back(evnt);
	}

	return this->input_cache;
}


constexpr const int max_log_msgs = 10;


Gui::Gui()
	:
	window{"openage engine test", 800, 600},
	renderer{window.make_renderer()} {

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

uniform vec3 col;
out vec4 frag;

void main() {
	frag = vec4(col.r, col.g, col.b, 1.0);
}
)s");

	auto shader = renderer->add_shader( { vshader_src, fshader_src } );

	auto proj_in = shader->new_uniform_input(
		"proj", Eigen::Affine3f::Identity().matrix()
	);

	auto col_in = shader->new_uniform_input(
		"col", Eigen::Vector3f()
	);
	auto col_red_in = shader->new_uniform_input(
		"col", Eigen::Vector3f(1, 0, 0)
	);

	auto quad = renderer->add_mesh_geometry(renderer::resources::MeshData::make_quad());

	this->ball = renderer::Renderable{
		shader->new_uniform_input(
			"pos", Eigen::Affine3f::Identity().matrix()
		),
		quad,
		true,
		true,
	};

	this->p1paddle = renderer::Renderable{
		shader->new_uniform_input(
			"pos", Eigen::Affine3f::Identity().matrix()
		),
		quad,
		true,
		true,
	};

	this->p2paddle = renderer::Renderable{
		shader->new_uniform_input(
			"pos", Eigen::Affine3f::Identity().matrix()
		),
		quad,
		true,
		true,
	};

	auto set_projmatrix = renderer::ShaderUpdate{proj_in};
	auto set_col = renderer::ShaderUpdate{col_in};
	auto set_col_red = renderer::ShaderUpdate{col_red_in};

	this->render_pass = this->renderer->add_render_pass(
		std::vector<renderer::Renderable>{
			std::move(set_projmatrix),
			std::move(set_col),
			this->ball,
			std::move(set_col_red),
			this->p1paddle,
			this->p2paddle
		},
		this->renderer->get_display_target()
	);

	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0, 1.0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	this->window.add_resize_callback(
		[=] (size_t w, size_t h) {
			Eigen::Matrix4f proj_matrix = renderer::util::ortho_matrix_f(
				0.0f, w, 0.0f, h, 0.0f, 1.0f
			);

			proj_in->update("proj", proj_matrix);

			for (auto &cb : this->resize_callbacks) {
				cb(w, h);
			}
		}
	);
}


void Gui::draw(const std::shared_ptr<PongState> &state, const curve::time_t &now) {

	constexpr float ball_size = 50.0f;
	constexpr float paddle_width = 20.0f;

	auto screen_size = state->area_size->get(now);

	// TODO draw score
	// TODO: draw middle line
	// TODO: draw debug info

	// draw the ball
	// TODO: use "⚽"
	auto ball_pos = state->ball->position->get(now);
	auto ball_pos_matrix = Eigen::Affine3f::Identity();
	ball_pos_matrix.prescale(Eigen::Vector3f(ball_size, ball_size, 1.0f));
	//ball_pos_matrix.prerotate(Eigen::AngleAxisf(45.0f * math::PI / 180.0f, Eigen::Vector3f::UnitZ()));
	ball_pos_matrix.pretranslate(Eigen::Vector3f(ball_pos[0], ball_pos[1], 0.0f));
	this->ball.uniform->update("pos", ball_pos_matrix.matrix());

	nyan::Object ballcolor = state->ballcfg.get_object("color");
	this->ball.uniform->update(
		"col",
		Eigen::Vector3f{
			ballcolor.get_int("r", now.to_int()) / 255.0f,
			ballcolor.get_int("g", now.to_int()) / 255.0f,
			ballcolor.get_int("b", now.to_int()) / 255.0f,
		}
	);

	auto p1_pos = state->p1->position->get(now);
	auto p1_size = state->p1->size->get(now);
	auto p1_pos_matrix = Eigen::Affine3f::Identity();
	p1_pos_matrix.prescale(Eigen::Vector3f(paddle_width, p1_size, 1.0f));
	p1_pos_matrix.pretranslate(Eigen::Vector3f(0, p1_pos, 0.0f));
	this->p1paddle.uniform->update("pos", p1_pos_matrix.matrix());

	auto p2_pos = state->p2->position->get(now);
	auto p2_size = state->p2->size->get(now);
	auto p2_pos_matrix = Eigen::Affine3f::Identity();
	p2_pos_matrix.prescale(Eigen::Vector3f(paddle_width, p2_size, 1.0f));
	p2_pos_matrix.pretranslate(Eigen::Vector3f(screen_size[0], p2_pos, 0.0f));
	this->p2paddle.uniform->update("pos", p2_pos_matrix.matrix());

	this->renderer->render(this->render_pass);
	this->window.update();
	renderer::opengl::GlContext::check_error();

	this->exit_requested = window.should_close();
	if (this->exit_requested) {
		log::log(INFO << "game exit requested due to window close");
	}
}


const util::Vector2s &Gui::get_window_size() const {
	return this->window.get_size();
}


void Gui::log(const std::string &msg) {

	log::log(INFO << "Gui::log: " << msg);

	if (this->log_msgs.size() >= max_log_msgs) {
		this->log_msgs.pop_back();
	}
	this->log_msgs.push_front(msg);
}


void Gui::add_resize_callback(const renderer::Window::resize_cb_t& cb) {
	this->resize_callbacks.push_back(cb);
}


void Gui::clear_resize_callbacks() {
	this->resize_callbacks.clear();
}

void Gui::update() {
	this->window.update();
}

} // openage::main::tests::pong
