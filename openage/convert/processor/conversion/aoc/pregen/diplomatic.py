# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create diplomatic stances for AoC.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject, ConverterObjectGroup

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_object_container import GenieObjectContainer

STANCE_PARENT = "engine.util.diplomatic_stance.DiplomaticStance"
STANCE_LOCATION = "data/util/diplomatic_stance/"


def generate_diplomatic_stances(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate DiplomaticStance objects.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    _generate_enemy_diplomatic_stance(full_data_set, pregen_converter_group)
    _generate_neutral_diplomatic_stance(full_data_set, pregen_converter_group)
    _generate_friendly_diplomatic_stance(full_data_set, pregen_converter_group)
    _generate_gaia_diplomatic_stance(full_data_set, pregen_converter_group)


def _generate_enemy_diplomatic_stance(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate the Enemy diplomatic stance.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    pregen_nyan_objects = full_data_set.pregen_nyan_objects
    api_objects = full_data_set.nyan_api_objects

    enemy_ref_in_modpack = "util.diplomatic_stance.types.Enemy"
    enemy_raw_api_object = RawAPIObject(enemy_ref_in_modpack,
                                        "Enemy", api_objects,
                                        STANCE_LOCATION)
    enemy_raw_api_object.set_filename("types")
    enemy_raw_api_object.add_raw_parent(STANCE_PARENT)

    pregen_converter_group.add_raw_api_object(enemy_raw_api_object)
    pregen_nyan_objects.update({enemy_ref_in_modpack: enemy_raw_api_object})


def _generate_neutral_diplomatic_stance(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate the Neutral diplomatic stance.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    pregen_nyan_objects = full_data_set.pregen_nyan_objects
    api_objects = full_data_set.nyan_api_objects

    neutral_ref_in_modpack = "util.diplomatic_stance.types.Neutral"
    neutral_raw_api_object = RawAPIObject(neutral_ref_in_modpack,
                                          "Neutral", api_objects,
                                          STANCE_LOCATION)
    neutral_raw_api_object.set_filename("types")
    neutral_raw_api_object.add_raw_parent(STANCE_PARENT)

    pregen_converter_group.add_raw_api_object(neutral_raw_api_object)
    pregen_nyan_objects.update({neutral_ref_in_modpack: neutral_raw_api_object})


def _generate_friendly_diplomatic_stance(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate the Friendly diplomatic stance.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    pregen_nyan_objects = full_data_set.pregen_nyan_objects
    api_objects = full_data_set.nyan_api_objects

    friendly_ref_in_modpack = "util.diplomatic_stance.types.Friendly"
    friendly_raw_api_object = RawAPIObject(friendly_ref_in_modpack,
                                           "Friendly", api_objects,
                                           STANCE_LOCATION)
    friendly_raw_api_object.set_filename("types")
    friendly_raw_api_object.add_raw_parent(STANCE_PARENT)

    pregen_converter_group.add_raw_api_object(friendly_raw_api_object)
    pregen_nyan_objects.update({friendly_ref_in_modpack: friendly_raw_api_object})


def _generate_gaia_diplomatic_stance(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate the Gaia diplomatic stance.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    pregen_nyan_objects = full_data_set.pregen_nyan_objects
    api_objects = full_data_set.nyan_api_objects

    gaia_ref_in_modpack = "util.diplomatic_stance.types.Gaia"
    gaia_raw_api_object = RawAPIObject(gaia_ref_in_modpack,
                                       "Gaia", api_objects,
                                       STANCE_LOCATION)
    gaia_raw_api_object.set_filename("types")
    gaia_raw_api_object.add_raw_parent(STANCE_PARENT)

    pregen_converter_group.add_raw_api_object(gaia_raw_api_object)
    pregen_nyan_objects.update({gaia_ref_in_modpack: gaia_raw_api_object})
