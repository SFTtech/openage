# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for the Visibility ability.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.aoc.genie_unit import GenieBuildingLineGroup, GenieAmbientGroup
from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup
    from .....value_object.conversion.forward_ref import ForwardRef


def visibility_ability(line: GenieGameEntityGroup) -> ForwardRef:
    """
    Adds the Visibility ability to a line.

    :param line: Unit/Building line that gets the ability.
    :returns: The forward reference for the ability.
    """
    current_unit_id = line.get_head_unit_id()
    dataset = line.data
    api_objects = dataset.nyan_api_objects

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

    game_entity_name = name_lookup_dict[current_unit_id][0]

    ability_ref = f"{game_entity_name}.Visibility"
    ability_raw_api_object = RawAPIObject(ability_ref, "Visibility", dataset.nyan_api_objects)
    ability_raw_api_object.add_raw_parent("engine.ability.type.Visibility")
    ability_location = ForwardRef(line, game_entity_name)
    ability_raw_api_object.set_location(ability_location)

    line.add_raw_api_object(ability_raw_api_object)

    # Units are not visible in fog...
    visible = False

    # ...Buidings and scenery is though
    if isinstance(line, (GenieBuildingLineGroup, GenieAmbientGroup)):
        visible = True

    ability_raw_api_object.add_raw_member("visible_in_fog", visible,
                                          "engine.ability.type.Visibility")

    # Diplomacy settings
    property_ref = f"{ability_ref}.Diplomatic"
    property_raw_api_object = RawAPIObject(property_ref,
                                           "Diplomatic",
                                           dataset.nyan_api_objects)
    property_raw_api_object.add_raw_parent("engine.ability.property.type.Diplomatic")
    property_location = ForwardRef(line, ability_ref)
    property_raw_api_object.set_location(property_location)

    line.add_raw_api_object(property_raw_api_object)

    diplomatic_stances = [
        dataset.nyan_api_objects["engine.util.diplomatic_stance.type.Self"],
        dataset.pregen_nyan_objects["util.diplomatic_stance.types.Friendly"].get_nyan_object(),
        dataset.pregen_nyan_objects["util.diplomatic_stance.types.Neutral"].get_nyan_object(),
        dataset.pregen_nyan_objects["util.diplomatic_stance.types.Enemy"].get_nyan_object(),
        dataset.pregen_nyan_objects["util.diplomatic_stance.types.Gaia"].get_nyan_object()
    ]
    property_raw_api_object.add_raw_member("stances", diplomatic_stances,
                                           "engine.ability.property.type.Diplomatic")

    property_forward_ref = ForwardRef(line, property_ref)

    # Ability properties
    properties = {
        api_objects["engine.ability.property.type.Diplomatic"]: property_forward_ref
    }
    ability_raw_api_object.add_raw_member("properties",
                                          properties,
                                          "engine.ability.Ability")

    # Add another Visibility ability for buildings with construction progress = 0.0
    # It is not returned by this method, but referenced by the Constructable ability
    if isinstance(line, GenieBuildingLineGroup):
        ability_ref = f"{game_entity_name}.VisibilityConstruct0"
        ability_raw_api_object = RawAPIObject(ability_ref,
                                              "VisibilityConstruct0",
                                              dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Visibility")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        line.add_raw_api_object(ability_raw_api_object)

        # The construction site is not visible in fog
        visible = False
        ability_raw_api_object.add_raw_member("visible_in_fog", visible,
                                              "engine.ability.type.Visibility")

        # Diplomacy settings
        property_ref = f"{ability_ref}.Diplomatic"
        property_raw_api_object = RawAPIObject(property_ref,
                                               "Diplomatic",
                                               dataset.nyan_api_objects)
        property_raw_api_object.add_raw_parent("engine.ability.property.type.Diplomatic")
        property_location = ForwardRef(line, ability_ref)
        property_raw_api_object.set_location(property_location)

        line.add_raw_api_object(property_raw_api_object)

        # Only the player and friendly players can see the construction site
        diplomatic_stances = [
            dataset.nyan_api_objects["engine.util.diplomatic_stance.type.Self"],
            dataset.pregen_nyan_objects["util.diplomatic_stance.types.Friendly"].get_nyan_object(
            )
        ]
        property_raw_api_object.add_raw_member("stances", diplomatic_stances,
                                               "engine.ability.property.type.Diplomatic")

        property_forward_ref = ForwardRef(line, property_ref)

        # Ability properties
        properties = {
            api_objects["engine.ability.property.type.Diplomatic"]: property_forward_ref
        }
        ability_raw_api_object.add_raw_member("properties",
                                              properties,
                                              "engine.ability.Ability")

    ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

    return ability_forward_ref
