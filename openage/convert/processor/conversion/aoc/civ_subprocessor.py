# Copyright 2020-2020 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-locals,too-many-statements,too-many-branches

"""
Creates patches and modifiers for civs.
"""
from .....nyan.nyan_structs import MemberOperator
from ....entity_object.conversion.aoc.genie_unit import GenieBuildingLineGroup
from ....entity_object.conversion.combined_sprite import CombinedSprite
from ....entity_object.conversion.converter_object import RawAPIObject
from ....service.conversion import internal_name_lookups
from ....value_object.conversion.forward_ref import ForwardRef
from .tech_subprocessor import AoCTechSubprocessor


class AoCCivSubprocessor:
    """
    Creates raw API objects for civs in AoC.
    """

    @classmethod
    def get_civ_setup(cls, civ_group):
        """
        Returns the patches for the civ setup which configures architecture sets
        unique units, unique techs, team boni and unique stat upgrades.
        """
        patches = []

        patches.extend(cls.setup_unique_units(civ_group))
        patches.extend(cls.setup_unique_techs(civ_group))
        patches.extend(cls.setup_tech_tree(civ_group))
        patches.extend(cls.setup_civ_bonus(civ_group))

        if len(civ_group.get_team_bonus_effects()) > 0:
            patches.extend(AoCTechSubprocessor.get_patches(civ_group.team_bonus))

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

        food_ref = f"{civ_name}.FoodStartingAmount"
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

        wood_ref = f"{civ_name}.WoodStartingAmount"
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

        gold_ref = f"{civ_name}.GoldStartingAmount"
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

        stone_ref = f"{civ_name}.StoneStartingAmount"
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

    @classmethod
    def setup_civ_bonus(cls, civ_group):
        """
        Returns global modifiers of a civ.
        """
        patches = []

        civ_id = civ_group.get_id()
        dataset = civ_group.data

        tech_lookup_dict = internal_name_lookups.get_tech_lookups(dataset.game_version)
        civ_lookup_dict = internal_name_lookups.get_civ_lookups(dataset.game_version)

        civ_name = civ_lookup_dict[civ_id][0]

        # key: tech_id; value patched in patches
        tech_patches = {}

        for civ_bonus in civ_group.civ_boni.values():
            if not civ_bonus.replaces_researchable_tech():
                bonus_patches = AoCTechSubprocessor.get_patches(civ_bonus)

                # civ boni might be unlocked by age ups. if so, patch them into the age up
                # patches are queued here
                required_tech_count = civ_bonus.tech["required_tech_count"].get_value()
                if required_tech_count > 0 and len(bonus_patches) > 0:
                    if required_tech_count == 1:
                        tech_id = civ_bonus.tech["required_techs"][0].get_value()

                    elif required_tech_count == 2:
                        tech_id = civ_bonus.tech["required_techs"][1].get_value()

                    if tech_id == 104:
                        # Skip Dark Age; it is not a tech in openage
                        patches.extend(bonus_patches)

                    elif tech_id in tech_patches.keys():
                        tech_patches[tech_id].extend(bonus_patches)

                    else:
                        tech_patches[tech_id] = bonus_patches

                else:
                    patches.extend(bonus_patches)

        for tech_id, patches in tech_patches.items():
            tech_group = dataset.tech_groups[tech_id]
            tech_name = tech_lookup_dict[tech_id][0]

            patch_target_ref = f"{tech_name}"
            patch_target_forward_ref = ForwardRef(tech_group, patch_target_ref)

            # Wrapper
            wrapper_name = f"{tech_name}CivBonusWrapper"
            wrapper_ref = f"{civ_name}.{wrapper_name}"
            wrapper_location = ForwardRef(civ_group, civ_name)
            wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                                  wrapper_name,
                                                  dataset.nyan_api_objects,
                                                  wrapper_location)
            wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

            # Nyan patch
            nyan_patch_name = f"{tech_name}CivBonus"
            nyan_patch_ref = f"{civ_name}.{wrapper_name}.{nyan_patch_name}"
            nyan_patch_location = ForwardRef(civ_group, wrapper_ref)
            nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                     nyan_patch_name,
                                                     dataset.nyan_api_objects,
                                                     nyan_patch_location)
            nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
            nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

            nyan_patch_raw_api_object.add_raw_patch_member("updates",
                                                           patches,
                                                           "engine.aux.tech.Tech",
                                                           MemberOperator.ADD)

            patch_forward_ref = ForwardRef(civ_group, nyan_patch_ref)
            wrapper_raw_api_object.add_raw_member("patch",
                                                  patch_forward_ref,
                                                  "engine.aux.patch.Patch")

            civ_group.add_raw_api_object(wrapper_raw_api_object)
            civ_group.add_raw_api_object(nyan_patch_raw_api_object)

            wrapper_forward_ref = ForwardRef(civ_group, wrapper_ref)
            patches.append(wrapper_forward_ref)

        return patches

    @staticmethod
    def setup_unique_units(civ_group):
        """
        Patches the unique units into their train location.
        """
        patches = []

        civ_id = civ_group.get_id()
        dataset = civ_group.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
        civ_lookup_dict = internal_name_lookups.get_civ_lookups(dataset.game_version)

        civ_name = civ_lookup_dict[civ_id][0]

        for unique_line in civ_group.unique_entities.values():
            head_unit_id = unique_line.get_head_unit_id()
            game_entity_name = name_lookup_dict[head_unit_id][0]

            # Get train location of line
            train_location_id = unique_line.get_train_location_id()
            if isinstance(unique_line, GenieBuildingLineGroup):
                train_location = dataset.unit_lines[train_location_id]
                train_location_name = name_lookup_dict[train_location_id][0]

            else:
                train_location = dataset.building_lines[train_location_id]
                train_location_name = name_lookup_dict[train_location_id][0]

            patch_target_ref = f"{train_location_name}.Create"
            patch_target_forward_ref = ForwardRef(train_location, patch_target_ref)

            # Wrapper
            wrapper_name = f"Add{game_entity_name}CreatableWrapper"
            wrapper_ref = f"{civ_name}.{wrapper_name}"
            wrapper_location = ForwardRef(civ_group, civ_name)
            wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                                  wrapper_name,
                                                  dataset.nyan_api_objects,
                                                  wrapper_location)
            wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

            # Nyan patch
            nyan_patch_name = f"Add{game_entity_name}Creatable"
            nyan_patch_ref = f"{civ_name}.{wrapper_name}.{nyan_patch_name}"
            nyan_patch_location = ForwardRef(civ_group, wrapper_ref)
            nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                     nyan_patch_name,
                                                     dataset.nyan_api_objects,
                                                     nyan_patch_location)
            nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
            nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

            # Add creatable
            creatable_ref = f"{game_entity_name}.CreatableGameEntity"
            creatable_forward_ref = ForwardRef(unique_line, creatable_ref)
            nyan_patch_raw_api_object.add_raw_patch_member("creatables",
                                                           [creatable_forward_ref],
                                                           "engine.ability.type.Create",
                                                           MemberOperator.ADD)

            patch_forward_ref = ForwardRef(civ_group, nyan_patch_ref)
            wrapper_raw_api_object.add_raw_member("patch",
                                                  patch_forward_ref,
                                                  "engine.aux.patch.Patch")

            civ_group.add_raw_api_object(wrapper_raw_api_object)
            civ_group.add_raw_api_object(nyan_patch_raw_api_object)

            wrapper_forward_ref = ForwardRef(civ_group, wrapper_ref)
            patches.append(wrapper_forward_ref)

        return patches

    @staticmethod
    def setup_unique_techs(civ_group):
        """
        Patches the unique techs into their research location.
        """
        patches = []

        civ_id = civ_group.get_id()
        dataset = civ_group.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
        tech_lookup_dict = internal_name_lookups.get_tech_lookups(dataset.game_version)
        civ_lookup_dict = internal_name_lookups.get_civ_lookups(dataset.game_version)

        civ_name = civ_lookup_dict[civ_id][0]

        for unique_tech in civ_group.unique_techs.values():
            tech_id = unique_tech.get_id()
            tech_name = tech_lookup_dict[tech_id][0]

            # Get train location of line
            research_location_id = unique_tech.get_research_location_id()
            research_location = dataset.building_lines[research_location_id]
            research_location_name = name_lookup_dict[research_location_id][0]

            patch_target_ref = f"{research_location_name}.Research"
            patch_target_forward_ref = ForwardRef(research_location, patch_target_ref)

            # Wrapper
            wrapper_name = f"Add{tech_name}ResearchableWrapper"
            wrapper_ref = f"{civ_name}.{wrapper_name}"
            wrapper_location = ForwardRef(civ_group, civ_name)
            wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                                  wrapper_name,
                                                  dataset.nyan_api_objects,
                                                  wrapper_location)
            wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

            # Nyan patch
            nyan_patch_name = f"Add{tech_name}Researchable"
            nyan_patch_ref = f"{civ_name}.{wrapper_name}.{nyan_patch_name}"
            nyan_patch_location = ForwardRef(civ_group, wrapper_ref)
            nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                     nyan_patch_name,
                                                     dataset.nyan_api_objects,
                                                     nyan_patch_location)
            nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
            nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

            # Add creatable
            researchable_ref = f"{tech_name}.ResearchableTech"
            researchable_forward_ref = ForwardRef(unique_tech, researchable_ref)
            nyan_patch_raw_api_object.add_raw_patch_member("researchables",
                                                           [researchable_forward_ref],
                                                           "engine.ability.type.Research",
                                                           MemberOperator.ADD)

            patch_forward_ref = ForwardRef(civ_group, nyan_patch_ref)
            wrapper_raw_api_object.add_raw_member("patch",
                                                  patch_forward_ref,
                                                  "engine.aux.patch.Patch")

            civ_group.add_raw_api_object(wrapper_raw_api_object)
            civ_group.add_raw_api_object(nyan_patch_raw_api_object)

            wrapper_forward_ref = ForwardRef(civ_group, wrapper_ref)
            patches.append(wrapper_forward_ref)

        return patches

    @staticmethod
    def setup_tech_tree(civ_group):
        """
        Patches standard techs and units out of Research and Create.
        """
        patches = []

        civ_id = civ_group.get_id()
        dataset = civ_group.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
        tech_lookup_dict = internal_name_lookups.get_tech_lookups(dataset.game_version)
        civ_lookup_dict = internal_name_lookups.get_civ_lookups(dataset.game_version)

        civ_name = civ_lookup_dict[civ_id][0]

        disabled_techs = dict()
        disabled_entities = dict()

        tech_tree = civ_group.get_tech_tree_effects()
        for effect in tech_tree:
            type_id = effect.get_type()

            if type_id == 101:
                patches.extend(AoCTechSubprocessor.tech_cost_modify_effect(civ_group, effect))
                continue

            if type_id == 103:
                patches.extend(AoCTechSubprocessor.tech_time_modify_effect(civ_group, effect))
                continue

            if type_id != 102:
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
            train_location_name = name_lookup_dict[train_location_id][0]

            patch_target_ref = f"{train_location_name}.Create"
            patch_target_forward_ref = ForwardRef(train_location, patch_target_ref)

            # Wrapper
            wrapper_name = f"Disable{train_location_name}CreatablesWrapper"
            wrapper_ref = f"{civ_name}.{wrapper_name}"
            wrapper_location = ForwardRef(civ_group, civ_name)
            wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                                  wrapper_name,
                                                  dataset.nyan_api_objects,
                                                  wrapper_location)
            wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

            # Nyan patch
            nyan_patch_name = f"Disable{train_location_name}Creatables"
            nyan_patch_ref = f"{civ_name}.{wrapper_name}.{nyan_patch_name}"
            nyan_patch_location = ForwardRef(civ_group, wrapper_ref)
            nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                     nyan_patch_name,
                                                     dataset.nyan_api_objects,
                                                     nyan_patch_location)
            nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
            nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

            entities_forward_refs = []
            for entity in entities:
                entity_id = entity.get_head_unit_id()
                game_entity_name = name_lookup_dict[entity_id][0]

                disabled_ref = f"{game_entity_name}.CreatableGameEntity"
                disabled_forward_ref = ForwardRef(entity, disabled_ref)
                entities_forward_refs.append(disabled_forward_ref)

            nyan_patch_raw_api_object.add_raw_patch_member("creatables",
                                                           entities_forward_refs,
                                                           "engine.ability.type.Create",
                                                           MemberOperator.SUBTRACT)

            patch_forward_ref = ForwardRef(civ_group, nyan_patch_ref)
            wrapper_raw_api_object.add_raw_member("patch",
                                                  patch_forward_ref,
                                                  "engine.aux.patch.Patch")

            civ_group.add_raw_api_object(wrapper_raw_api_object)
            civ_group.add_raw_api_object(nyan_patch_raw_api_object)

            wrapper_forward_ref = ForwardRef(civ_group, wrapper_ref)
            patches.append(wrapper_forward_ref)

        for research_location, techs in disabled_techs.items():
            research_location_id = research_location.get_head_unit_id()
            research_location_name = name_lookup_dict[research_location_id][0]

            patch_target_ref = f"{research_location_name}.Research"
            patch_target_forward_ref = ForwardRef(research_location, patch_target_ref)

            # Wrapper
            wrapper_name = f"Disable{research_location_name}ResearchablesWrapper"
            wrapper_ref = f"{civ_name}.{wrapper_name}"
            wrapper_location = ForwardRef(civ_group, civ_name)
            wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                                  wrapper_name,
                                                  dataset.nyan_api_objects,
                                                  wrapper_location)
            wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

            # Nyan patch
            nyan_patch_name = f"Disable{research_location_name}Researchables"
            nyan_patch_ref = f"{civ_name}.{wrapper_name}.{nyan_patch_name}"
            nyan_patch_location = ForwardRef(civ_group, wrapper_ref)
            nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                     nyan_patch_name,
                                                     dataset.nyan_api_objects,
                                                     nyan_patch_location)
            nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
            nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

            entities_forward_refs = []
            for tech_group in techs:
                tech_id = tech_group.get_id()
                tech_name = tech_lookup_dict[tech_id][0]

                disabled_ref = f"{tech_name}.ResearchableTech"
                disabled_forward_ref = ForwardRef(tech_group, disabled_ref)
                entities_forward_refs.append(disabled_forward_ref)

            nyan_patch_raw_api_object.add_raw_patch_member("researchables",
                                                           entities_forward_refs,
                                                           "engine.ability.type.Research",
                                                           MemberOperator.SUBTRACT)

            patch_forward_ref = ForwardRef(civ_group, nyan_patch_ref)
            wrapper_raw_api_object.add_raw_member("patch",
                                                  patch_forward_ref,
                                                  "engine.aux.patch.Patch")

            civ_group.add_raw_api_object(wrapper_raw_api_object)
            civ_group.add_raw_api_object(nyan_patch_raw_api_object)

            wrapper_forward_ref = ForwardRef(civ_group, wrapper_ref)
            patches.append(wrapper_forward_ref)

        return patches

    @staticmethod
    def create_animation(line, animation_id, nyan_patch_ref, animation_name, filename_prefix):
        """
        Generates an animation for an ability.
        """
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        animation_ref = f"{nyan_patch_ref}.{animation_name}Animation"
        animation_obj_name = f"{animation_name}Animation"
        animation_raw_api_object = RawAPIObject(animation_ref, animation_obj_name,
                                                dataset.nyan_api_objects)
        animation_raw_api_object.add_raw_parent("engine.aux.graphics.Animation")
        animation_location = ForwardRef(line, nyan_patch_ref)
        animation_raw_api_object.set_location(animation_location)

        if animation_id in dataset.combined_sprites.keys():
            animation_sprite = dataset.combined_sprites[animation_id]

        else:
            animation_filename = "%s%s" % (filename_prefix,
                                           name_lookup_dict[line.get_head_unit_id()][1])

            animation_sprite = CombinedSprite(animation_id,
                                              animation_filename,
                                              dataset)
            dataset.combined_sprites.update({animation_sprite.get_id(): animation_sprite})

        animation_sprite.add_reference(animation_raw_api_object)

        animation_raw_api_object.add_raw_member("sprite", animation_sprite,
                                                "engine.aux.graphics.Animation")

        line.add_raw_api_object(animation_raw_api_object)

        animation_forward_ref = ForwardRef(line, animation_ref)

        return animation_forward_ref
