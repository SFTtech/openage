// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <memory>
#include <optional>
#include <string>

#include "event/evententity.h"
#include "event/eventhandler.h"
#include "time/time.h"


namespace openage {
namespace event {
class EventLoop;
} // namespace event

namespace gamestate {
class GameState;
class GameEntity;

class GameEntityManager : public openage::event::EventEntity {
public:
	GameEntityManager(const std::shared_ptr<openage::event::EventLoop> &loop,
	                  const std::shared_ptr<openage::gamestate::GameState> &state,
	                  const std::shared_ptr<GameEntity> &game_entity);
	~GameEntityManager() = default;

	void run_activity_system(const time::time_t &time,
	                         const std::optional<openage::event::EventHandler::param_map> &ev_params = std::nullopt);

	size_t id() const override;
	std::string idstr() const override;

private:
	std::shared_ptr<openage::event::EventLoop> loop;

	std::shared_ptr<openage::gamestate::GameState> state;

	std::shared_ptr<GameEntity> game_entity;
};

} // namespace gamestate
} // namespace openage
