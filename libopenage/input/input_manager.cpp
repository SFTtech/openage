// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#include "input_manager.h"

#include "input/binding_context.h"
#include "input/controller.h"
#include "input/event.h"
#include "input/input_context.h"

namespace openage::input {

InputManager::InputManager() :
	global_context{std::make_shared<InputContext>("main")},
	active_contexts{},
	available_contexts{} {
	std::unordered_set<size_t> factions{{0, 1, 2, 3}};
	this->controller = std::make_shared<Controller>(factions, 0);
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

	// TODO: Implement

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

void InputManager::remove_context(const std::shared_ptr<InputContext> &context) {
	if (this->active_contexts.empty()) {
		return;
	}

	for (auto it = this->active_contexts.end(); it != this->active_contexts.begin(); --it) {
		if ((*it) == context) {
			this->active_contexts.erase(it);
			return;
		}
	}
}

void InputManager::remove_context(const std::string &id) {
	if (this->active_contexts.empty()) {
		return;
	}

	for (auto it = this->active_contexts.end(); it != this->active_contexts.begin(); --it) {
		if ((*it)->get_id() == id) {
			this->active_contexts.erase(it);
			return;
		}
	}
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
	// TODO
	// 1. Create input event (DONE)
	// 2. Lookup actions from current context (DONE)
	// 3. take any of multiple actions:
	//   - change context
	//   - forward to GUI
	//   - forward to controller
	//   - forward to renderer?

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
                                  const InputAction &action,
                                  const std::shared_ptr<BindingContext> &bind_ctx) {
	auto actions = action.action;
	if (actions) {
		actions.value()(ev);
	}
	else {
		// do default action if possible
		switch (action.action_type) {
		case action_t::PUSH_CONTEXT: {
			auto ctx_id = action.flags.at("id");
			this->push_context(ctx_id);
			break;
		}
		case action_t::POP_CONTEXT:
			this->pop_context();
			break;

		case action_t::REMOVE_CONTEXT: {
			auto ctx_id = action.flags.at("id");
			this->remove_context(ctx_id);
			break;
		}
		case action_t::CONTROLLER: {
			this->controller->process(ev, bind_ctx);
			break;
		}
		case action_t::GUI:
			// TODO
			break;

		case action_t::CUSTOM:
			throw Error{MSG(err) << "CUSTOM action type has no default action."};

		default:
			throw Error{MSG(err) << "Unrecognized action type."};
		}
	}
}

} // namespace openage::input
