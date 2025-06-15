# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create resources and resource types for AoC.
"""
from __future__ import annotations
import typing

from ......nyan.nyan_structs import MemberSpecialValue
from .....entity_object.conversion.converter_object import RawAPIObject, ConverterObjectGroup
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_object_container import GenieObjectContainer

RESOURCE_PARENT = "engine.util.resource.Resource"
RESOURCE_CONTINGENT_PARENT = "engine.util.resource.ResourceContingent"
NAME_VALUE_PARENT = "engine.util.language.translated.type.TranslatedString"
RESOURCES_LOCATION = "data/util/resource/"


def generate_resources(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate Resource objects.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    generate_food_resource(full_data_set, pregen_converter_group)
    generate_wood_resource(full_data_set, pregen_converter_group)
    generate_stone_resource(full_data_set, pregen_converter_group)
    generate_gold_resource(full_data_set, pregen_converter_group)
    generate_population_resource(full_data_set, pregen_converter_group)


def generate_food_resource(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate the food resource type.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    pregen_nyan_objects = full_data_set.pregen_nyan_objects
    api_objects = full_data_set.nyan_api_objects

    food_ref_in_modpack = "util.resource.types.Food"
    food_raw_api_object = RawAPIObject(food_ref_in_modpack,
                                       "Food", api_objects,
                                       RESOURCES_LOCATION)
    food_raw_api_object.set_filename("types")
    food_raw_api_object.add_raw_parent(RESOURCE_PARENT)

    pregen_converter_group.add_raw_api_object(food_raw_api_object)
    pregen_nyan_objects.update({food_ref_in_modpack: food_raw_api_object})

    food_raw_api_object.add_raw_member("max_storage",
                                       MemberSpecialValue.NYAN_INF,
                                       RESOURCE_PARENT)

    food_name_ref_in_modpack = "util.attribute.types.Food.FoodName"
    food_name_value = RawAPIObject(food_name_ref_in_modpack, "FoodName",
                                   api_objects, RESOURCES_LOCATION)
    food_name_value.set_filename("types")
    food_name_value.add_raw_parent(NAME_VALUE_PARENT)
    food_name_value.add_raw_member("translations", [], NAME_VALUE_PARENT)

    name_forward_ref = ForwardRef(pregen_converter_group,
                                  food_name_ref_in_modpack)
    food_raw_api_object.add_raw_member("name",
                                       name_forward_ref,
                                       RESOURCE_PARENT)

    pregen_converter_group.add_raw_api_object(food_name_value)
    pregen_nyan_objects.update({food_name_ref_in_modpack: food_name_value})


def generate_wood_resource(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate the wood resource type.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    pregen_nyan_objects = full_data_set.pregen_nyan_objects
    api_objects = full_data_set.nyan_api_objects

    wood_ref_in_modpack = "util.resource.types.Wood"
    wood_raw_api_object = RawAPIObject(wood_ref_in_modpack,
                                       "Wood", api_objects,
                                       RESOURCES_LOCATION)
    wood_raw_api_object.set_filename("types")
    wood_raw_api_object.add_raw_parent(RESOURCE_PARENT)

    pregen_converter_group.add_raw_api_object(wood_raw_api_object)
    pregen_nyan_objects.update({wood_ref_in_modpack: wood_raw_api_object})

    wood_raw_api_object.add_raw_member("max_storage",
                                       MemberSpecialValue.NYAN_INF,
                                       RESOURCE_PARENT)

    wood_name_ref_in_modpack = "util.attribute.types.Wood.WoodName"
    wood_name_value = RawAPIObject(wood_name_ref_in_modpack, "WoodName",
                                   api_objects, RESOURCES_LOCATION)
    wood_name_value.set_filename("types")
    wood_name_value.add_raw_parent(NAME_VALUE_PARENT)
    wood_name_value.add_raw_member("translations", [], NAME_VALUE_PARENT)

    name_forward_ref = ForwardRef(pregen_converter_group,
                                  wood_name_ref_in_modpack)
    wood_raw_api_object.add_raw_member("name",
                                       name_forward_ref,
                                       RESOURCE_PARENT)

    pregen_converter_group.add_raw_api_object(wood_name_value)
    pregen_nyan_objects.update({wood_name_ref_in_modpack: wood_name_value})


def generate_stone_resource(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate the stone resource type.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    pregen_nyan_objects = full_data_set.pregen_nyan_objects
    api_objects = full_data_set.nyan_api_objects

    stone_ref_in_modpack = "util.resource.types.Stone"
    stone_raw_api_object = RawAPIObject(stone_ref_in_modpack,
                                        "Stone", api_objects,
                                        RESOURCES_LOCATION)
    stone_raw_api_object.set_filename("types")
    stone_raw_api_object.add_raw_parent(RESOURCE_PARENT)

    pregen_converter_group.add_raw_api_object(stone_raw_api_object)
    pregen_nyan_objects.update({stone_ref_in_modpack: stone_raw_api_object})

    stone_raw_api_object.add_raw_member("max_storage",
                                        MemberSpecialValue.NYAN_INF,
                                        RESOURCE_PARENT)

    stone_name_ref_in_modpack = "util.attribute.types.Stone.StoneName"
    stone_name_value = RawAPIObject(stone_name_ref_in_modpack, "StoneName",
                                    api_objects, RESOURCES_LOCATION)
    stone_name_value.set_filename("types")
    stone_name_value.add_raw_parent(NAME_VALUE_PARENT)
    stone_name_value.add_raw_member("translations", [], NAME_VALUE_PARENT)

    name_forward_ref = ForwardRef(pregen_converter_group,
                                  stone_name_ref_in_modpack)
    stone_raw_api_object.add_raw_member("name",
                                        name_forward_ref,
                                        RESOURCE_PARENT)

    pregen_converter_group.add_raw_api_object(stone_name_value)
    pregen_nyan_objects.update({stone_name_ref_in_modpack: stone_name_value})


def generate_gold_resource(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate the gold resource type.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    pregen_nyan_objects = full_data_set.pregen_nyan_objects
    api_objects = full_data_set.nyan_api_objects

    gold_ref_in_modpack = "util.resource.types.Gold"
    gold_raw_api_object = RawAPIObject(gold_ref_in_modpack,
                                       "Gold", api_objects,
                                       RESOURCES_LOCATION)
    gold_raw_api_object.set_filename("types")
    gold_raw_api_object.add_raw_parent(RESOURCE_PARENT)

    pregen_converter_group.add_raw_api_object(gold_raw_api_object)
    pregen_nyan_objects.update({gold_ref_in_modpack: gold_raw_api_object})

    gold_raw_api_object.add_raw_member("max_storage",
                                       MemberSpecialValue.NYAN_INF,
                                       RESOURCE_PARENT)

    gold_name_ref_in_modpack = "util.attribute.types.Gold.GoldName"
    gold_name_value = RawAPIObject(gold_name_ref_in_modpack, "GoldName",
                                   api_objects, RESOURCES_LOCATION)
    gold_name_value.set_filename("types")
    gold_name_value.add_raw_parent(NAME_VALUE_PARENT)
    gold_name_value.add_raw_member("translations", [], NAME_VALUE_PARENT)

    name_forward_ref = ForwardRef(pregen_converter_group,
                                  gold_name_ref_in_modpack)
    gold_raw_api_object.add_raw_member("name",
                                       name_forward_ref,
                                       RESOURCE_PARENT)

    pregen_converter_group.add_raw_api_object(gold_name_value)
    pregen_nyan_objects.update({gold_name_ref_in_modpack: gold_name_value})


def generate_population_resource(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate the population space resource type.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    pregen_nyan_objects = full_data_set.pregen_nyan_objects
    api_objects = full_data_set.nyan_api_objects

    pop_ref_in_modpack = "util.resource.types.PopulationSpace"
    pop_raw_api_object = RawAPIObject(pop_ref_in_modpack,
                                      "PopulationSpace", api_objects,
                                      RESOURCES_LOCATION)
    pop_raw_api_object.set_filename("types")
    pop_raw_api_object.add_raw_parent(RESOURCE_CONTINGENT_PARENT)

    pregen_converter_group.add_raw_api_object(pop_raw_api_object)
    pregen_nyan_objects.update({pop_ref_in_modpack: pop_raw_api_object})

    pop_name_ref_in_modpack = "util.attribute.types.PopulationSpace.PopulationSpaceName"
    pop_name_value = RawAPIObject(pop_name_ref_in_modpack, "PopulationSpaceName",
                                  api_objects, RESOURCES_LOCATION)
    pop_name_value.set_filename("types")
    pop_name_value.add_raw_parent(NAME_VALUE_PARENT)
    pop_name_value.add_raw_member("translations", [], NAME_VALUE_PARENT)

    name_forward_ref = ForwardRef(pregen_converter_group,
                                  pop_name_ref_in_modpack)
    pop_raw_api_object.add_raw_member("name",
                                      name_forward_ref,
                                      RESOURCE_PARENT)
    pop_raw_api_object.add_raw_member("max_storage",
                                      MemberSpecialValue.NYAN_INF,
                                      RESOURCE_PARENT)
    pop_raw_api_object.add_raw_member("min_amount",
                                      0,
                                      RESOURCE_CONTINGENT_PARENT)
    pop_raw_api_object.add_raw_member("max_amount",
                                      200,
                                      RESOURCE_CONTINGENT_PARENT)

    pregen_converter_group.add_raw_api_object(pop_name_value)
    pregen_nyan_objects.update({pop_name_ref_in_modpack: pop_name_value})
