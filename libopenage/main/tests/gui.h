// Copyright 2019-2019 the openage authors. See copying.md for legal info.

#pragma once


#include <vector>
#include <deque>

#include "../../curve/curve.h"
#include "../../renderer/renderer.h"
#include "../../renderer/opengl/window.h"


namespace openage::main::tests::pong {

class PongEvent;
class PongState;
class PongPlayer;


class Gui {
public:
	Gui();

	const std::vector<PongEvent> &get_inputs(const std::shared_ptr<PongPlayer> &player);
	void get_display_size(const std::shared_ptr<PongState> &state,
	                      const curve::time_t &now);

	void draw(const std::shared_ptr<PongState> &state, const curve::time_t &now);

	void log(const std::string &msg);

	bool exit_requested = false;

private:
	renderer::opengl::GlWindow window;
	std::unique_ptr<renderer::Renderer> renderer;

	std::vector<PongEvent> input_cache;
	std::deque<std::string> log_msgs;

	// rendering objects
	renderer::Renderable ball;

	renderer::RenderPass pass;
};

} // openage::main::tests::pong
