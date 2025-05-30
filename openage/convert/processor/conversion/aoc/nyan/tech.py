# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Convert tech groups to openage techs.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.aoc.genie_tech import UnitLineUpgrade
from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef
from ..auxiliary_subprocessor import AoCAuxiliarySubprocessor
from ..tech_subprocessor import AoCTechSubprocessor

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_tech import GenieTechEffectBundleGroup


def tech_group_to_tech(tech_group: GenieTechEffectBundleGroup) -> None:
    """
    Creates raw API objects for a tech group.

    :param tech_group: Tech group that gets converted to a tech.
    :type tech_group: ..dataformat.converter_object.ConverterObjectGroup
    """
    tech_id = tech_group.get_id()

    # Skip Dark Age tech
    if tech_id == 104:
        return

    dataset = tech_group.data

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
    tech_lookup_dict = internal_name_lookups.get_tech_lookups(dataset.game_version)

    # Start with the Tech object
    tech_name = tech_lookup_dict[tech_id][0]
    raw_api_object = RawAPIObject(tech_name, tech_name,
                                  dataset.nyan_api_objects)
    raw_api_object.add_raw_parent("engine.util.tech.Tech")

    if isinstance(tech_group, UnitLineUpgrade):
        unit_line = dataset.unit_lines[tech_group.get_line_id()]
        head_unit_id = unit_line.get_head_unit_id()
        obj_location = f"data/game_entity/generic/{name_lookup_dict[head_unit_id][1]}/"

    else:
        obj_location = f"data/tech/generic/{tech_lookup_dict[tech_id][1]}/"

    raw_api_object.set_location(obj_location)
    raw_api_object.set_filename(tech_lookup_dict[tech_id][1])
    tech_group.add_raw_api_object(raw_api_object)

    # =======================================================================
    # Types
    # =======================================================================
    raw_api_object.add_raw_member("types", [], "engine.util.tech.Tech")

    # =======================================================================
    # Name
    # =======================================================================
    name_ref = f"{tech_name}.{tech_name}Name"
    name_raw_api_object = RawAPIObject(name_ref,
                                       f"{tech_name}Name",
                                       dataset.nyan_api_objects)
    name_raw_api_object.add_raw_parent("engine.util.language.translated.type.TranslatedString")
    name_location = ForwardRef(tech_group, tech_name)
    name_raw_api_object.set_location(name_location)

    name_raw_api_object.add_raw_member("translations",
                                       [],
                                       "engine.util.language.translated.type.TranslatedString")

    name_forward_ref = ForwardRef(tech_group, name_ref)
    raw_api_object.add_raw_member("name", name_forward_ref, "engine.util.tech.Tech")
    tech_group.add_raw_api_object(name_raw_api_object)

    # =======================================================================
    # Description
    # =======================================================================
    description_ref = f"{tech_name}.{tech_name}Description"
    description_raw_api_object = RawAPIObject(description_ref,
                                              f"{tech_name}Description",
                                              dataset.nyan_api_objects)
    description_raw_api_object.add_raw_parent(
        "engine.util.language.translated.type.TranslatedMarkupFile")
    description_location = ForwardRef(tech_group, tech_name)
    description_raw_api_object.set_location(description_location)

    description_raw_api_object.add_raw_member(
        "translations",
        [],
        "engine.util.language.translated.type.TranslatedMarkupFile"
    )

    description_forward_ref = ForwardRef(tech_group, description_ref)
    raw_api_object.add_raw_member("description",
                                  description_forward_ref,
                                  "engine.util.tech.Tech")
    tech_group.add_raw_api_object(description_raw_api_object)

    # =======================================================================
    # Long description
    # =======================================================================
    long_description_ref = f"{tech_name}.{tech_name}LongDescription"
    long_description_raw_api_object = RawAPIObject(long_description_ref,
                                                   f"{tech_name}LongDescription",
                                                   dataset.nyan_api_objects)
    long_description_raw_api_object.add_raw_parent(
        "engine.util.language.translated.type.TranslatedMarkupFile")
    long_description_location = ForwardRef(tech_group, tech_name)
    long_description_raw_api_object.set_location(long_description_location)

    long_description_raw_api_object.add_raw_member(
        "translations",
        [],
        "engine.util.language.translated.type.TranslatedMarkupFile"
    )

    long_description_forward_ref = ForwardRef(tech_group, long_description_ref)
    raw_api_object.add_raw_member("long_description",
                                  long_description_forward_ref,
                                  "engine.util.tech.Tech")
    tech_group.add_raw_api_object(long_description_raw_api_object)

    # =======================================================================
    # Updates
    # =======================================================================
    patches = []
    patches.extend(AoCTechSubprocessor.get_patches(tech_group))
    raw_api_object.add_raw_member("updates", patches, "engine.util.tech.Tech")

    # =======================================================================
    # Misc (Objects that are not used by the tech group itself, but use its values)
    # =======================================================================
    if tech_group.is_researchable():
        AoCAuxiliarySubprocessor.get_researchable_tech(tech_group)
