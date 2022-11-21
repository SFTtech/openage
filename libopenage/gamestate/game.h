// Copyright 2018-2018 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

namespace openage::gamestate {
class Universe;

/**
 * Manages a game session (settings, win conditions, etc.).
 */
class Game {
public:
	Game() = default;
	~Game() = default;

private:
	std::shared_ptr<Universe> universe;
};

} // namespace openage::gamestate
