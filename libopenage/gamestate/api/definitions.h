// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <utility>

#include <nyan/nyan.h>

#include "datastructure/constexpr_map.h"
#include "gamestate/activity/condition/command_in_queue.h"
#include "gamestate/activity/condition/next_command.h"
#include "gamestate/activity/condition/next_command_switch.h"
#include "gamestate/activity/event/command_in_queue.h"
#include "gamestate/activity/event/wait.h"
#include "gamestate/activity/types.h"
#include "gamestate/activity/xor_event_gate.h"
#include "gamestate/activity/xor_gate.h"
#include "gamestate/activity/xor_switch_gate.h"
#include "gamestate/api/types.h"
#include "gamestate/system/types.h"


namespace openage::gamestate::api {

/**
 * Maps internal ability types to nyan API values.
 */
static const auto ABILITY_DEFS = datastructure::create_const_map<ability_t, nyan::ValueHolder>(
	std::pair(ability_t::ACTIVITY,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>("engine.ability.type.Activity"))),
	std::pair(ability_t::APPLY_CONTINUOUS_EFFECT,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>("engine.ability.type.ApplyContinuousEffect"))),
	std::pair(ability_t::APPLY_DISCRETE_EFFECT,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>("engine.ability.type.ApplyDiscreteEffect"))),
	std::pair(ability_t::IDLE,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>("engine.ability.type.Idle"))),
	std::pair(ability_t::MOVE,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>("engine.ability.type.Move"))),
	std::pair(ability_t::LINE_OF_SIGHT,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>("engine.ability.type.LineOfSight"))),
	std::pair(ability_t::LIVE,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>("engine.ability.type.Live"))),
	std::pair(ability_t::RANGED_CONTINUOUS_EFFECT,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>("engine.ability.type.RangedContinuousEffect"))),
	std::pair(ability_t::RANGED_DISCRETE_EFFECT,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>("engine.ability.type.RangedDiscreteEffect"))),
	std::pair(ability_t::RESISTANCE,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>("engine.ability.type.Resistance"))),
	std::pair(ability_t::SELECTABLE,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>("engine.ability.type.Selectable"))),
	std::pair(ability_t::TURN,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>("engine.ability.type.Turn"))));

/**
 * Maps internal effect types to nyan API values.
 */
static const auto EFFECT_DEFS = datastructure::create_const_map<effect_t, nyan::ValueHolder>(
	std::pair(effect_t::CONTINUOUS_FLAC_DECREASE,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>(
				  "engine.effect.continuous.flat_attribute_change.type.FlatAttributeChangeDecrease"))),
	std::pair(effect_t::CONTINUOUS_FLAC_INCREASE,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>(
				  "engine.effect.continuous.flat_attribute_change.type.FlatAttributeChangeIncrease"))),
	std::pair(effect_t::CONTINUOUS_LURE,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>("engine.effect.continuous.type.Lure"))),
	std::pair(effect_t::CONTINUOUS_TRAC_DECREASE,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>(
				  "engine.effect.continuous.time_relative_attribute_change.type.TimeRelativeAttributeChangeDecrease"))),
	std::pair(effect_t::CONTINUOUS_TRAC_INCREASE,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>(
				  "engine.effect.continuous.time_relative_attribute_change.type.TimeRelativeAttributeChangeIncrease"))),
	std::pair(effect_t::CONTINUOUS_TRPC_DECREASE,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>(
				  "engine.effect.continuous.time_relative_progress_change.type.TimeRelativeProgressChangeDecrease"))),
	std::pair(effect_t::CONTINUOUS_TRPC_INCREASE,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>(
				  "engine.effect.continuous.time_relative_progress_change.type.TimeRelativeProgressChangeIncrease"))),
	std::pair(effect_t::DISCRETE_CONVERT,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>("engine.effect.discrete.Convert"))),
	std::pair(effect_t::DISCRETE_FLAC_DECREASE,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>(
				  "engine.effect.discrete.flat_attribute_change.type.FlatAttributeChangeDecrease"))),
	std::pair(effect_t::DISCRETE_FLAC_INCREASE,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>(
				  "engine.effect.discrete.flat_attribute_change.type.FlatAttributeChangeIncrease"))),
	std::pair(effect_t::DISCRETE_MAKE_HARVESTABLE,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>("engine.effect.discrete.type.MakeHarvestable"))),
	std::pair(effect_t::DISCRETE_SEND_TO_CONTAINER,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>("engine.effect.discrete.type.SendToContainer"))));

/**
 * Maps internal effect types to nyan API values.
 */
