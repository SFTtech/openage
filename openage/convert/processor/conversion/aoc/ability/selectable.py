# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for the Selectable ability.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.aoc.genie_unit import GenieUnitLineGroup
from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef
from .util import create_sound

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup
    from .....value_object.conversion.forward_ref import ForwardRef


def selectable_ability(line: GenieGameEntityGroup) -> ForwardRef:
    """
    Adds Selectable abilities to a line. Units will get two of these,
    one Rectangle box for the Self stance and one MatchToSprite box
    for other stances.

    :param line: Unit/Building line that gets the abilities.
    :returns: The forward reference for the abilities.
    """
    current_unit = line.get_head_unit()
    current_unit_id = line.get_head_unit_id()
    dataset = line.data
    api_objects = dataset.nyan_api_objects

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

    game_entity_name = name_lookup_dict[current_unit_id][0]

    ability_refs = (f"{game_entity_name}.Selectable",)
    ability_names = ("Selectable",)

    if isinstance(line, GenieUnitLineGroup):
        ability_refs = (f"{game_entity_name}.SelectableOthers",
                        f"{game_entity_name}.SelectableSelf")
        ability_names = ("SelectableOthers",
                         "SelectableSelf")

    abilities = []

    # First box (MatchToSprite)
    ability_ref = ability_refs[0]
    ability_name = ability_names[0]

    ability_raw_api_object = RawAPIObject(ability_ref, ability_name, dataset.nyan_api_objects)
    ability_raw_api_object.add_raw_parent("engine.ability.type.Selectable")
    ability_location = ForwardRef(line, game_entity_name)
    ability_raw_api_object.set_location(ability_location)

    # Selection box
    box_ref = dataset.nyan_api_objects["engine.util.selection_box.type.MatchToSprite"]
    ability_raw_api_object.add_raw_member("selection_box",
                                          box_ref,
                                          "engine.ability.type.Selectable")

    # Ability properties
    properties = {}

    # Diplomacy setting (for units)
    if isinstance(line, GenieUnitLineGroup):
        property_ref = f"{ability_ref}.Diplomatic"
        property_raw_api_object = RawAPIObject(property_ref,
                                               "Diplomatic",
                                               dataset.nyan_api_objects)
        property_raw_api_object.add_raw_parent("engine.ability.property.type.Diplomatic")
        property_location = ForwardRef(line, ability_ref)
        property_raw_api_object.set_location(property_location)

        line.add_raw_api_object(property_raw_api_object)

        stances = [
            dataset.pregen_nyan_objects["util.diplomatic_stance.types.Enemy"].get_nyan_object(),
            dataset.pregen_nyan_objects["util.diplomatic_stance.types.Neutral"].get_nyan_object(
            ),
            dataset.pregen_nyan_objects["util.diplomatic_stance.types.Friendly"].get_nyan_object(
            )
        ]
        property_raw_api_object.add_raw_member("stances",
                                               stances,
                                               "engine.ability.property.type.Diplomatic")

        property_forward_ref = ForwardRef(line, property_ref)
        properties.update({
            api_objects["engine.ability.property.type.Diplomatic"]: property_forward_ref
        })

        ability_raw_api_object.add_raw_member("properties",
                                              properties,
                                              "engine.ability.Ability")
    else:
        ability_comm_sound_id = current_unit["selection_sound_id"].value
        if ability_comm_sound_id > -1:
            property_ref = f"{ability_ref}.CommandSound"
            property_raw_api_object = RawAPIObject(property_ref,
                                                   "CommandSound",
                                                   dataset.nyan_api_objects)
            property_raw_api_object.add_raw_parent("engine.ability.property.type.CommandSound")
            property_location = ForwardRef(line, ability_ref)
            property_raw_api_object.set_location(property_location)

            line.add_raw_api_object(property_raw_api_object)

            sounds_set = []
            sound_forward_ref = create_sound(line,
                                             ability_comm_sound_id,
                                             property_ref,
                                             ability_name,
                                             "command_")
            sounds_set.append(sound_forward_ref)
            property_raw_api_object.add_raw_member("sounds",
                                                   sounds_set,
                                                   "engine.ability.property.type.CommandSound")

            property_forward_ref = ForwardRef(line, property_ref)
            properties.update({
                api_objects["engine.ability.property.type.CommandSound"]: property_forward_ref
            })
            ability_raw_api_object.add_raw_member("properties",
                                                  properties,
                                                  "engine.ability.Ability")

    line.add_raw_api_object(ability_raw_api_object)

    ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

    abilities.append(ability_forward_ref)

    if not isinstance(line, GenieUnitLineGroup):
        return abilities

    # Second box (Rectangle)
    ability_ref = ability_refs[1]
    ability_name = ability_names[1]

    ability_raw_api_object = RawAPIObject(ability_ref,
                                          ability_name,
                                          dataset.nyan_api_objects)
    ability_raw_api_object.add_raw_parent("engine.ability.type.Selectable")
    ability_location = ForwardRef(line, game_entity_name)
    ability_raw_api_object.set_location(ability_location)

    # Ability properties
    properties = {}

    # Command Sound
    ability_comm_sound_id = current_unit["selection_sound_id"].value
    if ability_comm_sound_id > -1:
        property_ref = f"{ability_ref}.CommandSound"
        property_raw_api_object = RawAPIObject(property_ref,
                                               "CommandSound",
                                               dataset.nyan_api_objects)
        property_raw_api_object.add_raw_parent("engine.ability.property.type.CommandSound")
        property_location = ForwardRef(line, ability_ref)
        property_raw_api_object.set_location(property_location)

        line.add_raw_api_object(property_raw_api_object)

        sounds_set = []
        sound_forward_ref = create_sound(line,
                                         ability_comm_sound_id,
                                         property_ref,
                                         ability_name,
                                         "command_")
        sounds_set.append(sound_forward_ref)
        property_raw_api_object.add_raw_member("sounds",
                                               sounds_set,
                                               "engine.ability.property.type.CommandSound")

        property_forward_ref = ForwardRef(line, property_ref)
        properties.update({
            api_objects["engine.ability.property.type.CommandSound"]: property_forward_ref
        })

    # Diplomacy settings
    property_ref = f"{ability_ref}.Diplomatic"
    property_raw_api_object = RawAPIObject(property_ref,
                                           "Diplomatic",
                                           dataset.nyan_api_objects)
    property_raw_api_object.add_raw_parent("engine.ability.property.type.Diplomatic")
    property_location = ForwardRef(line, ability_ref)
    property_raw_api_object.set_location(property_location)

    line.add_raw_api_object(property_raw_api_object)

    diplomatic_stances = [dataset.nyan_api_objects["engine.util.diplomatic_stance.type.Self"]]
    property_raw_api_object.add_raw_member("stances", diplomatic_stances,
                                           "engine.ability.property.type.Diplomatic")

    property_forward_ref = ForwardRef(line, property_ref)
    properties.update({
        api_objects["engine.ability.property.type.Diplomatic"]: property_forward_ref
    })

    ability_raw_api_object.add_raw_member("properties",
                                          properties,
                                          "engine.ability.Ability")

    # Selection box
    box_name = f"{game_entity_name}.SelectableSelf.Rectangle"
    box_raw_api_object = RawAPIObject(box_name, "Rectangle", dataset.nyan_api_objects)
    box_raw_api_object.add_raw_parent("engine.util.selection_box.type.Rectangle")
    box_location = ForwardRef(line, ability_ref)
    box_raw_api_object.set_location(box_location)

    width = current_unit["selection_shape_x"].value
    box_raw_api_object.add_raw_member("width",
                                      width,
                                      "engine.util.selection_box.type.Rectangle")

    height = current_unit["selection_shape_y"].value
    box_raw_api_object.add_raw_member("height",
                                      height,
                                      "engine.util.selection_box.type.Rectangle")

    line.add_raw_api_object(box_raw_api_object)

    box_forward_ref = ForwardRef(line, box_name)
    ability_raw_api_object.add_raw_member("selection_box",
                                          box_forward_ref,
                                          "engine.ability.type.Selectable")

    line.add_raw_api_object(ability_raw_api_object)

    ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

    abilities.append(ability_forward_ref)

    return abilities
