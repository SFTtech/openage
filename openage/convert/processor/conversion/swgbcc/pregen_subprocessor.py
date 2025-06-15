# Copyright 2020-2025 the openage authors. See copying.md for legal info.

"""
Creates nyan objects for things that are hardcoded into the Genie Engine,
but configurable in openage. E.g. HP.
"""
from __future__ import annotations
import typing

from ....entity_object.conversion.converter_object import ConverterObjectGroup
from ..aoc.pregen_processor import AoCPregenSubprocessor

from .pregen.effect import generate_effect_types
from .pregen.exchange import generate_exchange_objects
from .pregen.resource import generate_resources

if typing.TYPE_CHECKING:
    from openage.convert.entity_object.conversion.aoc.genie_object_container import GenieObjectContainer


class SWGBCCPregenSubprocessor:
    """
    Creates raw API objects for hardcoded settings in SWGB.
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
        AoCPregenSubprocessor.generate_team_property(full_data_set, pregen_converter_group)
        AoCPregenSubprocessor.generate_entity_types(full_data_set, pregen_converter_group)
        generate_effect_types(full_data_set, pregen_converter_group)
        generate_exchange_objects(full_data_set, pregen_converter_group)
        AoCPregenSubprocessor.generate_formation_types(full_data_set, pregen_converter_group)
        AoCPregenSubprocessor.generate_language_objects(full_data_set, pregen_converter_group)
        AoCPregenSubprocessor.generate_misc_effect_objects(full_data_set, pregen_converter_group)
        # generate_modifiers(gamedata, pregen_converter_group) ??
        # generate_terrain_types(gamedata, pregen_converter_group) TODO: Create terrain types
        AoCPregenSubprocessor.generate_path_types(full_data_set, pregen_converter_group)
        generate_resources(full_data_set, pregen_converter_group)
        AoCPregenSubprocessor.generate_death_condition(full_data_set, pregen_converter_group)
        AoCPregenSubprocessor.generate_garrison_empty_condition(full_data_set, pregen_converter_group)

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

    generate_effect_types = staticmethod(generate_effect_types)
    generate_exchange_objects = staticmethod(generate_exchange_objects)
    generate_resources = staticmethod(generate_resources)
