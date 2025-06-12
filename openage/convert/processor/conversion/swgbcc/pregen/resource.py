# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create resources and resource types for SWGB.
"""
from __future__ import annotations
import typing

from ......nyan.nyan_structs import MemberSpecialValue
from .....entity_object.conversion.converter_object import RawAPIObject
from .....value_object.conversion.forward_ref import ForwardRef
from ...aoc.pregen import resource as aoc_pregen_resource
from ...aoc.pregen.resource import RESOURCE_PARENT, RESOURCES_LOCATION, NAME_VALUE_PARENT

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.converter_object import ConverterObjectGroup
    from .....entity_object.conversion.aoc.genie_object_container import GenieObjectContainer


def generate_resources(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate Resource objects.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    aoc_pregen_resource.generate_food_resource(full_data_set, pregen_converter_group)
    generate_carbon_resource(full_data_set, pregen_converter_group)
    generate_ore_resource(full_data_set, pregen_converter_group)
    generate_nova_resource(full_data_set, pregen_converter_group)
    aoc_pregen_resource.generate_population_resource(full_data_set, pregen_converter_group)


def generate_carbon_resource(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate the carbon resource type.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    pregen_nyan_objects = full_data_set.pregen_nyan_objects
    api_objects = full_data_set.nyan_api_objects

    carbon_ref_in_modpack = "util.resource.types.Carbon"
    carbon_raw_api_object = RawAPIObject(carbon_ref_in_modpack,
                                         "Carbon", api_objects,
                                         RESOURCES_LOCATION)
    carbon_raw_api_object.set_filename("types")
    carbon_raw_api_object.add_raw_parent(RESOURCE_PARENT)

    pregen_converter_group.add_raw_api_object(carbon_raw_api_object)
    pregen_nyan_objects.update({carbon_ref_in_modpack: carbon_raw_api_object})

    carbon_raw_api_object.add_raw_member("max_storage",
                                         MemberSpecialValue.NYAN_INF,
                                         RESOURCE_PARENT)

    carbon_name_ref_in_modpack = "util.attribute.types.Carbon.CarbonName"
    carbon_name_value = RawAPIObject(carbon_name_ref_in_modpack, "CarbonName",
                                     api_objects, RESOURCES_LOCATION)
    carbon_name_value.set_filename("types")
    carbon_name_value.add_raw_parent(NAME_VALUE_PARENT)
    carbon_name_value.add_raw_member("translations", [], NAME_VALUE_PARENT)

    name_forward_ref = ForwardRef(pregen_converter_group,
                                  carbon_name_ref_in_modpack)
    carbon_raw_api_object.add_raw_member("name",
                                         name_forward_ref,
                                         RESOURCE_PARENT)

    pregen_converter_group.add_raw_api_object(carbon_name_value)
    pregen_nyan_objects.update({carbon_name_ref_in_modpack: carbon_name_value})


def generate_ore_resource(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate the ore resource type.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    pregen_nyan_objects = full_data_set.pregen_nyan_objects
    api_objects = full_data_set.nyan_api_objects

    ore_ref_in_modpack = "util.resource.types.Ore"
    ore_raw_api_object = RawAPIObject(ore_ref_in_modpack,
                                      "Ore", api_objects,
                                      RESOURCES_LOCATION)
    ore_raw_api_object.set_filename("types")
    ore_raw_api_object.add_raw_parent(RESOURCE_PARENT)

    pregen_converter_group.add_raw_api_object(ore_raw_api_object)
    pregen_nyan_objects.update({ore_ref_in_modpack: ore_raw_api_object})

    ore_raw_api_object.add_raw_member("max_storage",
                                      MemberSpecialValue.NYAN_INF,
                                      RESOURCE_PARENT)

    ore_name_ref_in_modpack = "util.attribute.types.Ore.OreName"
    ore_name_value = RawAPIObject(ore_name_ref_in_modpack, "OreName",
                                  api_objects, RESOURCES_LOCATION)
    ore_name_value.set_filename("types")
    ore_name_value.add_raw_parent(NAME_VALUE_PARENT)
    ore_name_value.add_raw_member("translations", [], NAME_VALUE_PARENT)

    name_forward_ref = ForwardRef(pregen_converter_group,
                                  ore_name_ref_in_modpack)
    ore_raw_api_object.add_raw_member("name",
                                      name_forward_ref,
                                      RESOURCE_PARENT)

    pregen_converter_group.add_raw_api_object(ore_name_value)
    pregen_nyan_objects.update({ore_name_ref_in_modpack: ore_name_value})


def generate_nova_resource(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate the nova resource type.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    pregen_nyan_objects = full_data_set.pregen_nyan_objects
    api_objects = full_data_set.nyan_api_objects

    nova_ref_in_modpack = "util.resource.types.Nova"
    nova_raw_api_object = RawAPIObject(nova_ref_in_modpack,
                                       "Nova", api_objects,
                                       RESOURCES_LOCATION)
    nova_raw_api_object.set_filename("types")
    nova_raw_api_object.add_raw_parent(RESOURCE_PARENT)

    pregen_converter_group.add_raw_api_object(nova_raw_api_object)
    pregen_nyan_objects.update({nova_ref_in_modpack: nova_raw_api_object})

    nova_raw_api_object.add_raw_member("max_storage",
                                       MemberSpecialValue.NYAN_INF,
                                       RESOURCE_PARENT)

    nova_name_ref_in_modpack = "util.attribute.types.Nova.NovaName"
    nova_name_value = RawAPIObject(nova_name_ref_in_modpack, "NovaName",
                                   api_objects, RESOURCES_LOCATION)
    nova_name_value.set_filename("types")
    nova_name_value.add_raw_parent(NAME_VALUE_PARENT)
    nova_name_value.add_raw_member("translations", [], NAME_VALUE_PARENT)

    name_forward_ref = ForwardRef(pregen_converter_group,
                                  nova_name_ref_in_modpack)
    nova_raw_api_object.add_raw_member("name",
                                       name_forward_ref,
                                       RESOURCE_PARENT)

    pregen_converter_group.add_raw_api_object(nova_name_value)
    pregen_nyan_objects.update({nova_name_ref_in_modpack: nova_name_value})
