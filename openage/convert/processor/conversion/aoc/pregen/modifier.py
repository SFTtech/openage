# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create modifiers for AoC.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject, ConverterObjectGroup
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_object_container import GenieObjectContainer

MODIFIER_PARENT = "engine.modifier.Modifier"
MPROP_PARENT = "engine.modifier.property.type.Multiplier"
FLYOVER_PARENT = "engine.modifier.effect.flat_attribute_change.type.Flyover"
ELEVATION_DIFF_HIGH_PARENT = (
    "engine.modifier.effect.flat_attribute_change.type.ElevationDifferenceHigh"
)
ELEVATION_DIFF_LOW_PARENT = (
    "engine.modifier.effect.flat_attribute_change.type.ElevationDifferenceLow"
)
ELEVATION_DIFF_LOCATION = "data/util/modifier/elevation_difference/"
FLYOVER_LOCATION = "data/util/modifier/flyover_cliff/"


def generate_modifiers(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate standard modifiers.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    _generate_flyover_cliff_modifier(full_data_set, pregen_converter_group)
    _generate_elevation_difference_modifiers(full_data_set, pregen_converter_group)


def _generate_flyover_cliff_modifier(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate the flyover cliff modifier.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    pregen_nyan_objects = full_data_set.pregen_nyan_objects
    api_objects = full_data_set.nyan_api_objects

    modifier_ref_in_modpack = "util.modifier.flyover_cliff.AttackFlyover"
    modifier_raw_api_object = RawAPIObject(modifier_ref_in_modpack,
                                           "AttackFlyover", api_objects,
                                           FLYOVER_LOCATION)
    modifier_raw_api_object.set_filename("flyover_cliff")
    modifier_raw_api_object.add_raw_parent(FLYOVER_PARENT)

    pregen_converter_group.add_raw_api_object(modifier_raw_api_object)
    pregen_nyan_objects.update({modifier_ref_in_modpack: modifier_raw_api_object})

    # Relative angle to cliff must not be smaller than 90°
    modifier_raw_api_object.add_raw_member("relative_angle",
                                           90,
                                           FLYOVER_PARENT)

    # Affects all cliffs
    types = [ForwardRef(pregen_converter_group, "util.game_entity_type.types.Cliff")]
    modifier_raw_api_object.add_raw_member("flyover_types",
                                           types,
                                           FLYOVER_PARENT)
    modifier_raw_api_object.add_raw_member("blacklisted_entities",
                                           [],
                                           FLYOVER_PARENT)

    # Multiplier property: Increases effect value by 25%
    # --------------------------------------------------
    prop_ref_in_modpack = "util.modifier.flyover_cliff.AttackFlyover.Multiplier"
    prop_raw_api_object = RawAPIObject(prop_ref_in_modpack,
                                       "Multiplier", api_objects,
                                       FLYOVER_LOCATION)
    prop_location = ForwardRef(pregen_converter_group, modifier_ref_in_modpack)
    prop_raw_api_object.set_location(prop_location)
    prop_raw_api_object.add_raw_parent(MPROP_PARENT)

    pregen_converter_group.add_raw_api_object(prop_raw_api_object)
    pregen_nyan_objects.update({prop_ref_in_modpack: prop_raw_api_object})

    prop_raw_api_object.add_raw_member("multiplier",
                                       1.25,
                                       MPROP_PARENT)
    # --------------------------------------------------
    # Assign property to modifier
    prop_forward_ref = ForwardRef(pregen_converter_group, prop_ref_in_modpack)
    properties = {api_objects[MPROP_PARENT]: prop_forward_ref}
    modifier_raw_api_object.add_raw_member("properties",
                                           properties,
                                           MODIFIER_PARENT)


def _generate_elevation_difference_modifiers(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate elevation difference modifiers.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    pregen_nyan_objects = full_data_set.pregen_nyan_objects
    api_objects = full_data_set.nyan_api_objects

    # =======================================================================
    # Elevation difference effect multiplier (higher unit)
    # =======================================================================
    modifier_ref_in_modpack = "util.modifier.elevation_difference.AttackHigh"
    modifier_raw_api_object = RawAPIObject(modifier_ref_in_modpack,
                                           "AttackHigh", api_objects,
                                           ELEVATION_DIFF_LOCATION)
    modifier_raw_api_object.set_filename("elevation_difference")
    modifier_raw_api_object.add_raw_parent(ELEVATION_DIFF_HIGH_PARENT)

    pregen_converter_group.add_raw_api_object(modifier_raw_api_object)
    pregen_nyan_objects.update({modifier_ref_in_modpack: modifier_raw_api_object})

    # Multiplier property: Increases effect value to 125%
    # --------------------------------------------------
    prop_ref_in_modpack = "util.modifier.elevation_difference.AttackHigh.Multiplier"
    prop_raw_api_object = RawAPIObject(prop_ref_in_modpack,
                                       "Multiplier", api_objects,
                                       ELEVATION_DIFF_LOCATION)
    prop_location = ForwardRef(pregen_converter_group, modifier_ref_in_modpack)
    prop_raw_api_object.set_location(prop_location)
    prop_raw_api_object.add_raw_parent(MPROP_PARENT)

    pregen_converter_group.add_raw_api_object(prop_raw_api_object)
    pregen_nyan_objects.update({prop_ref_in_modpack: prop_raw_api_object})

    prop_raw_api_object.add_raw_member("multiplier",
                                       1.25,
                                       MPROP_PARENT)
    # --------------------------------------------------
    # Assign property to modifier
    prop_forward_ref = ForwardRef(pregen_converter_group, prop_ref_in_modpack)
    properties = {api_objects[MPROP_PARENT]: prop_forward_ref}
    modifier_raw_api_object.add_raw_member("properties",
                                           properties,
                                           MODIFIER_PARENT)

    # =======================================================================
    # Elevation difference effect multiplier (lower unit)
    # =======================================================================
    modifier_ref_in_modpack = "util.modifier.elevation_difference.AttackLow"
    modifier_raw_api_object = RawAPIObject(modifier_ref_in_modpack,
                                           "AttackLow", api_objects,
                                           ELEVATION_DIFF_LOCATION)
    modifier_raw_api_object.set_filename("elevation_difference")
    modifier_raw_api_object.add_raw_parent(ELEVATION_DIFF_LOW_PARENT)

    pregen_converter_group.add_raw_api_object(modifier_raw_api_object)
    pregen_nyan_objects.update({modifier_ref_in_modpack: modifier_raw_api_object})

    # Multiplier property: Decreases effect value to 75%
    # --------------------------------------------------
    prop_ref_in_modpack = "util.modifier.elevation_difference.AttackLow.Multiplier"
    prop_raw_api_object = RawAPIObject(prop_ref_in_modpack,
                                       "Multiplier", api_objects,
                                       ELEVATION_DIFF_LOCATION)
    prop_location = ForwardRef(pregen_converter_group, modifier_ref_in_modpack)
    prop_raw_api_object.set_location(prop_location)
    prop_raw_api_object.add_raw_parent(MPROP_PARENT)

    pregen_converter_group.add_raw_api_object(prop_raw_api_object)
    pregen_nyan_objects.update({prop_ref_in_modpack: prop_raw_api_object})

    prop_raw_api_object.add_raw_member("multiplier",
                                       1.25,
                                       MPROP_PARENT)
    # --------------------------------------------------
    # Assign property to modifier
    prop_forward_ref = ForwardRef(pregen_converter_group, prop_ref_in_modpack)
    properties = {api_objects[MPROP_PARENT]: prop_forward_ref}
    modifier_raw_api_object.add_raw_member("properties",
                                           properties,
                                           MODIFIER_PARENT)
