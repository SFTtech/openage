# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for the LineOfSight ability.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.aoc.genie_unit import GenieBuildingLineGroup
from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup
    from .....value_object.conversion.forward_ref import ForwardRef


def live_ability(line: GenieGameEntityGroup) -> ForwardRef:
    """
    Adds the Live ability to a line.

    :param line: Unit/Building line that gets the ability.
    :returns: The forward reference for the ability.
    """
    current_unit = line.get_head_unit()
    current_unit_id = line.get_head_unit_id()
    dataset = line.data

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

    game_entity_name = name_lookup_dict[current_unit_id][0]

    ability_ref = f"{game_entity_name}.Live"
    ability_raw_api_object = RawAPIObject(ability_ref, "Live", dataset.nyan_api_objects)
    ability_raw_api_object.add_raw_parent("engine.ability.type.Live")
    ability_location = ForwardRef(line, game_entity_name)
    ability_raw_api_object.set_location(ability_location)

    attributes_set = []

    # Health
    # =======================================================================================
    health_ref = f"{game_entity_name}.Live.Health"
    health_raw_api_object = RawAPIObject(health_ref, "Health", dataset.nyan_api_objects)
    health_raw_api_object.add_raw_parent("engine.util.attribute.AttributeSetting")
    health_location = ForwardRef(line, ability_ref)
    health_raw_api_object.set_location(health_location)

    attribute_value = dataset.pregen_nyan_objects["util.attribute.types.Health"].get_nyan_object(
    )
    health_raw_api_object.add_raw_member("attribute",
                                         attribute_value,
                                         "engine.util.attribute.AttributeSetting")

    # Lowest HP can go
    health_raw_api_object.add_raw_member("min_value",
                                         0,
                                         "engine.util.attribute.AttributeSetting")

    # Max HP and starting HP
    max_hp_value = current_unit["hit_points"].value
    health_raw_api_object.add_raw_member("max_value",
                                         max_hp_value,
                                         "engine.util.attribute.AttributeSetting")

    starting_value = max_hp_value
    if isinstance(line, GenieBuildingLineGroup):
        # Buildings spawn with 1 HP
        starting_value = 1

    health_raw_api_object.add_raw_member("starting_value",
                                         starting_value,
                                         "engine.util.attribute.AttributeSetting")

    line.add_raw_api_object(health_raw_api_object)

    # =======================================================================================
    health_forward_ref = ForwardRef(line, health_raw_api_object.get_id())
    attributes_set.append(health_forward_ref)

    if current_unit_id == 125:
        # Faith (only monk)
        faith_ref = f"{game_entity_name}.Live.Faith"
        faith_raw_api_object = RawAPIObject(faith_ref, "Faith", dataset.nyan_api_objects)
        faith_raw_api_object.add_raw_parent("engine.util.attribute.AttributeSetting")
        faith_location = ForwardRef(line, ability_ref)
        faith_raw_api_object.set_location(faith_location)

        attribute_value = dataset.pregen_nyan_objects["util.attribute.types.Faith"].get_nyan_object(
        )
        faith_raw_api_object.add_raw_member("attribute", attribute_value,
                                            "engine.util.attribute.AttributeSetting")

        # Lowest faith can go
        faith_raw_api_object.add_raw_member("min_value",
                                            0,
                                            "engine.util.attribute.AttributeSetting")

        # Max faith and starting faith
        faith_raw_api_object.add_raw_member("max_value",
                                            100,
                                            "engine.util.attribute.AttributeSetting")
        faith_raw_api_object.add_raw_member("starting_value",
                                            100,
                                            "engine.util.attribute.AttributeSetting")

        line.add_raw_api_object(faith_raw_api_object)

        faith_forward_ref = ForwardRef(line, faith_ref)
        attributes_set.append(faith_forward_ref)

    ability_raw_api_object.add_raw_member("attributes", attributes_set,
                                          "engine.ability.type.Live")

    line.add_raw_api_object(ability_raw_api_object)

    ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

    return ability_forward_ref
