# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for the ProductionQueue ability.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup


def production_queue_ability(line: GenieGameEntityGroup) -> ForwardRef:
    """
    Adds the ProductionQueue ability to a line.

    :param line: Unit/Building line that gets the ability.
    :returns: The forward reference for the ability.
    """
    current_unit_id = line.get_head_unit_id()
    dataset = line.data

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

    game_entity_name = name_lookup_dict[current_unit_id][0]

    ability_ref = f"{game_entity_name}.ProductionQueue"
    ability_raw_api_object = RawAPIObject(ability_ref,
                                          "ProductionQueue",
                                          dataset.nyan_api_objects)
    ability_raw_api_object.add_raw_parent("engine.ability.type.ProductionQueue")
    ability_location = ForwardRef(line, game_entity_name)
    ability_raw_api_object.set_location(ability_location)

    # Size
    size = 22

    ability_raw_api_object.add_raw_member("size", size, "engine.ability.type.ProductionQueue")

    # Production modes
    modes = []

    mode_name = f"{game_entity_name}.ProvideContingent.CreatablesMode"
    mode_raw_api_object = RawAPIObject(mode_name, "CreatablesMode", dataset.nyan_api_objects)
    mode_raw_api_object.add_raw_parent("engine.util.production_mode.type.Creatables")
    mode_location = ForwardRef(line, ability_ref)
    mode_raw_api_object.set_location(mode_location)

    # RoR allows all creatables in production queue
    mode_raw_api_object.add_raw_member("exclude",
                                       [],
                                       "engine.util.production_mode.type.Creatables")

    mode_forward_ref = ForwardRef(line, mode_name)
    modes.append(mode_forward_ref)

    ability_raw_api_object.add_raw_member("production_modes",
                                          modes,
                                          "engine.ability.type.ProductionQueue")

    line.add_raw_api_object(mode_raw_api_object)
    line.add_raw_api_object(ability_raw_api_object)

    ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

    return ability_forward_ref
