# Copyright 2019-2020 the openage authors. See copying.md for legal info.

"""
Convert API-like objects to nyan objects. Subroutine of the
main AoC processor.
"""
from ...dataformat.aoc.internal_nyan_names import UNIT_LINE_LOOKUPS, CLASS_ID_LOOKUPS,\
    BUILDING_LINE_LOOKUPS, TECH_GROUP_LOOKUPS
from ...dataformat.converter_object import RawAPIObject
from ...dataformat.aoc.genie_unit import GenieVillagerGroup
from .ability_subprocessor import AoCAbilitySubprocessor
from openage.convert.processor.aoc.auxiliary_subprocessor import AoCAuxiliarySubprocessor
from openage.convert.dataformat.aoc.expected_pointer import ExpectedPointer
from openage.convert.dataformat.aoc.genie_tech import UnitLineUpgrade


class AoCNyanSubprocessor:

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

        for building_line in full_data_set.building_lines.values():
            building_line.create_nyan_objects()

        for tech_group in full_data_set.tech_groups.values():
            tech_group.create_nyan_objects()

        # TODO: civs, more complex game entities

    @classmethod
    def _create_nyan_members(cls, full_data_set):
        """
        Fill nyan member values of the API objects.
        """
        for unit_line in full_data_set.unit_lines.values():
            unit_line.create_nyan_members()

        for building_line in full_data_set.building_lines.values():
            building_line.create_nyan_members()

        for tech_group in full_data_set.tech_groups.values():
            tech_group.create_nyan_members()

        # TODO: civs, more complex game entities

    @classmethod
    def _process_game_entities(cls, full_data_set):

        for unit_line in full_data_set.unit_lines.values():
            cls._unit_line_to_game_entity(unit_line)

        for building_line in full_data_set.building_lines.values():
            cls._building_line_to_game_entity(building_line)

        for tech_group in full_data_set.tech_groups.values():
            if tech_group.is_researchable():
                cls._tech_group_to_tech(tech_group)

        # TODO: civs, more complex game entities

    @staticmethod
    def _unit_line_to_game_entity(unit_line):
        """
        Creates raw API objects for a unit line.

        TODO: Convert other units than the head unit.

        :param unit_line: Unit line that gets converted to a game entity.
        :type unit_line: ..dataformat.converter_object.ConverterObjectGroup
        """
        if isinstance(unit_line, GenieVillagerGroup):
            # TODO: Requires special treatment?
            current_unit = unit_line.variants[0].line[0]

        else:
            current_unit = unit_line.line[0]

        current_unit_id = unit_line.get_head_unit_id()

        dataset = unit_line.data

        # Start with the generic GameEntity
        game_entity_name = UNIT_LINE_LOOKUPS[current_unit_id][0]
        obj_location = "data/game_entity/generic/%s/" % (UNIT_LINE_LOOKUPS[current_unit_id][1])
        raw_api_object = RawAPIObject(game_entity_name, game_entity_name,
                                      dataset.nyan_api_objects)
        raw_api_object.add_raw_parent("engine.aux.game_entity.GameEntity")
        raw_api_object.set_location(obj_location)
        raw_api_object.set_filename(UNIT_LINE_LOOKUPS[current_unit_id][1])
        unit_line.add_raw_api_object(raw_api_object)

        # =======================================================================
        # Game Entity Types
        # ------------------
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
        class_name = CLASS_ID_LOOKUPS[unit_class]
        class_obj_name = "aux.game_entity_type.types.%s" % (class_name)

        # Create the game entity type on-the-fly if it not already exists
        if class_obj_name not in dataset.pregen_nyan_objects.keys():
            type_location = "data/aux/game_entity_type/"
            new_game_entity_type = RawAPIObject(class_obj_name, class_name,
                                                dataset.nyan_api_objects, type_location)
            new_game_entity_type.set_filename("types")
            new_game_entity_type.add_raw_parent("engine.aux.game_entity_type.GameEntityType")
            new_game_entity_type.create_nyan_object()
            dataset.pregen_nyan_objects.update({class_obj_name: new_game_entity_type})

        type_obj = dataset.pregen_nyan_objects[class_obj_name].get_nyan_object()
        types_set.append(type_obj)

        raw_api_object.add_raw_member("types", types_set, "engine.aux.game_entity.GameEntity")

        # =======================================================================
        # Abilities
        # =======================================================================
        abilities_set = []

        if len(unit_line.creates) > 0:
            abilities_set.append(AoCAbilitySubprocessor.create_ability(unit_line))

        ability = AoCAbilitySubprocessor.use_contingent_ability(unit_line)
        if ability:
            abilities_set.append(ability)

        has_primary_projectile, has_secondary_projectile = unit_line.is_ranged()
        if has_primary_projectile:
            abilities_set.append(AoCAbilitySubprocessor.shoot_projectile_ability(unit_line))

            # Projectile abilities are not appended to the unit, but to the projectile
            AoCAbilitySubprocessor.projectile_ability(unit_line, position=0)
            # This creates the projectiles and attaches abilities
            AoCAuxiliarySubprocessor.get_projectile(unit_line, position=0)

            # Same for secondary projectile if it exists
            if has_secondary_projectile:
                AoCAbilitySubprocessor.projectile_ability(unit_line, position=1)
                AoCAuxiliarySubprocessor.get_projectile(unit_line, position=1)

        abilities_set.append(AoCAbilitySubprocessor.idle_ability(unit_line))
        abilities_set.append(AoCAbilitySubprocessor.hitbox_ability(unit_line))
        abilities_set.append(AoCAbilitySubprocessor.live_ability(unit_line))
        abilities_set.append(AoCAbilitySubprocessor.los_ability(unit_line))
        abilities_set.append(AoCAbilitySubprocessor.move_ability(unit_line))
        abilities_set.append(AoCAbilitySubprocessor.named_ability(unit_line))
        abilities_set.append(AoCAbilitySubprocessor.stop_ability(unit_line))
        abilities_set.append(AoCAbilitySubprocessor.turn_ability(unit_line))
        abilities_set.append(AoCAbilitySubprocessor.visibility_ability(unit_line))

        # =======================================================================
        # TODO: Bunch of other abilities
        #       Death, Selectable, Hitbox, Despawn, ApplyEffect, Resistance, ...
        # =======================================================================
        raw_api_object.add_raw_member("abilities", abilities_set,
                                      "engine.aux.game_entity.GameEntity")

        # =======================================================================
        # TODO: Modifiers
        # =======================================================================
        modifiers_set = []

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

        # Start with the generic GameEntity
        game_entity_name = BUILDING_LINE_LOOKUPS[current_building_id][0]
        obj_location = "data/game_entity/generic/%s/" % (BUILDING_LINE_LOOKUPS[current_building_id][1])
        raw_api_object = RawAPIObject(game_entity_name, game_entity_name,
                                      dataset.nyan_api_objects)
        raw_api_object.add_raw_parent("engine.aux.game_entity.GameEntity")
        raw_api_object.set_location(obj_location)
        raw_api_object.set_filename(BUILDING_LINE_LOOKUPS[current_building_id][1])
        building_line.add_raw_api_object(raw_api_object)

        # =======================================================================
        # TODO: Game Entity Types
        # ------------------
        # we give a unit two types
        #    - aux.game_entity_type.types.Unit (if unit_type >= 70)
        #    - aux.game_entity_type.types.<Class> (depending on the class)
        # =======================================================================
        # Create or use existing auxiliary types
        types_set = []
        unit_type = current_building.get_member("unit_type").get_value()

        if unit_type >= 80:
            type_obj = dataset.pregen_nyan_objects["aux.game_entity_type.types.Building"].get_nyan_object()
            types_set.append(type_obj)

        unit_class = current_building.get_member("unit_class").get_value()
        class_name = CLASS_ID_LOOKUPS[unit_class]
        class_obj_name = "aux.game_entity_type.types.%s" % (class_name)

        # Create the game entity type on-the-fly if it not already exists
        if class_obj_name not in dataset.pregen_nyan_objects.keys():
            type_location = "data/aux/game_entity_type/"
            new_game_entity_type = RawAPIObject(class_obj_name, class_name,
                                                dataset.nyan_api_objects, type_location)
            new_game_entity_type.set_filename("types")
            new_game_entity_type.add_raw_parent("engine.aux.game_entity_type.GameEntityType")
            new_game_entity_type.create_nyan_object()
            dataset.pregen_nyan_objects.update({class_obj_name: new_game_entity_type})

        type_obj = dataset.pregen_nyan_objects[class_obj_name].get_nyan_object()
        types_set.append(type_obj)

        raw_api_object.add_raw_member("types", types_set, "engine.aux.game_entity.GameEntity")

        # =======================================================================
        # Abilities
        # =======================================================================
        abilities_set = []

        if len(building_line.creates) > 0:
            abilities_set.append(AoCAbilitySubprocessor.create_ability(building_line))
            abilities_set.append(AoCAbilitySubprocessor.production_queue_ability(building_line))

        if len(building_line.researches) > 0:
            abilities_set.append(AoCAbilitySubprocessor.research_ability(building_line))

        ability = AoCAbilitySubprocessor.provide_contingent_ability(building_line)
        if ability:
            abilities_set.append(ability)

        has_primary_projectile, has_secondary_projectile = building_line.is_ranged()
        if has_primary_projectile:
            abilities_set.append(AoCAbilitySubprocessor.shoot_projectile_ability(building_line))

            # Projectile abilities are not appended to the unit, but to the projectile
            AoCAbilitySubprocessor.projectile_ability(building_line, position=0)
            # This creates the projectiles and attaches abilities
            AoCAuxiliarySubprocessor.get_projectile(building_line, position=0)

            # Same for secondary projectile if it exists
            if has_secondary_projectile:
                AoCAbilitySubprocessor.projectile_ability(building_line, position=1)
                AoCAuxiliarySubprocessor.get_projectile(building_line, position=1)

        abilities_set.append(AoCAbilitySubprocessor.idle_ability(building_line))
        abilities_set.append(AoCAbilitySubprocessor.hitbox_ability(building_line))
        abilities_set.append(AoCAbilitySubprocessor.live_ability(building_line))
        abilities_set.append(AoCAbilitySubprocessor.los_ability(building_line))
        abilities_set.append(AoCAbilitySubprocessor.named_ability(building_line))
        abilities_set.append(AoCAbilitySubprocessor.stop_ability(building_line))
        abilities_set.append(AoCAbilitySubprocessor.visibility_ability(building_line))
        # =======================================================================
        # TODO: Bunch of other abilities
        #       Death, Selectable, Hitbox, Despawn, ApplyEffect, Resistance, ...
        # =======================================================================
        raw_api_object.add_raw_member("abilities", abilities_set,
                                      "engine.aux.game_entity.GameEntity")

        # =======================================================================
        # TODO: Modifiers
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

        # Start with the Tech object
        tech_name = TECH_GROUP_LOOKUPS[tech_id][0]
        raw_api_object = RawAPIObject(tech_name, tech_name,
                                      dataset.nyan_api_objects)
        raw_api_object.add_raw_parent("engine.aux.tech.Tech")

        if isinstance(tech_group, UnitLineUpgrade):
            unit_line = dataset.unit_lines_vertical_ref[tech_group.get_line_id()]
            head_unit_id = unit_line.get_head_unit_id()
            obj_location = "data/game_entity/generic/%s/" % (UNIT_LINE_LOOKUPS[head_unit_id][1])

        else:
            obj_location = "data/tech/generic/%s/" % (TECH_GROUP_LOOKUPS[tech_id][1])

        raw_api_object.set_location(obj_location)
        raw_api_object.set_filename(TECH_GROUP_LOOKUPS[tech_id][1])
        tech_group.add_raw_api_object(raw_api_object)

        # =======================================================================
        # Name
        # =======================================================================
        name_ref = "%s.%sName" % (tech_name, tech_name)
        name_raw_api_object = RawAPIObject(name_ref,
                                           "%sName"  % (tech_name),
                                           dataset.nyan_api_objects)
        name_raw_api_object.add_raw_parent("engine.aux.translated.type.TranslatedString")
        name_location = ExpectedPointer(tech_group, tech_name)
        name_raw_api_object.set_location(name_location)

        name_raw_api_object.add_raw_member("translations",
                                           [],
                                           "engine.aux.translated.type.TranslatedString")

        name_expected_pointer = ExpectedPointer(tech_group, name_ref)
        raw_api_object.add_raw_member("name", name_expected_pointer, "engine.aux.tech.Tech")
        tech_group.add_raw_api_object(name_raw_api_object)

        # =======================================================================
        # Description
        # =======================================================================
        description_ref = "%s.%sDescription" % (tech_name, tech_name)
        description_raw_api_object = RawAPIObject(description_ref,
                                                  "%sDescription"  % (tech_name),
                                                  dataset.nyan_api_objects)
        description_raw_api_object.add_raw_parent("engine.aux.translated.type.TranslatedMarkupFile")
        description_location = ExpectedPointer(tech_group, tech_name)
        description_raw_api_object.set_location(description_location)

        description_raw_api_object.add_raw_member("translations",
                                                  [],
                                                  "engine.aux.translated.type.TranslatedMarkupFile")

        description_expected_pointer = ExpectedPointer(tech_group, description_ref)
        raw_api_object.add_raw_member("description",
                                      description_expected_pointer,
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
        long_description_location = ExpectedPointer(tech_group, tech_name)
        long_description_raw_api_object.set_location(long_description_location)

        long_description_raw_api_object.add_raw_member("translations",
                                                       [],
                                                       "engine.aux.translated.type.TranslatedMarkupFile")

        long_description_expected_pointer = ExpectedPointer(tech_group, long_description_ref)
        raw_api_object.add_raw_member("long_description",
                                      long_description_expected_pointer,
                                      "engine.aux.tech.Tech")
        tech_group.add_raw_api_object(long_description_raw_api_object)

        # =======================================================================
        # TODO: Updates
        # =======================================================================
        raw_api_object.add_raw_member("updates", [], "engine.aux.tech.Tech")

        # =======================================================================
        # Misc (Objects that are not used by the tech group itself, but use its values)
        # =======================================================================
        if tech_group.is_researchable():
            AoCAuxiliarySubprocessor.get_researchable_tech(tech_group)
