# Copyright 2020-2025 the openage authors. See copying.md for legal info.

"""
Creates nyan objects for things that are hardcoded into the Genie Engine,
but configurable in openage. E.g. HP.
"""
from __future__ import annotations
import typing

from ....entity_object.conversion.converter_object import ConverterObjectGroup
from .pregen.activity import generate_activities
from .pregen.attribute import generate_attributes
from .pregen.diplomatic import generate_diplomatic_stances
from .pregen.condition import generate_death_condition, generate_garrison_empty_condition
from .pregen.resource import generate_resources
from .pregen.team_property import generate_team_property
from .pregen.entity import generate_entity_types
from .pregen.effect import generate_effect_types, generate_misc_effect_objects
from .pregen.exchange import generate_exchange_objects
from .pregen.formation import generate_formation_objects
from .pregen.language import generate_language_objects
from .pregen.modifier import generate_modifiers
from .pregen.terrain import generate_terrain_types
from .pregen.path import generate_path_types

if typing.TYPE_CHECKING:
    from ....entity_object.conversion.aoc.genie_object_container import GenieObjectContainer


class AoCPregenSubprocessor:
    """
    Creates raw API objects for hardcoded settings in AoC.
    """

    @classmethod
    def generate(cls, full_data_set: GenieObjectContainer) -> None:
        """
        Create nyan objects for hardcoded properties.
        """
        # Stores pregenerated raw API objects as a container
        pregen_converter_group = ConverterObjectGroup("pregen")

        generate_activities(full_data_set, pregen_converter_group)
        generate_attributes(full_data_set, pregen_converter_group)
        generate_diplomatic_stances(full_data_set, pregen_converter_group)
        generate_team_property(full_data_set, pregen_converter_group)
        generate_entity_types(full_data_set, pregen_converter_group)
        generate_effect_types(full_data_set, pregen_converter_group)
        generate_exchange_objects(full_data_set, pregen_converter_group)
        generate_formation_objects(full_data_set, pregen_converter_group)
        generate_language_objects(full_data_set, pregen_converter_group)
        generate_misc_effect_objects(full_data_set, pregen_converter_group)
        generate_modifiers(full_data_set, pregen_converter_group)
        generate_terrain_types(full_data_set, pregen_converter_group)
        generate_path_types(full_data_set, pregen_converter_group)
        generate_resources(full_data_set, pregen_converter_group)
        generate_death_condition(full_data_set, pregen_converter_group)
        generate_garrison_empty_condition(full_data_set, pregen_converter_group)

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

    generate_activities = staticmethod(generate_activities)
    generate_attributes = staticmethod(generate_attributes)
    generate_death_condition = staticmethod(generate_death_condition)
    generate_garrison_empty_condition = staticmethod(generate_garrison_empty_condition)
    generate_resources = staticmethod(generate_resources)
    generate_diplomatic_stances = staticmethod(generate_diplomatic_stances)
    generate_team_property = staticmethod(generate_team_property)
    generate_entity_types = staticmethod(generate_entity_types)
    generate_effect_types = staticmethod(generate_effect_types)
    generate_exchange_objects = staticmethod(generate_exchange_objects)
    generate_formation_types = staticmethod(generate_formation_objects)
    generate_language_objects = staticmethod(generate_language_objects)
    generate_modifiers = staticmethod(generate_modifiers)
    generate_terrain_types = staticmethod(generate_terrain_types)
    generate_path_types = staticmethod(generate_path_types)
    generate_misc_effect_objects = staticmethod(generate_misc_effect_objects)
