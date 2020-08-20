# Copyright 2020-2020 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-locals,too-many-branches,too-many-nested-blocks
#
# TODO:
# pylint: disable=line-too-long

"""
Derives and adds abilities to lines or civ groups. Subroutine of the
nyan subprocessor.
"""
from ...entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup,\
    GenieBuildingLineGroup, GenieVillagerGroup, GenieAmbientGroup,\
    GenieVariantGroup
from ...entity_object.conversion.converter_object import RawAPIObject
from ...service import internal_name_lookups
from ...value_object.conversion.forward_ref import ForwardRef


class AoCModifierSubprocessor:
    """
    Creates raw API objects for modifiers in AoC.
    """

    @staticmethod
    def elevation_attack_modifiers(converter_obj_group):
        """
        Adds the pregenerated elevation damage multipliers to a line or civ group.

        :param converter_obj_group: ConverterObjectGroup that gets the modifier.
        :type converter_obj_group: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward references for the modifier.
        :rtype: list
        """
        dataset = converter_obj_group.data
        modifiers = [
            dataset.pregen_nyan_objects["aux.modifier.elevation_difference.AttackMultiplierHigh"].get_nyan_object(),
            dataset.pregen_nyan_objects["aux.modifier.elevation_difference.AttackMultiplierLow"].get_nyan_object()
        ]

        return modifiers

    @staticmethod
    def flyover_effect_modifier(converter_obj_group):
        """
        Adds the pregenerated fly-over-cliff damage multiplier to a line or civ group.

        :param converter_obj_group: ConverterObjectGroup that gets the modifier.
        :type converter_obj_group: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the modifier.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        dataset = converter_obj_group.data
        modifier = dataset.pregen_nyan_objects["aux.modifier.flyover_cliff.AttackMultiplierFlyover"].get_nyan_object()

        return modifier

    @staticmethod
    def gather_rate_modifier(converter_obj_group):
        """
        Adds Gather modifiers to a line or civ group.

        :param converter_obj_group: ConverterObjectGroup that gets the modifier.
        :type converter_obj_group: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the modifier.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        dataset = converter_obj_group.data

        modifiers = []

        if isinstance(converter_obj_group, GenieGameEntityGroup):
            if isinstance(converter_obj_group, GenieVillagerGroup):
                gatherers = converter_obj_group.variants[0].line

            else:
                gatherers = [converter_obj_group.line[0]]

            head_unit_id = converter_obj_group.get_head_unit_id()

            name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

            target_obj_name = name_lookup_dict[head_unit_id][0]

            for gatherer in gatherers:
                unit_commands = gatherer["unit_commands"].get_value()

                for command in unit_commands:
                    # Find a gather ability.
                    type_id = command["type"].get_value()

                    if type_id not in (5, 110):
                        continue

                    work_value = command["work_value1"].get_value()

                    # Check if the work value is 1 (with some rounding error margin)
                    # if not we have to create a modifier
                    if work_value < 1.0001 or work_value > 0.9999:
                        continue

                    # Search for the lines with the matching class/unit id
                    class_id = command["class_id"].get_value()
                    unit_id = command["unit_id"].get_value()

                    entity_lines = {}
                    entity_lines.update(dataset.unit_lines)
                    entity_lines.update(dataset.building_lines)
                    entity_lines.update(dataset.ambient_groups)
                    entity_lines.update(dataset.variant_groups)

                    if unit_id != -1:
                        lines = [entity_lines[unit_id]]

                    elif class_id != -1:
                        lines = []
                        for line in entity_lines.values():
                            if line.get_class_id() == class_id:
                                lines.append(line)

                    # Create a modifier for each matching resource spot
                    for resource_line in lines:
                        head_unit_id = resource_line.get_head_unit_id()
                        if isinstance(resource_line, GenieBuildingLineGroup):
                            resource_line_name = name_lookup_dict[head_unit_id][0]

                        elif isinstance(resource_line, GenieAmbientGroup):
                            resource_line_name = name_lookup_dict[head_unit_id][0]

                        elif isinstance(resource_line, GenieVariantGroup):
                            resource_line_name = name_lookup_dict[head_unit_id][1]

                        modifier_ref = "%s.%sGatheringRate" % (target_obj_name,
                                                               resource_line_name)
                        modifier_raw_api_object = RawAPIObject(modifier_ref,
                                                               "%sGatheringRate",
                                                               dataset.nyan_api_objects)
                        modifier_raw_api_object.add_raw_parent("engine.modifier.multiplier.type.GatheringRate")
                        modifier_location = ForwardRef(converter_obj_group, target_obj_name)
                        modifier_raw_api_object.set_location(modifier_location)

                        # Multiplier
                        modifier_raw_api_object.add_raw_member("multiplier",
                                                               work_value,
                                                               "engine.modifier.multiplier.MultiplierModifier")

                        # Resource spot
                        spot_ref = "%s.Harvestable.%sResourceSpot" % (resource_line_name,
                                                                      resource_line_name)
                        spot_forward_ref = ForwardRef(resource_line, spot_ref)
                        modifier_raw_api_object.add_raw_member("resource_spot",
                                                               spot_forward_ref,
                                                               "engine.modifier.multiplier.type.GatheringRate")

                        converter_obj_group.add_raw_api_object(modifier_raw_api_object)
                        modifier_forward_ref = ForwardRef(converter_obj_group,
                                                          modifier_raw_api_object.get_id())
                        modifiers.append(modifier_forward_ref)

        return modifiers

    @staticmethod
    def move_speed_modifier(converter_obj_group, value=None):
        """
        Adds a MoveSpeed modifier to a line or civ group.

        :param converter_obj_group: ConverterObjectGroup that gets the modifier.
        :type converter_obj_group: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the modifier.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        dataset = converter_obj_group.data
        if isinstance(converter_obj_group, GenieGameEntityGroup):
            head_unit_id = converter_obj_group.get_head_unit_id()
            name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
            target_obj_name = name_lookup_dict[head_unit_id][0]

        else:
            # Civs
            civ_lookup_dict = internal_name_lookups.get_civ_lookups(dataset.game_version)
            target_obj_name = civ_lookup_dict[converter_obj_group.get_id()][0]

        modifier_ref = "%s.MoveSpeed" % (target_obj_name)
        modifier_raw_api_object = RawAPIObject(modifier_ref, "MoveSpeed", dataset.nyan_api_objects)
        modifier_raw_api_object.add_raw_parent("engine.modifier.multiplier.type.MoveSpeed")
        modifier_location = ForwardRef(converter_obj_group, target_obj_name)
        modifier_raw_api_object.set_location(modifier_location)

        modifier_raw_api_object.add_raw_member("multiplier",
                                               value,
                                               "engine.modifier.multiplier.MultiplierModifier")

        converter_obj_group.add_raw_api_object(modifier_raw_api_object)

        modifier_forward_ref = ForwardRef(converter_obj_group, modifier_raw_api_object.get_id())

        return modifier_forward_ref
