# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create the attributes used in AoC.

TODO: Fill translations
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject, ConverterObjectGroup
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_object_container import GenieObjectContainer

ATTRIBUTE_PARENT = "engine.util.attribute.Attribute"
ATTRIBUTES_LOCATION = "data/util/attribute/"


def generate_attributes(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate Attribute objects.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    _generate_hp_attribute(full_data_set, pregen_converter_group)
    _generate_faith_attribute(full_data_set, pregen_converter_group)


def _generate_hp_attribute(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate the HP attribute.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    pregen_nyan_objects = full_data_set.pregen_nyan_objects
    api_objects = full_data_set.nyan_api_objects

    health_ref_in_modpack = "util.attribute.types.Health"
    health_raw_api_object = RawAPIObject(health_ref_in_modpack,
                                         "Health", api_objects,
                                         ATTRIBUTES_LOCATION)
    health_raw_api_object.set_filename("types")
    health_raw_api_object.add_raw_parent(ATTRIBUTE_PARENT)

    name_forward_ref = ForwardRef(pregen_converter_group,
                                  "util.attribute.types.Health.HealthName")
    health_raw_api_object.add_raw_member("name", name_forward_ref,
                                         ATTRIBUTE_PARENT)
    abbrv_forward_ref = ForwardRef(pregen_converter_group,
                                   "util.attribute.types.Health.HealthAbbreviation")
    health_raw_api_object.add_raw_member("abbreviation", abbrv_forward_ref,
                                         ATTRIBUTE_PARENT)

    pregen_converter_group.add_raw_api_object(health_raw_api_object)
    pregen_nyan_objects.update({health_ref_in_modpack: health_raw_api_object})

    name_value_parent = "engine.util.language.translated.type.TranslatedString"
    health_name_ref_in_modpack = "util.attribute.types.Health.HealthName"
    health_name_value = RawAPIObject(health_name_ref_in_modpack, "HealthName",
                                     api_objects, ATTRIBUTES_LOCATION)
    health_name_value.set_filename("types")
    health_name_value.add_raw_parent(name_value_parent)
    health_name_value.add_raw_member("translations", [], name_value_parent)

    pregen_converter_group.add_raw_api_object(health_name_value)
    pregen_nyan_objects.update({health_name_ref_in_modpack: health_name_value})

    abbrv_value_parent = "engine.util.language.translated.type.TranslatedString"
    health_abbrv_ref_in_modpack = "util.attribute.types.Health.HealthAbbreviation"
    health_abbrv_value = RawAPIObject(health_abbrv_ref_in_modpack, "HealthAbbreviation",
                                      api_objects, ATTRIBUTES_LOCATION)
    health_abbrv_value.set_filename("types")
    health_abbrv_value.add_raw_parent(abbrv_value_parent)
    health_abbrv_value.add_raw_member("translations", [], abbrv_value_parent)

    pregen_converter_group.add_raw_api_object(health_abbrv_value)
    pregen_nyan_objects.update({health_abbrv_ref_in_modpack: health_abbrv_value})


def _generate_faith_attribute(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate the Faith attribute.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    pregen_nyan_objects = full_data_set.pregen_nyan_objects
    api_objects = full_data_set.nyan_api_objects

    faith_ref_in_modpack = "util.attribute.types.Faith"
    faith_raw_api_object = RawAPIObject(faith_ref_in_modpack,
                                        "Faith", api_objects,
                                        ATTRIBUTES_LOCATION)
    faith_raw_api_object.set_filename("types")
    faith_raw_api_object.add_raw_parent(ATTRIBUTE_PARENT)

    name_forward_ref = ForwardRef(pregen_converter_group,
                                  "util.attribute.types.Faith.FaithName")
    faith_raw_api_object.add_raw_member("name", name_forward_ref,
                                        ATTRIBUTE_PARENT)
    abbrv_forward_ref = ForwardRef(pregen_converter_group,
                                   "util.attribute.types.Faith.FaithAbbreviation")
    faith_raw_api_object.add_raw_member("abbreviation", abbrv_forward_ref,
                                        ATTRIBUTE_PARENT)

    pregen_converter_group.add_raw_api_object(faith_raw_api_object)
    pregen_nyan_objects.update({faith_ref_in_modpack: faith_raw_api_object})

    name_value_parent = "engine.util.language.translated.type.TranslatedString"
    faith_name_ref_in_modpack = "util.attribute.types.Faith.FaithName"
    faith_name_value = RawAPIObject(faith_name_ref_in_modpack, "FaithName",
                                    api_objects, ATTRIBUTES_LOCATION)
    faith_name_value.set_filename("types")
    faith_name_value.add_raw_parent(name_value_parent)
    faith_name_value.add_raw_member("translations", [], name_value_parent)

    pregen_converter_group.add_raw_api_object(faith_name_value)
    pregen_nyan_objects.update({faith_name_ref_in_modpack: faith_name_value})

    abbrv_value_parent = "engine.util.language.translated.type.TranslatedString"
    faith_abbrv_ref_in_modpack = "util.attribute.types.Faith.FaithAbbreviation"
    faith_abbrv_value = RawAPIObject(faith_abbrv_ref_in_modpack, "FaithAbbreviation",
                                     api_objects, ATTRIBUTES_LOCATION)
    faith_abbrv_value.set_filename("types")
    faith_abbrv_value.add_raw_parent(abbrv_value_parent)
    faith_abbrv_value.add_raw_member("translations", [], abbrv_value_parent)

    pregen_converter_group.add_raw_api_object(faith_abbrv_value)
    pregen_nyan_objects.update({faith_abbrv_ref_in_modpack: faith_abbrv_value})
