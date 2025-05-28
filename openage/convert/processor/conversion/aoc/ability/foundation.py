# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for the Foundation ability.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup
    from .....value_object.conversion.forward_ref import ForwardRef


def foundation_ability(line: GenieGameEntityGroup, terrain_id: int = -1) -> ForwardRef:
    """
    Adds the Foundation abilities to a line. Optionally chooses the specified
    terrain ID.

    :param line: Unit/Building line that gets the ability.
    :type line: ...dataformat.converter_object.ConverterObjectGroup
    :param terrain_id: Force this terrain ID as foundation
    :type terrain_id: int
    :returns: The forward references for the abilities.
    :rtype: ...dataformat.forward_ref.ForwardRef
    """
    current_unit = line.get_head_unit()
    current_unit_id = line.get_head_unit_id()
    dataset = line.data

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
    terrain_lookup_dict = internal_name_lookups.get_terrain_lookups(dataset.game_version)

    game_entity_name = name_lookup_dict[current_unit_id][0]

    ability_ref = f"{game_entity_name}.Foundation"
    ability_raw_api_object = RawAPIObject(ability_ref, "Foundation", dataset.nyan_api_objects)
    ability_raw_api_object.add_raw_parent("engine.ability.type.Foundation")
    ability_location = ForwardRef(line, game_entity_name)
    ability_raw_api_object.set_location(ability_location)

    line.add_raw_api_object(ability_raw_api_object)

    # Terrain
    if terrain_id == -1:
        terrain_id = current_unit["foundation_terrain_id"].value

    terrain = dataset.terrain_groups[terrain_id]
    terrain_forward_ref = ForwardRef(terrain, terrain_lookup_dict[terrain_id][1])
    ability_raw_api_object.add_raw_member("foundation_terrain",
                                          terrain_forward_ref,
                                          "engine.ability.type.Foundation")

    ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

    return ability_forward_ref
