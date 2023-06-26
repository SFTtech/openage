// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include "event/evententity.h"


namespace openage::gamestate {
class GameEntity;

class GameEntityManager : public openage::event::EventEntity {
public:
	GameEntityManager(const std::shared_ptr<openage::event::EventLoop> &loop,
	                  const std::shared_ptr<GameEntity> &game_entity);
	~GameEntityManager() = default;

	void run_activity_system();

	size_t id() const override;
	std::string idstr() const override;

private:
	std::shared_ptr<GameEntity> game_entity;
};

} // namespace openage::gamestate
