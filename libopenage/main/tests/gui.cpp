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

	auto ball_props = shader->new_uniform_input(
		"pos", pos1.matrix()
	);

	auto proj_in = shader->new_uniform_input(
		"proj", Eigen::Affine3f::Identity().matrix()
	);

	auto quad = renderer->add_mesh_geometry(renderer::resources::MeshData::make_quad());

	this->ball = renderer::Renderable{
		ball_props,
		quad,
		true,
		true,
	};

	auto set_projmatrix = renderer::ShaderUpdate{proj_in};

	this->pass = renderer::RenderPass{
		{ std::move(set_projmatrix), this->ball },
		this->renderer->get_display_target(),
	};

	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0, 1.0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	this->window.add_resize_callback(
		[=] (size_t w, size_t h) {
			Eigen::Matrix4f proj_matrix = renderer::util::ortho_matrix_f(
				0.0f, w, 0.0f, h, 0.0f, 1.0f
			);

			shader->update_uniform_input(proj_in, "proj", proj_matrix);
		}
	);
}


void Gui::get_display_size(const std::shared_ptr<PongState> &state,
                           const curve::time_t &/*now*/) {
	// TODO: make the display_boundary a curve as well.


	// TODO asdf actually get graphical display size

	state->display_boundary[0] = 80;
	state->display_boundary[1] = 60;
}


void Gui::draw(const std::shared_ptr<PongState> &state, const curve::time_t &now) {

	// TODO draw score
	//mvprintw(xpos,
	//         state->display_boundary[0] / 2 - 5,
	//         "P1 %i | P2 %i",
	//         state->p1->lives->get(now),
	//         state->p2->lives->get(now));

	// TODO: draw middle line

	// TODO: draw debug info
	/*
	mvprintw(0, 1, "NOW:  %f", now.to_double());
	mvprintw(1, 1, "SCR:  %f | %f", state->display_boundary[0], state->display_boundary[1]);
	mvprintw(2, 1,
	         "P1:   %f, %f, %i",
	         state->p1->position->get(now),
	         state->p1->paddle_x,
	         state->p1->state->get(now).state);
	mvprintw(3, 1,
	         "P2:   %f, %f, %i",
	         state->p2->position->get(now),
	         state->p2->paddle_x,
	         state->p2->state->get(now).state);
	*/

	// ball position predictions, 10s into the future
	for (int i = 0; i < 10; i++) {
		auto i_as_ctt = curve::time_t::from_int(i);
		/*
		mvprintw((5 + i), 1,
		         "BALL in %03f: %f | %f; SPEED: %f | %f | PLpos: %f, PRpos: %f",
		         i_as_ctt.to_double(),
		         state->ball->position->get(now + i_as_ctt)[0],
		         state->ball->position->get(now + i_as_ctt)[1],
		         state->ball->speed->get(now + i_as_ctt)[0],
		         state->ball->speed->get(now + i_as_ctt)[1],
		         state->p1->position->get(now + i_as_ctt),
		         state->p2->position->get(now + i_as_ctt)
		);
		*/
	}

	// show log messages
	for (auto & msg : this->log_msgs) {
		//mvprintw(ypos, state->display_boundary[0]/2 + 10, msg.c_str());
	}


	// draw player 1 paddle
	for (int i = -state->p1->size->get(now) / 2; i < state->p1->size->get(now) / 2; i++) {
		//mvprintw(state->p1->position->get(now) + i, state->p1->paddle_x, "|");
	}

	// draw player 2 paddle
	for (int i = -state->p2->size->get(now) / 2; i < state->p2->size->get(now) / 2; i++) {
		//mvprintw(state->p2->position->get(now) + i, state->p2->paddle_x, "|");
	}

	// ball position prediction 10s into the future
	for (int i = 1; i < 100; ++i) {
		auto i_as_ctt = curve::time_t::from_double(i/10.0);
		//this->draw_ball(state->ball->position->get(now + i_as_ctt), "x");
	}

	// draw the ball
	//this->draw_ball(state->ball->position->get(now), "M"); // TODO: use "⚽"

	auto ball_pos = state->ball->position->get(now);
	auto ball_pos_matrix = Eigen::Affine3f::Identity();
	ball_pos_matrix.prescale(Eigen::Vector3f(200.0f, 200.0f, 1.0f));
	//ball_pos_matrix.prerotate(Eigen::AngleAxisf(45.0f * math::PI / 180.0f, Eigen::Vector3f::UnitZ()));
	ball_pos_matrix.pretranslate(Eigen::Vector3f(ball_pos[0], ball_pos[1], 0.0f));
	this->ball.unif_in->update("pos", ball_pos_matrix.matrix());

	this->renderer->render(this->pass);
	this->window.update();
	renderer::opengl::GlContext::check_error();

	this->exit_requested = window.should_close();
	if (this->exit_requested) {
		log::log(INFO << "should exit");
	}
}


void Gui::log(const std::string &msg) {

	log::log(INFO << "Gui::log: " << msg);

	if (this->log_msgs.size() >= max_log_msgs) {
		this->log_msgs.pop_back();
	}
	this->log_msgs.push_front(msg);
}

} // openage::main::tests::pong
