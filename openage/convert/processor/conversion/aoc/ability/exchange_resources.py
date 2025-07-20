# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for the ExchangeContainer ability.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup
    from .....value_object.conversion.forward_ref import ForwardRef


def exchange_resources_ability(line: GenieGameEntityGroup) -> ForwardRef:
    """
    Adds the ExchangeResources ability to a line.

    :param line: Unit/Building line that gets the ability.
    :returns: The forward reference for the ability.
    """
    current_unit_id = line.get_head_unit_id()
    dataset = line.data

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

    game_entity_name = name_lookup_dict[current_unit_id][0]

    resource_names = ["Food", "Wood", "Stone"]

    abilities = []
    for resource_name in resource_names:
        ability_name = f"MarketExchange{resource_name}"
        ability_ref = f"{game_entity_name}.{ability_name}"
        ability_raw_api_object = RawAPIObject(
            ability_ref, ability_name, dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.ExchangeResources")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        line.add_raw_api_object(ability_raw_api_object)

        # Resource that is exchanged (resource A)
        resource_a = dataset.pregen_nyan_objects[f"util.resource.types.{resource_name}"].get_nyan_object(
        )
        ability_raw_api_object.add_raw_member("resource_a",
                                              resource_a,
                                              "engine.ability.type.ExchangeResources")

        # Resource that is exchanged for (resource B)
        resource_b = dataset.pregen_nyan_objects["util.resource.types.Gold"].get_nyan_object()
        ability_raw_api_object.add_raw_member("resource_b",
                                              resource_b,
                                              "engine.ability.type.ExchangeResources")

        # Exchange rate
        exchange_rate_ref = f"util.resource.market_trading.Market{resource_name}ExchangeRate"
        exchange_rate = dataset.pregen_nyan_objects[exchange_rate_ref].get_nyan_object()
        ability_raw_api_object.add_raw_member("exchange_rate",
                                              exchange_rate,
                                              "engine.ability.type.ExchangeResources")

        # Exchange modes
        buy_exchange_ref = "util.resource.market_trading.MarketBuyExchangeMode"
        sell_exchange_ref = "util.resource.market_trading.MarketSellExchangeMode"
        exchange_modes = [
            dataset.pregen_nyan_objects[buy_exchange_ref].get_nyan_object(),
            dataset.pregen_nyan_objects[sell_exchange_ref].get_nyan_object(),
        ]
        ability_raw_api_object.add_raw_member("exchange_modes",
                                              exchange_modes,
                                              "engine.ability.type.ExchangeResources")

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())
        abilities.append(ability_forward_ref)

    return abilities
