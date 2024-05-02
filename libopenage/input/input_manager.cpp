// Copyright 2015-2024 the openage authors. See copying.md for legal info.

#include "input_manager.h"

#include "input/controller/camera/controller.h"
#include "input/controller/game/controller.h"
#include "input/controller/hud/controller.h"
#include "input/event.h"
#include "input/input_context.h"
#include "renderer/gui/guisys/public/gui_input.h"


namespace openage::input {

InputManager::InputManager() :
	global_context{std::make_shared<InputContext>("main")},
	active_contexts{},
	available_contexts{},
	game_controller{nullptr},
	camera_controller{nullptr},
	hud_controller{nullptr},
	gui_input{nullptr} {
}

void InputManager::set_gui(const std::shared_ptr<qtgui::GuiInput> &gui_input) {
	this->gui_input = gui_input;
}

void InputManager::set_camera_controller(const std::shared_ptr<camera::Controller> &controller) {
	this->camera_controller = controller;
}

void InputManager::set_game_controller(const std::shared_ptr<game::Controller> &controller) {
	this->game_controller = controller;
}

void InputManager::set_hud_controller(const std::shared_ptr<hud::Controller> controller) {
	this->hud_controller = controller;
}

const std::shared_ptr<InputContext> &InputManager::get_global_context() {
	return this->global_context;
}

const std::shared_ptr<InputContext> &InputManager::get_top_context() {
	// return the global input context if the stack is empty
	if (this->active_contexts.empty()) {
		return this->global_context;
	}

	return this->active_contexts.back();
}

std::vector<std::string> InputManager::active_binds() const {
	std::vector<std::string> result;

	// remember events and classes bound in already visited contexts
	std::unordered_set<Event, event_hash> used_events;
	std::unordered_set<event_class, event_class_hash> used_classes;

	for (auto it = this->active_contexts.rbegin(); it != this->active_contexts.rend(); ++it) {
		auto event_binds = (*it)->get_event_binds();
		for (auto ev : event_binds) {
			if (used_events.contains(ev)) {
				// event is handled by a context with a higher priority
				continue;
			}
			for (auto cl : used_classes) {
				if (ev.cc.is_subclass(cl)) {
					// class is handled by a context with a higher priority
					continue;
				}
			}
			result.push_back(ev.info());
			used_events.insert(ev);
		}
		auto classes = (*it)->get_class_binds();
		used_classes.insert(classes.begin(), classes.end());
	}

	return result;
}

void InputManager::push_context(const std::shared_ptr<InputContext> &context) {
	this->active_contexts.push_back(context);
}

void InputManager::push_context(const std::string &id) {
	auto context = this->available_contexts.at(id);
	this->push_context(context);
}

void InputManager::pop_context() {
	if (not this->active_contexts.empty()) {
		this->active_contexts.pop_back();
	}
}

void InputManager::pop_context(const std::string &id) {
	if (this->active_contexts.empty()) {
		return;
	}

	for (auto it = this->active_contexts.begin(); it != this->active_contexts.end(); ++it) {
		if ((*it)->get_id() == id) {
			this->active_contexts.erase(it);
			return;
		}
	}
}

void InputManager::remove_context(const std::string &id) {
	this->available_contexts.erase(id);
}

void InputManager::add_context(const std::shared_ptr<InputContext> context) {
	this->available_contexts.emplace(context->get_id(), context);
}

void InputManager::set_mouse(int x, int y) {
	auto last_position = this->mouse_position;
	this->mouse_position = coord::input{coord::pixel_t{x}, coord::pixel_t{y}};
	this->mouse_motion = this->mouse_position - last_position;
}

void InputManager::set_motion(int x, int y) {
	this->mouse_motion.x = x;
	this->mouse_motion.y = y;
}

bool InputManager::process(const QEvent &ev) {
	input::Event input_ev{ev};

	// Check context list on top of the stack (most recent bound first)
	for (size_t i = this->active_contexts.size(); i > 0; --i) {
		auto &ctx = this->active_contexts.at(i - 1);
		if (ctx->is_bound(input_ev)) {
			auto &actions = ctx->lookup(input_ev);
			for (auto const &action : actions) {
				this->process_action(input_ev, action, ctx);
			}
			return true;
		}
	}

	// If no local keybinds were bound, check the global keybinds
	if (this->global_context->is_bound(input_ev)) {
		auto &actions = this->global_context->lookup(input_ev);
		for (auto const &action : actions) {
			this->process_action(input_ev, action, this->global_context);
		}
		return true;
	}

	return false;
}

void InputManager::process_action(const input::Event &ev,
                                  const input_action &action,
                                  const std::shared_ptr<InputContext> &ctx) {
	auto actions = action.action;
	event_arguments args{ev, this->mouse_position, this->mouse_motion, action.flags};
	if (actions) {
		actions.value()(args);
	}
	else {
		// do default action if possible
		switch (action.action_type) {
		case input_action_t::PUSH_CONTEXT: {
			auto ctx_id = action.flags.at("id");
			if (ctx_id != this->get_top_context()->get_id()) {
				// prevent unnecessary stacking of the same context
				this->push_context(ctx_id);
			}
			break;
		}
		case input_action_t::POP_CONTEXT:
			this->pop_context();
			break;

		case input_action_t::REMOVE_CONTEXT: {
			auto ctx_id = action.flags.at("id");
			this->pop_context(ctx_id);
			break;
		}
		case input_action_t::GAME:
			this->game_controller->process(args, ctx->get_game_bindings());
			break;

		case input_action_t::CAMERA:
			this->camera_controller->process(args, ctx->get_camera_bindings());
			break;

		case input_action_t::HUD:
			this->hud_controller->process(args, ctx->get_hud_bindings());
			break;

		case input_action_t::GUI:
			this->gui_input->process(args.e.get_event());
			break;

		case input_action_t::CUSTOM:
			throw Error{MSG(err) << "CUSTOM action type has no default action."};

		default:
			throw Error{MSG(err) << "Unrecognized action type."};
		}
	}
}


void setup_defaults(const std::shared_ptr<InputContext> &ctx) {
	// hud
	input_action hud_action{input_action_t::HUD};

	// camera
	input_action camera_action{input_action_t::CAMERA};

	Event ev_left{event_class::KEYBOARD, Qt::Key_Left, Qt::NoModifier, QEvent::KeyPress};
	Event ev_right{event_class::KEYBOARD, Qt::Key_Right, Qt::NoModifier, QEvent::KeyPress};
	Event ev_up{event_class::KEYBOARD, Qt::Key_Up, Qt::NoModifier, QEvent::KeyPress};
	Event ev_down{event_class::KEYBOARD, Qt::Key_Down, Qt::NoModifier, QEvent::KeyPress};
	Event ev_wheel_up{event_class::WHEEL, 1, Qt::NoModifier, QEvent::Wheel};
	Event ev_wheel_down{event_class::WHEEL, -1, Qt::NoModifier, QEvent::Wheel};

	ctx->bind(ev_left, camera_action);
	ctx->bind(ev_right, camera_action);
	ctx->bind(ev_up, camera_action);
	ctx->bind(ev_down, camera_action);
	ctx->bind(ev_wheel_up, camera_action);
	ctx->bind(ev_wheel_down, camera_action);
	ctx->bind(event_class::MOUSE_MOVE, {camera_action, hud_action});

	// game
	input_action game_action{input_action_t::GAME};

	Event ev_mouse_lmb{event_class::MOUSE_BUTTON, Qt::LeftButton, Qt::NoModifier, QEvent::MouseButtonRelease};
	Event ev_mouse_rmb{event_class::MOUSE_BUTTON, Qt::RightButton, Qt::NoModifier, QEvent::MouseButtonRelease};

	ctx->bind(ev_mouse_lmb, {game_action, hud_action});
	ctx->bind(ev_mouse_rmb, {game_action, hud_action});

	// also forward all other mouse button events
	ctx->bind(event_class::MOUSE_BUTTON, {game_action, hud_action});
}


} // namespace openage::input