static const auto RESISTANCE_DEFS = datastructure::create_const_map<effect_t, nyan::ValueHolder>(
	std::pair(effect_t::CONTINUOUS_FLAC_DECREASE,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>(
				  "engine.resistance.type.ContinuousFlatAttributeChangeDecrease"))),
	std::pair(effect_t::CONTINUOUS_FLAC_INCREASE,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>(
				  "engine.resistance.type.ContinuousFlatAttributeChangeIncrease"))),
	std::pair(effect_t::CONTINUOUS_LURE,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>("engine.resistance.type.Lure"))),
	std::pair(effect_t::CONTINUOUS_TRAC_DECREASE,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>(
				  "engine.resistance.type.ContinuousTimeRelativeAttributeChangeDecrease"))),
	std::pair(effect_t::CONTINUOUS_TRAC_INCREASE,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>(
				  "engine.resistance.type.ContinuousTimeRelativeAttributeChangeIncrease"))),
	std::pair(effect_t::CONTINUOUS_TRPC_DECREASE,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>(
				  "engine.resistance.type.ContinuousTimeRelativeProgressChangeDecrease"))),
	std::pair(effect_t::CONTINUOUS_TRPC_INCREASE,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>(
				  "engine.resistance.type.ContinuousTimeRelativeProgressChangeIncrease"))),
	std::pair(effect_t::DISCRETE_CONVERT,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>("engine.resistance.type.Convert"))),
	std::pair(effect_t::DISCRETE_FLAC_DECREASE,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>(
				  "engine.resistance.type.DiscreteFlatAttributeChangeDecrease"))),
	std::pair(effect_t::DISCRETE_FLAC_INCREASE,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>(
				  "engine.resistance.type.DiscreteFlatAttributeChangeIncrease"))),
	std::pair(effect_t::DISCRETE_MAKE_HARVESTABLE,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>("engine.resistance.type.MakeHarvestable"))),
	std::pair(effect_t::DISCRETE_SEND_TO_CONTAINER,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>("engine.resistance.type.SendToContainer"))));

/**
 * Maps API effect types to internal effect types.
 */
static const auto EFFECT_TYPE_LOOKUP = datastructure::create_const_map<nyan::fqon_t, effect_t>(
	std::pair("engine.effect.continuous.flat_attribute_change.type.FlatAttributeChangeDecrease",
              effect_t::CONTINUOUS_FLAC_DECREASE),
	std::pair("engine.effect.continuous.flat_attribute_change.type.FlatAttributeChangeIncrease",
              effect_t::CONTINUOUS_FLAC_INCREASE),
	std::pair("engine.effect.continuous.type.Lure",
              effect_t::CONTINUOUS_LURE),
	std::pair("engine.effect.continuous.time_relative_attribute_change.type.TimeRelativeAttributeChangeDecrease",
              effect_t::CONTINUOUS_TRAC_DECREASE),
	std::pair("engine.effect.continuous.time_relative_attribute_change.type.TimeRelativeAttributeChangeIncrease",
              effect_t::CONTINUOUS_TRAC_INCREASE),
	std::pair("engine.effect.continuous.time_relative_progress_change.type.TimeRelativeProgressChangeDecrease",
              effect_t::CONTINUOUS_TRPC_DECREASE),
	std::pair("engine.effect.continuous.time_relative_progress_change.type.TimeRelativeProgressChangeIncrease",
              effect_t::CONTINUOUS_TRPC_INCREASE),
	std::pair("engine.effect.discrete.Convert",
              effect_t::DISCRETE_CONVERT),
	std::pair("engine.effect.discrete.flat_attribute_change.type.FlatAttributeChangeDecrease",
              effect_t::DISCRETE_FLAC_DECREASE),
	std::pair("engine.effect.discrete.flat_attribute_change.type.FlatAttributeChangeIncrease",
              effect_t::DISCRETE_FLAC_INCREASE),
	std::pair("engine.effect.discrete.type.MakeHarvestable",
              effect_t::DISCRETE_MAKE_HARVESTABLE),
	std::pair("engine.effect.discrete.type.SendToContainer",
              effect_t::DISCRETE_SEND_TO_CONTAINER));

/**
 * Maps API resistance types to internal effect types.
 */
