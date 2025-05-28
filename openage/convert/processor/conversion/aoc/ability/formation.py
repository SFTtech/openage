# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for the Formation ability.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup
    from .....value_object.conversion.forward_ref import ForwardRef


def formation_ability(line: GenieGameEntityGroup) -> ForwardRef:
    """
    Adds the Formation ability to a line.

    :param line: Unit/Building line that gets the ability.
    :returns: The forward reference for the ability.
    """
    current_unit_id = line.get_head_unit_id()
    dataset = line.data

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

    game_entity_name = name_lookup_dict[current_unit_id][0]

    ability_ref = f"{game_entity_name}.Formation"
    ability_raw_api_object = RawAPIObject(ability_ref, "Formation", dataset.nyan_api_objects)
    ability_raw_api_object.add_raw_parent("engine.ability.type.Formation")
    ability_location = ForwardRef(line, game_entity_name)
    ability_raw_api_object.set_location(ability_location)

    # Formation definitions
    if line.get_class_id() in (6,):
        subformation = dataset.pregen_nyan_objects["util.formation.subformation.types.Infantry"].get_nyan_object(
        )

    elif line.get_class_id() in (12, 47):
        subformation = dataset.pregen_nyan_objects["util.formation.subformation.types.Cavalry"].get_nyan_object(
        )

    elif line.get_class_id() in (0, 23, 36, 44, 55):
        subformation = dataset.pregen_nyan_objects["util.formation.subformation.types.Ranged"].get_nyan_object(
        )

    elif line.get_class_id() in (2, 13, 18, 20, 35, 43, 51, 59):
        subformation = dataset.pregen_nyan_objects["util.formation.subformation.types.Siege"].get_nyan_object(
        )

    else:
        subformation = dataset.pregen_nyan_objects["util.formation.subformation.types.Support"].get_nyan_object(
        )

    formation_names = ["Line", "Staggered", "Box", "Flank"]

    formation_defs = []
    for formation_name in formation_names:
        ge_formation_ref = f"{game_entity_name}.Formation.{formation_name}"
        ge_formation_raw_api_object = RawAPIObject(ge_formation_ref,
                                                   formation_name,
                                                   dataset.nyan_api_objects)
        ge_formation_raw_api_object.add_raw_parent(
            "engine.util.game_entity_formation.GameEntityFormation")
        ge_formation_location = ForwardRef(line, ability_ref)
        ge_formation_raw_api_object.set_location(ge_formation_location)

        # Formation
        formation_ref = f"util.formation.types.{formation_name}"
        formation = dataset.pregen_nyan_objects[formation_ref].get_nyan_object()
        ge_formation_raw_api_object.add_raw_member("formation",
                                                   formation,
                                                   "engine.util.game_entity_formation.GameEntityFormation")

        # Subformation
        ge_formation_raw_api_object.add_raw_member("subformation",
                                                   subformation,
                                                   "engine.util.game_entity_formation.GameEntityFormation")

        line.add_raw_api_object(ge_formation_raw_api_object)
        ge_formation_forward_ref = ForwardRef(line, ge_formation_ref)
        formation_defs.append(ge_formation_forward_ref)

    ability_raw_api_object.add_raw_member("formations",
                                          formation_defs,
                                          "engine.ability.type.Formation")

    line.add_raw_api_object(ability_raw_api_object)

    ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

    return ability_forward_ref
