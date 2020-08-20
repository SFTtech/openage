# Copyright 2020-2020 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-locals,too-many-statements,too-many-branches

"""
Creates patches and modifiers for civs.
"""
from ...entity_object.conversion.converter_object import RawAPIObject
from ...service import internal_name_lookups
from ...value_object.conversion.forward_ref import ForwardRef
from ..aoc.civ_subprocessor import AoCCivSubprocessor
from .tech_subprocessor import SWGBCCTechSubprocessor


class SWGBCCCivSubprocessor:
    """
    Creates raw API objects for civs in SWGB.
    """

    @classmethod
    def get_civ_setup(cls, civ_group):
        """
        Returns the patches for the civ setup which configures architecture sets
        unique units, unique techs, team boni and unique stat upgrades.
        """
        patches = []

        patches.extend(AoCCivSubprocessor.setup_unique_units(civ_group))
        patches.extend(AoCCivSubprocessor.setup_unique_techs(civ_group))
        patches.extend(AoCCivSubprocessor.setup_tech_tree(civ_group))
        patches.extend(AoCCivSubprocessor.setup_civ_bonus(civ_group))

        if len(civ_group.get_team_bonus_effects()) > 0:
            patches.extend(SWGBCCTechSubprocessor.get_patches(civ_group.team_bonus))

        return patches

    @classmethod
    def get_modifiers(cls, civ_group):
        """
        Returns global modifiers of a civ.
        """
        modifiers = []

        for civ_bonus in civ_group.civ_boni.values():
            if civ_bonus.replaces_researchable_tech():
                # TODO: instant tech research modifier
                pass

        return modifiers

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
        food_amount = civ_group.civ["resources"][91].get_value()
        carbon_amount = civ_group.civ["resources"][92].get_value()
        nova_amount = civ_group.civ["resources"][93].get_value()
        ore_amount = civ_group.civ["resources"][94].get_value()

        # Find civ unique starting resources
        tech_tree = civ_group.get_tech_tree_effects()
        for effect in tech_tree:
            type_id = effect.get_type()

            if type_id != 1:
                continue

            resource_id = effect["attr_a"].get_value()
            amount = effect["attr_d"].get_value()
            if resource_id == 91:
                food_amount += amount

            elif resource_id == 92:
                carbon_amount += amount

            elif resource_id == 93:
                nova_amount += amount

            elif resource_id == 94:
                ore_amount += amount

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

        carbon_ref = "%s.CarbonStartingAmount" % (civ_name)
        carbon_raw_api_object = RawAPIObject(carbon_ref, "CarbonStartingAmount",
                                             dataset.nyan_api_objects)
        carbon_raw_api_object.add_raw_parent("engine.aux.resource.ResourceAmount")
        civ_location = ForwardRef(civ_group, civ_lookup_dict[civ_group.get_id()][0])
        carbon_raw_api_object.set_location(civ_location)

        resource = dataset.pregen_nyan_objects["aux.resource.types.Carbon"].get_nyan_object()
        carbon_raw_api_object.add_raw_member("type",
                                             resource,
                                             "engine.aux.resource.ResourceAmount")

        carbon_raw_api_object.add_raw_member("amount",
                                             carbon_amount,
                                             "engine.aux.resource.ResourceAmount")

        carbon_forward_ref = ForwardRef(civ_group, carbon_ref)
        resource_amounts.append(carbon_forward_ref)

        nova_ref = "%s.NovaStartingAmount" % (civ_name)
        nova_raw_api_object = RawAPIObject(nova_ref, "NovaStartingAmount",
                                           dataset.nyan_api_objects)
        nova_raw_api_object.add_raw_parent("engine.aux.resource.ResourceAmount")
        civ_location = ForwardRef(civ_group, civ_lookup_dict[civ_group.get_id()][0])
        nova_raw_api_object.set_location(civ_location)

        resource = dataset.pregen_nyan_objects["aux.resource.types.Nova"].get_nyan_object()
        nova_raw_api_object.add_raw_member("type",
                                           resource,
                                           "engine.aux.resource.ResourceAmount")

        nova_raw_api_object.add_raw_member("amount",
                                           nova_amount,
                                           "engine.aux.resource.ResourceAmount")

        nova_forward_ref = ForwardRef(civ_group, nova_ref)
        resource_amounts.append(nova_forward_ref)

        ore_ref = "%s.OreStartingAmount" % (civ_name)
        ore_raw_api_object = RawAPIObject(ore_ref, "OreStartingAmount",
                                          dataset.nyan_api_objects)
        ore_raw_api_object.add_raw_parent("engine.aux.resource.ResourceAmount")
        civ_location = ForwardRef(civ_group, civ_lookup_dict[civ_group.get_id()][0])
        ore_raw_api_object.set_location(civ_location)

        resource = dataset.pregen_nyan_objects["aux.resource.types.Ore"].get_nyan_object()
        ore_raw_api_object.add_raw_member("type",
                                          resource,
                                          "engine.aux.resource.ResourceAmount")

        ore_raw_api_object.add_raw_member("amount",
                                          ore_amount,
                                          "engine.aux.resource.ResourceAmount")

        ore_forward_ref = ForwardRef(civ_group, ore_ref)
        resource_amounts.append(ore_forward_ref)

        civ_group.add_raw_api_object(food_raw_api_object)
        civ_group.add_raw_api_object(carbon_raw_api_object)
        civ_group.add_raw_api_object(nova_raw_api_object)
        civ_group.add_raw_api_object(ore_raw_api_object)

        return resource_amounts
