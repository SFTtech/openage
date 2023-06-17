// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "controller.h"

#include <eigen3/Eigen/Dense>

#include "input/controller/camera/binding_context.h"
#include "renderer/camera/camera.h"
#include "renderer/stages/camera/manager.h"

namespace openage::input::camera {

Controller::Controller() {}

bool Controller::process(const event_arguments &ev_args,
                         const std::shared_ptr<BindingContext> &ctx) {
	if (not ctx->is_bound(ev_args.e)) {
		return false;
	}

	auto bind = ctx->lookup(ev_args.e);
	bind.action(ev_args);

	return true;
}

void setup_defaults(const std::shared_ptr<BindingContext> &ctx,
                    const std::shared_ptr<renderer::camera::Camera> &cam,
                    const std::shared_ptr<renderer::camera::CameraManager> &cam_manager) {
	// arrow movements
	binding_func_t move_left{[&](const event_arguments & /*args*/) {
		cam_manager->move_frame(renderer::camera::MoveDirection::LEFT, 0.5f);
	}};
	binding_func_t move_right{[&](const event_arguments & /*args*/) {
		cam_manager->move_frame(renderer::camera::MoveDirection::RIGHT, 0.5f);
	}};
	binding_func_t move_forward{[&](const event_arguments & /*args*/) {
		cam_manager->move_frame(renderer::camera::MoveDirection::FORWARD, 0.5f);
	}};
	binding_func_t move_backward{[&](const event_arguments & /*args*/) {
		cam_manager->move_frame(renderer::camera::MoveDirection::BACKWARD, 0.5f);
	}};

	binding_action move_left_action{move_left};
	binding_action move_right_action{move_right};
	binding_action move_forward_action{move_forward};
	binding_action move_backward_action{move_backward};

	Event ev_left{event_class::KEYBOARD, Qt::Key_Left, Qt::NoModifier, QEvent::KeyPress};
	Event ev_right{event_class::KEYBOARD, Qt::Key_Right, Qt::NoModifier, QEvent::KeyPress};
	Event ev_up{event_class::KEYBOARD, Qt::Key_Up, Qt::NoModifier, QEvent::KeyPress};
	Event ev_down{event_class::KEYBOARD, Qt::Key_Down, Qt::NoModifier, QEvent::KeyPress};

	ctx->bind(ev_left, move_left_action);
	ctx->bind(ev_right, move_right_action);
	ctx->bind(ev_up, move_forward_action);
	ctx->bind(ev_down, move_backward_action);

	// zoom
	binding_func_t zoom_in{[&](const event_arguments & /* args */) {
		// TODO: Use delta from QWheelEvent?
		cam_manager->zoom_frame(renderer::camera::ZoomDirection::IN, 0.05f);
	}};
	binding_func_t zoom_out{[&](const event_arguments & /* args */) {
		// TODO: Use delta from QWheelEvent?
		cam_manager->zoom_frame(renderer::camera::ZoomDirection::OUT, 0.05f);
	}};

	binding_action zoom_in_action{zoom_in};
	binding_action zoom_out_action{zoom_out};

	Event ev_wheel_up{event_class::WHEEL, 1, Qt::NoModifier, QEvent::Wheel};
	Event ev_wheel_down{event_class::WHEEL, -1, Qt::NoModifier, QEvent::Wheel};

	ctx->bind(ev_wheel_up, zoom_in_action);
	ctx->bind(ev_wheel_down, zoom_out_action);

	// edge movement
	binding_func_t edge_move{[&](const event_arguments &args) {
		auto event = std::dynamic_pointer_cast<QMouseEvent>(args.e.get_event());
		auto pos_x = event->position().x();
		auto pos_y = event->position().y();

		int move_directions = 0;
		if (pos_x < 10) {
			move_directions = move_directions | static_cast<int>(renderer::camera::MoveDirection::LEFT);
		}
		else if (pos_x > cam->get_viewport_size()[0] - 10) {
			move_directions = move_directions | static_cast<int>(renderer::camera::MoveDirection::RIGHT);
		}

		if (pos_y < 10) {
			move_directions = move_directions | static_cast<int>(renderer::camera::MoveDirection::FORWARD);
		}
		else if (pos_y > cam->get_viewport_size()[1] - 10) {
			move_directions = move_directions | static_cast<int>(renderer::camera::MoveDirection::BACKWARD);
		}

		cam_manager->set_move_motion_dirs(move_directions);
	}};

	binding_action edge_move_action{edge_move};
	ctx->bind(event_class::MOUSE_MOVE, edge_move_action);
}

} // namespace openage::input::camera
