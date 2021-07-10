# Copyright 2021-2021 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-few-public-methods,too-many-locals

"""
Derives and adds abilities to lines. Subroutine of the
nyan subprocessor.
"""

from ....entity_object.conversion.converter_object import RawAPIObject
from ....service.conversion import internal_name_lookups
from ....value_object.conversion.forward_ref import ForwardRef


class DE2AbilitySubprocessor:
    """
    Creates raw API objects for abilities in DE2.
    """

    @staticmethod
    def regenerate_attribute_ability(line):
        """
        Adds the RegenerateAttribute ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward references for the ability.
        :rtype: list
        """
        current_unit = line.get_head_unit()
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        attribute = None
        attribute_name = ""
        if current_unit_id == 125:
            # Monk; regenerates Faith
            attribute = dataset.pregen_nyan_objects["util.attribute.types.Faith"].get_nyan_object()
            attribute_name = "Faith"

        elif current_unit_id == 692:
            # Berserk: regenerates Health
            attribute = dataset.pregen_nyan_objects["util.attribute.types.Health"].get_nyan_object()
            attribute_name = "Health"

        else:
            return []

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_name = f"Regenerate{attribute_name}"
        ability_ref = f"{game_entity_name}.{ability_name}"
        ability_raw_api_object = RawAPIObject(ability_ref, ability_name, dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.RegenerateAttribute")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Attribute rate
        # ===============================================================================
        rate_name = f"{attribute_name}Rate"
        rate_ref = f"{game_entity_name}.{ability_name}.{rate_name}"
        rate_raw_api_object = RawAPIObject(rate_ref, rate_name, dataset.nyan_api_objects)
        rate_raw_api_object.add_raw_parent("engine.util.attribute.AttributeRate")
        rate_location = ForwardRef(line, ability_ref)
        rate_raw_api_object.set_location(rate_location)

        # Attribute
        rate_raw_api_object.add_raw_member("type",
                                           attribute,
                                           "engine.util.attribute.AttributeRate")

        # Rate
        attribute_rate = 0
        if current_unit_id == 125:
            # stored in civ resources
            attribute_rate = dataset.genie_civs[0]["resources"][35].get_value()

        elif current_unit_id == 692:
            # stored in unit, but has to get converted to amount/second
            heal_timer = current_unit["heal_timer"].get_value()
            attribute_rate = 1 / heal_timer

        rate_raw_api_object.add_raw_member("rate",
                                           attribute_rate,
                                           "engine.util.attribute.AttributeRate")

        line.add_raw_api_object(rate_raw_api_object)
        # ===============================================================================
        rate_forward_ref = ForwardRef(line, rate_ref)
        ability_raw_api_object.add_raw_member("rate",
                                              rate_forward_ref,
                                              "engine.ability.type.RegenerateAttribute")

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return [ability_forward_ref]
