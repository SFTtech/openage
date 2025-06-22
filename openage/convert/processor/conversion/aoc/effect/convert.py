# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create effects for converting units and buildings.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from openage.convert.entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup


def get_convert_effects(
    line: GenieGameEntityGroup,
    location_ref: str
) -> list[ForwardRef]:
    """
    Creates effects that are used for conversion (unit command: 104)

    :param line: Unit/Building line that gets the ability.
    :param location_ref: Reference to API object the effects are added to.
    :returns: The forward references for the effects.
    """
    current_unit = line.get_head_unit()
    dataset = line.data

    effects = []

    effect_parent = "engine.effect.discrete.convert.Convert"
    convert_parent = "engine.effect.discrete.convert.type.AoE2Convert"

    unit_commands = current_unit["unit_commands"].value
    for command in unit_commands:
        # Find the Heal command.
        type_id = command["type"].value

        if type_id == 104:
            skip_guaranteed_rounds = -1 * command["work_value1"].value
            skip_protected_rounds = -1 * command["work_value2"].value
            break

    else:
        # Return the empty set
        return effects

    # Unit conversion
    convert_ref = f"{location_ref}.ConvertUnitEffect"
    convert_raw_api_object = RawAPIObject(convert_ref,
                                          "ConvertUnitEffect",
                                          dataset.nyan_api_objects)
    convert_raw_api_object.add_raw_parent(convert_parent)
    convert_location = ForwardRef(line, location_ref)
    convert_raw_api_object.set_location(convert_location)

    # Type
    type_ref = "util.convert_type.types.UnitConvert"
    change_type = dataset.pregen_nyan_objects[type_ref].get_nyan_object()
    convert_raw_api_object.add_raw_member("type",
                                          change_type,
                                          effect_parent)

    # Min success (optional; not added because there is none in AoE2)
    # Max success (optional; not added because there is none in AoE2)

    # Chance
    # hardcoded resource
    chance_success = dataset.genie_civs[0]["resources"][182].value / 100
    convert_raw_api_object.add_raw_member("chance_success",
                                          chance_success,
                                          effect_parent)

    # Fail cost (optional; not added because there is none in AoE2)

    # Guaranteed rounds skip
    convert_raw_api_object.add_raw_member("skip_guaranteed_rounds",
                                          skip_guaranteed_rounds,
                                          convert_parent)

    # Protected rounds skip
    convert_raw_api_object.add_raw_member("skip_protected_rounds",
                                          skip_protected_rounds,
                                          convert_parent)

    line.add_raw_api_object(convert_raw_api_object)
    attack_forward_ref = ForwardRef(line, convert_ref)
    effects.append(attack_forward_ref)

    # Building conversion
    convert_ref = f"{location_ref}.ConvertBuildingEffect"
    convert_raw_api_object = RawAPIObject(convert_ref,
                                          "ConvertBuildingUnitEffect",
                                          dataset.nyan_api_objects)
    convert_raw_api_object.add_raw_parent(convert_parent)
    convert_location = ForwardRef(line, location_ref)
    convert_raw_api_object.set_location(convert_location)

    # Type
    type_ref = "util.convert_type.types.BuildingConvert"
    change_type = dataset.pregen_nyan_objects[type_ref].get_nyan_object()
    convert_raw_api_object.add_raw_member("type",
                                          change_type,
                                          effect_parent)

    # Min success (optional; not added because there is none in AoE2)
    # Max success (optional; not added because there is none in AoE2)

    # Chance
    # hardcoded resource
    chance_success = dataset.genie_civs[0]["resources"][182].value / 100
    convert_raw_api_object.add_raw_member("chance_success",
                                          chance_success,
                                          effect_parent)

    # Fail cost (optional; not added because there is none in AoE2)

    # Guaranteed rounds skip
    convert_raw_api_object.add_raw_member("skip_guaranteed_rounds",
                                          0,
                                          convert_parent)

    # Protected rounds skip
    convert_raw_api_object.add_raw_member("skip_protected_rounds",
                                          0,
                                          convert_parent)

    line.add_raw_api_object(convert_raw_api_object)
    attack_forward_ref = ForwardRef(line, convert_ref)
    effects.append(attack_forward_ref)

    return effects
