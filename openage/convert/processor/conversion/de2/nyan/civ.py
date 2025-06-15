# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Convert civ groups to openage player setups.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef
from ...aoc.civ_subprocessor import AoCCivSubprocessor
from ..civ_subprocessor import DE2CivSubprocessor

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_civ import GenieCivilizationGroup


@staticmethod
def civ_group_to_civ(civ_group: GenieCivilizationGroup) -> None:
    """
    Creates raw API objects for a civ group.

    :param civ_group: Terrain group that gets converted to a tech.
    """
    civ_id = civ_group.get_id()

    dataset = civ_group.data

    civ_lookup_dict = internal_name_lookups.get_civ_lookups(dataset.game_version)

    # Start with the Tech object
    tech_name = civ_lookup_dict[civ_id][0]
    raw_api_object = RawAPIObject(tech_name, tech_name,
                                  dataset.nyan_api_objects)
    raw_api_object.add_raw_parent("engine.util.setup.PlayerSetup")

    obj_location = f"data/civ/{civ_lookup_dict[civ_id][1]}/"

    raw_api_object.set_location(obj_location)
    raw_api_object.set_filename(civ_lookup_dict[civ_id][1])
    civ_group.add_raw_api_object(raw_api_object)

    # =======================================================================
    # Name
    # =======================================================================
    name_ref = f"{tech_name}.{tech_name}Name"
    name_raw_api_object = RawAPIObject(name_ref,
                                       f"{tech_name}Name",
                                       dataset.nyan_api_objects)
    name_raw_api_object.add_raw_parent("engine.util.language.translated.type.TranslatedString")
    name_location = ForwardRef(civ_group, tech_name)
    name_raw_api_object.set_location(name_location)

    name_raw_api_object.add_raw_member("translations",
                                       [],
                                       "engine.util.language.translated.type.TranslatedString")

    name_forward_ref = ForwardRef(civ_group, name_ref)
    raw_api_object.add_raw_member("name", name_forward_ref, "engine.util.setup.PlayerSetup")
    civ_group.add_raw_api_object(name_raw_api_object)

    # =======================================================================
    # Description
    # =======================================================================
    description_ref = f"{tech_name}.{tech_name}Description"
    description_raw_api_object = RawAPIObject(description_ref,
                                              f"{tech_name}Description",
                                              dataset.nyan_api_objects)
    description_raw_api_object.add_raw_parent(
        "engine.util.language.translated.type.TranslatedMarkupFile")
    description_location = ForwardRef(civ_group, tech_name)
    description_raw_api_object.set_location(description_location)

    description_raw_api_object.add_raw_member(
        "translations",
        [],
        "engine.util.language.translated.type.TranslatedMarkupFile"
    )

    description_forward_ref = ForwardRef(civ_group, description_ref)
    raw_api_object.add_raw_member("description",
                                  description_forward_ref,
                                  "engine.util.setup.PlayerSetup")
    civ_group.add_raw_api_object(description_raw_api_object)

    # =======================================================================
    # Long description
    # =======================================================================
    long_description_ref = f"{tech_name}.{tech_name}LongDescription"
    long_description_raw_api_object = RawAPIObject(long_description_ref,
                                                   f"{tech_name}LongDescription",
                                                   dataset.nyan_api_objects)
    long_description_raw_api_object.add_raw_parent(
        "engine.util.language.translated.type.TranslatedMarkupFile")
    long_description_location = ForwardRef(civ_group, tech_name)
    long_description_raw_api_object.set_location(long_description_location)

    long_description_raw_api_object.add_raw_member(
        "translations",
        [],
        "engine.util.language.translated.type.TranslatedMarkupFile"
    )

    long_description_forward_ref = ForwardRef(civ_group, long_description_ref)
    raw_api_object.add_raw_member("long_description",
                                  long_description_forward_ref,
                                  "engine.util.setup.PlayerSetup")
    civ_group.add_raw_api_object(long_description_raw_api_object)

    # =======================================================================
    # TODO: Leader names
    # =======================================================================
    raw_api_object.add_raw_member("leader_names",
                                  [],
                                  "engine.util.setup.PlayerSetup")

    # =======================================================================
    # Modifiers
    # =======================================================================
    modifiers = AoCCivSubprocessor.get_modifiers(civ_group)
    raw_api_object.add_raw_member("modifiers",
                                  modifiers,
                                  "engine.util.setup.PlayerSetup")

    # =======================================================================
    # Starting resources
    # =======================================================================
    resource_amounts = AoCCivSubprocessor.get_starting_resources(civ_group)
    raw_api_object.add_raw_member("starting_resources",
                                  resource_amounts,
                                  "engine.util.setup.PlayerSetup")

    # =======================================================================
    # Game setup
    # =======================================================================
    game_setup = DE2CivSubprocessor.get_civ_setup(civ_group)
    raw_api_object.add_raw_member("game_setup",
                                  game_setup,
                                  "engine.util.setup.PlayerSetup")
