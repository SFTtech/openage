// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <utility>

#include <nyan/nyan.h>

#include "datastructure/constexpr_map.h"
#include "gamestate/activity/condition/command_in_queue.h"
#include "gamestate/activity/condition/next_command.h"
#include "gamestate/activity/event/command_in_queue.h"
#include "gamestate/activity/event/wait.h"
#include "gamestate/activity/types.h"
#include "gamestate/activity/xor_event_gate.h"
#include "gamestate/activity/xor_gate.h"
#include "gamestate/api/types.h"
#include "gamestate/system/types.h"


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
 * Maps API activity task system types to engine system types.
 *
 * TODO: Expand this to include all systems.
 */
static const auto ACTIVITY_TASK_SYSTEM_DEFS = datastructure::create_const_map<std::string, system::system_id_t>(
	std::pair("engine.ability.type.Idle",
              system::system_id_t::IDLE),
	std::pair("engine.ability.type.Move",
              system::system_id_t::MOVE_COMMAND));

/**
 * Maps API activity condition types to engine condition types.
 */
static const auto ACTIVITY_CONDITIONS = datastructure::create_const_map<std::string, activity::condition_t>(
	std::pair("engine.util.activity.condition.type.CommandInQueue",
              std::function(gamestate::activity::command_in_queue)),
	std::pair("engine.util.activity.condition.type.NextCommandIdle",
              std::function(gamestate::activity::next_command_idle)),
	std::pair("engine.util.activity.condition.type.NextCommandMove",
              std::function(gamestate::activity::next_command_move)));

static const auto ACTIVITY_EVENT_PRIMERS = datastructure::create_const_map<std::string, activity::event_primer_t>(
	std::pair("engine.util.activity.event.type.CommandInQueue",
              std::function(gamestate::activity::primer_command_in_queue)),
	std::pair("engine.util.activity.event.type.Wait",
              std::function(gamestate::activity::primer_wait)),
	std::pair("engine.util.activity.event.type.WaitAbility",
              std::function(gamestate::activity::primer_wait)));

/**
 * Maps internal patch property types to nyan API values.
 */
static const auto PATCH_PROPERTY_DEFS = datastructure::create_const_map<patch_property_t, nyan::ValueHolder>(
	std::pair(patch_property_t::DIPLOMATIC,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>("engine.patch.property.type.Diplomatic"))));

} // namespace openage::gamestate::api
