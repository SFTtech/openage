// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once


namespace openage::gamestate::activity {

/**
 * Node types in the flow graph.
 */
enum class node_t {
	START,
	END,
	EVENT_GATEWAY,
	XOR_GATEWAY,
	TASK,
};

} // namespace openage::gamestate::activity
