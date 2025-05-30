# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Convert variant groups to openage game entities.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef
from ..ability_subprocessor import AoCAbilitySubprocessor
from ..upgrade_ability_subprocessor import AoCUpgradeAbilitySubprocessor

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_unit import GenieVariantGroup


def variant_group_to_game_entity(variant_group: GenieVariantGroup) -> None:
    """
    Creates raw API objects for a variant group.

    :param ambient_group: Unit line that gets converted to a game entity.
    :type ambient_group: ..dataformat.converter_object.ConverterObjectGroup
    """
    variant_main_unit = variant_group.get_head_unit()
    variant_id = variant_group.get_head_unit_id()

    dataset = variant_group.data

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
    class_lookup_dict = internal_name_lookups.get_class_lookups(dataset.game_version)

    # Start with the generic GameEntity
    game_entity_name = name_lookup_dict[variant_id][0]
    obj_location = f"data/game_entity/generic/{name_lookup_dict[variant_id][1]}/"
    raw_api_object = RawAPIObject(game_entity_name, game_entity_name,
                                  dataset.nyan_api_objects)
    raw_api_object.add_raw_parent("engine.util.game_entity.GameEntity")
    raw_api_object.set_location(obj_location)
    raw_api_object.set_filename(name_lookup_dict[variant_id][1])
    variant_group.add_raw_api_object(raw_api_object)

    # =======================================================================
    # Game Entity Types
    # =======================================================================
    # we give variants the types
    #    - util.game_entity_type.types.Ambient
    # =======================================================================
    # Create or use existing auxiliary types
    types_set = []

    type_obj = dataset.pregen_nyan_objects["util.game_entity_type.types.Ambient"].get_nyan_object(
    )
    types_set.append(type_obj)

    unit_class = variant_main_unit["unit_class"].value
    class_name = class_lookup_dict[unit_class]
    class_obj_name = f"util.game_entity_type.types.{class_name}"
    type_obj = dataset.pregen_nyan_objects[class_obj_name].get_nyan_object()
    types_set.append(type_obj)

    raw_api_object.add_raw_member("types", types_set, "engine.util.game_entity.GameEntity")

    # =======================================================================
    # Abilities
    # =======================================================================
    abilities_set = []

    abilities_set.append(AoCAbilitySubprocessor.death_ability(variant_group))
    abilities_set.append(AoCAbilitySubprocessor.despawn_ability(variant_group))
    abilities_set.append(AoCAbilitySubprocessor.idle_ability(variant_group))
    abilities_set.append(AoCAbilitySubprocessor.named_ability(variant_group))
    abilities_set.extend(AoCAbilitySubprocessor.selectable_ability(variant_group))
    abilities_set.append(AoCAbilitySubprocessor.terrain_requirement_ability(variant_group))
    abilities_set.append(AoCAbilitySubprocessor.visibility_ability(variant_group))

    if variant_main_unit.has_member("speed") and variant_main_unit["speed"].value > 0.0001\
            and variant_main_unit.has_member("command_sound_id"):
        # TODO: Let variant groups be converted without having command_sound_id member
        abilities_set.append(AoCAbilitySubprocessor.move_ability(variant_group))

    if variant_group.is_harvestable():
        abilities_set.append(AoCAbilitySubprocessor.harvestable_ability(variant_group))

    raw_api_object.add_raw_member("abilities", abilities_set,
                                  "engine.util.game_entity.GameEntity")

    # =======================================================================
    # Modifiers
    # =======================================================================
    modifiers_set = []

    raw_api_object.add_raw_member("modifiers", modifiers_set,
                                  "engine.util.game_entity.GameEntity")

    # =======================================================================
    # Variants
    # =======================================================================
    variants_set = []

    variant_type = name_lookup_dict[variant_id][3]

    index = 0
    for variant in variant_group.line:
        # Create a diff
        diff_variant = variant_main_unit.diff(variant)

        if variant_type == "random":
            variant_type_ref = "engine.util.variant.type.RandomVariant"

        elif variant_type == "angle":
            variant_type_ref = "engine.util.variant.type.PerspectiveVariant"

        elif variant_type == "misc":
            variant_type_ref = "engine.util.variant.type.MiscVariant"

        else:
            raise ValueError(f"Unknown variant type: '{variant_type}' for '{game_entity_name}'.")

        variant_name = f"Variant{str(index)}"
        variant_ref = f"{game_entity_name}.{variant_name}"
        variant_raw_api_object = RawAPIObject(variant_ref,
                                              variant_name,
                                              dataset.nyan_api_objects)
        variant_raw_api_object.add_raw_parent(variant_type_ref)
        variant_location = ForwardRef(variant_group, game_entity_name)
        variant_raw_api_object.set_location(variant_location)

        # Create patches for the diff
        patches = []

        patches.extend(AoCUpgradeAbilitySubprocessor.death_ability(variant_group,
                                                                   variant_group,
                                                                   variant_ref,
                                                                   diff_variant))
        patches.extend(AoCUpgradeAbilitySubprocessor.despawn_ability(variant_group,
                                                                     variant_group,
                                                                     variant_ref,
                                                                     diff_variant))
        patches.extend(AoCUpgradeAbilitySubprocessor.idle_ability(variant_group,
                                                                  variant_group,
                                                                  variant_ref,
                                                                  diff_variant))
        patches.extend(AoCUpgradeAbilitySubprocessor.named_ability(variant_group,
                                                                   variant_group,
                                                                   variant_ref,
                                                                   diff_variant))

        if variant_main_unit.has_member("speed") and variant_main_unit["speed"].value > 0.0001\
                and variant_main_unit.has_member("command_sound_id"):
            # TODO: Let variant groups be converted without having command_sound_id member:
            patches.extend(AoCUpgradeAbilitySubprocessor.move_ability(variant_group,
                                                                      variant_group,
                                                                      variant_ref,
                                                                      diff_variant))

        # Changes
        variant_raw_api_object.add_raw_member("changes",
                                              patches,
                                              "engine.util.variant.Variant")

        # Prority
        variant_raw_api_object.add_raw_member("priority",
                                              1,
                                              "engine.util.variant.Variant")

        if variant_type == "random":
            variant_raw_api_object.add_raw_member("chance_share",
                                                  1 / len(variant_group.line),
                                                  "engine.util.variant.type.RandomVariant")

        elif variant_type == "angle":
            variant_raw_api_object.add_raw_member("angle",
                                                  index,
                                                  "engine.util.variant.type.PerspectiveVariant")

        variants_forward_ref = ForwardRef(variant_group, variant_ref)
        variants_set.append(variants_forward_ref)
        variant_group.add_raw_api_object(variant_raw_api_object)

        index += 1

    raw_api_object.add_raw_member("variants", variants_set,
                                  "engine.util.game_entity.GameEntity")
