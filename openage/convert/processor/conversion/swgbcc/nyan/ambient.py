# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Convert ambient groups to openage game entities.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from ...aoc.ability_subprocessor import AoCAbilitySubprocessor
from ..ability_subprocessor import SWGBCCAbilitySubprocessor

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_unit import GenieAmbientGroup


def ambient_group_to_game_entity(ambient_group: GenieAmbientGroup) -> None:
    """
    Creates raw API objects for an ambient group.

    :param ambient_group: Unit line that gets converted to a game entity.
    """
    ambient_unit = ambient_group.get_head_unit()
    ambient_id = ambient_group.get_head_unit_id()

    dataset = ambient_group.data

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
    class_lookup_dict = internal_name_lookups.get_class_lookups(dataset.game_version)

    # Start with the generic GameEntity
    game_entity_name = name_lookup_dict[ambient_id][0]
    obj_location = f"data/game_entity/generic/{name_lookup_dict[ambient_id][1]}/"
    raw_api_object = RawAPIObject(game_entity_name, game_entity_name,
                                  dataset.nyan_api_objects)
    raw_api_object.add_raw_parent("engine.util.game_entity.GameEntity")
    raw_api_object.set_location(obj_location)
    raw_api_object.set_filename(name_lookup_dict[ambient_id][1])
    ambient_group.add_raw_api_object(raw_api_object)

    # =======================================================================
    # Game Entity Types
    # =======================================================================
    # we give an ambient the types
    #    - util.game_entity_type.types.Ambient
    # =======================================================================
    # Create or use existing auxiliary types
    types_set = []

    type_obj = dataset.pregen_nyan_objects["util.game_entity_type.types.Ambient"].get_nyan_object(
    )
    types_set.append(type_obj)

    unit_class = ambient_unit["unit_class"].value
    class_name = class_lookup_dict[unit_class]
    class_obj_name = f"util.game_entity_type.types.{class_name}"
    type_obj = dataset.pregen_nyan_objects[class_obj_name].get_nyan_object()
    types_set.append(type_obj)

    raw_api_object.add_raw_member("types", types_set, "engine.util.game_entity.GameEntity")

    # =======================================================================
    # Abilities
    # =======================================================================
    abilities_set = []

    interaction_mode = ambient_unit["interaction_mode"].value

    if interaction_mode >= 0:
        abilities_set.append(AoCAbilitySubprocessor.death_ability(ambient_group))
        abilities_set.append(AoCAbilitySubprocessor.collision_ability(ambient_group))
        abilities_set.append(AoCAbilitySubprocessor.idle_ability(ambient_group))
        abilities_set.append(AoCAbilitySubprocessor.live_ability(ambient_group))
        abilities_set.append(AoCAbilitySubprocessor.named_ability(ambient_group))
        abilities_set.append(AoCAbilitySubprocessor.resistance_ability(ambient_group))
        abilities_set.append(AoCAbilitySubprocessor.terrain_requirement_ability(ambient_group))
        abilities_set.append(AoCAbilitySubprocessor.visibility_ability(ambient_group))

    if interaction_mode >= 2:
        abilities_set.extend(AoCAbilitySubprocessor.selectable_ability(ambient_group))

        if not ambient_group.is_passable():
            abilities_set.append(AoCAbilitySubprocessor.pathable_ability(ambient_group))

    if ambient_group.is_harvestable():
        abilities_set.append(SWGBCCAbilitySubprocessor.harvestable_ability(ambient_group))

    # =======================================================================
    # Abilities
    # =======================================================================
    raw_api_object.add_raw_member("abilities", abilities_set,
                                  "engine.util.game_entity.GameEntity")

    # =======================================================================
    # Modifiers
    # =======================================================================
    modifiers_set = []

    raw_api_object.add_raw_member("modifiers", modifiers_set,
                                  "engine.util.game_entity.GameEntity")

    # =======================================================================
    # TODO: Variants
    # =======================================================================
    variants_set = []

    raw_api_object.add_raw_member("variants", variants_set,
                                  "engine.util.game_entity.GameEntity")
