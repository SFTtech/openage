# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Helper functions for AoC creatables and researchables.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.converter_object import ConverterObjectGroup


def get_condition(
    converter_obj_group: ConverterObjectGroup,
    obj_ref: str,
    tech_id: int,
    top_level: bool = False
) -> list[ForwardRef]:
    """
    Creates the condition for a creatable or researchable from tech
    by recursively searching the required techs.

    :param converter_object: ConverterObjectGroup that the condition objects should be nested in.
    :param obj_ref: Reference of converter_object inside the modpack.
    :param tech_id: tech ID of a tech wth a conditional unlock.
    :param top_level: True if the condition has subconditions, False otherwise.
    """
    dataset = converter_obj_group.data
    tech = dataset.genie_techs[tech_id]

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
    tech_lookup_dict = internal_name_lookups.get_tech_lookups(dataset.game_version)

    if not top_level and\
        (tech_id in dataset.initiated_techs.keys() or
            (tech_id in dataset.tech_groups.keys() and
             dataset.tech_groups[tech_id].is_researchable())):
        # The tech condition is a building or a researchable tech
        # and thus a literal.
        if tech_id in dataset.initiated_techs.keys():
            initiated_tech = dataset.initiated_techs[tech_id]
            building_id = initiated_tech.get_building_id()
            building_name = name_lookup_dict[building_id][0]
            literal_name = f"{building_name}Built"
            literal_parent = "engine.util.logic.literal.type.GameEntityProgress"

        elif dataset.tech_groups[tech_id].is_researchable():
            tech_name = tech_lookup_dict[tech_id][0]
            literal_name = f"{tech_name}Researched"
            literal_parent = "engine.util.logic.literal.type.TechResearched"

        else:
            raise ValueError("Required tech id {tech_id} is neither intiated nor researchable")

        literal_ref = f"{obj_ref}.{literal_name}"
        literal_raw_api_object = RawAPIObject(literal_ref,
                                              literal_name,
                                              dataset.nyan_api_objects)
        literal_raw_api_object.add_raw_parent(literal_parent)
        literal_location = ForwardRef(converter_obj_group, obj_ref)
        literal_raw_api_object.set_location(literal_location)

        if tech_id in dataset.initiated_techs.keys():
            building_line = dataset.unit_ref[building_id]
            building_forward_ref = ForwardRef(building_line, building_name)

            # Building
            literal_raw_api_object.add_raw_member("game_entity",
                                                  building_forward_ref,
                                                  literal_parent)

            # Progress
            # =======================================================================
            progress_ref = f"{literal_ref}.ProgressStatus"
            progress_raw_api_object = RawAPIObject(progress_ref,
                                                   "ProgressStatus",
                                                   dataset.nyan_api_objects)
            progress_raw_api_object.add_raw_parent("engine.util.progress_status.ProgressStatus")
            progress_location = ForwardRef(converter_obj_group, literal_ref)
            progress_raw_api_object.set_location(progress_location)

            # Type
            progress_type = dataset.nyan_api_objects["engine.util.progress_type.type.Construct"]
            progress_raw_api_object.add_raw_member("progress_type",
                                                   progress_type,
                                                   "engine.util.progress_status.ProgressStatus")

            # Progress (building must be 100% constructed)
            progress_raw_api_object.add_raw_member("progress",
                                                   100,
                                                   "engine.util.progress_status.ProgressStatus")

            converter_obj_group.add_raw_api_object(progress_raw_api_object)
            # =======================================================================
            progress_forward_ref = ForwardRef(converter_obj_group, progress_ref)
            literal_raw_api_object.add_raw_member("progress_status",
                                                  progress_forward_ref,
                                                  literal_parent)

        elif dataset.tech_groups[tech_id].is_researchable():
            tech_group = dataset.tech_groups[tech_id]
            tech_forward_ref = ForwardRef(tech_group, tech_name)
            literal_raw_api_object.add_raw_member("tech",
                                                  tech_forward_ref,
                                                  literal_parent)

        # LiteralScope
        # ==========================================================================
        scope_ref = f"{literal_ref}.LiteralScope"
        scope_raw_api_object = RawAPIObject(scope_ref,
                                            "LiteralScope",
                                            dataset.nyan_api_objects)
        scope_raw_api_object.add_raw_parent("engine.util.logic.literal_scope.type.Any")
        scope_location = ForwardRef(converter_obj_group, literal_ref)
        scope_raw_api_object.set_location(scope_location)

        scope_diplomatic_stances = [
            dataset.nyan_api_objects["engine.util.diplomatic_stance.type.Self"]
        ]
        scope_raw_api_object.add_raw_member("stances",
                                            scope_diplomatic_stances,
                                            "engine.util.logic.literal_scope.LiteralScope")

        converter_obj_group.add_raw_api_object(scope_raw_api_object)
        # ==========================================================================
        scope_forward_ref = ForwardRef(converter_obj_group, scope_ref)
        literal_raw_api_object.add_raw_member("scope",
                                              scope_forward_ref,
                                              "engine.util.logic.literal.Literal")

        literal_raw_api_object.add_raw_member("only_once",
                                              True,
                                              "engine.util.logic.LogicElement")

        converter_obj_group.add_raw_api_object(literal_raw_api_object)
        literal_forward_ref = ForwardRef(converter_obj_group, literal_ref)

        return [literal_forward_ref]

    else:
        # The tech condition has other requirements that need to be resolved

        # Find required techs for the current tech
        assoc_tech_id_members = []
        assoc_tech_id_members.extend(tech["required_techs"].value)
        required_tech_count = tech["required_tech_count"].value

        # Remove tech ids that are invalid or those we don't use
        relevant_ids = []
        for tech_id_member in assoc_tech_id_members:
            required_tech_id = tech_id_member.value
            if required_tech_id == -1:
                continue

            if required_tech_id == 104:
                # Skip Dark Age tech
                required_tech_count -= 1
                continue

            if required_tech_id in dataset.civ_boni.keys():
                continue

            relevant_ids.append(required_tech_id)

        if len(relevant_ids) == 0:
            return []

        if len(relevant_ids) == 1:
            # If there's only one required tech we don't need a gate
            # we can just return the logic element of the only required tech
            required_tech_id = relevant_ids[0]
            return get_condition(converter_obj_group,
                                 obj_ref,
                                 required_tech_id)

        gate_ref = f"{obj_ref}.UnlockCondition"
        gate_raw_api_object = RawAPIObject(gate_ref,
                                           "UnlockCondition",
                                           dataset.nyan_api_objects)

        if required_tech_count == len(relevant_ids):
            gate_raw_api_object.add_raw_parent("engine.util.logic.gate.type.AND")
            gate_location = ForwardRef(converter_obj_group, obj_ref)

        else:
            gate_raw_api_object.add_raw_parent("engine.util.logic.gate.type.SUBSETMIN")
            gate_location = ForwardRef(converter_obj_group, obj_ref)

            gate_raw_api_object.add_raw_member("size",
                                               required_tech_count,
                                               "engine.util.logic.gate.type.SUBSETMIN")

        gate_raw_api_object.set_location(gate_location)

        # Once unlocked, a creatable/researchable is unlocked forever
        gate_raw_api_object.add_raw_member("only_once",
                                           True,
                                           "engine.util.logic.LogicElement")

        # Get requirements from subtech recursively
        inputs = []
        for required_tech_id in relevant_ids:
            required = get_condition(converter_obj_group,
                                     gate_ref,
                                     required_tech_id)
            inputs.extend(required)

        gate_raw_api_object.add_raw_member("inputs",
                                           inputs,
                                           "engine.util.logic.gate.LogicGate")

        converter_obj_group.add_raw_api_object(gate_raw_api_object)
        gate_forward_ref = ForwardRef(converter_obj_group, gate_ref)
        return [gate_forward_ref]
