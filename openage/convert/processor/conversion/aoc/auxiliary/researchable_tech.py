# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for researchables (techs).
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef
from .util import get_condition

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_tech import GenieTechEffectBundleGroup


def get_researchable_tech(tech_group: GenieTechEffectBundleGroup) -> None:
    """
    Creates the ResearchableTech object for a Tech.

    :param tech_group: Tech group that is a technology.
    """
    dataset = tech_group.data
    research_location_id = tech_group.get_research_location_id()
    research_location = dataset.building_lines[research_location_id]

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
    tech_lookup_dict = internal_name_lookups.get_tech_lookups(dataset.game_version)
    civ_lookup_dict = internal_name_lookups.get_civ_lookups(dataset.game_version)

    research_location_name = name_lookup_dict[research_location_id][0]
    tech_name = tech_lookup_dict[tech_group.get_id()][0]

    obj_ref = f"{tech_name}.ResearchableTech"
    obj_name = f"{tech_name}Researchable"
    researchable_raw_api_object = RawAPIObject(obj_ref, obj_name, dataset.nyan_api_objects)
    researchable_raw_api_object.add_raw_parent("engine.util.research.ResearchableTech")

    # Location of the object depends on whether it'a a unique tech or a normal tech
    if tech_group.is_unique():
        # Add object to the Civ object
        civ_id = tech_group.get_civilization()
        civ = dataset.civ_groups[civ_id]
        civ_name = civ_lookup_dict[civ_id][0]

        researchable_location = ForwardRef(civ, civ_name)

    else:
        # Add object to the research location's Research ability
        researchable_location = ForwardRef(research_location,
                                           f"{research_location_name}.Research")

    researchable_raw_api_object.set_location(researchable_location)

    # Tech
    tech_forward_ref = ForwardRef(tech_group, tech_name)
    researchable_raw_api_object.add_raw_member("tech",
                                               tech_forward_ref,
                                               "engine.util.research.ResearchableTech")

    # Cost
    cost_ref = f"{tech_name}.ResearchableTech.{tech_name}Cost"
    cost_raw_api_object = RawAPIObject(cost_ref,
                                       f"{tech_name}Cost",
                                       dataset.nyan_api_objects)
    cost_raw_api_object.add_raw_parent("engine.util.cost.type.ResourceCost")
    tech_forward_ref = ForwardRef(tech_group, obj_ref)
    cost_raw_api_object.set_location(tech_forward_ref)

    payment_mode = dataset.nyan_api_objects["engine.util.payment_mode.type.Advance"]
    cost_raw_api_object.add_raw_member("payment_mode",
                                       payment_mode,
                                       "engine.util.cost.Cost")

    cost_amounts = []
    for resource_amount in tech_group.tech["research_resource_costs"].value:
        resource_id = resource_amount["type_id"].value
        resource = None
        resource_name = ""
        if resource_id == -1:
            # Not a valid resource
            continue

        if resource_id == 0:
            resource = dataset.pregen_nyan_objects["util.resource.types.Food"].get_nyan_object()
            resource_name = "Food"

        elif resource_id == 1:
            resource = dataset.pregen_nyan_objects["util.resource.types.Wood"].get_nyan_object()
            resource_name = "Wood"

        elif resource_id == 2:
            resource = dataset.pregen_nyan_objects["util.resource.types.Stone"].get_nyan_object(
            )
            resource_name = "Stone"

        elif resource_id == 3:
            resource = dataset.pregen_nyan_objects["util.resource.types.Gold"].get_nyan_object()
            resource_name = "Gold"

        else:
            # Other resource ids are handled differently
            continue

        # Skip resources that are only expected to be there
        if not resource_amount["enabled"].value:
            continue

        amount = resource_amount["amount"].value

        cost_amount_ref = f"{cost_ref}.{resource_name}Amount"
        cost_amount = RawAPIObject(cost_amount_ref,
                                   f"{resource_name}Amount",
                                   dataset.nyan_api_objects)
        cost_amount.add_raw_parent("engine.util.resource.ResourceAmount")
        cost_forward_ref = ForwardRef(tech_group, cost_ref)
        cost_amount.set_location(cost_forward_ref)

        cost_amount.add_raw_member("type",
                                   resource,
                                   "engine.util.resource.ResourceAmount")
        cost_amount.add_raw_member("amount",
                                   amount,
                                   "engine.util.resource.ResourceAmount")

        cost_amount_forward_ref = ForwardRef(tech_group, cost_amount_ref)
        cost_amounts.append(cost_amount_forward_ref)
        tech_group.add_raw_api_object(cost_amount)

    cost_raw_api_object.add_raw_member("amount",
                                       cost_amounts,
                                       "engine.util.cost.type.ResourceCost")

    cost_forward_ref = ForwardRef(tech_group, cost_ref)
    researchable_raw_api_object.add_raw_member("cost",
                                               cost_forward_ref,
                                               "engine.util.research.ResearchableTech")

    research_time = tech_group.tech["research_time"].value
    researchable_raw_api_object.add_raw_member("research_time",
                                               research_time,
                                               "engine.util.research.ResearchableTech")

    # Create sound object
    sound_ref = f"{tech_name}.ResearchableTech.Sound"
    sound_raw_api_object = RawAPIObject(sound_ref, "ResearchSound",
                                        dataset.nyan_api_objects)
    sound_raw_api_object.add_raw_parent("engine.util.sound.Sound")
    sound_location = ForwardRef(tech_group,
                                f"{tech_name}.ResearchableTech")
    sound_raw_api_object.set_location(sound_location)

    # AoE doesn't support sounds here, so this is empty
    sound_raw_api_object.add_raw_member("play_delay",
                                        0,
                                        "engine.util.sound.Sound")
    sound_raw_api_object.add_raw_member("sounds",
                                        [],
                                        "engine.util.sound.Sound")

    sound_forward_ref = ForwardRef(tech_group, sound_ref)
    researchable_raw_api_object.add_raw_member("research_sounds",
                                               [sound_forward_ref],
                                               "engine.util.research.ResearchableTech")

    tech_group.add_raw_api_object(sound_raw_api_object)

    # Condition
    unlock_conditions = []
    if tech_group.get_id() > -1:
        unlock_conditions.extend(get_condition(tech_group,
                                               obj_ref,
                                               tech_group.get_id(),
                                               top_level=True))

    researchable_raw_api_object.add_raw_member("condition",
                                               unlock_conditions,
                                               "engine.util.research.ResearchableTech")

    tech_group.add_raw_api_object(researchable_raw_api_object)
    tech_group.add_raw_api_object(cost_raw_api_object)
