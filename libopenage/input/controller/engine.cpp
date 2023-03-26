// Copyright 2021-2023 the openage authors. See copying.md for legal info.

#include "engine.h"

#include "input/binding_context.h"

namespace openage::input {

EngineController::EngineController(const std::unordered_set<size_t> &controlled_factions,
                                   size_t active_faction_id) :
	controlled_factions{controlled_factions},
	active_faction_id{active_faction_id},
	outqueue{} {}

void EngineController::set_control(size_t faction_id) {
	if (this->controlled_factions.find(faction_id) != this->controlled_factions.end()) {
		this->active_faction_id = faction_id;
	}
}

size_t EngineController::get_controlled() {
	return this->active_faction_id;
}

bool EngineController::process(const event_arguments &ev_args, const std::shared_ptr<BindingContext> &ctx) {
	// TODO: check if action is allowed
	auto bind = ctx->lookup(ev_args.e);
	auto game_event = bind.transform(ev_args);

	switch (bind.action) {
	case forward_action_t::SEND:
		this->outqueue.push_back(game_event);
		for (auto event : this->outqueue) {
			// TODO: Send gamestate event
		}
		break;

	case forward_action_t::QUEUE:
		this->outqueue.push_back(game_event);
		break;

	case forward_action_t::CLEAR:
		this->outqueue.clear();
		break;

	default:
		throw Error{MSG(err) << "Unrecognized action type."};
	}

	return true;
}

} // namespace openage::input
