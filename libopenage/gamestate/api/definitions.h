// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <utility>

#include <nyan/nyan.h>

#include "datastructure/constexpr_map.h"
#include "gamestate/activity/types.h"
#include "gamestate/api/types.h"


namespace openage::gamestate::api {

/**
 * Maps internal ability types to nyan API values.
 */
static const auto ABILITY_DEFS = datastructure::create_const_map<ability_t, nyan::ValueHolder>(
	std::pair(ability_t::IDLE,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>("engine.ability.type.Idle"))),
	std::pair(ability_t::MOVE,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>("engine.ability.type.Move"))),
	std::pair(ability_t::LIVE,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>("engine.ability.type.Live"))),
	std::pair(ability_t::TURN,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>("engine.ability.type.Turn"))));

/**
 * Maps internal property types to nyan API values.
 */
static const auto ABILITY_PROPERTY_DEFS = datastructure::create_const_map<ability_property_t, nyan::ValueHolder>(
	std::pair(ability_property_t::ANIMATED,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>("engine.ability.property.type.Animated"))),
	std::pair(ability_property_t::ANIMATION_OVERRIDE,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>("engine.ability.property.type.AnimationOverride"))),
	std::pair(ability_property_t::COMMAND_SOUND,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>("engine.ability.property.type.CommandSound"))),
	std::pair(ability_property_t::EXECUTION_SOUND,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>("engine.ability.property.type.ExecutionSound"))),
	std::pair(ability_property_t::DIPLOMATIC,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>("engine.ability.property.type.Diplomatic"))),
	std::pair(ability_property_t::LOCK,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>("engine.ability.property.type.Lock"))));

/**
 * Maps API activity node types to engine node types.
 */
static const auto ACTIVITY_NODE_DEFS = datastructure::create_const_map<std::string, activity::node_t>(
	std::pair("engine.util.activity.node.type.Start",
              activity::node_t::START),
	std::pair("engine.util.activity.node.type.End",
              activity::node_t::END),
	std::pair("engine.util.activity.node.type.Ability",
              activity::node_t::TASK_SYSTEM),
	std::pair("engine.util.activity.node.type.XORGate",
              activity::node_t::XOR_GATE),
	std::pair("engine.util.activity.node.type.XOREventGate",
              activity::node_t::XOR_EVENT_GATE));

/**
 * Maps internal patch property types to nyan API values.
 */
static const auto PATCH_PROPERTY_DEFS = datastructure::create_const_map<patch_property_t, nyan::ValueHolder>(
	std::pair(patch_property_t::DIPLOMATIC,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>("engine.patch.property.type.Diplomatic"))));

} // namespace openage::gamestate::api
