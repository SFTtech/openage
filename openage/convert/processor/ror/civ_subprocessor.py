# Copyright 2020-2020 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-few-public-methods,too-many-statements,too-many-locals

"""
Creates patches and modifiers for civs.
"""
from ...dataformat.converter_object import RawAPIObject
from ...service import internal_name_lookups
from ...value_object.conversion.forward_ref import ForwardRef


class RoRCivSubprocessor:
    """
    Creates raw API objects for civs in RoR.
    """

    @staticmethod
    def get_starting_resources(civ_group):
        """
        Returns the starting resources of a civ.
        """
        resource_amounts = []

        civ_id = civ_group.get_id()
        dataset = civ_group.data

        civ_lookup_dict = internal_name_lookups.get_civ_lookups(dataset.game_version)

        civ_name = civ_lookup_dict[civ_id][0]

        # Find starting resource amounts
        food_amount = civ_group.civ["resources"][0].get_value()
        wood_amount = civ_group.civ["resources"][1].get_value()
        gold_amount = civ_group.civ["resources"][2].get_value()
        stone_amount = civ_group.civ["resources"][3].get_value()

        # Find civ unique starting resources
        tech_tree = civ_group.get_tech_tree_effects()
        for effect in tech_tree:
            type_id = effect.get_type()

            if type_id != 1:
                continue

            resource_id = effect["attr_a"].get_value()
            amount = effect["attr_d"].get_value()
            if resource_id == 0:
                food_amount += amount

            elif resource_id == 1:
                wood_amount += amount

            elif resource_id == 2:
                gold_amount += amount

            elif resource_id == 3:
                stone_amount += amount

        food_ref = "%s.FoodStartingAmount" % (civ_name)
        food_raw_api_object = RawAPIObject(food_ref, "FoodStartingAmount",
                                           dataset.nyan_api_objects)
        food_raw_api_object.add_raw_parent("engine.aux.resource.ResourceAmount")
        civ_location = ForwardRef(civ_group, civ_lookup_dict[civ_group.get_id()][0])
        food_raw_api_object.set_location(civ_location)

        resource = dataset.pregen_nyan_objects["aux.resource.types.Food"].get_nyan_object()
        food_raw_api_object.add_raw_member("type",
                                           resource,
                                           "engine.aux.resource.ResourceAmount")

        food_raw_api_object.add_raw_member("amount",
                                           food_amount,
                                           "engine.aux.resource.ResourceAmount")

        food_forward_ref = ForwardRef(civ_group, food_ref)
        resource_amounts.append(food_forward_ref)

        wood_ref = "%s.WoodStartingAmount" % (civ_name)
        wood_raw_api_object = RawAPIObject(wood_ref, "WoodStartingAmount",
                                           dataset.nyan_api_objects)
        wood_raw_api_object.add_raw_parent("engine.aux.resource.ResourceAmount")
        civ_location = ForwardRef(civ_group, civ_lookup_dict[civ_group.get_id()][0])
        wood_raw_api_object.set_location(civ_location)

        resource = dataset.pregen_nyan_objects["aux.resource.types.Wood"].get_nyan_object()
        wood_raw_api_object.add_raw_member("type",
                                           resource,
                                           "engine.aux.resource.ResourceAmount")

        wood_raw_api_object.add_raw_member("amount",
                                           wood_amount,
                                           "engine.aux.resource.ResourceAmount")

        wood_forward_ref = ForwardRef(civ_group, wood_ref)
        resource_amounts.append(wood_forward_ref)

        gold_ref = "%s.GoldStartingAmount" % (civ_name)
        gold_raw_api_object = RawAPIObject(gold_ref, "GoldStartingAmount",
                                           dataset.nyan_api_objects)
        gold_raw_api_object.add_raw_parent("engine.aux.resource.ResourceAmount")
        civ_location = ForwardRef(civ_group, civ_lookup_dict[civ_group.get_id()][0])
        gold_raw_api_object.set_location(civ_location)

        resource = dataset.pregen_nyan_objects["aux.resource.types.Gold"].get_nyan_object()
        gold_raw_api_object.add_raw_member("type",
                                           resource,
                                           "engine.aux.resource.ResourceAmount")

        gold_raw_api_object.add_raw_member("amount",
                                           gold_amount,
                                           "engine.aux.resource.ResourceAmount")

        gold_forward_ref = ForwardRef(civ_group, gold_ref)
        resource_amounts.append(gold_forward_ref)

        stone_ref = "%s.StoneStartingAmount" % (civ_name)
        stone_raw_api_object = RawAPIObject(stone_ref, "StoneStartingAmount",
                                            dataset.nyan_api_objects)
        stone_raw_api_object.add_raw_parent("engine.aux.resource.ResourceAmount")
        civ_location = ForwardRef(civ_group, civ_lookup_dict[civ_group.get_id()][0])
        stone_raw_api_object.set_location(civ_location)

        resource = dataset.pregen_nyan_objects["aux.resource.types.Stone"].get_nyan_object()
        stone_raw_api_object.add_raw_member("type",
                                            resource,
                                            "engine.aux.resource.ResourceAmount")

        stone_raw_api_object.add_raw_member("amount",
                                            stone_amount,
                                            "engine.aux.resource.ResourceAmount")

        stone_forward_ref = ForwardRef(civ_group, stone_ref)
        resource_amounts.append(stone_forward_ref)

        civ_group.add_raw_api_object(food_raw_api_object)
        civ_group.add_raw_api_object(wood_raw_api_object)
        civ_group.add_raw_api_object(gold_raw_api_object)
        civ_group.add_raw_api_object(stone_raw_api_object)

        return resource_amounts
