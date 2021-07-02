# Copyright 2019-2021 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-lines,too-many-locals,too-many-statements,too-many-branches
#
# TODO:
# pylint: disable=line-too-long

"""
Convert API-like objects to nyan objects. Subroutine of the
main AoC processor.
"""
from ....entity_object.conversion.aoc.genie_tech import UnitLineUpgrade
from ....entity_object.conversion.aoc.genie_unit import GenieGarrisonMode,\
    GenieMonkGroup, GenieStackBuildingGroup
from ....entity_object.conversion.aoc.genie_unit import GenieVillagerGroup
from ....entity_object.conversion.combined_terrain import CombinedTerrain
from ....entity_object.conversion.converter_object import RawAPIObject
from ....service.conversion import internal_name_lookups
from ....value_object.conversion.forward_ref import ForwardRef
from .ability_subprocessor import AoCAbilitySubprocessor
from .auxiliary_subprocessor import AoCAuxiliarySubprocessor
from .civ_subprocessor import AoCCivSubprocessor
from .modifier_subprocessor import AoCModifierSubprocessor
from .tech_subprocessor import AoCTechSubprocessor
from .upgrade_ability_subprocessor import AoCUpgradeAbilitySubprocessor


class AoCNyanSubprocessor:
    """
    Transform an AoC dataset to nyan objects.
    """

    @classmethod
    def convert(cls, gamedata):
        """
        Create nyan objects from the given dataset.
        """
        cls._process_game_entities(gamedata)
        cls._create_nyan_objects(gamedata)
        cls._create_nyan_members(gamedata)

        cls._check_objects(gamedata)

    @classmethod
    def _check_objects(cls, full_data_set):
        """
        Check if objects are valid.
        """
        for unit_line in full_data_set.unit_lines.values():
            unit_line.check_readiness()

        for building_line in full_data_set.building_lines.values():
            building_line.check_readiness()

        for ambient_group in full_data_set.ambient_groups.values():
            ambient_group.check_readiness()

        for variant_group in full_data_set.variant_groups.values():
            variant_group.check_readiness()

        for tech_group in full_data_set.tech_groups.values():
            tech_group.check_readiness()

        for terrain_group in full_data_set.terrain_groups.values():
            terrain_group.check_readiness()

        for civ_group in full_data_set.civ_groups.values():
            civ_group.check_readiness()

    @classmethod
    def _create_nyan_objects(cls, full_data_set):
        """
        Creates nyan objects from the API objects.
        """
        for unit_line in full_data_set.unit_lines.values():
            unit_line.create_nyan_objects()
            unit_line.execute_raw_member_pushs()

        for building_line in full_data_set.building_lines.values():
            building_line.create_nyan_objects()
            building_line.execute_raw_member_pushs()

        for ambient_group in full_data_set.ambient_groups.values():
            ambient_group.create_nyan_objects()
            ambient_group.execute_raw_member_pushs()

        for variant_group in full_data_set.variant_groups.values():
            variant_group.create_nyan_objects()
            variant_group.execute_raw_member_pushs()

        for tech_group in full_data_set.tech_groups.values():
            tech_group.create_nyan_objects()
            tech_group.execute_raw_member_pushs()

        for terrain_group in full_data_set.terrain_groups.values():
            terrain_group.create_nyan_objects()
            terrain_group.execute_raw_member_pushs()

        for civ_group in full_data_set.civ_groups.values():
            civ_group.create_nyan_objects()
            civ_group.execute_raw_member_pushs()

    @classmethod
    def _create_nyan_members(cls, full_data_set):
        """
        Fill nyan member values of the API objects.
        """
        for unit_line in full_data_set.unit_lines.values():
            unit_line.create_nyan_members()

        for building_line in full_data_set.building_lines.values():
            building_line.create_nyan_members()

        for ambient_group in full_data_set.ambient_groups.values():
            ambient_group.create_nyan_members()

        for variant_group in full_data_set.variant_groups.values():
            variant_group.create_nyan_members()

        for tech_group in full_data_set.tech_groups.values():
            tech_group.create_nyan_members()

        for terrain_group in full_data_set.terrain_groups.values():
            terrain_group.create_nyan_members()

        for civ_group in full_data_set.civ_groups.values():
            civ_group.create_nyan_members()

    @classmethod
    def _process_game_entities(cls, full_data_set):
        """
        Create the RawAPIObject representation of the objects.
        """
        for unit_line in full_data_set.unit_lines.values():
            cls.unit_line_to_game_entity(unit_line)

        for building_line in full_data_set.building_lines.values():
            cls.building_line_to_game_entity(building_line)

        for ambient_group in full_data_set.ambient_groups.values():
            cls.ambient_group_to_game_entity(ambient_group)

        for variant_group in full_data_set.variant_groups.values():
            cls.variant_group_to_game_entity(variant_group)

        for tech_group in full_data_set.tech_groups.values():
            if tech_group.is_researchable():
                cls.tech_group_to_tech(tech_group)

        for terrain_group in full_data_set.terrain_groups.values():
            cls.terrain_group_to_terrain(terrain_group)

        for civ_group in full_data_set.civ_groups.values():
            cls.civ_group_to_civ(civ_group)

    @staticmethod
    def unit_line_to_game_entity(unit_line):
        """
        Creates raw API objects for a unit line.

        :param unit_line: Unit line that gets converted to a game entity.
        :type unit_line: ..dataformat.converter_object.ConverterObjectGroup
        """
        current_unit = unit_line.get_head_unit()
        current_unit_id = unit_line.get_head_unit_id()

        dataset = unit_line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
        class_lookup_dict = internal_name_lookups.get_class_lookups(dataset.game_version)

        # Start with the generic GameEntity
        game_entity_name = name_lookup_dict[current_unit_id][0]
        obj_location = f"data/game_entity/generic/{name_lookup_dict[current_unit_id][1]}/"
        raw_api_object = RawAPIObject(game_entity_name, game_entity_name,
                                      dataset.nyan_api_objects)
        raw_api_object.add_raw_parent("engine.util.game_entity.GameEntity")
        raw_api_object.set_location(obj_location)
        raw_api_object.set_filename(name_lookup_dict[current_unit_id][1])
        unit_line.add_raw_api_object(raw_api_object)

        # =======================================================================
        # Game Entity Types
        # =======================================================================
        # we give a unit two types
        #    - util.game_entity_type.types.Unit (if unit_type >= 70)
        #    - util.game_entity_type.types.<Class> (depending on the class)
        # =======================================================================
        # Create or use existing auxiliary types
        types_set = []
        unit_type = current_unit["unit_type"].get_value()

        if unit_type >= 70:
            type_obj = dataset.pregen_nyan_objects["util.game_entity_type.types.Unit"].get_nyan_object()
            types_set.append(type_obj)

        unit_class = current_unit["unit_class"].get_value()
        class_name = class_lookup_dict[unit_class]
        class_obj_name = f"util.game_entity_type.types.{class_name}"
        type_obj = dataset.pregen_nyan_objects[class_obj_name].get_nyan_object()
        types_set.append(type_obj)

        raw_api_object.add_raw_member("types", types_set, "engine.util.game_entity.GameEntity")

        # =======================================================================
        # Abilities
        # =======================================================================
        abilities_set = []

        abilities_set.append(AoCAbilitySubprocessor.death_ability(unit_line))
        abilities_set.append(AoCAbilitySubprocessor.delete_ability(unit_line))
        abilities_set.append(AoCAbilitySubprocessor.despawn_ability(unit_line))
        abilities_set.append(AoCAbilitySubprocessor.idle_ability(unit_line))
        abilities_set.append(AoCAbilitySubprocessor.hitbox_ability(unit_line))
        abilities_set.append(AoCAbilitySubprocessor.live_ability(unit_line))
        abilities_set.append(AoCAbilitySubprocessor.los_ability(unit_line))
        abilities_set.append(AoCAbilitySubprocessor.move_ability(unit_line))
        abilities_set.append(AoCAbilitySubprocessor.named_ability(unit_line))
        abilities_set.append(AoCAbilitySubprocessor.resistance_ability(unit_line))
        abilities_set.extend(AoCAbilitySubprocessor.selectable_ability(unit_line))
        abilities_set.append(AoCAbilitySubprocessor.stop_ability(unit_line))
        abilities_set.append(AoCAbilitySubprocessor.terrain_requirement_ability(unit_line))
        abilities_set.append(AoCAbilitySubprocessor.turn_ability(unit_line))
        abilities_set.append(AoCAbilitySubprocessor.visibility_ability(unit_line))

        # Creation
        if len(unit_line.creates) > 0:
            abilities_set.append(AoCAbilitySubprocessor.create_ability(unit_line))

        # Config
        ability = AoCAbilitySubprocessor.use_contingent_ability(unit_line)
        if ability:
            abilities_set.append(ability)

        if unit_line.get_head_unit_id() in (125, 692):
            # Healing/Recharging attribute points (monks, berserks)
            abilities_set.extend(AoCAbilitySubprocessor.regenerate_attribute_ability(unit_line))

        # Applying effects and shooting projectiles
        if unit_line.is_projectile_shooter():
            abilities_set.append(AoCAbilitySubprocessor.shoot_projectile_ability(unit_line, 7))
            AoCNyanSubprocessor.projectiles_from_line(unit_line)

        elif unit_line.is_melee() or unit_line.is_ranged():
            if unit_line.has_command(7):
                # Attack
                abilities_set.append(AoCAbilitySubprocessor.apply_discrete_effect_ability(unit_line,
                                                                                          7,
                                                                                          unit_line.is_ranged()))

            if unit_line.has_command(101):
                # Build
                abilities_set.append(AoCAbilitySubprocessor.apply_continuous_effect_ability(unit_line,
                                                                                            101,
                                                                                            unit_line.is_ranged()))

            if unit_line.has_command(104):
                # convert
                abilities_set.append(AoCAbilitySubprocessor.apply_discrete_effect_ability(unit_line,
                                                                                          104,
                                                                                          unit_line.is_ranged()))

            if unit_line.has_command(105):
                # Heal
                abilities_set.append(AoCAbilitySubprocessor.apply_continuous_effect_ability(unit_line,
                                                                                            105,
                                                                                            unit_line.is_ranged()))

            if unit_line.has_command(106):
                # Repair
                abilities_set.append(AoCAbilitySubprocessor.apply_continuous_effect_ability(unit_line,
                                                                                            106,
                                                                                            unit_line.is_ranged()))

        # Formation/Stance
        if not isinstance(unit_line, GenieVillagerGroup):
            abilities_set.append(AoCAbilitySubprocessor.formation_ability(unit_line))
            abilities_set.append(AoCAbilitySubprocessor.game_entity_stance_ability(unit_line))

        # Storage abilities
        if unit_line.is_garrison():
            abilities_set.append(AoCAbilitySubprocessor.storage_ability(unit_line))
            abilities_set.append(AoCAbilitySubprocessor.remove_storage_ability(unit_line))

            garrison_mode = unit_line.get_garrison_mode()

            if garrison_mode == GenieGarrisonMode.MONK:
                abilities_set.append(AoCAbilitySubprocessor.collect_storage_ability(unit_line))

        if len(unit_line.garrison_locations) > 0:
            ability = AoCAbilitySubprocessor.enter_container_ability(unit_line)
            if ability:
                abilities_set.append(ability)

            ability = AoCAbilitySubprocessor.exit_container_ability(unit_line)
            if ability:
                abilities_set.append(ability)

        if isinstance(unit_line, GenieMonkGroup):
            abilities_set.append(AoCAbilitySubprocessor.transfer_storage_ability(unit_line))

        # Resource abilities
        if unit_line.is_gatherer():
            abilities_set.append(AoCAbilitySubprocessor.drop_resources_ability(unit_line))
            abilities_set.extend(AoCAbilitySubprocessor.gather_ability(unit_line))

        # Resource storage
        if unit_line.is_gatherer() or unit_line.has_command(111):
            abilities_set.append(AoCAbilitySubprocessor.resource_storage_ability(unit_line))

        if isinstance(unit_line, GenieVillagerGroup):
            # Farm restocking
            abilities_set.append(AoCAbilitySubprocessor.restock_ability(unit_line, 50))

        if unit_line.is_harvestable():
            abilities_set.append(AoCAbilitySubprocessor.harvestable_ability(unit_line))

        if unit_type == 70 and unit_line.get_class_id() not in (9, 10, 58):
            # Excludes trebuchets and animals
            abilities_set.append(AoCAbilitySubprocessor.herd_ability(unit_line))

        if unit_line.get_class_id() == 58:
            abilities_set.append(AoCAbilitySubprocessor.herdable_ability(unit_line))

        # Trade abilities
        if unit_line.has_command(111):
            abilities_set.append(AoCAbilitySubprocessor.trade_ability(unit_line))

        # =======================================================================
        # TODO: Transform
        # =======================================================================
        raw_api_object.add_raw_member("abilities", abilities_set,
                                      "engine.util.game_entity.GameEntity")

        # =======================================================================
        # Modifiers
        # =======================================================================
        modifiers_set = []

        if unit_line.has_command(7) and not unit_line.is_projectile_shooter():
            modifiers_set.extend(AoCModifierSubprocessor.elevation_attack_modifiers(unit_line))

        if unit_line.is_gatherer():
            modifiers_set.extend(AoCModifierSubprocessor.gather_rate_modifier(unit_line))

        raw_api_object.add_raw_member("modifiers", modifiers_set,
                                      "engine.util.game_entity.GameEntity")

        # =======================================================================
        # TODO: Variants
        # =======================================================================
        raw_api_object.add_raw_member("variants", [], "engine.util.game_entity.GameEntity")

        # =======================================================================
        # Misc (Objects that are not used by the unit line itself, but use its values)
        # =======================================================================
        if unit_line.is_creatable():
            AoCAuxiliarySubprocessor.get_creatable_game_entity(unit_line)

    @staticmethod
    def building_line_to_game_entity(building_line):
        """
        Creates raw API objects for a building line.

        :param building_line: Building line that gets converted to a game entity.
        :type building_line: ..dataformat.converter_object.ConverterObjectGroup
        """
        current_building = building_line.line[0]
        current_building_id = building_line.get_head_unit_id()
        dataset = building_line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
        class_lookup_dict = internal_name_lookups.get_class_lookups(dataset.game_version)

        # Start with the generic GameEntity
        game_entity_name = name_lookup_dict[current_building_id][0]
        obj_location = f"data/game_entity/generic/{name_lookup_dict[current_building_id][1]}/"
        raw_api_object = RawAPIObject(game_entity_name, game_entity_name,
                                      dataset.nyan_api_objects)
        raw_api_object.add_raw_parent("engine.util.game_entity.GameEntity")
        raw_api_object.set_location(obj_location)
        raw_api_object.set_filename(name_lookup_dict[current_building_id][1])
        building_line.add_raw_api_object(raw_api_object)

        # =======================================================================
        # Game Entity Types
        # =======================================================================
        # we give a building two types
        #    - util.game_entity_type.types.Building (if unit_type >= 80)
        #    - util.game_entity_type.types.<Class> (depending on the class)
        # and additionally
        #    - util.game_entity_type.types.DropSite (only if this is used as a drop site)
        # =======================================================================
        # Create or use existing auxiliary types
        types_set = []
        unit_type = current_building["unit_type"].get_value()

        if unit_type >= 80:
            type_obj = dataset.pregen_nyan_objects["util.game_entity_type.types.Building"].get_nyan_object()
            types_set.append(type_obj)

        unit_class = current_building["unit_class"].get_value()
        class_name = class_lookup_dict[unit_class]
        class_obj_name = f"util.game_entity_type.types.{class_name}"
        type_obj = dataset.pregen_nyan_objects[class_obj_name].get_nyan_object()
        types_set.append(type_obj)

        if building_line.is_dropsite():
            type_obj = dataset.pregen_nyan_objects["util.game_entity_type.types.DropSite"].get_nyan_object()
            types_set.append(type_obj)

        raw_api_object.add_raw_member("types", types_set, "engine.util.game_entity.GameEntity")

        # =======================================================================
        # Abilities
        # =======================================================================
        abilities_set = []

        abilities_set.append(AoCAbilitySubprocessor.attribute_change_tracker_ability(building_line))
        abilities_set.append(AoCAbilitySubprocessor.death_ability(building_line))
        abilities_set.append(AoCAbilitySubprocessor.delete_ability(building_line))
        abilities_set.append(AoCAbilitySubprocessor.despawn_ability(building_line))
        abilities_set.append(AoCAbilitySubprocessor.idle_ability(building_line))
        abilities_set.append(AoCAbilitySubprocessor.hitbox_ability(building_line))
        abilities_set.append(AoCAbilitySubprocessor.live_ability(building_line))
        abilities_set.append(AoCAbilitySubprocessor.los_ability(building_line))
        abilities_set.append(AoCAbilitySubprocessor.named_ability(building_line))
        abilities_set.append(AoCAbilitySubprocessor.resistance_ability(building_line))
        abilities_set.extend(AoCAbilitySubprocessor.selectable_ability(building_line))
        abilities_set.append(AoCAbilitySubprocessor.stop_ability(building_line))
        abilities_set.append(AoCAbilitySubprocessor.terrain_requirement_ability(building_line))
        abilities_set.append(AoCAbilitySubprocessor.visibility_ability(building_line))

        # Config abilities
        if building_line.is_creatable():
            abilities_set.append(AoCAbilitySubprocessor.constructable_ability(building_line))

        if building_line.is_passable() or\
                (isinstance(building_line, GenieStackBuildingGroup) and building_line.is_gate()):
            abilities_set.append(AoCAbilitySubprocessor.passable_ability(building_line))

        if building_line.has_foundation():
            if building_line.get_class_id() == 49:
                # Use OverlayTerrain for the farm terrain
                abilities_set.append(AoCAbilitySubprocessor.overlay_terrain_ability(building_line))
                abilities_set.append(AoCAbilitySubprocessor.foundation_ability(building_line,
                                                                               terrain_id=27))

            else:
                abilities_set.append(AoCAbilitySubprocessor.foundation_ability(building_line))

        # Creation/Research abilities
        if len(building_line.creates) > 0:
            abilities_set.append(AoCAbilitySubprocessor.create_ability(building_line))
            abilities_set.append(AoCAbilitySubprocessor.production_queue_ability(building_line))

        if len(building_line.researches) > 0:
            abilities_set.append(AoCAbilitySubprocessor.research_ability(building_line))

        # Effect abilities
        if building_line.is_projectile_shooter():
            abilities_set.append(AoCAbilitySubprocessor.shoot_projectile_ability(building_line, 7))
            abilities_set.append(AoCAbilitySubprocessor.game_entity_stance_ability(building_line))
            AoCNyanSubprocessor.projectiles_from_line(building_line)

        # Storage abilities
        if building_line.is_garrison():
            abilities_set.append(AoCAbilitySubprocessor.storage_ability(building_line))
            abilities_set.append(AoCAbilitySubprocessor.remove_storage_ability(building_line))

            garrison_mode = building_line.get_garrison_mode()

            if garrison_mode == GenieGarrisonMode.NATURAL:
                abilities_set.append(AoCAbilitySubprocessor.send_back_to_task_ability(building_line))

            if garrison_mode in (GenieGarrisonMode.NATURAL, GenieGarrisonMode.SELF_PRODUCED):
                abilities_set.append(AoCAbilitySubprocessor.rally_point_ability(building_line))

        # Resource abilities
        if building_line.is_harvestable():
            abilities_set.append(AoCAbilitySubprocessor.harvestable_ability(building_line))

        if building_line.is_dropsite():
            abilities_set.append(AoCAbilitySubprocessor.drop_site_ability(building_line))

        ability = AoCAbilitySubprocessor.provide_contingent_ability(building_line)
        if ability:
            abilities_set.append(ability)

        # Trade abilities
        if building_line.is_trade_post():
            abilities_set.append(AoCAbilitySubprocessor.trade_post_ability(building_line))

        if building_line.get_id() == 84:
            # Market trading
            abilities_set.extend(AoCAbilitySubprocessor.exchange_resources_ability(building_line))

        raw_api_object.add_raw_member("abilities", abilities_set,
                                      "engine.util.game_entity.GameEntity")

        # =======================================================================
        # Modifiers
        # =======================================================================
        raw_api_object.add_raw_member("modifiers", [], "engine.util.game_entity.GameEntity")

        # =======================================================================
        # TODO: Variants
        # =======================================================================
        raw_api_object.add_raw_member("variants", [], "engine.util.game_entity.GameEntity")

        # =======================================================================
        # Misc (Objects that are not used by the unit line itself, but use its values)
        # =======================================================================
        if building_line.is_creatable():
            AoCAuxiliarySubprocessor.get_creatable_game_entity(building_line)

    @staticmethod
    def ambient_group_to_game_entity(ambient_group):
        """
        Creates raw API objects for an ambient group.

        :param ambient_group: Unit line that gets converted to a game entity.
        :type ambient_group: ..dataformat.converter_object.ConverterObjectGroup
        """
        ambient_unit = ambient_group.get_head_unit()
        ambient_id = ambient_group.get_head_unit_id()

        dataset = ambient_group.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
        class_lookup_dict = internal_name_lookups.get_class_lookups(dataset.game_version)

        # Start with the generic GameEntity
        game_entity_name = name_lookup_dict[ambient_id][0]
        obj_location = f"data/game_entity/generic/{name_lookup_dict[ambient_id][1]}/"
        raw_api_object = RawAPIObject(game_entity_name, game_entity_name,
                                      dataset.nyan_api_objects)
        raw_api_object.add_raw_parent("engine.util.game_entity.GameEntity")
        raw_api_object.set_location(obj_location)
        raw_api_object.set_filename(name_lookup_dict[ambient_id][1])
        ambient_group.add_raw_api_object(raw_api_object)

        # =======================================================================
        # Game Entity Types
        # =======================================================================
        # we give an ambient the types
        #    - util.game_entity_type.types.Ambient
        # =======================================================================
        # Create or use existing auxiliary types
        types_set = []

        type_obj = dataset.pregen_nyan_objects["util.game_entity_type.types.Ambient"].get_nyan_object()
        types_set.append(type_obj)

        unit_class = ambient_unit["unit_class"].get_value()
        class_name = class_lookup_dict[unit_class]
        class_obj_name = f"util.game_entity_type.types.{class_name}"
        type_obj = dataset.pregen_nyan_objects[class_obj_name].get_nyan_object()
        types_set.append(type_obj)

        raw_api_object.add_raw_member("types", types_set, "engine.util.game_entity.GameEntity")

        # =======================================================================
        # Abilities
        # =======================================================================
        abilities_set = []

        interaction_mode = ambient_unit["interaction_mode"].get_value()

        if interaction_mode >= 0:
            abilities_set.append(AoCAbilitySubprocessor.death_ability(ambient_group))
            abilities_set.append(AoCAbilitySubprocessor.hitbox_ability(ambient_group))
            abilities_set.append(AoCAbilitySubprocessor.idle_ability(ambient_group))
            abilities_set.append(AoCAbilitySubprocessor.live_ability(ambient_group))
            abilities_set.append(AoCAbilitySubprocessor.named_ability(ambient_group))
            abilities_set.append(AoCAbilitySubprocessor.resistance_ability(ambient_group))
            abilities_set.append(AoCAbilitySubprocessor.terrain_requirement_ability(ambient_group))
            abilities_set.append(AoCAbilitySubprocessor.visibility_ability(ambient_group))

        if interaction_mode >= 2:
            abilities_set.extend(AoCAbilitySubprocessor.selectable_ability(ambient_group))

            if ambient_group.is_passable():
                abilities_set.append(AoCAbilitySubprocessor.passable_ability(ambient_group))

        if ambient_group.is_harvestable():
            abilities_set.append(AoCAbilitySubprocessor.harvestable_ability(ambient_group))

        # =======================================================================
        # Abilities
        # =======================================================================
        raw_api_object.add_raw_member("abilities", abilities_set,
                                      "engine.util.game_entity.GameEntity")

        # =======================================================================
        # Modifiers
        # =======================================================================
        modifiers_set = []

        raw_api_object.add_raw_member("modifiers", modifiers_set,
                                      "engine.util.game_entity.GameEntity")

        # =======================================================================
        # TODO: Variants
        # =======================================================================
        raw_api_object.add_raw_member("variants", [], "engine.util.game_entity.GameEntity")

    @staticmethod
    def variant_group_to_game_entity(variant_group):
        """
        Creates raw API objects for a variant group.

        :param ambient_group: Unit line that gets converted to a game entity.
        :type ambient_group: ..dataformat.converter_object.ConverterObjectGroup
        """
        variant_main_unit = variant_group.get_head_unit()
        variant_id = variant_group.get_head_unit_id()

        dataset = variant_group.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
        class_lookup_dict = internal_name_lookups.get_class_lookups(dataset.game_version)

        # Start with the generic GameEntity
        game_entity_name = name_lookup_dict[variant_id][0]
        obj_location = f"data/game_entity/generic/{name_lookup_dict[variant_id][1]}/"
        raw_api_object = RawAPIObject(game_entity_name, game_entity_name,
                                      dataset.nyan_api_objects)
        raw_api_object.add_raw_parent("engine.util.game_entity.GameEntity")
        raw_api_object.set_location(obj_location)
        raw_api_object.set_filename(name_lookup_dict[variant_id][1])
        variant_group.add_raw_api_object(raw_api_object)

        # =======================================================================
        # Game Entity Types
        # =======================================================================
        # we give variants the types
        #    - util.game_entity_type.types.Ambient
        # =======================================================================
        # Create or use existing auxiliary types
        types_set = []

        type_obj = dataset.pregen_nyan_objects["util.game_entity_type.types.Ambient"].get_nyan_object()
        types_set.append(type_obj)

        unit_class = variant_main_unit["unit_class"].get_value()
        class_name = class_lookup_dict[unit_class]
        class_obj_name = f"util.game_entity_type.types.{class_name}"
        type_obj = dataset.pregen_nyan_objects[class_obj_name].get_nyan_object()
        types_set.append(type_obj)

        raw_api_object.add_raw_member("types", types_set, "engine.util.game_entity.GameEntity")

        # =======================================================================
        # Abilities
        # =======================================================================
        abilities_set = []

        abilities_set.append(AoCAbilitySubprocessor.death_ability(variant_group))
        abilities_set.append(AoCAbilitySubprocessor.despawn_ability(variant_group))
        abilities_set.append(AoCAbilitySubprocessor.idle_ability(variant_group))
        abilities_set.append(AoCAbilitySubprocessor.named_ability(variant_group))
        abilities_set.extend(AoCAbilitySubprocessor.selectable_ability(variant_group))
        abilities_set.append(AoCAbilitySubprocessor.terrain_requirement_ability(variant_group))
        abilities_set.append(AoCAbilitySubprocessor.visibility_ability(variant_group))

        if variant_main_unit.has_member("speed") and variant_main_unit["speed"].get_value() > 0.0001\
                and variant_main_unit.has_member("command_sound_id"):
            # TODO: Let variant groups be converted without having command_sound_id member
            abilities_set.append(AoCAbilitySubprocessor.move_ability(variant_group))

        if variant_group.is_harvestable():
            abilities_set.append(AoCAbilitySubprocessor.harvestable_ability(variant_group))

        raw_api_object.add_raw_member("abilities", abilities_set,
                                      "engine.util.game_entity.GameEntity")

        # =======================================================================
        # Modifiers
        # =======================================================================
        modifiers_set = []

        raw_api_object.add_raw_member("modifiers", modifiers_set,
                                      "engine.util.game_entity.GameEntity")

        # =======================================================================
        # Variants
        # =======================================================================
        variants_set = []

        variant_type = name_lookup_dict[variant_id][3]

        index = 0
        for variant in variant_group.line:
            # Create a diff
            diff_variant = variant_main_unit.diff(variant)

            if variant_type == "random":
                variant_type_ref = "engine.util.variant.type.RandomVariant"

            elif variant_type == "angle":
                variant_type_ref = "engine.util.variant.type.PerspectiveVariant"

            elif variant_type == "misc":
                variant_type_ref = "engine.util.variant.type.MiscVariant"

            variant_name = f"Variant{str(index)}"
            variant_ref = f"{game_entity_name}.{variant_name}"
            variant_raw_api_object = RawAPIObject(variant_ref,
                                                  variant_name,
                                                  dataset.nyan_api_objects)
            variant_raw_api_object.add_raw_parent(variant_type_ref)
            variant_location = ForwardRef(variant_group, game_entity_name)
            variant_raw_api_object.set_location(variant_location)

            # Create patches for the diff
            patches = []

            patches.extend(AoCUpgradeAbilitySubprocessor.death_ability(variant_group,
                                                                       variant_group,
                                                                       variant_ref,
                                                                       diff_variant))
            patches.extend(AoCUpgradeAbilitySubprocessor.despawn_ability(variant_group,
                                                                         variant_group,
                                                                         variant_ref,
                                                                         diff_variant))
            patches.extend(AoCUpgradeAbilitySubprocessor.idle_ability(variant_group,
                                                                      variant_group,
                                                                      variant_ref,
                                                                      diff_variant))
            patches.extend(AoCUpgradeAbilitySubprocessor.named_ability(variant_group,
                                                                       variant_group,
                                                                       variant_ref,
                                                                       diff_variant))

            if variant_main_unit.has_member("speed") and variant_main_unit["speed"].get_value() > 0.0001\
                    and variant_main_unit.has_member("command_sound_id"):
                # TODO: Let variant groups be converted without having command_sound_id member:
                patches.extend(AoCUpgradeAbilitySubprocessor.move_ability(variant_group,
                                                                          variant_group,
                                                                          variant_ref,
                                                                          diff_variant))

            # Changes
            variant_raw_api_object.add_raw_member("changes",
                                                  patches,
                                                  "engine.util.variant.Variant")

            # Prority
            variant_raw_api_object.add_raw_member("priority",
                                                  1,
                                                  "engine.util.variant.Variant")

            if variant_type == "random":
                variant_raw_api_object.add_raw_member("chance_share",
                                                      1 / len(variant_group.line),
                                                      "engine.util.variant.type.RandomVariant")

            elif variant_type == "angle":
                variant_raw_api_object.add_raw_member("angle",
                                                      index,
                                                      "engine.util.variant.type.PerspectiveVariant")

            variants_forward_ref = ForwardRef(variant_group, variant_ref)
            variants_set.append(variants_forward_ref)
            variant_group.add_raw_api_object(variant_raw_api_object)

            index += 1

        raw_api_object.add_raw_member("variants", variants_set,
                                      "engine.util.game_entity.GameEntity")

    @staticmethod
    def tech_group_to_tech(tech_group):
        """
        Creates raw API objects for a tech group.

        :param tech_group: Tech group that gets converted to a tech.
        :type tech_group: ..dataformat.converter_object.ConverterObjectGroup
        """
        tech_id = tech_group.get_id()

        # Skip Dark Age tech
        if tech_id == 104:
            return

        dataset = tech_group.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
        tech_lookup_dict = internal_name_lookups.get_tech_lookups(dataset.game_version)

        # Start with the Tech object
        tech_name = tech_lookup_dict[tech_id][0]
        raw_api_object = RawAPIObject(tech_name, tech_name,
                                      dataset.nyan_api_objects)
        raw_api_object.add_raw_parent("engine.util.tech.Tech")

        if isinstance(tech_group, UnitLineUpgrade):
            unit_line = dataset.unit_lines[tech_group.get_line_id()]
            head_unit_id = unit_line.get_head_unit_id()
            obj_location = f"data/game_entity/generic/{name_lookup_dict[head_unit_id][1]}/"

        else:
            obj_location = f"data/tech/generic/{tech_lookup_dict[tech_id][1]}/"

        raw_api_object.set_location(obj_location)
        raw_api_object.set_filename(tech_lookup_dict[tech_id][1])
        tech_group.add_raw_api_object(raw_api_object)

        # =======================================================================
        # Types
        # =======================================================================
        raw_api_object.add_raw_member("types", [], "engine.util.tech.Tech")

        # =======================================================================
        # Name
        # =======================================================================
        name_ref = f"{tech_name}.{tech_name}Name"
        name_raw_api_object = RawAPIObject(name_ref,
                                           f"{tech_name}Name",
                                           dataset.nyan_api_objects)
        name_raw_api_object.add_raw_parent("engine.util.language.translated.type.TranslatedString")
        name_location = ForwardRef(tech_group, tech_name)
        name_raw_api_object.set_location(name_location)

        name_raw_api_object.add_raw_member("translations",
                                           [],
                                           "engine.util.language.translated.type.TranslatedString")

        name_forward_ref = ForwardRef(tech_group, name_ref)
        raw_api_object.add_raw_member("name", name_forward_ref, "engine.util.tech.Tech")
        tech_group.add_raw_api_object(name_raw_api_object)

        # =======================================================================
        # Description
        # =======================================================================
        description_ref = f"{tech_name}.{tech_name}Description"
        description_raw_api_object = RawAPIObject(description_ref,
                                                  f"{tech_name}Description",
                                                  dataset.nyan_api_objects)
        description_raw_api_object.add_raw_parent("engine.util.language.translated.type.TranslatedMarkupFile")
        description_location = ForwardRef(tech_group, tech_name)
        description_raw_api_object.set_location(description_location)

        description_raw_api_object.add_raw_member("translations",
                                                  [],
                                                  "engine.util.language.translated.type.TranslatedMarkupFile")

        description_forward_ref = ForwardRef(tech_group, description_ref)
        raw_api_object.add_raw_member("description",
                                      description_forward_ref,
                                      "engine.util.tech.Tech")
        tech_group.add_raw_api_object(description_raw_api_object)

        # =======================================================================
        # Long description
        # =======================================================================
        long_description_ref = f"{tech_name}.{tech_name}LongDescription"
        long_description_raw_api_object = RawAPIObject(long_description_ref,
                                                       f"{tech_name}LongDescription",
                                                       dataset.nyan_api_objects)
        long_description_raw_api_object.add_raw_parent("engine.util.language.translated.type.TranslatedMarkupFile")
        long_description_location = ForwardRef(tech_group, tech_name)
        long_description_raw_api_object.set_location(long_description_location)

        long_description_raw_api_object.add_raw_member("translations",
                                                       [],
                                                       "engine.util.language.translated.type.TranslatedMarkupFile")

        long_description_forward_ref = ForwardRef(tech_group, long_description_ref)
        raw_api_object.add_raw_member("long_description",
                                      long_description_forward_ref,
                                      "engine.util.tech.Tech")
        tech_group.add_raw_api_object(long_description_raw_api_object)

        # =======================================================================
        # Updates
        # =======================================================================
        patches = []
        patches.extend(AoCTechSubprocessor.get_patches(tech_group))
        raw_api_object.add_raw_member("updates", patches, "engine.util.tech.Tech")

        # =======================================================================
        # Misc (Objects that are not used by the tech group itself, but use its values)
        # =======================================================================
        if tech_group.is_researchable():
            AoCAuxiliarySubprocessor.get_researchable_tech(tech_group)

    @staticmethod
    def terrain_group_to_terrain(terrain_group):
        """
        Creates raw API objects for a terrain group.

        :param terrain_group: Terrain group that gets converted to a tech.
        :type terrain_group: ..dataformat.converter_object.ConverterObjectGroup
        """
        terrain_index = terrain_group.get_id()

        dataset = terrain_group.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
        terrain_lookup_dict = internal_name_lookups.get_terrain_lookups(dataset.game_version)
        terrain_type_lookup_dict = internal_name_lookups.get_terrain_type_lookups(dataset.game_version)

        # Start with the Terrain object
        terrain_name = terrain_lookup_dict[terrain_index][1]
        raw_api_object = RawAPIObject(terrain_name, terrain_name,
                                      dataset.nyan_api_objects)
        raw_api_object.add_raw_parent("engine.util.terrain.Terrain")
        obj_location = f"data/terrain/{terrain_lookup_dict[terrain_index][2]}/"
        raw_api_object.set_location(obj_location)
        raw_api_object.set_filename(terrain_lookup_dict[terrain_index][2])
        terrain_group.add_raw_api_object(raw_api_object)

        # =======================================================================
        # Types
        # =======================================================================
        terrain_types = []

        for terrain_type in terrain_type_lookup_dict.values():
            if terrain_index in terrain_type[0]:
                type_name = f"util.terrain_type.types.{terrain_type[2]}"
                type_obj = dataset.pregen_nyan_objects[type_name].get_nyan_object()
                terrain_types.append(type_obj)

        raw_api_object.add_raw_member("types", terrain_types, "engine.util.terrain.Terrain")

        # =======================================================================
        # Name
        # =======================================================================
        name_ref = f"{terrain_name}.{terrain_name}Name"
        name_raw_api_object = RawAPIObject(name_ref,
                                           f"{terrain_name}Name",
                                           dataset.nyan_api_objects)
        name_raw_api_object.add_raw_parent("engine.util.language.translated.type.TranslatedString")
        name_location = ForwardRef(terrain_group, terrain_name)
        name_raw_api_object.set_location(name_location)

        name_raw_api_object.add_raw_member("translations",
                                           [],
                                           "engine.util.language.translated.type.TranslatedString")

        name_forward_ref = ForwardRef(terrain_group, name_ref)
        raw_api_object.add_raw_member("name", name_forward_ref, "engine.util.terrain.Terrain")
        terrain_group.add_raw_api_object(name_raw_api_object)

        # =======================================================================
        # Sound
        # =======================================================================
        sound_name = f"{terrain_name}.Sound"
        sound_raw_api_object = RawAPIObject(sound_name, "Sound",
                                            dataset.nyan_api_objects)
        sound_raw_api_object.add_raw_parent("engine.util.sound.Sound")
        sound_location = ForwardRef(terrain_group, terrain_name)
        sound_raw_api_object.set_location(sound_location)

        # Sounds for terrains don't exist in AoC
        sounds = []

        sound_raw_api_object.add_raw_member("play_delay",
                                            0,
                                            "engine.util.sound.Sound")
        sound_raw_api_object.add_raw_member("sounds",
                                            sounds,
                                            "engine.util.sound.Sound")

        sound_forward_ref = ForwardRef(terrain_group, sound_name)
        raw_api_object.add_raw_member("sound",
                                      sound_forward_ref,
                                      "engine.util.terrain.Terrain")

        terrain_group.add_raw_api_object(sound_raw_api_object)

        # =======================================================================
        # Ambience
        # =======================================================================
        terrain = terrain_group.get_terrain()
        ambients_count = terrain["terrain_units_used_count"].get_value()

        ambience = []
        for ambient_index in range(ambients_count):
            ambient_id = terrain["terrain_unit_id"][ambient_index].get_value()

            if ambient_id == -1:
                continue

            ambient_line = dataset.unit_ref[ambient_id]
            ambient_name = name_lookup_dict[ambient_line.get_head_unit_id()][0]

            ambient_ref = f"{terrain_name}.Ambient{str(ambient_index)}"
            ambient_raw_api_object = RawAPIObject(ambient_ref,
                                                  f"Ambient{str(ambient_index)}",
                                                  dataset.nyan_api_objects)
            ambient_raw_api_object.add_raw_parent("engine.util.terrain.TerrainAmbient")
            ambient_location = ForwardRef(terrain_group, terrain_name)
            ambient_raw_api_object.set_location(ambient_location)

            # Game entity reference
            ambient_line_forward_ref = ForwardRef(ambient_line, ambient_name)
            ambient_raw_api_object.add_raw_member("object",
                                                  ambient_line_forward_ref,
                                                  "engine.util.terrain.TerrainAmbient")

            # Max density
            max_density = terrain["terrain_unit_density"][ambient_index].get_value()
            ambient_raw_api_object.add_raw_member("max_density",
                                                  max_density,
                                                  "engine.util.terrain.TerrainAmbient")

            terrain_group.add_raw_api_object(ambient_raw_api_object)
            terrain_ambient_forward_ref = ForwardRef(terrain_group, ambient_ref)
            ambience.append(terrain_ambient_forward_ref)

        raw_api_object.add_raw_member("ambience", ambience, "engine.util.terrain.Terrain")

        # =======================================================================
        # Graphic
        # =======================================================================
        if terrain_group.has_subterrain():
            subterrain = terrain_group.get_subterrain()
            slp_id = subterrain["slp_id"].get_value()

        else:
            slp_id = terrain_group.get_terrain()["slp_id"].get_value()

        # Create animation object
        graphic_name = f"{terrain_name}.TerrainTexture"
        graphic_raw_api_object = RawAPIObject(graphic_name, "TerrainTexture",
                                              dataset.nyan_api_objects)
        graphic_raw_api_object.add_raw_parent("engine.util.graphics.Terrain")
        graphic_location = ForwardRef(terrain_group, terrain_name)
        graphic_raw_api_object.set_location(graphic_location)

        if slp_id in dataset.combined_terrains.keys():
            terrain_graphic = dataset.combined_terrains[slp_id]

        else:
            terrain_graphic = CombinedTerrain(slp_id,
                                              f"texture_{terrain_lookup_dict[terrain_index][2]}",
                                              dataset)
            dataset.combined_terrains.update({terrain_graphic.get_id(): terrain_graphic})

        terrain_graphic.add_reference(graphic_raw_api_object)

        graphic_raw_api_object.add_raw_member("sprite", terrain_graphic,
                                              "engine.util.graphics.Terrain")

        terrain_group.add_raw_api_object(graphic_raw_api_object)
        graphic_forward_ref = ForwardRef(terrain_group, graphic_name)
        raw_api_object.add_raw_member("terrain_graphic", graphic_forward_ref,
                                      "engine.util.terrain.Terrain")

    @staticmethod
    def civ_group_to_civ(civ_group):
        """
        Creates raw API objects for a civ group.

        :param civ_group: Terrain group that gets converted to a tech.
        :type civ_group: ..dataformat.converter_object.ConverterObjectGroup
        """
        civ_id = civ_group.get_id()

        dataset = civ_group.data

        civ_lookup_dict = internal_name_lookups.get_civ_lookups(dataset.game_version)

        # Start with the Tech object
        tech_name = civ_lookup_dict[civ_id][0]
        raw_api_object = RawAPIObject(tech_name, tech_name,
                                      dataset.nyan_api_objects)
        raw_api_object.add_raw_parent("engine.util.setup.PlayerSetup")

        obj_location = f"data/civ/{civ_lookup_dict[civ_id][1]}/"

        raw_api_object.set_location(obj_location)
        raw_api_object.set_filename(civ_lookup_dict[civ_id][1])
        civ_group.add_raw_api_object(raw_api_object)

        # =======================================================================
        # Name
        # =======================================================================
        name_ref = f"{tech_name}.{tech_name}Name"
        name_raw_api_object = RawAPIObject(name_ref,
                                           f"{tech_name}Name",
                                           dataset.nyan_api_objects)
        name_raw_api_object.add_raw_parent("engine.util.language.translated.type.TranslatedString")
        name_location = ForwardRef(civ_group, tech_name)
        name_raw_api_object.set_location(name_location)

        name_raw_api_object.add_raw_member("translations",
                                           [],
                                           "engine.util.language.translated.type.TranslatedString")

        name_forward_ref = ForwardRef(civ_group, name_ref)
        raw_api_object.add_raw_member("name", name_forward_ref, "engine.util.setup.PlayerSetup")
        civ_group.add_raw_api_object(name_raw_api_object)

        # =======================================================================
        # Description
        # =======================================================================
        description_ref = f"{tech_name}.{tech_name}Description"
        description_raw_api_object = RawAPIObject(description_ref,
                                                  f"{tech_name}Description",
                                                  dataset.nyan_api_objects)
        description_raw_api_object.add_raw_parent("engine.util.language.translated.type.TranslatedMarkupFile")
        description_location = ForwardRef(civ_group, tech_name)
        description_raw_api_object.set_location(description_location)

        description_raw_api_object.add_raw_member("translations",
                                                  [],
                                                  "engine.util.language.translated.type.TranslatedMarkupFile")

        description_forward_ref = ForwardRef(civ_group, description_ref)
        raw_api_object.add_raw_member("description",
                                      description_forward_ref,
                                      "engine.util.setup.PlayerSetup")
        civ_group.add_raw_api_object(description_raw_api_object)

        # =======================================================================
        # Long description
        # =======================================================================
        long_description_ref = f"{tech_name}.{tech_name}LongDescription"
        long_description_raw_api_object = RawAPIObject(long_description_ref,
                                                       f"{tech_name}LongDescription",
                                                       dataset.nyan_api_objects)
        long_description_raw_api_object.add_raw_parent("engine.util.language.translated.type.TranslatedMarkupFile")
        long_description_location = ForwardRef(civ_group, tech_name)
        long_description_raw_api_object.set_location(long_description_location)

        long_description_raw_api_object.add_raw_member("translations",
                                                       [],
                                                       "engine.util.language.translated.type.TranslatedMarkupFile")

        long_description_forward_ref = ForwardRef(civ_group, long_description_ref)
        raw_api_object.add_raw_member("long_description",
                                      long_description_forward_ref,
                                      "engine.util.setup.PlayerSetup")
        civ_group.add_raw_api_object(long_description_raw_api_object)

        # =======================================================================
        # TODO: Leader names
        # =======================================================================
        raw_api_object.add_raw_member("leader_names",
                                      [],
                                      "engine.util.setup.PlayerSetup")

        # =======================================================================
        # Modifiers
        # =======================================================================
        modifiers = AoCCivSubprocessor.get_modifiers(civ_group)
        raw_api_object.add_raw_member("modifiers",
                                      modifiers,
                                      "engine.util.setup.PlayerSetup")

        # =======================================================================
        # Starting resources
        # =======================================================================
        resource_amounts = AoCCivSubprocessor.get_starting_resources(civ_group)
        raw_api_object.add_raw_member("starting_resources",
                                      resource_amounts,
                                      "engine.util.setup.PlayerSetup")

        # =======================================================================
        # Game setup
        # =======================================================================
        game_setup = AoCCivSubprocessor.get_civ_setup(civ_group)
        raw_api_object.add_raw_member("game_setup",
                                      game_setup,
                                      "engine.util.setup.PlayerSetup")

    @staticmethod
    def projectiles_from_line(line):
        """
        Creates Projectile(GameEntity) raw API objects for a unit/building line.

        :param line: Line for which the projectiles are extracted.
        :type line: ..dataformat.converter_object.ConverterObjectGroup
        """
        current_unit = line.get_head_unit()
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]
        game_entity_filename = name_lookup_dict[current_unit_id][1]

        projectiles_location = f"data/game_entity/generic/{game_entity_filename}/projectiles/"

        projectile_indices = []
        projectile_primary = current_unit["attack_projectile_primary_unit_id"].get_value()
        if projectile_primary > -1:
            projectile_indices.append(0)

        projectile_secondary = current_unit["attack_projectile_secondary_unit_id"].get_value()
        if projectile_secondary > -1:
            projectile_indices.append(1)

        for projectile_num in projectile_indices:
            obj_ref = "%s.ShootProjectile.Projectile%s" % (game_entity_name,
                                                           str(projectile_num))
            obj_name = f"Projectile{str(projectile_num)}"
            proj_raw_api_object = RawAPIObject(obj_ref, obj_name, dataset.nyan_api_objects)
            proj_raw_api_object.add_raw_parent("engine.util.game_entity.GameEntity")
            proj_raw_api_object.set_location(projectiles_location)
            proj_raw_api_object.set_filename(f"{game_entity_filename}_projectiles")

            # =======================================================================
            # Types
            # =======================================================================
            types_set = [dataset.pregen_nyan_objects["util.game_entity_type.types.Projectile"].get_nyan_object()]
            proj_raw_api_object.add_raw_member("types", types_set, "engine.util.game_entity.GameEntity")

            # =======================================================================
            # Abilities
            # =======================================================================
            abilities_set = []
            abilities_set.append(AoCAbilitySubprocessor.projectile_ability(line, position=projectile_num))
            abilities_set.append(AoCAbilitySubprocessor.move_projectile_ability(line, position=projectile_num))
            abilities_set.append(AoCAbilitySubprocessor.apply_discrete_effect_ability(line, 7, False, projectile_num))
            # TODO: Death, Despawn
            proj_raw_api_object.add_raw_member("abilities", abilities_set, "engine.util.game_entity.GameEntity")

            # =======================================================================
            # Modifiers
            # =======================================================================
            modifiers_set = []

            modifiers_set.append(AoCModifierSubprocessor.flyover_effect_modifier(line))
            modifiers_set.extend(AoCModifierSubprocessor.elevation_attack_modifiers(line))

            proj_raw_api_object.add_raw_member("modifiers", modifiers_set, "engine.util.game_entity.GameEntity")

            # =======================================================================
            # Variants
            # =======================================================================
            variants_set = []
            proj_raw_api_object.add_raw_member("variants", variants_set, "engine.util.game_entity.GameEntity")

            line.add_raw_api_object(proj_raw_api_object)
