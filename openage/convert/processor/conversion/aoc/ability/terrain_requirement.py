# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for the TerrainRequirement ability.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup
    from .....value_object.conversion.forward_ref import ForwardRef


def terrain_requirement_ability(line: GenieGameEntityGroup) -> ForwardRef:
    """
    Adds the TerrainRequirement to a line.

    :param line: Unit/Building line that gets the ability.
    :returns: The forward references for the abilities.
    """
    current_unit = line.get_head_unit()
    current_unit_id = line.get_head_unit_id()
    dataset = line.data

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
    terrain_type_lookup_dict = internal_name_lookups.get_terrain_type_lookups(
        dataset.game_version)

    game_entity_name = name_lookup_dict[current_unit_id][0]

    ability_ref = f"{game_entity_name}.TerrainRequirement"
    ability_raw_api_object = RawAPIObject(ability_ref,
                                          "TerrainRequirement",
                                          dataset.nyan_api_objects)
    ability_raw_api_object.add_raw_parent("engine.ability.type.TerrainRequirement")
    ability_location = ForwardRef(line, game_entity_name)
    ability_raw_api_object.set_location(ability_location)

    # Allowed types
    allowed_types = []
    terrain_restriction = current_unit["terrain_restriction"].value
    for terrain_type in terrain_type_lookup_dict.values():
        # Check if terrain type is covered by terrain restriction
        if terrain_restriction in terrain_type[1]:
            type_name = f"util.terrain_type.types.{terrain_type[2]}"
            type_obj = dataset.pregen_nyan_objects[type_name].get_nyan_object()
            allowed_types.append(type_obj)

    ability_raw_api_object.add_raw_member("allowed_types",
                                          allowed_types,
                                          "engine.ability.type.TerrainRequirement")

    # Blacklisted terrains
    ability_raw_api_object.add_raw_member("blacklisted_terrains",
                                          [],
                                          "engine.ability.type.TerrainRequirement")

    line.add_raw_api_object(ability_raw_api_object)

    ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

    return ability_forward_ref
