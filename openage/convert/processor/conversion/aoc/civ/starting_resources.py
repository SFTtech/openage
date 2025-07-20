# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for civ starting resources.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_civ import GenieCivilizationGroup


def get_starting_resources(civ_group: GenieCivilizationGroup) -> list[ForwardRef]:
    """
    Returns the starting resources of a civ.

    :param civ_group: Civ group representing an AoC civilization.
    """
    resource_amounts = []

    civ_id = civ_group.get_id()
    dataset = civ_group.data

    civ_lookup_dict = internal_name_lookups.get_civ_lookups(dataset.game_version)

    civ_name = civ_lookup_dict[civ_id][0]

    # Find starting resource amounts
    food_amount = civ_group.civ["resources"][91].value
    wood_amount = civ_group.civ["resources"][92].value
    gold_amount = civ_group.civ["resources"][93].value
    stone_amount = civ_group.civ["resources"][94].value

    # Find civ unique starting resources
    tech_tree = civ_group.get_tech_tree_effects()
    for effect in tech_tree:
        type_id = effect.get_type()

        if type_id != 1:
            continue

        resource_id = effect["attr_a"].value
        amount = effect["attr_d"].value
        if resource_id == 91:
            food_amount += amount

        elif resource_id == 92:
            wood_amount += amount

        elif resource_id == 93:
            gold_amount += amount

        elif resource_id == 94:
            stone_amount += amount

    food_ref = f"{civ_name}.FoodStartingAmount"
    food_raw_api_object = RawAPIObject(food_ref, "FoodStartingAmount",
                                       dataset.nyan_api_objects)
    food_raw_api_object.add_raw_parent("engine.util.resource.ResourceAmount")
    civ_location = ForwardRef(civ_group, civ_lookup_dict[civ_group.get_id()][0])
    food_raw_api_object.set_location(civ_location)

    resource = dataset.pregen_nyan_objects["util.resource.types.Food"].get_nyan_object()
    food_raw_api_object.add_raw_member("type",
                                       resource,
                                       "engine.util.resource.ResourceAmount")

    food_raw_api_object.add_raw_member("amount",
                                       food_amount,
                                       "engine.util.resource.ResourceAmount")

    food_forward_ref = ForwardRef(civ_group, food_ref)
    resource_amounts.append(food_forward_ref)

    wood_ref = f"{civ_name}.WoodStartingAmount"
    wood_raw_api_object = RawAPIObject(wood_ref, "WoodStartingAmount",
                                       dataset.nyan_api_objects)
    wood_raw_api_object.add_raw_parent("engine.util.resource.ResourceAmount")
    civ_location = ForwardRef(civ_group, civ_lookup_dict[civ_group.get_id()][0])
    wood_raw_api_object.set_location(civ_location)

    resource = dataset.pregen_nyan_objects["util.resource.types.Wood"].get_nyan_object()
    wood_raw_api_object.add_raw_member("type",
                                       resource,
                                       "engine.util.resource.ResourceAmount")

    wood_raw_api_object.add_raw_member("amount",
                                       wood_amount,
                                       "engine.util.resource.ResourceAmount")

    wood_forward_ref = ForwardRef(civ_group, wood_ref)
    resource_amounts.append(wood_forward_ref)

    gold_ref = f"{civ_name}.GoldStartingAmount"
    gold_raw_api_object = RawAPIObject(gold_ref, "GoldStartingAmount",
                                       dataset.nyan_api_objects)
    gold_raw_api_object.add_raw_parent("engine.util.resource.ResourceAmount")
    civ_location = ForwardRef(civ_group, civ_lookup_dict[civ_group.get_id()][0])
    gold_raw_api_object.set_location(civ_location)

    resource = dataset.pregen_nyan_objects["util.resource.types.Gold"].get_nyan_object()
    gold_raw_api_object.add_raw_member("type",
                                       resource,
                                       "engine.util.resource.ResourceAmount")

    gold_raw_api_object.add_raw_member("amount",
                                       gold_amount,
                                       "engine.util.resource.ResourceAmount")

    gold_forward_ref = ForwardRef(civ_group, gold_ref)
    resource_amounts.append(gold_forward_ref)

    stone_ref = f"{civ_name}.StoneStartingAmount"
    stone_raw_api_object = RawAPIObject(stone_ref, "StoneStartingAmount",
                                        dataset.nyan_api_objects)
    stone_raw_api_object.add_raw_parent("engine.util.resource.ResourceAmount")
    civ_location = ForwardRef(civ_group, civ_lookup_dict[civ_group.get_id()][0])
    stone_raw_api_object.set_location(civ_location)

    resource = dataset.pregen_nyan_objects["util.resource.types.Stone"].get_nyan_object()
    stone_raw_api_object.add_raw_member("type",
                                        resource,
                                        "engine.util.resource.ResourceAmount")

    stone_raw_api_object.add_raw_member("amount",
                                        stone_amount,
                                        "engine.util.resource.ResourceAmount")

    stone_forward_ref = ForwardRef(civ_group, stone_ref)
    resource_amounts.append(stone_forward_ref)

    civ_group.add_raw_api_object(food_raw_api_object)
    civ_group.add_raw_api_object(wood_raw_api_object)
    civ_group.add_raw_api_object(gold_raw_api_object)
    civ_group.add_raw_api_object(stone_raw_api_object)

    return resource_amounts
