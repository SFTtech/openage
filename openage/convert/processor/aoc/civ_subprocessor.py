# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Creates patches and modifiers for civs.
"""
from openage.convert.dataformat.aoc.internal_nyan_names import CIV_GROUP_LOOKUPS,\
    UNIT_LINE_LOOKUPS, BUILDING_LINE_LOOKUPS, TECH_GROUP_LOOKUPS,\
    GRAPHICS_SET_LOOKUPS
from openage.convert.dataformat.aoc.expected_pointer import ExpectedPointer
from openage.convert.dataformat.converter_object import RawAPIObject
from openage.convert.dataformat.aoc.genie_unit import GenieBuildingLineGroup,\
    GenieVillagerGroup
from openage.nyan.nyan_structs import MemberOperator
from openage.convert.dataformat.aoc.combined_sprite import CombinedSprite
from openage.convert.processor.aoc.tech_subprocessor import AoCTechSubprocessor


class AoCCivSubprocessor:

    @classmethod
    def get_civ_setup(cls, civ_group):
        """
        Returns the patches for the civ setup which configures architecture sets
        unique units, unique techs, team boni and unique stat upgrades.
        """
        patches = []

        patches.extend(cls._setup_graphics_set(civ_group))
        patches.extend(cls._setup_unique_units(civ_group))
        patches.extend(cls._setup_unique_techs(civ_group))
        patches.extend(cls._setup_tech_tree(civ_group))

        # TODO: Civ bonus

        if len(civ_group.get_team_bonus_effects()) > 0:
            patches.extend(AoCTechSubprocessor.get_patches(civ_group))

        return patches

    @classmethod
    def get_modifiers(cls, civ_group):
        """
        Returns global modifiers of a civ.
        """
        modifiers = []

        # TODO: Implement

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

    @classmethod
    def create_graphics_sets(cls, full_data_set):
        """
        Creates patches for the graphics sets of civs.
        """
        graphics_sets = GRAPHICS_SET_LOOKUPS

        for set_id, graphics_set in graphics_sets.items():
            if set_id == 0:
                # The standard graphics set can be skipped
                continue

            # Use the first civ for creation
            civ_id = graphics_set[0][0]
            civ_group = full_data_set.civ_groups[civ_id]
            civ_units = civ_group.civ["units"].get_value()

            # We don't need a full diff, only a few animations change
            for building_line in full_data_set.building_lines.values():
                std_head_unit = building_line.get_head_unit()
                std_head_unit_id = building_line.get_head_unit_id()
                civ_head_unit = civ_units[std_head_unit_id]

                std_idle_animation_id = std_head_unit["idle_graphic0"].get_value()
                civ_idle_animation_id = civ_head_unit["idle_graphic0"].get_value()

                if std_idle_animation_id != civ_idle_animation_id:
                    cls._idle_graphics_set(building_line, civ_idle_animation_id,
                                           graphics_set[1], graphics_set[2])

                # TODO: Building upgrades

            for unit_line in full_data_set.unit_lines.values():
                if isinstance(unit_line, GenieVillagerGroup):
                    # Villagers have different IDs, but the sprites are unchanged
                    continue

                std_head_unit = unit_line.get_head_unit()
                std_head_unit_id = unit_line.get_head_unit_id()
                if std_head_unit_id in civ_units.keys():
                    civ_head_unit = civ_units[std_head_unit_id]

                else:
                    continue

                std_idle_animation_id = std_head_unit["idle_graphic0"].get_value()
                civ_idle_animation_id = civ_head_unit["idle_graphic0"].get_value()

                if std_idle_animation_id != civ_idle_animation_id:
                    cls._idle_graphics_set(unit_line, civ_idle_animation_id,
                                           graphics_set[1], graphics_set[2])

                # TODO: Other unit animations

    @staticmethod
    def _setup_graphics_set(civ_group):
        """
        Patches the graphics set in.
        """
        patches = []

        civ_id = civ_group.get_id()
        graphics_sets = GRAPHICS_SET_LOOKUPS
        civ_graphics_set_lookup = None
        dataset = civ_group.data

        for set_id, graphics_set in graphics_sets.items():
            if civ_id in graphics_set[0]:
                civ_graphics_set_lookup = graphics_set

                if set_id == 0:
                    # This is the standard set that doesn't need extra patching
                    return patches

                break

        civ_units = civ_group.civ["units"].get_value()
        graphics_set_name = civ_graphics_set_lookup[1]

        for building_line in dataset.building_lines.values():
            std_head_unit = building_line.get_head_unit()
            std_head_unit_id = building_line.get_head_unit_id()
            civ_head_unit = civ_units[std_head_unit_id]

            std_idle_animation_id = std_head_unit["idle_graphic0"].get_value()
            civ_idle_animation_id = civ_head_unit["idle_graphic0"].get_value()

            building_name = BUILDING_LINE_LOOKUPS[std_head_unit_id][0]

            if std_idle_animation_id != civ_idle_animation_id:
                graphics_change_expected_pointer = ExpectedPointer(building_line,
                                                                   "%s.%sIdleAnimationWrapper"
                                                                   % (building_name, graphics_set_name))
                patches.append(graphics_change_expected_pointer)

        for unit_line in dataset.unit_lines.values():
            if isinstance(unit_line, GenieVillagerGroup):
                # Villagers have different IDs, but the sprites are unchanged
                continue

            std_head_unit = unit_line.get_head_unit()
            std_head_unit_id = unit_line.get_head_unit_id()
            if std_head_unit_id in civ_units.keys():
                civ_head_unit = civ_units[std_head_unit_id]

            else:
                continue

            std_idle_animation_id = std_head_unit["idle_graphic0"].get_value()
            civ_idle_animation_id = civ_head_unit["idle_graphic0"].get_value()

            unit_name = UNIT_LINE_LOOKUPS[std_head_unit_id][0]

            if std_idle_animation_id != civ_idle_animation_id:
                graphics_change_expected_pointer = ExpectedPointer(unit_line,
                                                                   "%s.%sIdleAnimationWrapper"
                                                                   % (unit_name, graphics_set_name))
                patches.append(graphics_change_expected_pointer)

        return patches

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
            train_location_id = unique_line.get_train_location_id()
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

    @staticmethod
    def _setup_tech_tree(civ_group):
        """
        Patches standard techs and units out of Research and Create.
        """
        patches = []

        civ_id = civ_group.get_id()
        dataset = civ_group.data
        civ_name = CIV_GROUP_LOOKUPS[civ_id][0]

        disabled_techs = dict()
        disabled_entities = dict()

        tech_tree = civ_group.get_tech_tree_effects()
        for effect in tech_tree:
            type_id = effect.get_type()

            if type_id == 101:
                patches.extend(AoCTechSubprocessor._tech_cost_modify_effect(civ_group, effect))
                continue

            elif type_id == 103:
                patches.extend(AoCTechSubprocessor._tech_time_modify_effect(civ_group, effect))
                continue

            elif type_id != 102:
                continue

            # Get tech id
            tech_id = int(effect["attr_d"].get_value())

            # Check what the purpose of the tech is
            if tech_id in dataset.unit_unlocks.keys():
                unlock_tech = dataset.unit_unlocks[tech_id]
                unlocked_line = unlock_tech.get_unlocked_line()
                train_location_id = unlocked_line.get_train_location_id()

                if isinstance(unlocked_line, GenieBuildingLineGroup):
                    train_location = dataset.unit_lines[train_location_id]

                else:
                    train_location = dataset.building_lines[train_location_id]

                if train_location in disabled_entities.keys():
                    disabled_entities[train_location].append(unlocked_line)

                else:
                    disabled_entities[train_location] = [unlocked_line]

            elif tech_id in dataset.civ_boni.keys():
                # Disables civ boni of other civs
                continue

            elif tech_id in dataset.tech_groups.keys():
                tech_group = dataset.tech_groups[tech_id]
                if tech_group.is_researchable():
                    research_location_id = tech_group.get_research_location_id()
                    research_location = dataset.building_lines[research_location_id]

                    if research_location in disabled_techs.keys():
                        disabled_techs[research_location].append(tech_group)

                    else:
                        disabled_techs[research_location] = [tech_group]

            else:
                continue

        for train_location, entities in disabled_entities.items():
            train_location_id = train_location.get_head_unit_id()
            if isinstance(train_location, GenieBuildingLineGroup):
                train_location_name = BUILDING_LINE_LOOKUPS[train_location_id][0]

            else:
                train_location_name = UNIT_LINE_LOOKUPS[train_location_id][0]

            patch_target_ref = "%s.Create" % (train_location_name)
            patch_target_expected_pointer = ExpectedPointer(train_location, patch_target_ref)

            # Wrapper
            wrapper_name = "Disable%sCreatablesWrapper" % (train_location_name)
            wrapper_ref = "%s.%s" % (civ_name, wrapper_name)
            wrapper_location = ExpectedPointer(civ_group, civ_name)
            wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                                  wrapper_name,
                                                  dataset.nyan_api_objects,
                                                  wrapper_location)
            wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

            # Nyan patch
            nyan_patch_name = "Disable%sCreatables" % (train_location_name)
            nyan_patch_ref = "%s.%s.%s" % (civ_name, wrapper_name, nyan_patch_name)
            nyan_patch_location = ExpectedPointer(civ_group, wrapper_ref)
            nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                     nyan_patch_name,
                                                     dataset.nyan_api_objects,
                                                     nyan_patch_location)
            nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
            nyan_patch_raw_api_object.set_patch_target(patch_target_expected_pointer)

            entities_expected_pointers = []
            for entity in entities:
                entity_id = entity.get_head_unit_id()
                if isinstance(entity, GenieBuildingLineGroup):
                    game_entity_name = BUILDING_LINE_LOOKUPS[entity_id][0]

                else:
                    game_entity_name = UNIT_LINE_LOOKUPS[entity_id][0]

                disabled_ref = "%s.CreatableGameEntity" % (game_entity_name)
                disabled_expected_pointer = ExpectedPointer(entity, disabled_ref)
                entities_expected_pointers.append(disabled_expected_pointer)

            nyan_patch_raw_api_object.add_raw_patch_member("creatables",
                                                           entities_expected_pointers,
                                                           "engine.ability.type.Create",
                                                           MemberOperator.SUBTRACT)

            patch_expected_pointer = ExpectedPointer(civ_group, nyan_patch_ref)
            wrapper_raw_api_object.add_raw_member("patch",
                                                  patch_expected_pointer,
                                                  "engine.aux.patch.Patch")

            civ_group.add_raw_api_object(wrapper_raw_api_object)
            civ_group.add_raw_api_object(nyan_patch_raw_api_object)

            wrapper_expected_pointer = ExpectedPointer(civ_group, wrapper_ref)
            patches.append(wrapper_expected_pointer)

        for research_location, techs in disabled_techs.items():
            research_location_id = research_location.get_head_unit_id()
            research_location_name = BUILDING_LINE_LOOKUPS[research_location_id][0]

            patch_target_ref = "%s.Research" % (research_location_name)
            patch_target_expected_pointer = ExpectedPointer(research_location, patch_target_ref)

            # Wrapper
            wrapper_name = "Disable%sResearchablesWrapper" % (research_location_name)
            wrapper_ref = "%s.%s" % (civ_name, wrapper_name)
            wrapper_location = ExpectedPointer(civ_group, civ_name)
            wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                                  wrapper_name,
                                                  dataset.nyan_api_objects,
                                                  wrapper_location)
            wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

            # Nyan patch
            nyan_patch_name = "Disable%sResearchables" % (research_location_name)
            nyan_patch_ref = "%s.%s.%s" % (civ_name, wrapper_name, nyan_patch_name)
            nyan_patch_location = ExpectedPointer(civ_group, wrapper_ref)
            nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                     nyan_patch_name,
                                                     dataset.nyan_api_objects,
                                                     nyan_patch_location)
            nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
            nyan_patch_raw_api_object.set_patch_target(patch_target_expected_pointer)

            entities_expected_pointers = []
            for tech_group in techs:
                tech_id = tech_group.get_id()
                tech_name = TECH_GROUP_LOOKUPS[tech_id][0]

                disabled_ref = "%s.ResearchableTech" % (tech_name)
                disabled_expected_pointer = ExpectedPointer(tech_group, disabled_ref)
                entities_expected_pointers.append(disabled_expected_pointer)

            nyan_patch_raw_api_object.add_raw_patch_member("researchables",
                                                           entities_expected_pointers,
                                                           "engine.ability.type.Research",
                                                           MemberOperator.SUBTRACT)

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
    def _idle_graphics_set(line, animation_id, graphics_set_name, graphics_set_filename_prefix):
        """
        Creates patches for civ-specific graühics the Idle ability of a line.

        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        head_unit_id = line.get_head_unit_id()
        dataset = line.data

        patches = []

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[head_unit_id][0]

        patch_target_ref = "%s.Idle" % (game_entity_name)
        patch_target_expected_pointer = ExpectedPointer(line, patch_target_ref)

        # Wrapper
        wrapper_name = "%sIdleAnimationWrapper" % (graphics_set_name)
        wrapper_ref = "%s.%s" % (game_entity_name, wrapper_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Store cib graphic changes next to their game entity definition,
        wrapper_raw_api_object.set_location("data/game_entity/generic/%s/"
                                            % (name_lookup_dict[head_unit_id][1]))
        wrapper_raw_api_object.set_filename("%s_graphics_set" % (graphics_set_filename_prefix))

        # Nyan patch
        nyan_patch_name = "%sIdleAnimation" % (graphics_set_name)
        nyan_patch_ref = "%s.%s.%s" % (game_entity_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ExpectedPointer(line, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_expected_pointer)

        animations_set = []
        # Patch the new animation in
        animation_expected_pointer = AoCCivSubprocessor._create_animation(line,
                                                                          animation_id,
                                                                          nyan_patch_ref,
                                                                          "%sIdle" % (graphics_set_name),
                                                                          "%s_idle_"
                                                                          % (graphics_set_filename_prefix))
        animations_set.append(animation_expected_pointer)

        nyan_patch_raw_api_object.add_raw_patch_member("animations",
                                                       animations_set,
                                                       "engine.ability.specialization.AnimatedAbility",
                                                       MemberOperator.ASSIGN)

        patch_expected_pointer = ExpectedPointer(line, nyan_patch_ref)
        wrapper_raw_api_object.add_raw_member("patch",
                                              patch_expected_pointer,
                                              "engine.aux.patch.Patch")

        line.add_raw_api_object(wrapper_raw_api_object)
        line.add_raw_api_object(nyan_patch_raw_api_object)

        wrapper_expected_pointer = ExpectedPointer(line, wrapper_ref)
        patches.append(wrapper_expected_pointer)

        return patches

    @staticmethod
    def _create_animation(line, animation_id, nyan_patch_ref, animation_name, filename_prefix):
        """
        Generates an animation for an ability.
        """
        dataset = line.data

        animation_ref = "%s.%sAnimation" % (nyan_patch_ref, animation_name)
        animation_obj_name = "%sAnimation" % (animation_name)
        animation_raw_api_object = RawAPIObject(animation_ref, animation_obj_name,
                                                dataset.nyan_api_objects)
        animation_raw_api_object.add_raw_parent("engine.aux.graphics.Animation")
        animation_location = ExpectedPointer(line, nyan_patch_ref)
        animation_raw_api_object.set_location(animation_location)

        if animation_id in dataset.combined_sprites.keys():
            animation_sprite = dataset.combined_sprites[animation_id]

        else:
            if isinstance(line, GenieBuildingLineGroup):
                animation_filename = "%s%s" % (filename_prefix,
                                               BUILDING_LINE_LOOKUPS[line.get_head_unit_id()][1])

            else:
                animation_filename = "%s%s" % (filename_prefix,
                                               UNIT_LINE_LOOKUPS[line.get_head_unit_id()][1])

            animation_sprite = CombinedSprite(animation_id,
                                              animation_filename,
                                              dataset)
            dataset.combined_sprites.update({animation_sprite.get_id(): animation_sprite})

        animation_sprite.add_reference(animation_raw_api_object)

        animation_raw_api_object.add_raw_member("sprite", animation_sprite,
                                                "engine.aux.graphics.Animation")

        line.add_raw_api_object(animation_raw_api_object)

        animation_expected_pointer = ExpectedPointer(line, animation_ref)

        return animation_expected_pointer
