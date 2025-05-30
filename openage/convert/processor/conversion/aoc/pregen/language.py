# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create language objects for AoC.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject, ConverterObjectGroup

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_object_container import GenieObjectContainer

LANGUAGE_PARENT = "engine.util.language.Language"
LANGUAGE_LOCATION = "data/util/language/"


def generate_language_objects(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate language objects from the string resources

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    pregen_nyan_objects = full_data_set.pregen_nyan_objects
    api_objects = full_data_set.nyan_api_objects

    languages = full_data_set.strings.get_tables().keys()

    for language in languages:
        language_ref_in_modpack = f"util.language.{language}"
        language_raw_api_object = RawAPIObject(language_ref_in_modpack,
                                               language,
                                               api_objects,
                                               LANGUAGE_LOCATION)
        language_raw_api_object.set_filename("language")
        language_raw_api_object.add_raw_parent(LANGUAGE_PARENT)

        language_raw_api_object.add_raw_member("ietf_string",
                                               language,
                                               LANGUAGE_PARENT)

        pregen_converter_group.add_raw_api_object(language_raw_api_object)
        pregen_nyan_objects.update({language_ref_in_modpack: language_raw_api_object})
