# Copyright 2020-2024 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-locals

"""
Creates nyan objects for things that are hardcoded into the Genie Engine,
but configurable in openage. E.g. HP.
"""
from __future__ import annotations
import typing

from ....entity_object.conversion.converter_object import ConverterObjectGroup
from ..aoc.pregen_processor import AoCPregenSubprocessor

if typing.TYPE_CHECKING:
    from ....entity_object.conversion.aoc.genie_object_container\
        import GenieObjectContainer


class RoRPregenSubprocessor:
    """
    Creates raw API objects for hardcoded settings in RoR.
    """

    @classmethod
    def generate(cls, full_data_set: GenieObjectContainer) -> None:
        """
        Create nyan objects for hardcoded properties.
        """
        # Stores pregenerated raw API objects as a container
        pregen_converter_group = ConverterObjectGroup("pregen")

        AoCPregenSubprocessor.generate_activities(full_data_set, pregen_converter_group)
        AoCPregenSubprocessor.generate_attributes(full_data_set, pregen_converter_group)
        AoCPregenSubprocessor.generate_diplomatic_stances(full_data_set, pregen_converter_group)
        AoCPregenSubprocessor.generate_entity_types(full_data_set, pregen_converter_group)
        AoCPregenSubprocessor.generate_effect_types(full_data_set, pregen_converter_group)
        AoCPregenSubprocessor.generate_language_objects(full_data_set, pregen_converter_group)
        AoCPregenSubprocessor.generate_misc_effect_objects(full_data_set, pregen_converter_group)
        # TODO:
        # cls._generate_modifiers(gamedata, pregen_converter_group)
        AoCPregenSubprocessor.generate_terrain_types(full_data_set, pregen_converter_group)
        AoCPregenSubprocessor.generate_path_types(full_data_set, pregen_converter_group)
        AoCPregenSubprocessor.generate_resources(full_data_set, pregen_converter_group)
        AoCPregenSubprocessor.generate_death_condition(full_data_set, pregen_converter_group)

        pregen_nyan_objects = full_data_set.pregen_nyan_objects
        # Create nyan objects from the raw API objects
        for pregen_object in pregen_nyan_objects.values():
            pregen_object.create_nyan_object()

        # This has to be a separate for-loop because of possible object interdependencies
        for pregen_object in pregen_nyan_objects.values():
            pregen_object.create_nyan_members()

            if not pregen_object.is_ready():
                raise RuntimeError(f"{repr(pregen_object)}: Pregenerated object is not ready "
                                   "for export. Member or object not initialized.")
