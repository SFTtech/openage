# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Convert API-like objects to nyan objects. Subroutine of the
main RoR processor. Reuses functionality from the AoC subprocessor.
"""
from openage.convert.dataformat.aoc.genie_unit import GenieVillagerGroup
from openage.convert.dataformat.converter_object import RawAPIObject
from openage.convert.dataformat.ror.genie_unit import RoRVillagerGroup
from openage.convert.processor.aoc.ability_subprocessor import AoCAbilitySubprocessor
from openage.convert.processor.aoc.auxiliary_subprocessor import AoCAuxiliarySubprocessor
from openage.convert.processor.aoc.modifier_subprocessor import AoCModifierSubprocessor
from openage.convert.service import internal_name_lookups


class RoRNyanSubprocessor:

    @classmethod
    def convert(cls, gamedata):

        cls._process_game_entities(gamedata)
        cls._create_nyan_objects(gamedata)
        cls._create_nyan_members(gamedata)

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

    @classmethod
    def _create_nyan_members(cls, full_data_set):
        """
        Fill nyan member values of the API objects.
        """
        for unit_line in full_data_set.unit_lines.values():
            unit_line.create_nyan_members()

        for building_line in full_data_set.building_lines.values():
            building_line.create_nyan_members()

    @classmethod
    def _process_game_entities(cls, full_data_set):

        for unit_line in full_data_set.unit_lines.values():
            cls._unit_line_to_game_entity(unit_line)

        for building_line in full_data_set.building_lines.values():
            cls._building_line_to_game_entity(building_line)

    @staticmethod
    def _unit_line_to_game_entity(unit_line):
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
        obj_location = "data/game_entity/generic/%s/" % (name_lookup_dict[current_unit_id][1])
        raw_api_object = RawAPIObject(game_entity_name, game_entity_name,
                                      dataset.nyan_api_objects)
        raw_api_object.add_raw_parent("engine.aux.game_entity.GameEntity")
        raw_api_object.set_location(obj_location)
        raw_api_object.set_filename(name_lookup_dict[current_unit_id][1])
        unit_line.add_raw_api_object(raw_api_object)

        # =======================================================================
        # Game Entity Types
        # =======================================================================
        # we give a unit two types
        #    - aux.game_entity_type.types.Unit (if unit_type >= 70)
        #    - aux.game_entity_type.types.<Class> (depending on the class)
        # =======================================================================
        # Create or use existing auxiliary types
        types_set = []
        unit_type = current_unit.get_member("unit_type").get_value()

        if unit_type >= 70:
            type_obj = dataset.pregen_nyan_objects["aux.game_entity_type.types.Unit"].get_nyan_object()
            types_set.append(type_obj)

        unit_class = current_unit.get_member("unit_class").get_value()
        class_name = class_lookup_dict[unit_class]
        class_obj_name = "aux.game_entity_type.types.%s" % (class_name)
        type_obj = dataset.pregen_nyan_objects[class_obj_name].get_nyan_object()
        types_set.append(type_obj)

        raw_api_object.add_raw_member("types", types_set, "engine.aux.game_entity.GameEntity")

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
        # abilities_set.append(AoCAbilitySubprocessor.resistance_ability(unit_line))  TODO: Conversion
        abilities_set.extend(AoCAbilitySubprocessor.selectable_ability(unit_line))
        abilities_set.append(AoCAbilitySubprocessor.stop_ability(unit_line))
        # abilities_set.append(AoCAbilitySubprocessor.terrain_requirement_ability(unit_line)) TODO: Terrain types
        abilities_set.append(AoCAbilitySubprocessor.turn_ability(unit_line))
        abilities_set.append(AoCAbilitySubprocessor.visibility_ability(unit_line))

        # Creation
        if len(unit_line.creates) > 0:
            abilities_set.append(AoCAbilitySubprocessor.create_ability(unit_line))

        # Config
        ability = AoCAbilitySubprocessor.use_contingent_ability(unit_line)
        if ability:
            abilities_set.append(ability)

        if unit_line.has_command(104):
            # Recharging attribute points (priests)
            abilities_set.extend(AoCAbilitySubprocessor.regenerate_attribute_ability(unit_line))

        # Applying effects and shooting projectiles
        if unit_line.is_projectile_shooter():
            # TODO: no second rojectile in RoR
            # abilities_set.append(AoCAbilitySubprocessor.shoot_projectile_ability(unit_line, 7))
            # RoRNyanSubprocessor._projectiles_from_line(unit_line)
            pass

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
                # TODO: Success chance is not a resource in RoR
                # convert
                # abilities_set.append(AoCAbilitySubprocessor.apply_discrete_effect_ability(unit_line,
                #                                                                           104,
                #                                                                           unit_line.is_ranged()))
                pass

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
            # abilities_set.append(AoCAbilitySubprocessor.game_entity_stance_ability(unit_line)) TODO: What stances are there?
            pass

        # Storage abilities
        if unit_line.is_garrison():
            # abilities_set.append(AoCAbilitySubprocessor.storage_ability(unit_line)) TODO: No garrison graphic
            abilities_set.append(AoCAbilitySubprocessor.remove_storage_ability(unit_line))

        if len(unit_line.garrison_locations) > 0:
            ability = AoCAbilitySubprocessor.enter_container_ability(unit_line)
            if ability:
                abilities_set.append(ability)

            ability = AoCAbilitySubprocessor.exit_container_ability(unit_line)
            if ability:
                abilities_set.append(ability)

        # Resource abilities
        if unit_line.is_gatherer():
            abilities_set.append(AoCAbilitySubprocessor.drop_resources_ability(unit_line))
            abilities_set.extend(AoCAbilitySubprocessor.gather_ability(unit_line))
            abilities_set.append(AoCAbilitySubprocessor.resource_storage_ability(unit_line))

        if unit_line.is_harvestable():
            abilities_set.append(AoCAbilitySubprocessor.harvestable_ability(unit_line))

        # Trade abilities
        if unit_line.has_command(111):
            abilities_set.append(AoCAbilitySubprocessor.trade_ability(unit_line))

        raw_api_object.add_raw_member("abilities", abilities_set,
                                      "engine.aux.game_entity.GameEntity")

        # =======================================================================
        # Modifiers
        # =======================================================================
        modifiers_set = []

        if unit_line.is_gatherer():
            modifiers_set.extend(AoCModifierSubprocessor.gather_rate_modifier(unit_line))

        raw_api_object.add_raw_member("modifiers", modifiers_set,
                                      "engine.aux.game_entity.GameEntity")

        # =======================================================================
        # TODO: Variants
        # =======================================================================
        variants_set = []

        raw_api_object.add_raw_member("variants", variants_set,
                                      "engine.aux.game_entity.GameEntity")

        # =======================================================================
        # Misc (Objects that are not used by the unit line itself, but use its values)
        # =======================================================================
        if unit_line.is_creatable():
            AoCAuxiliarySubprocessor.get_creatable_game_entity(unit_line)

    @staticmethod
    def _building_line_to_game_entity(building_line):
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
        obj_location = "data/game_entity/generic/%s/" % (name_lookup_dict[current_building_id][1])
        raw_api_object = RawAPIObject(game_entity_name, game_entity_name,
                                      dataset.nyan_api_objects)
        raw_api_object.add_raw_parent("engine.aux.game_entity.GameEntity")
        raw_api_object.set_location(obj_location)
        raw_api_object.set_filename(name_lookup_dict[current_building_id][1])
        building_line.add_raw_api_object(raw_api_object)

        # =======================================================================
        # Game Entity Types
        # =======================================================================
        # we give a building two types
        #    - aux.game_entity_type.types.Building (if unit_type >= 80)
        #    - aux.game_entity_type.types.<Class> (depending on the class)
        # and additionally
        #    - aux.game_entity_type.types.DropSite (only if this is used as a drop site)
        # =======================================================================
        # Create or use existing auxiliary types
        types_set = []
        unit_type = current_building.get_member("unit_type").get_value()

        if unit_type >= 80:
            type_obj = dataset.pregen_nyan_objects["aux.game_entity_type.types.Building"].get_nyan_object()
            types_set.append(type_obj)

        unit_class = current_building.get_member("unit_class").get_value()
        class_name = class_lookup_dict[unit_class]
        class_obj_name = "aux.game_entity_type.types.%s" % (class_name)
        type_obj = dataset.pregen_nyan_objects[class_obj_name].get_nyan_object()
        types_set.append(type_obj)

        if building_line.is_dropsite():
            type_obj = dataset.pregen_nyan_objects["aux.game_entity_type.types.DropSite"].get_nyan_object()
            types_set.append(type_obj)

        raw_api_object.add_raw_member("types", types_set, "engine.aux.game_entity.GameEntity")

        # =======================================================================
        # Abilities
        # =======================================================================
        abilities_set = []

        # abilities_set.append(AoCAbilitySubprocessor.attribute_change_tracker_ability(building_line)) TODO: Damage graphic count
        abilities_set.append(AoCAbilitySubprocessor.death_ability(building_line))
        abilities_set.append(AoCAbilitySubprocessor.delete_ability(building_line))
        abilities_set.append(AoCAbilitySubprocessor.idle_ability(building_line))
        abilities_set.append(AoCAbilitySubprocessor.hitbox_ability(building_line))
        abilities_set.append(AoCAbilitySubprocessor.live_ability(building_line))
        abilities_set.append(AoCAbilitySubprocessor.los_ability(building_line))
        abilities_set.append(AoCAbilitySubprocessor.named_ability(building_line))
        # abilities_set.append(AoCAbilitySubprocessor.resistance_ability(building_line)) TODO: Conversion
        abilities_set.extend(AoCAbilitySubprocessor.selectable_ability(building_line))
        abilities_set.append(AoCAbilitySubprocessor.stop_ability(building_line))
        # abilities_set.append(AoCAbilitySubprocessor.terrain_requirement_ability(building_line)) TODO: terrain types
        abilities_set.append(AoCAbilitySubprocessor.visibility_ability(building_line))

        # Config abilities
        if building_line.is_creatable():
            abilities_set.append(AoCAbilitySubprocessor.constructable_ability(building_line))

        if building_line.has_foundation():
            abilities_set.append(AoCAbilitySubprocessor.foundation_ability(building_line))

        # Creation/Research abilities
        if len(building_line.creates) > 0:
            abilities_set.append(AoCAbilitySubprocessor.create_ability(building_line))
            # abilities_set.append(AoCAbilitySubprocessor.production_queue_ability(building_line)) TODO: Production queue size

        if len(building_line.researches) > 0:
            abilities_set.append(AoCAbilitySubprocessor.research_ability(building_line))

        # Effect abilities
        if building_line.is_projectile_shooter():
            # TODO: RoR has no secondary projectile
            # abilities_set.append(AoCAbilitySubprocessor.shoot_projectile_ability(building_line, 7))
            # abilities_set.append(AoCAbilitySubprocessor.game_entity_stance_ability(building_line))
            # RoRNyanSubprocessor._projectiles_from_line(building_line)
            pass

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

        raw_api_object.add_raw_member("abilities", abilities_set,
                                      "engine.aux.game_entity.GameEntity")

        # =======================================================================
        # Modifiers
        # =======================================================================
        raw_api_object.add_raw_member("modifiers", [], "engine.aux.game_entity.GameEntity")

        # =======================================================================
        # TODO: Variants
        # =======================================================================
        raw_api_object.add_raw_member("variants", [], "engine.aux.game_entity.GameEntity")

        # =======================================================================
        # Misc (Objects that are not used by the unit line itself, but use its values)
        # =======================================================================
        if building_line.is_creatable():
            AoCAuxiliarySubprocessor.get_creatable_game_entity(building_line)
