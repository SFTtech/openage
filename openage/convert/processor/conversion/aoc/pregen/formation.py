# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create formation types for AoC.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject, ConverterObjectGroup
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_object_container import GenieObjectContainer

FORMATION_PARENT = "engine.util.formation.Formation"
SUBFORMATION_PARENT = "engine.util.formation.Subformation"
FORMATION_LOCATION = "data/util/formation/"
SUBFORMATION_LOCATION = "data/util/formation/"


def generate_formation_objects(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate Formation and Subformation objects.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    _generate_formation_types(full_data_set, pregen_converter_group)
    _generate_subformation_types(full_data_set, pregen_converter_group)


def _generate_formation_types(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate formation types for AoC.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    pregen_nyan_objects = full_data_set.pregen_nyan_objects
    api_objects = full_data_set.nyan_api_objects

    # =======================================================================
    # Line formation
    # =======================================================================
    formation_ref_in_modpack = "util.formation.types.Line"
    formation_raw_api_object = RawAPIObject(formation_ref_in_modpack,
                                            "Line",
                                            api_objects,
                                            FORMATION_LOCATION)
    formation_raw_api_object.set_filename("types")
    formation_raw_api_object.add_raw_parent(FORMATION_PARENT)

    subformations = [
        ForwardRef(pregen_converter_group, "util.formation.subformation.types.Cavalry"),
        ForwardRef(pregen_converter_group, "util.formation.subformation.types.Infantry"),
        ForwardRef(pregen_converter_group, "util.formation.subformation.types.Ranged"),
        ForwardRef(pregen_converter_group, "util.formation.subformation.types.Siege"),
        ForwardRef(pregen_converter_group, "util.formation.subformation.types.Support"),
    ]
    formation_raw_api_object.add_raw_member("subformations",
                                            subformations,
                                            FORMATION_PARENT)

    pregen_converter_group.add_raw_api_object(formation_raw_api_object)
    pregen_nyan_objects.update({formation_ref_in_modpack: formation_raw_api_object})

    # =======================================================================
    # Staggered formation
    # =======================================================================
    formation_ref_in_modpack = "util.formation.types.Staggered"
    formation_raw_api_object = RawAPIObject(formation_ref_in_modpack,
                                            "Staggered",
                                            api_objects,
                                            FORMATION_LOCATION)
    formation_raw_api_object.set_filename("types")
    formation_raw_api_object.add_raw_parent(FORMATION_PARENT)

    subformations = [
        ForwardRef(pregen_converter_group, "util.formation.subformation.types.Cavalry"),
        ForwardRef(pregen_converter_group, "util.formation.subformation.types.Infantry"),
        ForwardRef(pregen_converter_group, "util.formation.subformation.types.Ranged"),
        ForwardRef(pregen_converter_group, "util.formation.subformation.types.Siege"),
        ForwardRef(pregen_converter_group, "util.formation.subformation.types.Support"),
    ]
    formation_raw_api_object.add_raw_member("subformations",
                                            subformations,
                                            FORMATION_PARENT)

    pregen_converter_group.add_raw_api_object(formation_raw_api_object)
    pregen_nyan_objects.update({formation_ref_in_modpack: formation_raw_api_object})

    # =======================================================================
    # Box formation
    # =======================================================================
    formation_ref_in_modpack = "util.formation.types.Box"
    formation_raw_api_object = RawAPIObject(formation_ref_in_modpack,
                                            "Box",
                                            api_objects,
                                            FORMATION_LOCATION)
    formation_raw_api_object.set_filename("types")
    formation_raw_api_object.add_raw_parent(FORMATION_PARENT)

    subformations = [
        ForwardRef(pregen_converter_group, "util.formation.subformation.types.Cavalry"),
        ForwardRef(pregen_converter_group, "util.formation.subformation.types.Infantry"),
        ForwardRef(pregen_converter_group, "util.formation.subformation.types.Ranged"),
        ForwardRef(pregen_converter_group, "util.formation.subformation.types.Siege"),
        ForwardRef(pregen_converter_group, "util.formation.subformation.types.Support"),
    ]
    formation_raw_api_object.add_raw_member("subformations",
                                            subformations,
                                            FORMATION_PARENT)

    pregen_converter_group.add_raw_api_object(formation_raw_api_object)
    pregen_nyan_objects.update({formation_ref_in_modpack: formation_raw_api_object})

    # =======================================================================
    # Flank formation
    # =======================================================================
    formation_ref_in_modpack = "util.formation.types.Flank"
    formation_raw_api_object = RawAPIObject(formation_ref_in_modpack,
                                            "Flank",
                                            api_objects,
                                            FORMATION_LOCATION)
    formation_raw_api_object.set_filename("types")
    formation_raw_api_object.add_raw_parent(FORMATION_PARENT)

    subformations = [
        ForwardRef(pregen_converter_group, "util.formation.subformation.types.Cavalry"),
        ForwardRef(pregen_converter_group, "util.formation.subformation.types.Infantry"),
        ForwardRef(pregen_converter_group, "util.formation.subformation.types.Ranged"),
        ForwardRef(pregen_converter_group, "util.formation.subformation.types.Siege"),
        ForwardRef(pregen_converter_group, "util.formation.subformation.types.Support"),
    ]
    formation_raw_api_object.add_raw_member("subformations",
                                            subformations,
                                            FORMATION_PARENT)

    pregen_converter_group.add_raw_api_object(formation_raw_api_object)
    pregen_nyan_objects.update({formation_ref_in_modpack: formation_raw_api_object})


def _generate_subformation_types(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate subformation types for AoC.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    pregen_nyan_objects = full_data_set.pregen_nyan_objects
    api_objects = full_data_set.nyan_api_objects

    # =======================================================================
    # Cavalry subformation
    # =======================================================================
    subformation_ref_in_modpack = "util.formation.subformation.types.Cavalry"
    subformation_raw_api_object = RawAPIObject(subformation_ref_in_modpack,
                                               "Cavalry",
                                               api_objects,
                                               SUBFORMATION_LOCATION)
    subformation_raw_api_object.set_filename("subformations")
    subformation_raw_api_object.add_raw_parent(SUBFORMATION_PARENT)

    subformation_raw_api_object.add_raw_member("ordering_priority",
                                               5,
                                               SUBFORMATION_PARENT)

    pregen_converter_group.add_raw_api_object(subformation_raw_api_object)
    pregen_nyan_objects.update({subformation_ref_in_modpack: subformation_raw_api_object})

    # =======================================================================
    # Infantry subformation
    # =======================================================================
    subformation_ref_in_modpack = "util.formation.subformation.types.Infantry"
    subformation_raw_api_object = RawAPIObject(subformation_ref_in_modpack,
                                               "Infantry",
                                               api_objects,
                                               SUBFORMATION_LOCATION)
    subformation_raw_api_object.set_filename("subformations")
    subformation_raw_api_object.add_raw_parent(SUBFORMATION_PARENT)

    subformation_raw_api_object.add_raw_member("ordering_priority",
                                               4,
                                               SUBFORMATION_PARENT)

    pregen_converter_group.add_raw_api_object(subformation_raw_api_object)
    pregen_nyan_objects.update({subformation_ref_in_modpack: subformation_raw_api_object})

    # =======================================================================
    # Ranged subformation
    # =======================================================================
    subformation_ref_in_modpack = "util.formation.subformation.types.Ranged"
    subformation_raw_api_object = RawAPIObject(subformation_ref_in_modpack,
                                               "Ranged",
                                               api_objects,
                                               SUBFORMATION_LOCATION)
    subformation_raw_api_object.set_filename("subformations")
    subformation_raw_api_object.add_raw_parent(SUBFORMATION_PARENT)

    subformation_raw_api_object.add_raw_member("ordering_priority",
                                               3,
                                               SUBFORMATION_PARENT)

    pregen_converter_group.add_raw_api_object(subformation_raw_api_object)
    pregen_nyan_objects.update({subformation_ref_in_modpack: subformation_raw_api_object})

    # =======================================================================
    # Siege subformation
    # =======================================================================
    subformation_ref_in_modpack = "util.formation.subformation.types.Siege"
    subformation_raw_api_object = RawAPIObject(subformation_ref_in_modpack,
                                               "Siege",
                                               api_objects,
                                               SUBFORMATION_LOCATION)
    subformation_raw_api_object.set_filename("subformations")
    subformation_raw_api_object.add_raw_parent(SUBFORMATION_PARENT)

    subformation_raw_api_object.add_raw_member("ordering_priority",
                                               2,
                                               SUBFORMATION_PARENT)

    pregen_converter_group.add_raw_api_object(subformation_raw_api_object)
    pregen_nyan_objects.update({subformation_ref_in_modpack: subformation_raw_api_object})

    # =======================================================================
    # Support subformation
    # =======================================================================
    subformation_ref_in_modpack = "util.formation.subformation.types.Support"
    subformation_raw_api_object = RawAPIObject(subformation_ref_in_modpack,
                                               "Support",
                                               api_objects,
                                               SUBFORMATION_LOCATION)
    subformation_raw_api_object.set_filename("subformations")
    subformation_raw_api_object.add_raw_parent(SUBFORMATION_PARENT)

    subformation_raw_api_object.add_raw_member("ordering_priority",
                                               1,
                                               SUBFORMATION_PARENT)

    pregen_converter_group.add_raw_api_object(subformation_raw_api_object)
    pregen_nyan_objects.update({subformation_ref_in_modpack: subformation_raw_api_object})
