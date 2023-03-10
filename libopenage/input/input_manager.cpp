// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#include "input_manager.h"

#include "input/controller.h"
#include "input/event.h"
#include "input/input_context.h"

namespace openage::input {

InputManager::InputManager() :
	global_context{std::make_shared<InputContext>()},
	contexts{} {
	std::unordered_set<size_t> factions{{0, 1, 2, 3}};
	this->controller = std::make_shared<Controller>(factions, 0);
}

const std::shared_ptr<InputContext> &InputManager::get_global_context() {
	return this->global_context;
}

const std::shared_ptr<InputContext> &InputManager::get_top_context() {
	// return the global input context if the stack is empty
	if (this->contexts.empty()) {
		return this->global_context;
	}

	return this->contexts.back();
}

std::vector<std::string> InputManager::active_binds() const {
	std::vector<std::string> result;

	// TODO: Implement

	return result;
}

void InputManager::push_context(const std::shared_ptr<InputContext> &context) {
	this->contexts.push_back(context);
}

void InputManager::pop_context() {
	if (not this->contexts.empty()) {
		this->contexts.pop_back();
	}
}

void InputManager::remove_context(const std::shared_ptr<InputContext> &context) {
	if (this->contexts.empty()) {
		return;
	}

	for (auto it = this->contexts.end(); it != this->contexts.begin(); --it) {
		if ((*it) == context) {
			this->contexts.erase(it);
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

bool InputManager::process(const QKeyEvent &ev) {
	// TODO
	// 1. Create input event (DONE)
	// 2. Lookup actions from current context (DONE)
	// 3. take any of multiple actions:
	//   - change context
	//   - forward to GUI
	//   - forward to controller
	//   - forward to renderer?

	KeyEvent input_ev{ev.keyCombination()};

	// Check context list on top of the stack (most recent bound first)
	for (auto const &ctx : this->contexts) {
		if (ctx->is_bound(input_ev)) {
			auto actions = ctx->lookup(input_ev);
			actions(input_ev);
			return true;
		}
	}

	// If no local keybinds were bound, check the global keybinds
	if (this->global_context->is_bound(input_ev)) {
		auto actions = this->global_context->lookup(input_ev);
		actions(input_ev);
		return true;
	}

	return false;
}

bool InputManager::process(const QMouseEvent &ev) {
	// TODO: Implement
	return false;
}

bool InputManager::process(const QWheelEvent &ev) {
	// TODO: Implement
	return false;
}

} // namespace openage::input
