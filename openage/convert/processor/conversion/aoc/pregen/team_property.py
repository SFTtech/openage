# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create team properties for AoC.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject, ConverterObjectGroup
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_object_container import GenieObjectContainer


def generate_team_property(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate the property used in team patches objects.

    :param full_data_set: GenieObjectContainer instance that
                            contains all relevant data for the conversion
                            process.
    :type full_data_set: ...dataformat.aoc.genie_object_container.GenieObjectContainer
    :param pregen_converter_group: GenieObjectGroup instance that stores
                                    pregenerated API objects for referencing with
                                    ForwardRef
    :type pregen_converter_group: ...dataformat.aoc.genie_object_container.GenieObjectGroup
    """
    pregen_nyan_objects = full_data_set.pregen_nyan_objects
    api_objects = full_data_set.nyan_api_objects

    prop_ref_in_modpack = "util.patch.property.types.Team"
    prop_raw_api_object = RawAPIObject(prop_ref_in_modpack,
                                       "Team",
                                       api_objects,
                                       "data/util/patch/property/")
    prop_raw_api_object.set_filename("types")
    prop_raw_api_object.add_raw_parent("engine.util.patch.property.type.Diplomatic")

    pregen_converter_group.add_raw_api_object(prop_raw_api_object)
    pregen_nyan_objects.update({prop_ref_in_modpack: prop_raw_api_object})

    stances = [
        full_data_set.nyan_api_objects["engine.util.diplomatic_stance.type.Self"],
        ForwardRef(pregen_converter_group, "util.diplomatic_stance.types.Friendly")
    ]
    prop_raw_api_object.add_raw_member("stances",
                                       stances,
                                       "engine.util.patch.property.type.Diplomatic")
