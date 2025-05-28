# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create resistances for converting units and buildings.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.aoc.genie_unit import GenieUnitLineGroup
from .....entity_object.conversion.converter_object import RawAPIObject
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from openage.convert.entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup


@staticmethod
def get_convert_resistances(
    line: GenieGameEntityGroup,
    ability_ref: str
) -> list[ForwardRef]:
    """
    Creates resistances that are used for conversion (unit command: 104)

    :param line: Unit/Building line that gets the ability.
    :type line: ...dataformat.converter_object.ConverterObjectGroup
    :param ability_ref: Reference of the ability raw API object the effects are added to.
    :type ability_ref: str
    :returns: The forward references for the effects.
    :rtype: list
    """
    dataset = line.data

    resistances = []

    # AoE2Convert
    resistance_parent = "engine.resistance.discrete.convert.Convert"
    convert_parent = "engine.resistance.discrete.convert.type.AoE2Convert"

    resistance_ref = f"{ability_ref}.Convert"
    resistance_raw_api_object = RawAPIObject(
        resistance_ref, "Convert", dataset.nyan_api_objects)
    resistance_raw_api_object.add_raw_parent(convert_parent)
    resistance_location = ForwardRef(line, ability_ref)
    resistance_raw_api_object.set_location(resistance_location)

    # Type
    if isinstance(line, GenieUnitLineGroup):
        type_ref = "util.convert_type.types.UnitConvert"

    else:
        type_ref = "util.convert_type.types.BuildingConvert"

    convert_type = dataset.pregen_nyan_objects[type_ref].get_nyan_object()
    resistance_raw_api_object.add_raw_member("type",
                                             convert_type,
                                             resistance_parent)

    # Chance resist
    # hardcoded resource
    chance_resist = dataset.genie_civs[0]["resources"][77].value / 100
    resistance_raw_api_object.add_raw_member("chance_resist",
                                             chance_resist,
                                             resistance_parent)

    if isinstance(line, GenieUnitLineGroup):
        guaranteed_rounds = dataset.genie_civs[0]["resources"][178].value
        protected_rounds = dataset.genie_civs[0]["resources"][179].value

    else:
        guaranteed_rounds = dataset.genie_civs[0]["resources"][180].value
        protected_rounds = dataset.genie_civs[0]["resources"][181].value

    # Guaranteed rounds
    resistance_raw_api_object.add_raw_member("guaranteed_resist_rounds",
                                             guaranteed_rounds,
                                             convert_parent)

    # Protected rounds
    resistance_raw_api_object.add_raw_member("protected_rounds",
                                             protected_rounds,
                                             convert_parent)

    # Protection recharge
    resistance_raw_api_object.add_raw_member("protection_round_recharge_time",
                                             0.0,
                                             convert_parent)

    line.add_raw_api_object(resistance_raw_api_object)
    resistance_forward_ref = ForwardRef(line, resistance_ref)
    resistances.append(resistance_forward_ref)

    return resistances
