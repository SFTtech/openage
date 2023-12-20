// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "process_command.h"

#include "event/event_loop.h"
#include "gamestate/component/internal/command_queue.h"
#include "gamestate/game_entity.h"
#include "gamestate/game_state.h"
#include "gamestate/manager.h"


namespace openage::gamestate::event {

ProcessCommandHandler::ProcessCommandHandler() :
	OnceEventHandler{"game.process_command"} {}

void ProcessCommandHandler::setup_event(const std::shared_ptr<openage::event::Event> & /* event */,
                                        const std::shared_ptr<openage::event::State> & /* state */) {
	// TODO
}

void ProcessCommandHandler::invoke(openage::event::EventLoop & /* loop */,
                                   const std::shared_ptr<openage::event::EventEntity> &target,
                                   const std::shared_ptr<openage::event::State> & /* state */,
                                   const time::time_t &time,
                                   const param_map &params) {
	auto mgr = std::dynamic_pointer_cast<openage::gamestate::GameEntityManager>(target);
	mgr->run_activity_system(time, params);
}

time::time_t ProcessCommandHandler::predict_invoke_time(const std::shared_ptr<openage::event::EventEntity> & /* target */,
                                                        const std::shared_ptr<openage::event::State> & /* state */,
                                                        const time::time_t &at) {
	return at;
}

std::shared_ptr<openage::event::Event> primer_process_command(const time::time_t &,
                                                              const std::shared_ptr<gamestate::GameEntity> &entity,
                                                              const std::shared_ptr<openage::event::EventLoop> &loop,
                                                              const std::shared_ptr<gamestate::GameState> &state,
                                                              size_t next_id) {
	openage::event::EventHandler::param_map::map_t params{{"next", next_id}}; // move->get_id();
	auto ev = loop->create_event("game.process_command",
	                             entity->get_manager(),
	                             state,
	                             // event is not executed until a command is available
	                             std::numeric_limits<time::time_t>::max(),
	                             params);
	auto entity_queue = std::dynamic_pointer_cast<component::CommandQueue>(
		entity->get_component(component::component_t::COMMANDQUEUE));
	auto &queue = const_cast<curve::Queue<std::shared_ptr<component::command::Command>> &>(entity_queue->get_queue());
	queue.add_dependent(ev);

	return ev;
};

} // namespace openage::gamestate::event
