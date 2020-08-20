# Copyright 2020-2020 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-locals,too-many-lines,too-many-statements,too-many-public-methods,invalid-name
#
# TODO: Remove when all methods are implemented
# pylint: disable=unused-argument,line-too-long

"""
Creates upgrade patches for resource modification effects in AoC.
"""
from ....nyan.nyan_structs import MemberOperator
from ...entity_object.conversion.aoc.genie_tech import GenieTechEffectBundleGroup
from ...entity_object.conversion.converter_object import RawAPIObject
from ...service import internal_name_lookups
from ...value_object.conversion.forward_ref import ForwardRef


class AoCUpgradeResourceSubprocessor:
    """
    Creates raw API objects for resource upgrade effects in AoC.
    """

    @staticmethod
    def berserk_heal_rate_upgrade(converter_group, value, operator, team=False):
        """
        Creates a patch for the berserk heal rate modify effect (ID: 96).

        :param converter_group: Tech/Civ that gets the patch.
        :type converter_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The forward references for the generated patches.
        :rtype: list
        """
        berserk_id = 692
        dataset = converter_group.data
        line = dataset.unit_lines[berserk_id]

        patches = []

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        obj_id = converter_group.get_id()
        if isinstance(converter_group, GenieTechEffectBundleGroup):
            tech_lookup_dict = internal_name_lookups.get_tech_lookups(dataset.game_version)
            obj_name = tech_lookup_dict[obj_id][0]

        else:
            civ_lookup_dict = internal_name_lookups.get_civ_lookups(dataset.game_version)
            obj_name = civ_lookup_dict[obj_id][0]

        game_entity_name = name_lookup_dict[berserk_id][0]

        patch_target_ref = "%s.RegenerateHealth.HealthRate" % (game_entity_name)
        patch_target_forward_ref = ForwardRef(line, patch_target_ref)

        # Wrapper
        wrapper_name = "Change%sHealthRegenerationWrapper" % (game_entity_name)
        wrapper_ref = "%s.%s" % (obj_name, wrapper_name)
        wrapper_location = ForwardRef(converter_group, obj_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_name = "Change%sHealthRegeneration" % (game_entity_name)
        nyan_patch_ref = "%s.%s.%s" % (obj_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ForwardRef(converter_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

        # Regeneration is on a counter, so we have to invert the value
        value = 1 / value
        nyan_patch_raw_api_object.add_raw_patch_member("rate",
                                                       value,
                                                       "engine.aux.attribute.AttributeRate",
                                                       operator)

        patch_forward_ref = ForwardRef(converter_group, nyan_patch_ref)
        wrapper_raw_api_object.add_raw_member("patch",
                                              patch_forward_ref,
                                              "engine.aux.patch.Patch")

        if team:
            wrapper_raw_api_object.add_raw_parent("engine.aux.patch.type.DiplomaticPatch")
            stances = [
                dataset.nyan_api_objects["engine.aux.diplomatic_stance.type.Self"],
                dataset.pregen_nyan_objects["aux.diplomatic_stance.types.Friendly"].get_nyan_object()
            ]
            wrapper_raw_api_object.add_raw_member("stances",
                                                  stances,
                                                  "engine.aux.patch.type.DiplomaticPatch")

        converter_group.add_raw_api_object(wrapper_raw_api_object)
        converter_group.add_raw_api_object(nyan_patch_raw_api_object)

        wrapper_forward_ref = ForwardRef(converter_group, wrapper_ref)
        patches.append(wrapper_forward_ref)

        return patches

    @staticmethod
    def bonus_population_upgrade(converter_group, value, operator, team=False):
        """
        Creates a patch for the bonus population effect (ID: 32).

        :param converter_group: Tech/Civ that gets the patch.
        :type converter_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The forward references for the generated patches.
        :rtype: list
        """
        dataset = converter_group.data

        patches = []

        obj_id = converter_group.get_id()
        if isinstance(converter_group, GenieTechEffectBundleGroup):
            tech_lookup_dict = internal_name_lookups.get_tech_lookups(dataset.game_version)
            obj_name = tech_lookup_dict[obj_id][0]

        else:
            civ_lookup_dict = internal_name_lookups.get_civ_lookups(dataset.game_version)
            obj_name = civ_lookup_dict[obj_id][0]

        patch_target_ref = "aux.resource.types.PopulationSpace"
        patch_target = dataset.pregen_nyan_objects[patch_target_ref].get_nyan_object()

        # Wrapper
        wrapper_name = "ChangePopulationCapWrapper"
        wrapper_ref = "%s.%s" % (obj_name, wrapper_name)
        wrapper_location = ForwardRef(converter_group, obj_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_name = "ChangePopulationCap"
        nyan_patch_ref = "%s.%s.%s" % (obj_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ForwardRef(converter_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target)

        nyan_patch_raw_api_object.add_raw_patch_member("max_amount",
                                                       value,
                                                       "engine.aux.resource.ResourceContingent",
                                                       operator)

        patch_forward_ref = ForwardRef(converter_group, nyan_patch_ref)
        wrapper_raw_api_object.add_raw_member("patch",
                                              patch_forward_ref,
                                              "engine.aux.patch.Patch")

        if team:
            wrapper_raw_api_object.add_raw_parent("engine.aux.patch.type.DiplomaticPatch")
            stances = [
                dataset.nyan_api_objects["engine.aux.diplomatic_stance.type.Self"],
                dataset.pregen_nyan_objects["aux.diplomatic_stance.types.Friendly"].get_nyan_object()
            ]
            wrapper_raw_api_object.add_raw_member("stances",
                                                  stances,
                                                  "engine.aux.patch.type.DiplomaticPatch")

        converter_group.add_raw_api_object(wrapper_raw_api_object)
        converter_group.add_raw_api_object(nyan_patch_raw_api_object)

        wrapper_forward_ref = ForwardRef(converter_group, wrapper_ref)
        patches.append(wrapper_forward_ref)

        return patches

    @staticmethod
    def building_conversion_upgrade(converter_group, value, operator, team=False):
        """
        Creates a patch for the building conversion effect (ID: 28).

        :param converter_group: Tech/Civ that gets the patch.
        :type converter_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The forward references for the generated patches.
        :rtype: list
        """
        monk_id = 125
        dataset = converter_group.data
        line = dataset.unit_lines[monk_id]

        patches = []

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        obj_id = converter_group.get_id()
        if isinstance(converter_group, GenieTechEffectBundleGroup):
            tech_lookup_dict = internal_name_lookups.get_tech_lookups(dataset.game_version)
            obj_name = tech_lookup_dict[obj_id][0]

        else:
            civ_lookup_dict = internal_name_lookups.get_civ_lookups(dataset.game_version)
            obj_name = civ_lookup_dict[obj_id][0]

        game_entity_name = name_lookup_dict[monk_id][0]

        patch_target_ref = "%s.Convert" % (game_entity_name)
        patch_target_forward_ref = ForwardRef(line, patch_target_ref)

        # Building conversion

        # Wrapper
        wrapper_name = "EnableBuildingConversionWrapper"
        wrapper_ref = "%s.%s" % (obj_name, wrapper_name)
        wrapper_location = ForwardRef(converter_group, obj_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_name = "EnableBuildingConversion"
        nyan_patch_ref = "%s.%s.%s" % (obj_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ForwardRef(converter_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

        # New allowed types
        allowed_types = [dataset.pregen_nyan_objects["aux.game_entity_type.types.Building"].get_nyan_object()]
        nyan_patch_raw_api_object.add_raw_patch_member("allowed_types",
                                                       allowed_types,
                                                       "engine.ability.type.ApplyDiscreteEffect",
                                                       MemberOperator.ADD)

        # Blacklisted buildings
        tc_line = dataset.building_lines[109]
        farm_line = dataset.building_lines[50]
        fish_trap_line = dataset.building_lines[199]
        monastery_line = dataset.building_lines[104]
        castle_line = dataset.building_lines[82]
        palisade_line = dataset.building_lines[72]
        stone_wall_line = dataset.building_lines[117]
        stone_gate_line = dataset.building_lines[64]
        wonder_line = dataset.building_lines[276]

        blacklisted_forward_refs = [ForwardRef(tc_line, "TownCenter"),
                                    ForwardRef(farm_line, "Farm"),
                                    ForwardRef(fish_trap_line, "FishingTrap"),
                                    ForwardRef(monastery_line, "Monastery"),
                                    ForwardRef(castle_line, "Castle"),
                                    ForwardRef(palisade_line, "PalisadeWall"),
                                    ForwardRef(stone_wall_line, "StoneWall"),
                                    ForwardRef(stone_gate_line, "StoneGate"),
                                    ForwardRef(wonder_line, "Wonder"),
                                    ]
        nyan_patch_raw_api_object.add_raw_patch_member("blacklisted_entities",
                                                       blacklisted_forward_refs,
                                                       "engine.ability.type.ApplyDiscreteEffect",
                                                       MemberOperator.ADD)

        patch_forward_ref = ForwardRef(converter_group, nyan_patch_ref)
        wrapper_raw_api_object.add_raw_member("patch",
                                              patch_forward_ref,
                                              "engine.aux.patch.Patch")

        converter_group.add_raw_api_object(wrapper_raw_api_object)
        converter_group.add_raw_api_object(nyan_patch_raw_api_object)

        wrapper_forward_ref = ForwardRef(converter_group, wrapper_ref)
        patches.append(wrapper_forward_ref)

        # Siege unit conversion

        # Wrapper
        wrapper_name = "EnableSiegeUnitConversionWrapper"
        wrapper_ref = "%s.%s" % (obj_name, wrapper_name)
        wrapper_location = ForwardRef(converter_group, obj_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_name = "EnableSiegeUnitConversion"
        nyan_patch_ref = "%s.%s.%s" % (obj_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ForwardRef(converter_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

        # Blacklisted units
        blacklisted_entities = []
        for unit_line in dataset.unit_lines.values():
            if unit_line.get_class_id() in (13, 55):
                # Siege units
                blacklisted_name = name_lookup_dict[unit_line.get_head_unit_id()][0]
                blacklisted_entities.append(ForwardRef(unit_line, blacklisted_name))

        nyan_patch_raw_api_object.add_raw_patch_member("blacklisted_entities",
                                                       blacklisted_entities,
                                                       "engine.ability.type.ApplyDiscreteEffect",
                                                       MemberOperator.SUBTRACT)

        patch_forward_ref = ForwardRef(converter_group, nyan_patch_ref)
        wrapper_raw_api_object.add_raw_member("patch",
                                              patch_forward_ref,
                                              "engine.aux.patch.Patch")

        if team:
            wrapper_raw_api_object.add_raw_parent("engine.aux.patch.type.DiplomaticPatch")
            stances = [
                dataset.nyan_api_objects["engine.aux.diplomatic_stance.type.Self"],
                dataset.pregen_nyan_objects["aux.diplomatic_stance.types.Friendly"].get_nyan_object()
            ]
            wrapper_raw_api_object.add_raw_member("stances",
                                                  stances,
                                                  "engine.aux.patch.type.DiplomaticPatch")

        converter_group.add_raw_api_object(wrapper_raw_api_object)
        converter_group.add_raw_api_object(nyan_patch_raw_api_object)

        wrapper_forward_ref = ForwardRef(converter_group, wrapper_ref)
        patches.append(wrapper_forward_ref)

        return patches

    @staticmethod
    def chinese_tech_discount_upgrade(converter_group, value, operator, team=False):
        """
        Creates a patch for the chinese tech discount effect (ID: 85).

        :param converter_group: Tech/Civ that gets the patch.
        :type converter_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The forward references for the generated patches.
        :rtype: list
        """
        patches = []

        # TODO: Implement

        return patches

    @staticmethod
    def construction_speed_upgrade(converter_group, value, operator, team=False):
        """
        Creates a patch for the construction speed modify effect (ID: 195).

        :param converter_group: Tech/Civ that gets the patch.
        :type converter_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The forward references for the generated patches.
        :rtype: list
        """
        patches = []

        # TODO: Implement

        return patches

    @staticmethod
    def conversion_resistance_upgrade(converter_group, value, operator, team=False):
        """
        Creates a patch for the conversion resistance modify effect (ID: 77).

        :param converter_group: Tech/Civ that gets the patch.
        :type converter_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The forward references for the generated patches.
        :rtype: list
        """
        patches = []

        # TODO: Implement

        return patches

    @staticmethod
    def conversion_resistance_min_rounds_upgrade(converter_group, value, operator, team=False):
        """
        Creates a patch for the conversion resistance modify effect (ID: 178).

        :param converter_group: Tech/Civ that gets the patch.
        :type converter_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The forward references for the generated patches.
        :rtype: list
        """
        patches = []

        # TODO: Implement

        return patches

    @staticmethod
    def conversion_resistance_max_rounds_upgrade(converter_group, value, operator, team=False):
        """
        Creates a patch for the conversion resistance modify effect (ID: 179).

        :param converter_group: Tech/Civ that gets the patch.
        :type converter_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The forward references for the generated patches.
        :rtype: list
        """
        patches = []

        # TODO: Implement

        return patches

    @staticmethod
    def crenellations_upgrade(converter_group, value, operator, team=False):
        """
        Creates a patch for the crenellations effect (ID: 194).

        :param converter_group: Tech/Civ that gets the patch.
        :type converter_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The forward references for the generated patches.
        :rtype: list
        """
        patches = []

        # TODO: Implement

        return patches

    @staticmethod
    def faith_recharge_rate_upgrade(converter_group, value, operator, team=False):
        """
        Creates a patch for the faith_recharge_rate modify effect (ID: 35).

        :param converter_group: Tech/Civ that gets the patch.
        :type converter_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The forward references for the generated patches.
        :rtype: list
        """
        monk_id = 125
        dataset = converter_group.data
        line = dataset.unit_lines[monk_id]

        patches = []

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        obj_id = converter_group.get_id()
        if isinstance(converter_group, GenieTechEffectBundleGroup):
            tech_lookup_dict = internal_name_lookups.get_tech_lookups(dataset.game_version)
            obj_name = tech_lookup_dict[obj_id][0]

        else:
            civ_lookup_dict = internal_name_lookups.get_civ_lookups(dataset.game_version)
            obj_name = civ_lookup_dict[obj_id][0]

        game_entity_name = name_lookup_dict[monk_id][0]

        patch_target_ref = "%s.RegenerateFaith.FaithRate" % (game_entity_name)
        patch_target_forward_ref = ForwardRef(line, patch_target_ref)

        # Wrapper
        wrapper_name = "Change%sFaithRegenerationWrapper" % (game_entity_name)
        wrapper_ref = "%s.%s" % (obj_name, wrapper_name)
        wrapper_location = ForwardRef(converter_group, obj_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_name = "Change%sFaithRegeneration" % (game_entity_name)
        nyan_patch_ref = "%s.%s.%s" % (obj_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ForwardRef(converter_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

        nyan_patch_raw_api_object.add_raw_patch_member("rate",
                                                       value,
                                                       "engine.aux.attribute.AttributeRate",
                                                       operator)

        patch_forward_ref = ForwardRef(converter_group, nyan_patch_ref)
        wrapper_raw_api_object.add_raw_member("patch",
                                              patch_forward_ref,
                                              "engine.aux.patch.Patch")

        if team:
            wrapper_raw_api_object.add_raw_parent("engine.aux.patch.type.DiplomaticPatch")
            stances = [
                dataset.nyan_api_objects["engine.aux.diplomatic_stance.type.Self"],
                dataset.pregen_nyan_objects["aux.diplomatic_stance.types.Friendly"].get_nyan_object()
            ]
            wrapper_raw_api_object.add_raw_member("stances",
                                                  stances,
                                                  "engine.aux.patch.type.DiplomaticPatch")

        converter_group.add_raw_api_object(wrapper_raw_api_object)
        converter_group.add_raw_api_object(nyan_patch_raw_api_object)

        wrapper_forward_ref = ForwardRef(converter_group, wrapper_ref)
        patches.append(wrapper_forward_ref)

        return patches

    @staticmethod
    def farm_food_upgrade(converter_group, value, operator, team=False):
        """
        Creates a patch for the farm food modify effect (ID: 36).

        :param converter_group: Tech/Civ that gets the patch.
        :type converter_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The forward references for the generated patches.
        :rtype: list
        """
        farm_id = 50
        dataset = converter_group.data
        line = dataset.building_lines[farm_id]

        patches = []

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        obj_id = converter_group.get_id()
        if isinstance(converter_group, GenieTechEffectBundleGroup):
            tech_lookup_dict = internal_name_lookups.get_tech_lookups(dataset.game_version)
            obj_name = tech_lookup_dict[obj_id][0]

        else:
            civ_lookup_dict = internal_name_lookups.get_civ_lookups(dataset.game_version)
            obj_name = civ_lookup_dict[obj_id][0]

        game_entity_name = name_lookup_dict[farm_id][0]

        patch_target_ref = "%s.Harvestable.%sResourceSpot" % (game_entity_name, game_entity_name)
        patch_target_forward_ref = ForwardRef(line, patch_target_ref)

        # Wrapper
        wrapper_name = "Change%sFoodAmountWrapper" % (game_entity_name)
        wrapper_ref = "%s.%s" % (obj_name, wrapper_name)
        wrapper_location = ForwardRef(converter_group, obj_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_name = "Change%sFoodAmount" % (game_entity_name)
        nyan_patch_ref = "%s.%s.%s" % (obj_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ForwardRef(converter_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

        nyan_patch_raw_api_object.add_raw_patch_member("max_amount",
                                                       value,
                                                       "engine.aux.resource_spot.ResourceSpot",
                                                       operator)

        patch_forward_ref = ForwardRef(converter_group, nyan_patch_ref)
        wrapper_raw_api_object.add_raw_member("patch",
                                              patch_forward_ref,
                                              "engine.aux.patch.Patch")

        if team:
            wrapper_raw_api_object.add_raw_parent("engine.aux.patch.type.DiplomaticPatch")
            stances = [
                dataset.nyan_api_objects["engine.aux.diplomatic_stance.type.Self"],
                dataset.pregen_nyan_objects["aux.diplomatic_stance.types.Friendly"].get_nyan_object()
            ]
            wrapper_raw_api_object.add_raw_member("stances",
                                                  stances,
                                                  "engine.aux.patch.type.DiplomaticPatch")

        converter_group.add_raw_api_object(wrapper_raw_api_object)
        converter_group.add_raw_api_object(nyan_patch_raw_api_object)

        wrapper_forward_ref = ForwardRef(converter_group, wrapper_ref)
        patches.append(wrapper_forward_ref)

        return patches

    @staticmethod
    def gather_food_efficiency_upgrade(converter_group, value, operator, team=False):
        """
        Creates a patch for the food gathering efficiency modify effect (ID: 190).

        :param converter_group: Tech/Civ that gets the patch.
        :type converter_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The forward references for the generated patches.
        :rtype: list
        """
        patches = []

        # TODO: Implement

        return patches

    @staticmethod
    def gather_wood_efficiency_upgrade(converter_group, value, operator, team=False):
        """
        Creates a patch for the wood gathering efficiency modify effect (ID: 189).

        :param converter_group: Tech/Civ that gets the patch.
        :type converter_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The forward references for the generated patches.
        :rtype: list
        """
        patches = []

        # TODO: Implement

        return patches

    @staticmethod
    def gather_gold_efficiency_upgrade(converter_group, value, operator, team=False):
        """
        Creates a patch for the gold gathering efficiency modify effect (ID: 47).

        :param converter_group: Tech/Civ that gets the patch.
        :type converter_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The forward references for the generated patches.
        :rtype: list
        """
        patches = []

        # TODO: Implement

        return patches

    @staticmethod
    def gather_stone_efficiency_upgrade(converter_group, value, operator, team=False):
        """
        Creates a patch for the stone gathering efficiency modify effect (ID: 79).

        :param converter_group: Tech/Civ that gets the patch.
        :type converter_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The forward references for the generated patches.
        :rtype: list
        """
        patches = []

        # TODO: Implement

        return patches

    @staticmethod
    def heal_range_upgrade(converter_group, value, operator, team=False):
        """
        Creates a patch for the heal range modify effect (ID: 90).

        :param converter_group: Tech/Civ that gets the patch.
        :type converter_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The forward references for the generated patches.
        :rtype: list
        """
        monk_id = 125
        dataset = converter_group.data
        line = dataset.unit_lines[monk_id]

        patches = []

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        obj_id = converter_group.get_id()
        if isinstance(converter_group, GenieTechEffectBundleGroup):
            tech_lookup_dict = internal_name_lookups.get_tech_lookups(dataset.game_version)
            obj_name = tech_lookup_dict[obj_id][0]

        else:
            civ_lookup_dict = internal_name_lookups.get_civ_lookups(dataset.game_version)
            obj_name = civ_lookup_dict[obj_id][0]

        game_entity_name = name_lookup_dict[monk_id][0]

        patch_target_ref = "%s.Heal" % (game_entity_name)
        patch_target_forward_ref = ForwardRef(line, patch_target_ref)

        # Wrapper
        wrapper_name = "Change%sHealRangeWrapper" % (game_entity_name)
        wrapper_ref = "%s.%s" % (obj_name, wrapper_name)
        wrapper_location = ForwardRef(converter_group, obj_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_name = "Change%sHealRange" % (game_entity_name)
        nyan_patch_ref = "%s.%s.%s" % (obj_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ForwardRef(converter_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

        nyan_patch_raw_api_object.add_raw_patch_member("max_range",
                                                       value,
                                                       "engine.ability.type.RangedContinuousEffect",
                                                       operator)

        patch_forward_ref = ForwardRef(converter_group, nyan_patch_ref)
        wrapper_raw_api_object.add_raw_member("patch",
                                              patch_forward_ref,
                                              "engine.aux.patch.Patch")

        if team:
            wrapper_raw_api_object.add_raw_parent("engine.aux.patch.type.DiplomaticPatch")
            stances = [
                dataset.nyan_api_objects["engine.aux.diplomatic_stance.type.Self"],
                dataset.pregen_nyan_objects["aux.diplomatic_stance.types.Friendly"].get_nyan_object()
            ]
            wrapper_raw_api_object.add_raw_member("stances",
                                                  stances,
                                                  "engine.aux.patch.type.DiplomaticPatch")

        converter_group.add_raw_api_object(wrapper_raw_api_object)
        converter_group.add_raw_api_object(nyan_patch_raw_api_object)

        wrapper_forward_ref = ForwardRef(converter_group, wrapper_ref)
        patches.append(wrapper_forward_ref)

        return patches

    @staticmethod
    def heal_rate_upgrade(converter_group, value, operator, team=False):
        """
        Creates a patch for the heal rate modify effect (ID: 89).

        :param converter_group: Tech/Civ that gets the patch.
        :type converter_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The forward references for the generated patches.
        :rtype: list
        """
        patches = []

        # Unused in AoC

        return patches

    @staticmethod
    def herding_dominance_upgrade(converter_group, value, operator, team=False):
        """
        Creates a patch for the herding dominance effect (ID: 97).

        :param converter_group: Tech/Civ that gets the patch.
        :type converter_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The forward references for the generated patches.
        :rtype: list
        """
        patches = []

        # TODO: Implement

        return patches

    @staticmethod
    def heresy_upgrade(converter_group, value, operator, team=False):
        """
        Creates a patch for the heresy effect (ID: 192).

        :param converter_group: Tech/Civ that gets the patch.
        :type converter_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The forward references for the generated patches.
        :rtype: list
        """
        patches = []

        # TODO: Implement

        return patches

    @staticmethod
    def monk_conversion_upgrade(converter_group, value, operator, team=False):
        """
        Creates a patch for the monk conversion effect (ID: 27).

        :param converter_group: Tech/Civ that gets the patch.
        :type converter_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The forward references for the generated patches.
        :rtype: list
        """
        monk_id = 125
        dataset = converter_group.data
        line = dataset.unit_lines[monk_id]

        patches = []

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        obj_id = converter_group.get_id()
        if isinstance(converter_group, GenieTechEffectBundleGroup):
            tech_lookup_dict = internal_name_lookups.get_tech_lookups(dataset.game_version)
            obj_name = tech_lookup_dict[obj_id][0]

        else:
            civ_lookup_dict = internal_name_lookups.get_civ_lookups(dataset.game_version)
            obj_name = civ_lookup_dict[obj_id][0]

        game_entity_name = name_lookup_dict[monk_id][0]

        patch_target_ref = "%s.Convert" % (game_entity_name)
        patch_target_forward_ref = ForwardRef(line, patch_target_ref)

        # Wrapper
        wrapper_name = "Enable%sConversionWrapper" % (game_entity_name)
        wrapper_ref = "%s.%s" % (obj_name, wrapper_name)
        wrapper_location = ForwardRef(converter_group, obj_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_name = "Enable%sConversion" % (game_entity_name)
        nyan_patch_ref = "%s.%s.%s" % (obj_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ForwardRef(converter_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

        monk_forward_ref = ForwardRef(line, game_entity_name)
        nyan_patch_raw_api_object.add_raw_patch_member("blacklisted_entities",
                                                       [monk_forward_ref],
                                                       "engine.ability.type.ApplyDiscreteEffect",
                                                       MemberOperator.SUBTRACT)

        patch_forward_ref = ForwardRef(converter_group, nyan_patch_ref)
        wrapper_raw_api_object.add_raw_member("patch",
                                              patch_forward_ref,
                                              "engine.aux.patch.Patch")

        if team:
            wrapper_raw_api_object.add_raw_parent("engine.aux.patch.type.DiplomaticPatch")
            stances = [
                dataset.nyan_api_objects["engine.aux.diplomatic_stance.type.Self"],
                dataset.pregen_nyan_objects["aux.diplomatic_stance.types.Friendly"].get_nyan_object()
            ]
            wrapper_raw_api_object.add_raw_member("stances",
                                                  stances,
                                                  "engine.aux.patch.type.DiplomaticPatch")

        converter_group.add_raw_api_object(wrapper_raw_api_object)
        converter_group.add_raw_api_object(nyan_patch_raw_api_object)

        wrapper_forward_ref = ForwardRef(converter_group, wrapper_ref)
        patches.append(wrapper_forward_ref)

        return patches

    @staticmethod
    def relic_gold_bonus_upgrade(converter_group, value, operator, team=False):
        """
        Creates a patch for the relic gold bonus modify effect (ID: 191).

        :param converter_group: Tech/Civ that gets the patch.
        :type converter_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The forward references for the generated patches.
        :rtype: list
        """
        patches = []

        return patches

    @staticmethod
    def reveal_ally_upgrade(converter_group, value, operator, team=False):
        """
        Creates a patch for the reveal ally modify effect (ID: 50).

        :param converter_group: Tech/Civ that gets the patch.
        :type converter_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The forward references for the generated patches.
        :rtype: list
        """
        patches = []

        # TODO: Implement

        return patches

    @staticmethod
    def reveal_enemy_upgrade(converter_group, value, operator, team=False):
        """
        Creates a patch for the reveal enemy modify effect (ID: 183).

        :param converter_group: Tech/Civ that gets the patch.
        :type converter_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The forward references for the generated patches.
        :rtype: list
        """
        patches = []

        # TODO: Implement

        return patches

    @staticmethod
    def ship_conversion_upgrade(converter_group, value, operator, team=False):
        """
        Creates a patch for the ship conversion effect (ID: 87).

        :param converter_group: Tech/Civ that gets the patch.
        :type converter_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The forward references for the generated patches.
        :rtype: list
        """
        patches = []

        # Unused in AoC

        return patches

    @staticmethod
    def spies_discount_upgrade(converter_group, value, operator, team=False):
        """
        Creates a patch for the spies discount effect (ID: 197).

        :param converter_group: Tech/Civ that gets the patch.
        :type converter_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The forward references for the generated patches.
        :rtype: list
        """
        patches = []

        # TODO: Implement

        return patches

    @staticmethod
    def starting_food_upgrade(converter_group, value, operator, team=False):
        """
        Creates a patch for the starting food modify effect (ID: 91).

        :param converter_group: Tech/Civ that gets the patch.
        :type converter_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The forward references for the generated patches.
        :rtype: list
        """
        patches = []

        # TODO: Implement

        return patches

    @staticmethod
    def starting_wood_upgrade(converter_group, value, operator, team=False):
        """
        Creates a patch for the starting wood modify effect (ID: 92).

        :param converter_group: Tech/Civ that gets the patch.
        :type converter_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The forward references for the generated patches.
        :rtype: list
        """
        patches = []

        # TODO: Implement

        return patches

    @staticmethod
    def starting_villagers_upgrade(converter_group, value, operator, team=False):
        """
        Creates a patch for the starting villagers modify effect (ID: 84).

        :param converter_group: Tech/Civ that gets the patch.
        :type converter_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The forward references for the generated patches.
        :rtype: list
        """
        patches = []

        # TODO: Implement

        return patches

    @staticmethod
    def starting_population_space_upgrade(converter_group, value, operator, team=False):
        """
        Creates a patch for the starting popspace modify effect (ID: 4).

        :param converter_group: Tech/Civ that gets the patch.
        :type converter_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The forward references for the generated patches.
        :rtype: list
        """
        dataset = converter_group.data

        patches = []

        obj_id = converter_group.get_id()
        if isinstance(converter_group, GenieTechEffectBundleGroup):
            tech_lookup_dict = internal_name_lookups.get_tech_lookups(dataset.game_version)
            obj_name = tech_lookup_dict[obj_id][0]

        else:
            civ_lookup_dict = internal_name_lookups.get_civ_lookups(dataset.game_version)
            obj_name = civ_lookup_dict[obj_id][0]

        patch_target_ref = "aux.resource.types.PopulationSpace"
        patch_target = dataset.pregen_nyan_objects[patch_target_ref].get_nyan_object()

        # Wrapper
        wrapper_name = "ChangeInitialPopulationLimitWrapper"
        wrapper_ref = "%s.%s" % (obj_name, wrapper_name)
        wrapper_location = ForwardRef(converter_group, obj_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_name = "ChangeInitialPopulationLimit"
        nyan_patch_ref = "%s.%s.%s" % (obj_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ForwardRef(converter_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target)

        nyan_patch_raw_api_object.add_raw_patch_member("min_amount",
                                                       value,
                                                       "engine.aux.resource.ResourceContingent",
                                                       operator)

        patch_forward_ref = ForwardRef(converter_group, nyan_patch_ref)
        wrapper_raw_api_object.add_raw_member("patch",
                                              patch_forward_ref,
                                              "engine.aux.patch.Patch")

        if team:
            wrapper_raw_api_object.add_raw_parent("engine.aux.patch.type.DiplomaticPatch")
            stances = [
                dataset.nyan_api_objects["engine.aux.diplomatic_stance.type.Self"],
                dataset.pregen_nyan_objects["aux.diplomatic_stance.types.Friendly"].get_nyan_object()
            ]
            wrapper_raw_api_object.add_raw_member("stances",
                                                  stances,
                                                  "engine.aux.patch.type.DiplomaticPatch")

        converter_group.add_raw_api_object(wrapper_raw_api_object)
        converter_group.add_raw_api_object(nyan_patch_raw_api_object)

        wrapper_forward_ref = ForwardRef(converter_group, wrapper_ref)
        patches.append(wrapper_forward_ref)

        return patches

    @staticmethod
    def theocracy_upgrade(converter_group, value, operator, team=False):
        """
        Creates a patch for the theocracy effect (ID: 193).

        :param converter_group: Tech/Civ that gets the patch.
        :type converter_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The forward references for the generated patches.
        :rtype: list
        """
        patches = []

        # TODO: Implement

        return patches

    @staticmethod
    def trade_penalty_upgrade(converter_group, value, operator, team=False):
        """
        Creates a patch for the trade penalty modify effect (ID: 78).

        :param converter_group: Tech/Civ that gets the patch.
        :type converter_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The forward references for the generated patches.
        :rtype: list
        """
        patches = []

        # TODO: Implement

        return patches

    @staticmethod
    def tribute_inefficiency_upgrade(converter_group, value, operator, team=False):
        """
        Creates a patch for the tribute inefficiency modify effect (ID: 46).

        :param converter_group: Tech/Civ that gets the patch.
        :type converter_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The forward references for the generated patches.
        :rtype: list
        """
        patches = []

        # TODO: Implement

        return patches

    @staticmethod
    def wonder_time_increase_upgrade(converter_group, value, operator, team=False):
        """
        Creates a patch for the wonder time modify effect (ID: 196).

        :param converter_group: Tech/Civ that gets the patch.
        :type converter_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The forward references for the generated patches.
        :rtype: list
        """
        patches = []

        # TODO: Implement

        return patches
