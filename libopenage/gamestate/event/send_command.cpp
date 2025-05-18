// Copyright 2023-2025 the openage authors. See copying.md for legal info.

#include "send_command.h"

#include <vector>

#include "coord/phys.h"
#include "gamestate/component/internal/command_queue.h"
#include "gamestate/component/internal/commands/apply_effect.h"
#include "gamestate/component/internal/commands/idle.h"
#include "gamestate/component/internal/commands/move.h"
#include "gamestate/component/types.h"
#include "gamestate/game_entity.h"
#include "gamestate/game_state.h"
#include "gamestate/types.h"


namespace openage::gamestate {
namespace component {
class CommandQueue;

namespace command {
class Idle;
class Move;
} // namespace command
} // namespace component

namespace event {

Commander::Commander(const std::shared_ptr<openage::event::EventLoop> &loop) :
	openage::event::EventEntity{loop} {
}

size_t Commander::id() const {
	return 0;
}

std::string Commander::idstr() const {
	return "command_target";
}


SendCommandHandler::SendCommandHandler() :
	openage::event::OnceEventHandler{"game.send_command"} {
}

void SendCommandHandler::setup_event(const std::shared_ptr<openage::event::Event> & /* event */,
                                     const std::shared_ptr<openage::event::State> & /* state */) {
	// TODO
}

void SendCommandHandler::invoke(openage::event::EventLoop & /* loop */,
                                const std::shared_ptr<openage::event::EventEntity> & /* target */,
                                const std::shared_ptr<openage::event::State> &state,
                                const time::time_t &time,
                                const param_map &params) {
	auto gstate = std::dynamic_pointer_cast<openage::gamestate::GameState>(state);

	auto command_type = params.get("type", component::command::command_t::NONE);
	std::vector<gamestate::entity_id_t> ids = params.get("entity_ids",
	                                                     std::vector<gamestate::entity_id_t>{});
	for (auto id : ids) {
		auto entity = gstate->get_game_entity(id);
		auto command_queue = std::dynamic_pointer_cast<component::CommandQueue>(
			entity->get_component(component::component_t::COMMANDQUEUE));

		switch (command_type) {
		case component::command::command_t::IDLE: {
			command_queue->add_command(time, std::make_shared<component::command::Idle>());
			break;
		}
		case component::command::command_t::MOVE: {
			auto target_pos = params.get("target", coord::phys3{0, 0, 0});
			command_queue->add_command(time, std::make_shared<component::command::Move>(target_pos));
			break;
		}
		case component::command::command_t::APPLY_EFFECT: {
			auto target_id = params.get<entity_id_t>("target", 0);
			command_queue->add_command(time, std::make_shared<component::command::ApplyEffect>(target_id));
			break;
		}
		default:
			break;
		}
	}
}

time::time_t SendCommandHandler::predict_invoke_time(const std::shared_ptr<openage::event::EventEntity> & /* target */,
                                                     const std::shared_ptr<openage::event::State> & /* state */,
                                                     const time::time_t &at) {
	return at;
}

} // namespace event
} // namespace openage::gamestate
