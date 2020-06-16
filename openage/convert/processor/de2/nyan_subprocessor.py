# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Convert API-like objects to nyan objects. Subroutine of the
main DE2 processor.
"""
from openage.convert.dataformat.aoc.forward_ref import ForwardRef
from openage.convert.dataformat.aoc.genie_tech import UnitLineUpgrade
from openage.convert.dataformat.aoc.genie_unit import GenieVillagerGroup,\
    GenieGarrisonMode, GenieMonkGroup, GenieStackBuildingGroup
from openage.convert.dataformat.converter_object import RawAPIObject
from openage.convert.processor.aoc.ability_subprocessor import AoCAbilitySubprocessor
from openage.convert.processor.aoc.auxiliary_subprocessor import AoCAuxiliarySubprocessor
from openage.convert.processor.aoc.modifier_subprocessor import AoCModifierSubprocessor
from openage.convert.processor.aoc.nyan_subprocessor import AoCNyanSubprocessor
from openage.convert.processor.de2.tech_subprocessor import DE2TechSubprocessor
from openage.convert.service import internal_name_lookups


class DE2NyanSubprocessor:

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

        for unit_line in full_data_set.unit_lines.values():
            cls._unit_line_to_game_entity(unit_line)

        for building_line in full_data_set.building_lines.values():
            cls._building_line_to_game_entity(building_line)

        for ambient_group in full_data_set.ambient_groups.values():
            AoCNyanSubprocessor._ambient_group_to_game_entity(ambient_group)

        for variant_group in full_data_set.variant_groups.values():
            AoCNyanSubprocessor._variant_group_to_game_entity(variant_group)

        for tech_group in full_data_set.tech_groups.values():
            if tech_group.is_researchable():
                cls._tech_group_to_tech(tech_group)

        for terrain_group in full_data_set.terrain_groups.values():
            cls._terrain_group_to_terrain(terrain_group)

        for civ_group in full_data_set.civ_groups.values():
            cls._civ_group_to_civ(civ_group)

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
            AoCNyanSubprocessor._projectiles_from_line(unit_line)

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
                                      "engine.aux.game_entity.GameEntity")

        # =======================================================================
        # Modifiers
        # =======================================================================
        modifiers_set = []

        if unit_line.has_command(7) and not unit_line.is_projectile_shooter():
            modifiers_set.extend(AoCModifierSubprocessor.elevation_attack_modifiers(unit_line))

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
            AoCNyanSubprocessor._projectiles_from_line(building_line)

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

    @staticmethod
    def _tech_group_to_tech(tech_group):
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
        raw_api_object.add_raw_parent("engine.aux.tech.Tech")

        if isinstance(tech_group, UnitLineUpgrade):
            unit_line = dataset.unit_lines_vertical_ref[tech_group.get_line_id()]
            head_unit_id = unit_line.get_head_unit_id()
            obj_location = "data/game_entity/generic/%s/" % (name_lookup_dict[head_unit_id][1])

        else:
            obj_location = "data/tech/generic/%s/" % (tech_lookup_dict[tech_id][1])

        raw_api_object.set_location(obj_location)
        raw_api_object.set_filename(tech_lookup_dict[tech_id][1])
        tech_group.add_raw_api_object(raw_api_object)

        # =======================================================================
        # Types
        # =======================================================================
        raw_api_object.add_raw_member("types", [], "engine.aux.tech.Tech")

        # =======================================================================
        # Name
        # =======================================================================
        name_ref = "%s.%sName" % (tech_name, tech_name)
        name_raw_api_object = RawAPIObject(name_ref,
                                           "%sName"  % (tech_name),
                                           dataset.nyan_api_objects)
        name_raw_api_object.add_raw_parent("engine.aux.translated.type.TranslatedString")
        name_location = ForwardRef(tech_group, tech_name)
        name_raw_api_object.set_location(name_location)

        name_raw_api_object.add_raw_member("translations",
                                           [],
                                           "engine.aux.translated.type.TranslatedString")

        name_forward_ref = ForwardRef(tech_group, name_ref)
        raw_api_object.add_raw_member("name", name_forward_ref, "engine.aux.tech.Tech")
        tech_group.add_raw_api_object(name_raw_api_object)

        # =======================================================================
        # Description
        # =======================================================================
        description_ref = "%s.%sDescription" % (tech_name, tech_name)
        description_raw_api_object = RawAPIObject(description_ref,
                                                  "%sDescription"  % (tech_name),
                                                  dataset.nyan_api_objects)
        description_raw_api_object.add_raw_parent("engine.aux.translated.type.TranslatedMarkupFile")
        description_location = ForwardRef(tech_group, tech_name)
        description_raw_api_object.set_location(description_location)

        description_raw_api_object.add_raw_member("translations",
                                                  [],
                                                  "engine.aux.translated.type.TranslatedMarkupFile")

        description_forward_ref = ForwardRef(tech_group, description_ref)
        raw_api_object.add_raw_member("description",
                                      description_forward_ref,
                                      "engine.aux.tech.Tech")
        tech_group.add_raw_api_object(description_raw_api_object)

        # =======================================================================
        # Long description
        # =======================================================================
        long_description_ref = "%s.%sLongDescription" % (tech_name, tech_name)
        long_description_raw_api_object = RawAPIObject(long_description_ref,
                                                       "%sLongDescription"  % (tech_name),
                                                       dataset.nyan_api_objects)
        long_description_raw_api_object.add_raw_parent("engine.aux.translated.type.TranslatedMarkupFile")
        long_description_location = ForwardRef(tech_group, tech_name)
        long_description_raw_api_object.set_location(long_description_location)

        long_description_raw_api_object.add_raw_member("translations",
                                                       [],
                                                       "engine.aux.translated.type.TranslatedMarkupFile")

        long_description_forward_ref = ForwardRef(tech_group, long_description_ref)
        raw_api_object.add_raw_member("long_description",
                                      long_description_forward_ref,
                                      "engine.aux.tech.Tech")
        tech_group.add_raw_api_object(long_description_raw_api_object)

        # =======================================================================
        # Updates
        # =======================================================================
        patches = []
        patches.extend(DE2TechSubprocessor.get_patches(tech_group))
        raw_api_object.add_raw_member("updates", patches, "engine.aux.tech.Tech")

        # =======================================================================
        # Misc (Objects that are not used by the tech group itself, but use its values)
        # =======================================================================
        if tech_group.is_researchable():
            AoCAuxiliarySubprocessor.get_researchable_tech(tech_group)

    @staticmethod
    def _terrain_group_to_terrain(terrain_group):
        """
        Creates raw API objects for a terrain group.

        :param terrain_group: Terrain group that gets converted to a tech.
        :type terrain_group: ..dataformat.converter_object.ConverterObjectGroup
        """
        # TODO: Implement
    @staticmethod
    def _civ_group_to_civ(civ_group):
        """
        Creates raw API objects for a civ group.

        :param civ_group: Terrain group that gets converted to a tech.
        :type civ_group: ..dataformat.converter_object.ConverterObjectGroup
        """
        # TODO: Implement
    @staticmethod
    def _projectiles_from_line(line):
        """
        Creates Projectile(GameEntity) raw API objects for a unit/building line.

        :param line: Line for which the projectiles are extracted.
        :type line: ..dataformat.converter_object.ConverterObjectGroup
        """
        # TODO: Implement