static const auto RESISTANCE_TYPE_LOOKUP = datastructure::create_const_map<nyan::fqon_t, effect_t>(
	std::pair("engine.resistance.continuous.flat_attribute_change.type.FlatAttributeChangeDecrease",
              effect_t::CONTINUOUS_FLAC_DECREASE),
	std::pair("engine.resistance.continuous.flat_attribute_change.type.FlatAttributeChangeIncrease",
              effect_t::CONTINUOUS_FLAC_INCREASE),
	std::pair("engine.resistance.continuous.type.Lure",
              effect_t::CONTINUOUS_LURE),
	std::pair("engine.resistance.continuous.type.TimeRelativeAttributeChangeDecrease",
              effect_t::CONTINUOUS_TRAC_DECREASE),
	std::pair("engine.resistance.continuous.type.TimeRelativeAttributeChangeIncrease",
              effect_t::CONTINUOUS_TRAC_INCREASE),
	std::pair("engine.resistance.continuous.type.TimeRelativeProgressChangeDecrease",
              effect_t::CONTINUOUS_TRPC_DECREASE),
	std::pair("engine.resistance.continuous.type.TimeRelativeProgressChangeIncrease",
              effect_t::CONTINUOUS_TRPC_INCREASE),
	std::pair("engine.resistance.discrete.type.Convert",
              effect_t::DISCRETE_CONVERT),
	std::pair("engine.resistance.discrete.convert.type.AoE2Convert", // TODO: Remove from API
              effect_t::DISCRETE_CONVERT),
	std::pair("engine.resistance.discrete.flat_attribute_change.type.FlatAttributeChangeDecrease",
              effect_t::DISCRETE_FLAC_DECREASE),
	std::pair("engine.resistance.discrete.flat_attribute_change.type.FlatAttributeChangeIncrease",
              effect_t::DISCRETE_FLAC_INCREASE),
	std::pair("engine.resistance.discrete.type.MakeHarvestable",
              effect_t::DISCRETE_MAKE_HARVESTABLE),
	std::pair("engine.resistance.discrete.type.SendToContainer",
              effect_t::DISCRETE_SEND_TO_CONTAINER));


/**
 * Maps internal ability property types to nyan API values.
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
 * Maps internal effect property types to nyan API values.
 */
static const auto EFFECT_PROPERTY_DEFS = datastructure::create_const_map<effect_property_t, nyan::ValueHolder>(
	std::pair(effect_property_t::AREA,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>("engine.effect.property.type.Area"))),
	std::pair(effect_property_t::COST,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>("engine.effect.property.type.Cost"))),
	std::pair(effect_property_t::DIPLOMATIC,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>("engine.effect.property.type.Diplomatic"))),
	std::pair(effect_property_t::PRIORITY,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>("engine.effect.property.type.Priority"))));

/**
 * Maps internal resistance property types to nyan API values.
 */
static const auto RESISTANCE_PROPERTY_DEFS = datastructure::create_const_map<resistance_property_t, nyan::ValueHolder>(
	std::pair(resistance_property_t::COST,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>("engine.resistance.property.type.Cost"))),
	std::pair(resistance_property_t::STACKED,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>("engine.resistance.property.type.Stacked"))));

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
              activity::node_t::XOR_EVENT_GATE),
	std::pair("engine.util.activity.node.type.XORSwitchGate",
              activity::node_t::XOR_SWITCH_GATE));

/**
 * Maps API activity task system types to engine system types.
 *
 * TODO: Expand this to include all systems.
 */
static const auto ACTIVITY_TASK_SYSTEM_DEFS = datastructure::create_const_map<std::string, system::system_id_t>(
	std::pair("engine.ability.type.ApplyDiscreteEffect",
              system::system_id_t::APPLY_EFFECT),
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
	std::pair("engine.util.activity.condition.type.NextCommandApplyEffect",
              std::function(gamestate::activity::next_command_apply_effect)),
	std::pair("engine.util.activity.condition.type.NextCommandIdle",
              std::function(gamestate::activity::next_command_idle)),
	std::pair("engine.util.activity.condition.type.NextCommandMove",
              std::function(gamestate::activity::next_command_move)));

/**
 * Maps API activity event types to event primer functions.
 */
static const auto ACTIVITY_EVENT_PRIMERS = datastructure::create_const_map<std::string, activity::event_primer_t>(
	std::pair("engine.util.activity.event.type.CommandInQueue",
              std::function(gamestate::activity::primer_command_in_queue)),
	std::pair("engine.util.activity.event.type.Wait",
              std::function(gamestate::activity::primer_wait)),
	std::pair("engine.util.activity.event.type.WaitAbility",
              std::function(gamestate::activity::primer_wait)));

/**
 * Maps API activity switch condition types to lookup functions.
 */
static const auto ACTIVITY_SWITCH_CONDITIONS = datastructure::create_const_map<std::string,
                                                                               activity::XorSwitchGate::lookup_function_t>(
	std::pair("engine.util.activity.switch_condition.type.NextCommand",
              std::function(gamestate::activity::next_command_switch)));

static const auto ACTIVITY_SWITCH_CONDITION_TYPES = datastructure::create_const_map<std::string,
                                                                                    switch_condition_t>(
	std::pair("engine.util.activity.switch_condition.type.NextCommand",
              switch_condition_t::NEXT_COMMAND));

/**
 * Maps internal patch property types to nyan API values.
 */
static const auto PATCH_PROPERTY_DEFS = datastructure::create_const_map<patch_property_t, nyan::ValueHolder>(
	std::pair(patch_property_t::DIPLOMATIC,
              nyan::ValueHolder(std::make_shared<nyan::ObjectValue>("engine.patch.property.type.Diplomatic"))));

} // namespace openage::gamestate::api
