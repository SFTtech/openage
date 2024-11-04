// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once


namespace openage::gamestate::activity {

/**
 * Node types in the flow graph.
 */
enum class node_t {
	START,
	END,
	XOR_EVENT_GATE,
	XOR_GATE,
	XOR_SWITCH_GATE,
	TASK_CUSTOM,
	TASK_SYSTEM,
};

} // namespace openage::gamestate::activity
