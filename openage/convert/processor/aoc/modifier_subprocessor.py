# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Derives and adds abilities to lines or civ groups. Subroutine of the
nyan subprocessor.
"""
from openage.convert.dataformat.aoc.genie_unit import GenieGameEntityGroup,\
    GenieBuildingLineGroup
from openage.convert.dataformat.aoc.internal_nyan_names import BUILDING_LINE_LOOKUPS,\
    UNIT_LINE_LOOKUPS
from openage.convert.dataformat.converter_object import RawAPIObject
from openage.convert.dataformat.aoc.expected_pointer import ExpectedPointer


class AoCModifierSubprocessor:

    @staticmethod
    def move_speed_modifier(converter_obj_group, value):
        """
        Adds a MoveSpeed modifier to a line or civ group.

        :param converter_obj_group: ConverterObjectGroup that gets the modifier.
        :type converter_obj_group: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the modifier.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        if isinstance(converter_obj_group, GenieGameEntityGroup):
            head_unit_id = converter_obj_group.get_head_unit_id()
            dataset = converter_obj_group.data

            if isinstance(converter_obj_group, GenieBuildingLineGroup):
                name_lookup_dict = BUILDING_LINE_LOOKUPS

            else:
                name_lookup_dict = UNIT_LINE_LOOKUPS

            target_obj_name = name_lookup_dict[head_unit_id][0]

        else:
            # Civs
            pass

        modifier_ref = "%s.MoveSpeed" % (target_obj_name)
        modifier_raw_api_object = RawAPIObject(modifier_ref, "MoveSpeed", dataset.nyan_api_objects)
        modifier_raw_api_object.add_raw_parent("engine.modifier.multiplier.type.MoveSpeed")
        modifier_location = ExpectedPointer(converter_obj_group, target_obj_name)
        modifier_raw_api_object.set_location(modifier_location)

        modifier_raw_api_object.add_raw_member("multiplier",
                                               value,
                                               "engine.modifier.multiplier.type.MoveSpeed")

        converter_obj_group.add_raw_api_object(modifier_raw_api_object)

        modifier_expected_pointer = ExpectedPointer(converter_obj_group, modifier_raw_api_object.get_id())

        return modifier_expected_pointer
