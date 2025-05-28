# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create effects for repairing buildings.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.aoc.genie_unit import GenieBuildingLineGroup
from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from openage.convert.entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup


def get_repair_effects(
    line: GenieGameEntityGroup,
    location_ref: str
) -> list[ForwardRef]:
    """
    Creates effects that are used for repairing (unit command: 106)

    :param line: Unit/Building line that gets the ability.
    :param location_ref: Reference to API object the effects are added to.
    :returns: The forward references for the effects.
    """
    dataset = line.data
    api_objects = dataset.nyan_api_objects

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

    effects = []

    effect_parent = "engine.effect.continuous.flat_attribute_change.FlatAttributeChange"
    repair_parent = "engine.effect.continuous.flat_attribute_change.type.FlatAttributeChangeIncrease"

    repairable_lines = []
    repairable_lines.extend(dataset.building_lines.values())
    for unit_line in dataset.unit_lines.values():
        if unit_line.is_repairable():
            repairable_lines.append(unit_line)

    for repairable_line in repairable_lines:
        game_entity_name = name_lookup_dict[repairable_line.get_head_unit_id()][0]

        repair_name = f"{game_entity_name}RepairEffect"
        repair_ref = f"{location_ref}.{repair_name}"
        repair_raw_api_object = RawAPIObject(repair_ref,
                                             repair_name,
                                             dataset.nyan_api_objects)
        repair_raw_api_object.add_raw_parent(repair_parent)
        repair_location = ForwardRef(line, location_ref)
        repair_raw_api_object.set_location(repair_location)

        line.add_raw_api_object(repair_raw_api_object)

        # Type
        type_ref = f"util.attribute_change_type.types.{game_entity_name}Repair"
        change_type = dataset.pregen_nyan_objects[type_ref].get_nyan_object()
        repair_raw_api_object.add_raw_member("type",
                                             change_type,
                                             effect_parent)

        # Min value (optional; not added because buildings don't block repairing)

        # Max value (optional; not added because there is none in AoE2)

        # Change rate
        # =================================================================================
        rate_name = f"{location_ref}.{repair_name}.ChangeRate"
        rate_raw_api_object = RawAPIObject(rate_name, "ChangeRate", dataset.nyan_api_objects)
        rate_raw_api_object.add_raw_parent("engine.util.attribute.AttributeRate")
        rate_location = ForwardRef(line, repair_ref)
        rate_raw_api_object.set_location(rate_location)

        attribute = dataset.pregen_nyan_objects["util.attribute.types.Health"].get_nyan_object()
        rate_raw_api_object.add_raw_member("type",
                                           attribute,
                                           "engine.util.attribute.AttributeRate")

        # Hardcoded repair rate:
        # - Buildings: 750 HP/min = 12.5 HP/s
        # - Ships/Siege: 187.5 HP/min = 3.125 HP/s
        if isinstance(repairable_line, GenieBuildingLineGroup):
            repair_rate = 12.5

        else:
            repair_rate = 3.125

        rate_raw_api_object.add_raw_member("rate",
                                           repair_rate,
                                           "engine.util.attribute.AttributeRate")

        line.add_raw_api_object(rate_raw_api_object)
        # =================================================================================
        rate_forward_ref = ForwardRef(line, rate_name)
        repair_raw_api_object.add_raw_member("change_rate",
                                             rate_forward_ref,
                                             effect_parent)

        # Ignore protection
        repair_raw_api_object.add_raw_member("ignore_protection",
                                             [],
                                             effect_parent)

        # Repair cost
        property_ref = f"{repair_ref}.Cost"
        property_raw_api_object = RawAPIObject(property_ref,
                                               "Cost",
                                               dataset.nyan_api_objects)
        property_raw_api_object.add_raw_parent("engine.effect.property.type.Cost")
        property_location = ForwardRef(line, repair_ref)
        property_raw_api_object.set_location(property_location)

        line.add_raw_api_object(property_raw_api_object)

        cost_ref = f"{game_entity_name}.CreatableGameEntity.{game_entity_name}RepairCost"
        cost_forward_ref = ForwardRef(repairable_line, cost_ref)
        property_raw_api_object.add_raw_member("cost",
                                               cost_forward_ref,
                                               "engine.effect.property.type.Cost")

        property_forward_ref = ForwardRef(line, property_ref)
        properties = {
            api_objects["engine.effect.property.type.Cost"]: property_forward_ref
        }

        repair_raw_api_object.add_raw_member("properties",
                                             properties,
                                             "engine.effect.Effect")

        repair_forward_ref = ForwardRef(line, repair_ref)
        effects.append(repair_forward_ref)

    return effects
