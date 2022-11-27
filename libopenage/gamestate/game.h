// Copyright 2018-2018 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include "util/path.h"

namespace openage {

namespace renderer {
class RenderFactory;
}

namespace gamestate {
class Universe;

/**
 * Manages a game session (settings, win conditions, etc.).
 */
class Game {
public:
	Game(const util::Path &root_dir);
	~Game() = default;

	void attach_renderer(const std::shared_ptr<renderer::RenderFactory> &render_factory);

private:
	std::shared_ptr<Universe> universe;
};

} // namespace gamestate
} // namespace openage