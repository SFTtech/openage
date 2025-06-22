# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for the Activity ability.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.aoc.genie_unit import GenieUnitLineGroup
from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup
    from .....value_object.conversion.forward_ref import ForwardRef


def activity_ability(line: GenieGameEntityGroup) -> ForwardRef:
    """
    Adds the Activity ability to a line.

    :param line: Unit/Building line that gets the ability.
    :returns: The forward reference for the ability.
    """
    current_unit_id = line.get_head_unit_id()

    dataset = line.data

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

    game_entity_name = name_lookup_dict[current_unit_id][0]

    ability_ref = f"{game_entity_name}.Activity"
    ability_raw_api_object = RawAPIObject(ability_ref, "Activity", dataset.nyan_api_objects)
    ability_raw_api_object.add_raw_parent("engine.ability.type.Activity")
    ability_location = ForwardRef(line, game_entity_name)
    ability_raw_api_object.set_location(ability_location)

    # activity graph
    if isinstance(line, GenieUnitLineGroup):
        activity = dataset.pregen_nyan_objects["util.activity.types.Unit"].get_nyan_object()

    else:
        activity = dataset.pregen_nyan_objects["util.activity.types.Default"].get_nyan_object()

    ability_raw_api_object.add_raw_member("graph", activity, "engine.ability.type.Activity")

    line.add_raw_api_object(ability_raw_api_object)

    ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

    return ability_forward_ref
