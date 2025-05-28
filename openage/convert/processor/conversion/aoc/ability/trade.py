# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for the Trade ability.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup
    from .....value_object.conversion.forward_ref import ForwardRef


def trade_ability(line: GenieGameEntityGroup) -> ForwardRef:
    """
    Adds the Trade ability to a line.

    :param line: Unit/Building line that gets the ability.
    :returns: The forward reference for the ability.
    """
    current_unit = line.get_head_unit()
    current_unit_id = line.get_head_unit_id()
    dataset = line.data

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

    game_entity_name = name_lookup_dict[current_unit_id][0]

    ability_ref = f"{game_entity_name}.Trade"
    ability_raw_api_object = RawAPIObject(ability_ref, "Trade", dataset.nyan_api_objects)
    ability_raw_api_object.add_raw_parent("engine.ability.type.Trade")
    ability_location = ForwardRef(line, game_entity_name)
    ability_raw_api_object.set_location(ability_location)

    # Trade route (use the trade route to the market)
    trade_routes = []

    unit_commands = current_unit["unit_commands"].value
    for command in unit_commands:
        # Find the trade command and the trade post id
        type_id = command["type"].value

        if type_id != 111:
            continue

        trade_post_id = command["unit_id"].value
        if trade_post_id not in dataset.building_lines.keys():
            # Skips trade workshop
            continue

        trade_post_line = dataset.building_lines[trade_post_id]
        trade_post_name = name_lookup_dict[trade_post_id][0]

        trade_route_ref = f"{trade_post_name}.TradePost.AoE2{trade_post_name}TradeRoute"
        trade_route_forward_ref = ForwardRef(trade_post_line, trade_route_ref)
        trade_routes.append(trade_route_forward_ref)

    ability_raw_api_object.add_raw_member("trade_routes",
                                          trade_routes,
                                          "engine.ability.type.Trade")

    # container
    container_forward_ref = ForwardRef(
        line, f"{game_entity_name}.ResourceStorage.TradeContainer")
    ability_raw_api_object.add_raw_member("container",
                                          container_forward_ref,
                                          "engine.ability.type.Trade")

    line.add_raw_api_object(ability_raw_api_object)

    ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

    return ability_forward_ref
