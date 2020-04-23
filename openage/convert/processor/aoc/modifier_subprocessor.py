# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Derives and adds abilities to lines or civ groups. Subroutine of the
nyan subprocessor.
"""
from openage.convert.dataformat.aoc.genie_unit import GenieGameEntityGroup,\
    GenieBuildingLineGroup, GenieVillagerGroup, GenieAmbientGroup,\
    GenieVariantGroup
from openage.convert.dataformat.aoc.internal_nyan_names import BUILDING_LINE_LOOKUPS,\
    UNIT_LINE_LOOKUPS, CIV_GROUP_LOOKUPS, AMBIENT_GROUP_LOOKUPS,\
    VARIANT_GROUP_LOOKUPS
from openage.convert.dataformat.converter_object import RawAPIObject
from openage.convert.dataformat.aoc.expected_pointer import ExpectedPointer
from openage.util.ordered_set import OrderedSet


class AoCModifierSubprocessor:

    @staticmethod
    def elevation_attack_modifiers(converter_obj_group):
        """
        Adds the pregenerated elevation damage multipliers to a line or civ group.

        :param converter_obj_group: ConverterObjectGroup that gets the modifier.
        :type converter_obj_group: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointers for the modifier.
        :rtype: list
        """
        dataset = converter_obj_group.data
        modifiers = [dataset.pregen_nyan_objects["aux.modifier.elevation_difference.AttackMultiplierHigh"].get_nyan_object(),
                     dataset.pregen_nyan_objects["aux.modifier.elevation_difference.AttackMultiplierLow"].get_nyan_object()]

        return modifiers

    @staticmethod
    def flyover_effect_modifier(converter_obj_group):
        """
        Adds the pregenerated fly-over-cliff damage multiplier to a line or civ group.

        :param converter_obj_group: ConverterObjectGroup that gets the modifier.
        :type converter_obj_group: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the modifier.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        dataset = converter_obj_group.data
        modifier = dataset.pregen_nyan_objects["aux.modifier.flyover_cliff.AttackMultiplierFlyover"].get_nyan_object()

        return modifier

    @staticmethod
    def gather_rate_modifier(converter_obj_group, value=None):
        """
        Adds Gather modifiers to a line or civ group.

        :param converter_obj_group: ConverterObjectGroup that gets the modifier.
        :type converter_obj_group: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the modifier.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        dataset = converter_obj_group.data

        modifiers = []

        if isinstance(converter_obj_group, GenieGameEntityGroup):
            if isinstance(converter_obj_group, GenieVillagerGroup):
                gatherers = converter_obj_group.variants[1].line

            else:
                gatherers = [converter_obj_group.line[0]]

            head_unit_id = converter_obj_group.get_head_unit_id()

            if isinstance(converter_obj_group, GenieBuildingLineGroup):
                name_lookup_dict = BUILDING_LINE_LOOKUPS

            else:
                name_lookup_dict = UNIT_LINE_LOOKUPS

            target_obj_name = name_lookup_dict[head_unit_id][0]

            for gatherer in gatherers:
                unit_commands = gatherer.get_member("unit_commands").get_value()

                for command in unit_commands:
                    # Find a gather ability.
                    type_id = command.get_value()["type"].get_value()

                    if type_id not in (5, 110):
                        continue

                    work_value = command.get_value()["work_value1"].get_value()

                    # Check if the work value is 1 (with some rounding error margin)
                    # if not we have to create a modifier
                    if work_value < 1.0001 or work_value > 0.9999:
                        continue

                    # Search for the lines with the matching class/unit id
                    class_id = command.get_value()["class_id"].get_value()
                    unit_id = command.get_value()["unit_id"].get_value()

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
                            resource_line_name = BUILDING_LINE_LOOKUPS[head_unit_id][0]

                        elif isinstance(resource_line, GenieAmbientGroup):
                            resource_line_name = AMBIENT_GROUP_LOOKUPS[head_unit_id][0]

                        elif isinstance(resource_line, GenieVariantGroup):
                            resource_line_name = VARIANT_GROUP_LOOKUPS[head_unit_id][1]

                        modifier_ref = "%s.%sGatheringRate" % (target_obj_name, resource_line_name)
                        modifier_raw_api_object = RawAPIObject(modifier_ref,
                                                               "%sGatheringRate",
                                                               dataset.nyan_api_objects)
                        modifier_raw_api_object.add_raw_parent("engine.modifier.multiplier.type.GatheringRate")
                        modifier_location = ExpectedPointer(converter_obj_group, target_obj_name)
                        modifier_raw_api_object.set_location(modifier_location)

                        # Multiplier
                        modifier_raw_api_object.add_raw_member("multiplier",
                                                               work_value,
                                                               "engine.modifier.multiplier.MultiplierModifier")

                        # Resource spot
                        spot_ref = "%s.Harvestable.%sResourceSpot" (resource_line_name, resource_line_name)
                        spot_expected_pointer = ExpectedPointer(resource_line, spot_ref)
                        modifier_raw_api_object.add_raw_member("resource_spot",
                                                               spot_expected_pointer,
                                                               "engine.modifier.multiplier.type.GatheringRate")

                        converter_obj_group.add_raw_api_object(modifier_raw_api_object)
                        modifier_expected_pointer = ExpectedPointer(converter_obj_group,
                                                                    modifier_raw_api_object.get_id())
                        modifiers.append(modifier_expected_pointer)

        return modifiers

    @staticmethod
    def move_speed_modifier(converter_obj_group, value=None):
        """
        Adds a MoveSpeed modifier to a line or civ group.

        :param converter_obj_group: ConverterObjectGroup that gets the modifier.
        :type converter_obj_group: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the modifier.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        dataset = converter_obj_group.data
        if isinstance(converter_obj_group, GenieGameEntityGroup):
            head_unit_id = converter_obj_group.get_head_unit_id()

            if isinstance(converter_obj_group, GenieBuildingLineGroup):
                name_lookup_dict = BUILDING_LINE_LOOKUPS

            else:
                name_lookup_dict = UNIT_LINE_LOOKUPS

            target_obj_name = name_lookup_dict[head_unit_id][0]

        else:
            # Civs
            target_obj_name = CIV_GROUP_LOOKUPS[converter_obj_group.get_id()][0]

        modifier_ref = "%s.MoveSpeed" % (target_obj_name)
        modifier_raw_api_object = RawAPIObject(modifier_ref, "MoveSpeed", dataset.nyan_api_objects)
        modifier_raw_api_object.add_raw_parent("engine.modifier.multiplier.type.MoveSpeed")
        modifier_location = ExpectedPointer(converter_obj_group, target_obj_name)
        modifier_raw_api_object.set_location(modifier_location)

        modifier_raw_api_object.add_raw_member("multiplier",
                                               value,
                                               "engine.modifier.multiplier.MultiplierModifier")

        converter_obj_group.add_raw_api_object(modifier_raw_api_object)

        modifier_expected_pointer = ExpectedPointer(converter_obj_group, modifier_raw_api_object.get_id())

        return modifier_expected_pointer
