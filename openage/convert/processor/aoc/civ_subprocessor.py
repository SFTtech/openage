# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Creates patches and modifiers for civs.
"""
from openage.convert.dataformat.aoc.internal_nyan_names import CIV_GROUP_LOOKUPS,\
    UNIT_LINE_LOOKUPS, BUILDING_LINE_LOOKUPS, TECH_GROUP_LOOKUPS
from openage.convert.dataformat.aoc.expected_pointer import ExpectedPointer
from openage.convert.dataformat.converter_object import RawAPIObject
from openage.convert.dataformat.aoc.genie_unit import GenieBuildingLineGroup
from openage.nyan.nyan_structs import MemberOperator


class AoCCivSubprocessor:

    @classmethod
    def get_civ_setup(cls, civ_group):
        """
        Returns the patches for the civ setup which configures architecture sets
        unique units, unique techs, team boni and unique stat upgrades.
        """
        patches = []

        # TODO: Architecture set

        patches.extend(cls._setup_unique_units(civ_group))
        patches.extend(cls._setup_unique_techs(civ_group))

        # TODO: Tech tree

        # TODO: Team bonus

        return patches

    @classmethod
    def get_modifiers(cls, civ_group):
        """
        Returns global modifiers of a civ.
        """
        modifiers = []

        return modifiers

    @staticmethod
    def get_starting_resources(civ_group):
        """
        Returns the starting resources of a civ.
        """
        resource_amounts = []

        civ_id = civ_group.get_id()
        civ_name = CIV_GROUP_LOOKUPS[civ_id][0]
        dataset = civ_group.data

        # Find starting resource amounts
        food_amount = civ_group.civ["resources"][91].get_value()
        wood_amount = civ_group.civ["resources"][92].get_value()
        gold_amount = civ_group.civ["resources"][93].get_value()
        stone_amount = civ_group.civ["resources"][94].get_value()

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
                wood_amount += amount

            elif resource_id == 93:
                gold_amount += amount

            elif resource_id == 94:
                stone_amount += amount

        food_ref = "%s.FoodStartingAmount" % (civ_name)
        food_raw_api_object = RawAPIObject(food_ref, "FoodStartingAmount",
                                           dataset.nyan_api_objects)
        food_raw_api_object.add_raw_parent("engine.aux.resource.ResourceAmount")
        civ_location = ExpectedPointer(civ_group, CIV_GROUP_LOOKUPS[civ_group.get_id()][0])
        food_raw_api_object.set_location(civ_location)

        resource = dataset.pregen_nyan_objects["aux.resource.types.Food"].get_nyan_object()
        food_raw_api_object.add_raw_member("type",
                                           resource,
                                           "engine.aux.resource.ResourceAmount")

        food_raw_api_object.add_raw_member("amount",
                                           food_amount,
                                           "engine.aux.resource.ResourceAmount")

        food_expected_pointer = ExpectedPointer(civ_group, food_ref)
        resource_amounts.append(food_expected_pointer)

        wood_ref = "%s.WoodStartingAmount" % (civ_name)
        wood_raw_api_object = RawAPIObject(wood_ref, "WoodStartingAmount",
                                           dataset.nyan_api_objects)
        wood_raw_api_object.add_raw_parent("engine.aux.resource.ResourceAmount")
        civ_location = ExpectedPointer(civ_group, CIV_GROUP_LOOKUPS[civ_group.get_id()][0])
        wood_raw_api_object.set_location(civ_location)

        resource = dataset.pregen_nyan_objects["aux.resource.types.Wood"].get_nyan_object()
        wood_raw_api_object.add_raw_member("type",
                                           resource,
                                           "engine.aux.resource.ResourceAmount")

        wood_raw_api_object.add_raw_member("amount",
                                           wood_amount,
                                           "engine.aux.resource.ResourceAmount")

        wood_expected_pointer = ExpectedPointer(civ_group, wood_ref)
        resource_amounts.append(wood_expected_pointer)

        gold_ref = "%s.GoldStartingAmount" % (civ_name)
        gold_raw_api_object = RawAPIObject(gold_ref, "GoldStartingAmount",
                                           dataset.nyan_api_objects)
        gold_raw_api_object.add_raw_parent("engine.aux.resource.ResourceAmount")
        civ_location = ExpectedPointer(civ_group, CIV_GROUP_LOOKUPS[civ_group.get_id()][0])
        gold_raw_api_object.set_location(civ_location)

        resource = dataset.pregen_nyan_objects["aux.resource.types.Gold"].get_nyan_object()
        gold_raw_api_object.add_raw_member("type",
                                           resource,
                                           "engine.aux.resource.ResourceAmount")

        gold_raw_api_object.add_raw_member("amount",
                                           gold_amount,
                                           "engine.aux.resource.ResourceAmount")

        gold_expected_pointer = ExpectedPointer(civ_group, gold_ref)
        resource_amounts.append(gold_expected_pointer)

        stone_ref = "%s.StoneStartingAmount" % (civ_name)
        stone_raw_api_object = RawAPIObject(stone_ref, "StoneStartingAmount",
                                            dataset.nyan_api_objects)
        stone_raw_api_object.add_raw_parent("engine.aux.resource.ResourceAmount")
        civ_location = ExpectedPointer(civ_group, CIV_GROUP_LOOKUPS[civ_group.get_id()][0])
        stone_raw_api_object.set_location(civ_location)

        resource = dataset.pregen_nyan_objects["aux.resource.types.Stone"].get_nyan_object()
        stone_raw_api_object.add_raw_member("type",
                                            resource,
                                            "engine.aux.resource.ResourceAmount")

        stone_raw_api_object.add_raw_member("amount",
                                            stone_amount,
                                            "engine.aux.resource.ResourceAmount")

        stone_expected_pointer = ExpectedPointer(civ_group, stone_ref)
        resource_amounts.append(stone_expected_pointer)

        civ_group.add_raw_api_object(food_raw_api_object)
        civ_group.add_raw_api_object(wood_raw_api_object)
        civ_group.add_raw_api_object(gold_raw_api_object)
        civ_group.add_raw_api_object(stone_raw_api_object)

        return resource_amounts

    @staticmethod
    def _setup_unique_units(civ_group):
        """
        Patches the unique units into their train location.
        """
        patches = []

        civ_id = civ_group.get_id()
        dataset = civ_group.data
        civ_name = CIV_GROUP_LOOKUPS[civ_id][0]

        for unique_line in civ_group.unique_entities.values():
            head_unit_id = unique_line.get_head_unit_id()
            if isinstance(unique_line, GenieBuildingLineGroup):
                game_entity_name = BUILDING_LINE_LOOKUPS[head_unit_id][0]

            else:
                game_entity_name = UNIT_LINE_LOOKUPS[head_unit_id][0]

            # Get train location of line
            train_location_id = unique_line.get_train_location()
            if isinstance(unique_line, GenieBuildingLineGroup):
                train_location = dataset.unit_lines[train_location_id]
                train_location_name = UNIT_LINE_LOOKUPS[train_location_id][0]

            else:
                train_location = dataset.building_lines[train_location_id]
                train_location_name = BUILDING_LINE_LOOKUPS[train_location_id][0]

            patch_target_ref = "%s.Create" % (train_location_name)
            patch_target_expected_pointer = ExpectedPointer(train_location, patch_target_ref)

            # Wrapper
            wrapper_name = "Add%sCreatableWrapper" % (game_entity_name)
            wrapper_ref = "%s.%s" % (civ_name, wrapper_name)
            wrapper_location = ExpectedPointer(civ_group, civ_name)
            wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                                  wrapper_name,
                                                  dataset.nyan_api_objects,
                                                  wrapper_location)
            wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

            # Nyan patch
            nyan_patch_name = "Add%sCreatable" % (game_entity_name)
            nyan_patch_ref = "%s.%s.%s" % (civ_name, wrapper_name, nyan_patch_name)
            nyan_patch_location = ExpectedPointer(civ_group, wrapper_ref)
            nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                     nyan_patch_name,
                                                     dataset.nyan_api_objects,
                                                     nyan_patch_location)
            nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
            nyan_patch_raw_api_object.set_patch_target(patch_target_expected_pointer)

            # Add creatable
            creatable_ref = "%s.CreatableGameEntity" % (game_entity_name)
            creatable_expected_pointer = ExpectedPointer(unique_line, creatable_ref)
            nyan_patch_raw_api_object.add_raw_patch_member("creatables",
                                                           [creatable_expected_pointer],
                                                           "engine.ability.type.Create",
                                                           MemberOperator.ADD)

            patch_expected_pointer = ExpectedPointer(civ_group, nyan_patch_ref)
            wrapper_raw_api_object.add_raw_member("patch",
                                                  patch_expected_pointer,
                                                  "engine.aux.patch.Patch")

            civ_group.add_raw_api_object(wrapper_raw_api_object)
            civ_group.add_raw_api_object(nyan_patch_raw_api_object)

            wrapper_expected_pointer = ExpectedPointer(civ_group, wrapper_ref)
            patches.append(wrapper_expected_pointer)

        return patches

    @staticmethod
    def _setup_unique_techs(civ_group):
        """
        Patches the unique techs into their research location.
        """
        patches = []

        civ_id = civ_group.get_id()
        dataset = civ_group.data
        civ_name = CIV_GROUP_LOOKUPS[civ_id][0]

        for unique_tech in civ_group.unique_techs.values():
            tech_id = unique_tech.get_id()
            tech_name = TECH_GROUP_LOOKUPS[tech_id][0]

            # Get train location of line
            research_location_id = unique_tech.get_research_location_id()
            research_location = dataset.building_lines[research_location_id]
            research_location_name = BUILDING_LINE_LOOKUPS[research_location_id][0]

            patch_target_ref = "%s.Research" % (research_location_name)
            patch_target_expected_pointer = ExpectedPointer(research_location, patch_target_ref)

            # Wrapper
            wrapper_name = "Add%sResearchableWrapper" % (tech_name)
            wrapper_ref = "%s.%s" % (civ_name, wrapper_name)
            wrapper_location = ExpectedPointer(civ_group, civ_name)
            wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                                  wrapper_name,
                                                  dataset.nyan_api_objects,
                                                  wrapper_location)
            wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

            # Nyan patch
            nyan_patch_name = "Add%sResearchable" % (tech_name)
            nyan_patch_ref = "%s.%s.%s" % (civ_name, wrapper_name, nyan_patch_name)
            nyan_patch_location = ExpectedPointer(civ_group, wrapper_ref)
            nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                     nyan_patch_name,
                                                     dataset.nyan_api_objects,
                                                     nyan_patch_location)
            nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
            nyan_patch_raw_api_object.set_patch_target(patch_target_expected_pointer)

            # Add creatable
            researchable_ref = "%s.ResearchableTech" % (tech_name)
            researchable_expected_pointer = ExpectedPointer(unique_tech, researchable_ref)
            nyan_patch_raw_api_object.add_raw_patch_member("researchables",
                                                           [researchable_expected_pointer],
                                                           "engine.ability.type.Research",
                                                           MemberOperator.ADD)

            patch_expected_pointer = ExpectedPointer(civ_group, nyan_patch_ref)
            wrapper_raw_api_object.add_raw_member("patch",
                                                  patch_expected_pointer,
                                                  "engine.aux.patch.Patch")

            civ_group.add_raw_api_object(wrapper_raw_api_object)
            civ_group.add_raw_api_object(nyan_patch_raw_api_object)

            wrapper_expected_pointer = ExpectedPointer(civ_group, wrapper_ref)
            patches.append(wrapper_expected_pointer)

        return patches
