// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "wait.h"

#include "gamestate/manager.h"


namespace openage::gamestate::event {

WaitHandler::WaitHandler() :
	OnceEventHandler{"game.wait"} {}

void WaitHandler::setup_event(const std::shared_ptr<openage::event::Event> & /* event */,
                              const std::shared_ptr<openage::event::State> & /* state */) {
	// TODO
}

void WaitHandler::invoke(openage::event::EventLoop & /* loop */,
                         const std::shared_ptr<openage::event::EventEntity> &target,
                         const std::shared_ptr<openage::event::State> & /* state */,
                         const time::time_t &time,
                         const param_map &params) {
	auto mgr = std::dynamic_pointer_cast<openage::gamestate::GameEntityManager>(target);
	mgr->run_activity_system(time, params);
}

time::time_t WaitHandler::predict_invoke_time(const std::shared_ptr<openage::event::EventEntity> & /* target */,
                                              const std::shared_ptr<openage::event::State> & /* state */,
                                              const time::time_t &at) {
	return at;
}


} // namespace openage::gamestate::event
