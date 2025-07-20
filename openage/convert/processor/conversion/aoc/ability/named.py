# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for the Named ability.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef
from .util import create_language_strings

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup
    from .....value_object.conversion.forward_ref import ForwardRef


def named_ability(line: GenieGameEntityGroup) -> ForwardRef:
    """
    Adds the Named ability to a line.

    :param line: Unit/Building line that gets the ability.
    :returns: The forward reference for the ability.
    """
    current_unit = line.get_head_unit()
    current_unit_id = line.get_head_unit_id()
    dataset = line.data

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

    game_entity_name = name_lookup_dict[current_unit_id][0]

    ability_ref = f"{game_entity_name}.Named"
    ability_raw_api_object = RawAPIObject(ability_ref, "Named", dataset.nyan_api_objects)
    ability_raw_api_object.add_raw_parent("engine.ability.type.Named")
    ability_location = ForwardRef(line, game_entity_name)
    ability_raw_api_object.set_location(ability_location)

    # Name
    name_ref = f"{game_entity_name}.Named.{game_entity_name}Name"
    name_raw_api_object = RawAPIObject(name_ref,
                                       f"{game_entity_name}Name",
                                       dataset.nyan_api_objects)
    name_raw_api_object.add_raw_parent("engine.util.language.translated.type.TranslatedString")
    name_location = ForwardRef(line, ability_ref)
    name_raw_api_object.set_location(name_location)

    name_string_id = current_unit["language_dll_name"].value
    translations = create_language_strings(line,
                                           name_string_id,
                                           name_ref,
                                           f"{game_entity_name}Name")
    name_raw_api_object.add_raw_member("translations",
                                       translations,
                                       "engine.util.language.translated.type.TranslatedString")

    name_forward_ref = ForwardRef(line, name_ref)
    ability_raw_api_object.add_raw_member("name", name_forward_ref, "engine.ability.type.Named")
    line.add_raw_api_object(name_raw_api_object)

    # Description
    description_ref = f"{game_entity_name}.Named.{game_entity_name}Description"
    description_raw_api_object = RawAPIObject(description_ref,
                                              f"{game_entity_name}Description",
                                              dataset.nyan_api_objects)
    description_raw_api_object.add_raw_parent(
        "engine.util.language.translated.type.TranslatedMarkupFile")
    description_location = ForwardRef(line, ability_ref)
    description_raw_api_object.set_location(description_location)

    description_raw_api_object.add_raw_member("translations",
                                              [],
                                              "engine.util.language.translated.type.TranslatedMarkupFile")

    description_forward_ref = ForwardRef(line, description_ref)
    ability_raw_api_object.add_raw_member("description",
                                          description_forward_ref,
                                          "engine.ability.type.Named")
    line.add_raw_api_object(description_raw_api_object)

    # Long description
    long_description_ref = f"{game_entity_name}.Named.{game_entity_name}LongDescription"
    long_description_raw_api_object = RawAPIObject(long_description_ref,
                                                   f"{game_entity_name}LongDescription",
                                                   dataset.nyan_api_objects)
    long_description_raw_api_object.add_raw_parent(
        "engine.util.language.translated.type.TranslatedMarkupFile")
    long_description_location = ForwardRef(line, ability_ref)
    long_description_raw_api_object.set_location(long_description_location)

    long_description_raw_api_object.add_raw_member("translations",
                                                   [],
                                                   "engine.util.language.translated.type.TranslatedMarkupFile")

    long_description_forward_ref = ForwardRef(line, long_description_ref)
    ability_raw_api_object.add_raw_member("long_description",
                                          long_description_forward_ref,
                                          "engine.ability.type.Named")
    line.add_raw_api_object(long_description_raw_api_object)

    line.add_raw_api_object(ability_raw_api_object)

    ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

    return ability_forward_ref
