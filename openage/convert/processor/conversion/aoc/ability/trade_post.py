# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for the TradePost ability.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup
    from .....value_object.conversion.forward_ref import ForwardRef


def trade_post_ability(line: GenieGameEntityGroup) -> ForwardRef:
    """
    Adds the TradePost ability to a line.

    :param line: Unit/Building line that gets the ability.
    :returns: The forward reference for the ability.
    """
    current_unit_id = line.get_head_unit_id()
    dataset = line.data

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

    game_entity_name = name_lookup_dict[current_unit_id][0]

    ability_ref = f"{game_entity_name}.TradePost"
    ability_raw_api_object = RawAPIObject(ability_ref,
                                          "TradePost",
                                          dataset.nyan_api_objects)
    ability_raw_api_object.add_raw_parent("engine.ability.type.TradePost")
    ability_location = ForwardRef(line, game_entity_name)
    ability_raw_api_object.set_location(ability_location)

    # Trade route
    trade_routes = []
    # =====================================================================================
    trade_route_name = f"AoE2{game_entity_name}TradeRoute"
    trade_route_ref = f"{game_entity_name}.TradePost.{trade_route_name}"
    trade_route_raw_api_object = RawAPIObject(trade_route_ref,
                                              trade_route_name,
                                              dataset.nyan_api_objects)
    trade_route_raw_api_object.add_raw_parent("engine.util.trade_route.type.AoE2TradeRoute")
    trade_route_location = ForwardRef(line, ability_ref)
    trade_route_raw_api_object.set_location(trade_route_location)

    # Trade resource
    resource = dataset.pregen_nyan_objects["util.resource.types.Gold"].get_nyan_object()
    trade_route_raw_api_object.add_raw_member("trade_resource",
                                              resource,
                                              "engine.util.trade_route.TradeRoute")

    # Start- and endpoints
    market_forward_ref = ForwardRef(line, game_entity_name)
    trade_route_raw_api_object.add_raw_member("start_trade_post",
                                              market_forward_ref,
                                              "engine.util.trade_route.TradeRoute")
    trade_route_raw_api_object.add_raw_member("end_trade_post",
                                              market_forward_ref,
                                              "engine.util.trade_route.TradeRoute")

    trade_route_forward_ref = ForwardRef(line, trade_route_ref)
    trade_routes.append(trade_route_forward_ref)

    line.add_raw_api_object(trade_route_raw_api_object)
    # =====================================================================================
    ability_raw_api_object.add_raw_member("trade_routes",
                                          trade_routes,
                                          "engine.ability.type.TradePost")

    line.add_raw_api_object(ability_raw_api_object)

    ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

    return ability_forward_ref
