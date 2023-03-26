// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#include "input_manager.h"

#include "input/controller/engine.h"
#include "input/event.h"
#include "input/input_context.h"

#include "renderer/gui/guisys/public/gui_input.h"

namespace openage::input {

InputManager::InputManager() :
	global_context{std::make_shared<InputContext>("main")},
	active_contexts{},
	available_contexts{},
	gui_input{nullptr} {
	std::unordered_set<size_t> factions{{0, 1, 2, 3}};
	this->controller = std::make_shared<EngineController>(factions, 0);
}

void InputManager::attach_gui(const std::shared_ptr<qtgui::GuiInput> &gui_input) {
	this->gui_input = gui_input;
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
	for (auto const &ctx : this->active_contexts) {
		if (ctx->is_bound(input_ev)) {
			this->process_action(input_ev, ctx->lookup(input_ev));
			return true;
		}
	}

	// If no local keybinds were bound, check the global keybinds
	if (this->global_context->is_bound(input_ev)) {
		this->process_action(input_ev, this->global_context->lookup(input_ev));
		return true;
	}

	return false;
}

void InputManager::process_action(const input::Event &ev,
                                  const input_action &action,
                                  const std::shared_ptr<BindingContext> &bind_ctx) {
	auto actions = action.action;
	event_arguments args{ev, this->mouse_position, this->mouse_motion, action.flags};
	if (actions) {
		actions.value()(args);
	}
	else {
		// do default action if possible
		switch (action.action_type) {
		case action_t::PUSH_CONTEXT: {
			auto ctx_id = action.flags.at("id");
			if (ctx_id != this->get_top_context()->get_id()) {
				// prevent unnecessary stacking of the same context
				this->push_context(ctx_id);
			}
			break;
		}
		case action_t::POP_CONTEXT:
			this->pop_context();
			break;

		case action_t::REMOVE_CONTEXT: {
			auto ctx_id = action.flags.at("id");
			this->pop_context(ctx_id);
			break;
		}
		case action_t::CONTROLLER: {
			this->controller->process(args, bind_ctx);
			break;
		}
		case action_t::GUI:
			this->gui_input->process(args.e.get_event());
			break;

		case action_t::CUSTOM:
			throw Error{MSG(err) << "CUSTOM action type has no default action."};

		default:
			throw Error{MSG(err) << "Unrecognized action type."};
		}
	}
}

} // namespace openage::input
