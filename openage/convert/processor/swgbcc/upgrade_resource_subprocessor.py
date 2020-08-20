# Copyright 2020-2020 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-locals,too-many-lines,too-many-statements,too-many-public-methods
#
# TODO: Remove when all methods are implemented
# pylint: disable=unused-argument,line-too-long

"""
Creates upgrade patches for resource modification effects in SWGB.
"""
from ....nyan.nyan_structs import MemberOperator
from ...entity_object.conversion.aoc.genie_tech import GenieTechEffectBundleGroup
from ...entity_object.conversion.converter_object import RawAPIObject
from ...service import internal_name_lookups
from ...value_object.conversion.forward_ref import ForwardRef


class SWGBCCUpgradeResourceSubprocessor:
    """
    Creates raw API objects for resource upgrade effects in SWGB.
    """

    @staticmethod
    def assault_mech_anti_air_upgrade(converter_group, value, operator, team=False):
        """
        Creates a patch for the assault mech anti air effect (ID: 31).

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
        berserk_id = 8
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
        force_ids = [115, 180]
        dataset = converter_group.data

        patches = []

        for force_id in force_ids:
            line = dataset.unit_lines[force_id]

            name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

            obj_id = converter_group.get_id()
            if isinstance(converter_group, GenieTechEffectBundleGroup):
                tech_lookup_dict = internal_name_lookups.get_tech_lookups(dataset.game_version)
                obj_name = tech_lookup_dict[obj_id][0]

            else:
                civ_lookup_dict = internal_name_lookups.get_civ_lookups(dataset.game_version)
                obj_name = civ_lookup_dict[obj_id][0]

            game_entity_name = name_lookup_dict[force_id][0]

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
            allowed_types = [
                dataset.pregen_nyan_objects["aux.game_entity_type.types.Building"].get_nyan_object()
            ]
            nyan_patch_raw_api_object.add_raw_patch_member("allowed_types",
                                                           allowed_types,
                                                           "engine.ability.type.ApplyDiscreteEffect",
                                                           MemberOperator.ADD)

            # Blacklisted buildings
            tc_line = dataset.building_lines[109]
            farm_line = dataset.building_lines[50]
            temple_line = dataset.building_lines[104]
            wonder_line = dataset.building_lines[276]

            blacklisted_forward_refs = [ForwardRef(tc_line, "CommandCenter"),
                                        ForwardRef(farm_line, "Farm"),
                                        ForwardRef(temple_line, "Temple"),
                                        ForwardRef(wonder_line, "Monument"),
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

        return patches

    @staticmethod
    def cloak_upgrade(converter_group, value, operator, team=False):
        """
        Creates a patch for the force cloak effect (ID: 56).

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
    def concentration_upgrade(converter_group, value, operator, team=False):
        """
        Creates a patch for the concentration effect (ID: 87).

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
    def conversion_range_upgrade(converter_group, value, operator, team=False):
        """
        Creates a patch for the conversion range modify effect (ID: 5).

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
    def detect_cloak_upgrade(converter_group, value, operator, team=False):
        """
        Creates a patch for the force detect cloak effect (ID: 58).

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
        force_ids = [115, 180]
        dataset = converter_group.data

        patches = []

        for force_id in force_ids:
            line = dataset.unit_lines[force_id]

            name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

            obj_id = converter_group.get_id()
            if isinstance(converter_group, GenieTechEffectBundleGroup):
                tech_lookup_dict = internal_name_lookups.get_tech_lookups(dataset.game_version)
                obj_name = tech_lookup_dict[obj_id][0]

            else:
                civ_lookup_dict = internal_name_lookups.get_civ_lookups(dataset.game_version)
                obj_name = civ_lookup_dict[obj_id][0]

            game_entity_name = name_lookup_dict[force_id][0]

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
        medic_id = 939
        dataset = converter_group.data

        patches = []

        line = dataset.unit_lines[medic_id]

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        obj_id = converter_group.get_id()
        if isinstance(converter_group, GenieTechEffectBundleGroup):
            tech_lookup_dict = internal_name_lookups.get_tech_lookups(dataset.game_version)
            obj_name = tech_lookup_dict[obj_id][0]

        else:
            civ_lookup_dict = internal_name_lookups.get_civ_lookups(dataset.game_version)
            obj_name = civ_lookup_dict[obj_id][0]

        game_entity_name = name_lookup_dict[medic_id][0]

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
        force_ids = [115, 180]
        dataset = converter_group.data

        patches = []

        for force_id in force_ids:
            line = dataset.unit_lines[force_id]

            name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

            obj_id = converter_group.get_id()
            if isinstance(converter_group, GenieTechEffectBundleGroup):
                tech_lookup_dict = internal_name_lookups.get_tech_lookups(dataset.game_version)
                obj_name = tech_lookup_dict[obj_id][0]

            else:
                civ_lookup_dict = internal_name_lookups.get_civ_lookups(dataset.game_version)
                obj_name = civ_lookup_dict[obj_id][0]

            game_entity_name = name_lookup_dict[force_id][0]

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
    def shield_air_units_upgrade(converter_group, value, operator, team=False):
        """
        Creates a patch for the shield bomber/fighter effect (ID: 38).

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
    def shield_dropoff_time_upgrade(converter_group, value, operator, team=False):
        """
        Creates a patch for the shield dropoff time modify effect (ID: 26).

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
    def shield_power_core_upgrade(converter_group, value, operator, team=False):
        """
        Creates a patch for the shield power core effect (ID: 33).

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
    def shield_recharge_rate_upgrade(converter_group, value, operator, team=False):
        """
        Creates a patch for the shield recharge rate modify effect (ID: 10).

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
    def submarine_detect_upgrade(converter_group, value, operator, team=False):
        """
        Creates a patch for the submarine detect effect (ID: 23).

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

        # Unused

        return patches
