# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for the Herd ability.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup
    from .....value_object.conversion.forward_ref import ForwardRef


def herdable_ability(line: GenieGameEntityGroup) -> ForwardRef:
    """
    Adds the Herdable ability to a line.

    :param line: Unit/Building line that gets the ability.
    :type line: ...dataformat.converter_object.ConverterObjectGroup
    :returns: The forward reference for the ability.
    :rtype: ...dataformat.forward_ref.ForwardRef
    """
    current_unit_id = line.get_head_unit_id()
    dataset = line.data

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

    game_entity_name = name_lookup_dict[current_unit_id][0]

    ability_ref = f"{game_entity_name}.Herdable"
    ability_raw_api_object = RawAPIObject(ability_ref,
                                          "Herdable",
                                          dataset.nyan_api_objects)
    ability_raw_api_object.add_raw_parent("engine.ability.type.Herdable")
    ability_location = ForwardRef(line, game_entity_name)
    ability_raw_api_object.set_location(ability_location)

    # Mode
    mode = dataset.nyan_api_objects["engine.util.herdable_mode.type.LongestTimeInRange"]
    ability_raw_api_object.add_raw_member("mode", mode, "engine.ability.type.Herdable")

    # Discover range
    ability_raw_api_object.add_raw_member("adjacent_discover_range",
                                          1.0,
                                          "engine.ability.type.Herdable")

    line.add_raw_api_object(ability_raw_api_object)

    ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

    return ability_forward_ref
