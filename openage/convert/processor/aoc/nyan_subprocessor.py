# Copyright 2019-2020 the openage authors. See copying.md for legal info.

"""
Convert API-like objects to nyan objects.
"""
from ...dataformat.aoc.internal_nyan_names import UNIT_LINE_LOOKUPS, CLASS_ID_LOOKUPS
from ...dataformat.converter_object import RawAPIObject
from ....nyan.nyan_structs import MemberSpecialValue
from ...dataformat.aoc.combined_sprite import CombinedSprite
from ...dataformat.aoc.expected_pointer import ExpectedPointer
from ...dataformat.aoc.genie_unit import GenieVillagerGroup


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

        # TODO: Techs, civs, more complex game entities

    @classmethod
    def _create_nyan_members(cls, full_data_set):
        """
        Fill nyan member values of the API objects.
        """
        for unit_line in full_data_set.unit_lines.values():
            unit_line.create_nyan_members()

        for building_line in full_data_set.building_lines.values():
            building_line.create_nyan_members()

        # TODO: Techs, civs, more complex game entities

    @classmethod
    def _process_game_entities(cls, full_data_set):

        for unit_line in full_data_set.unit_lines.values():
            cls._unit_line_to_game_entity(unit_line)

        for building_line in full_data_set.building_lines.values():
            cls._building_line_to_game_entity(building_line)

        # TODO: Techs, civs, more complex game entities

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

        # =======================================================================
        # Idle ability
        # =======================================================================
        obj_name = "%s.Idle" % (game_entity_name)
        idle_raw_api_object = RawAPIObject(obj_name, "Idle", dataset.nyan_api_objects)
        idle_raw_api_object.add_raw_parent("engine.ability.type.Idle")
        idle_location = ExpectedPointer(unit_line, game_entity_name)
        idle_raw_api_object.set_location(idle_location)

        idle_animation_id = current_unit.get_member("idle_graphic0").get_value()

        if idle_animation_id > -1:
            # Make the ability animated
            idle_raw_api_object.add_raw_parent("engine.ability.specialization.AnimatedAbility")

            animations_set = []

            # Create animation object
            obj_name = "%s.Idle.IdleAnimation" % (game_entity_name)
            animation_raw_api_object = RawAPIObject(obj_name, "IdleAnimation", dataset.nyan_api_objects)
            animation_raw_api_object.add_raw_parent("engine.aux.graphics.Animation")
            animation_location = ExpectedPointer(unit_line, "%s.Idle" % (game_entity_name))
            animation_raw_api_object.set_location(animation_location)

            idle_sprite = CombinedSprite(idle_animation_id,
                                         "idle_%s" % (UNIT_LINE_LOOKUPS[current_unit_id][1]),
                                         dataset)
            dataset.combined_sprites.update({idle_sprite.get_id(): idle_sprite})
            idle_sprite.add_reference(animation_raw_api_object)

            animation_raw_api_object.add_raw_member("sprite", idle_sprite,
                                                    "engine.aux.graphics.Animation")

            animation_expected_pointer = ExpectedPointer(unit_line, obj_name)
            animations_set.append(animation_expected_pointer)

            idle_raw_api_object.add_raw_member("animations", animations_set,
                                               "engine.ability.specialization.AnimatedAbility")

            unit_line.add_raw_api_object(animation_raw_api_object)

        abilities_set.append(idle_raw_api_object)

        unit_line.add_raw_api_object(idle_raw_api_object)

        # =======================================================================
        # Move ability
        # =======================================================================
        obj_name = "%s.Move" % (game_entity_name)
        move_raw_api_object = RawAPIObject(obj_name, "Move", dataset.nyan_api_objects)
        move_raw_api_object.add_raw_parent("engine.ability.type.Move")
        move_location = ExpectedPointer(unit_line, game_entity_name)
        move_raw_api_object.set_location(move_location)

        # Animation
        move_animation_id = current_unit.get_member("move_graphics").get_value()

        if move_animation_id > -1:
            # Make the ability animated
            move_raw_api_object.add_raw_parent("engine.ability.specialization.AnimatedAbility")

            animations_set = []

            # Create animation object
            obj_name = "%s.Move.MoveAnimation" % (game_entity_name)
            animation_raw_api_object = RawAPIObject(obj_name, "MoveAnimation", dataset.nyan_api_objects)
            animation_raw_api_object.add_raw_parent("engine.aux.graphics.Animation")
            animation_location = ExpectedPointer(unit_line, "%s.Move" % (game_entity_name))
            animation_raw_api_object.set_location(animation_location)

            move_sprite = CombinedSprite(move_animation_id,
                                         "move_%s" % (UNIT_LINE_LOOKUPS[current_unit_id][1]),
                                         dataset)
            dataset.combined_sprites.update({move_sprite.get_id(): move_sprite})
            move_sprite.add_reference(animation_raw_api_object)

            animation_raw_api_object.add_raw_member("sprite", move_sprite,
                                                    "engine.aux.graphics.Animation")

            animation_expected_pointer = ExpectedPointer(unit_line, obj_name)
            animations_set.append(animation_expected_pointer)

            move_raw_api_object.add_raw_member("animations", animations_set,
                                               "engine.ability.specialization.AnimatedAbility")

            unit_line.add_raw_api_object(animation_raw_api_object)

        # Speed
        move_speed = current_unit.get_member("speed").get_value()
        move_raw_api_object.add_raw_member("speed", move_speed, "engine.ability.type.Move")

        # Standard move modes
        move_modes = [dataset.nyan_api_objects["engine.aux.move_mode.type.AttackMove"],
                      dataset.nyan_api_objects["engine.aux.move_mode.type.Normal"],
                      dataset.nyan_api_objects["engine.aux.move_mode.type.Patrol"]]
        # Follow
        obj_name = "%s.Move.Follow" % (game_entity_name)
        follow_raw_api_object = RawAPIObject(obj_name, "Follow", dataset.nyan_api_objects)
        follow_raw_api_object.add_raw_parent("engine.aux.move_mode.type.Follow")
        follow_location = ExpectedPointer(unit_line, "%s.Move" % (game_entity_name))
        follow_raw_api_object.set_location(follow_location)

        follow_range = current_unit.get_member("line_of_sight").get_value() - 1
        follow_raw_api_object.add_raw_member("range", follow_range, "engine.aux.move_mode.type.Follow")

        unit_line.add_raw_api_object(follow_raw_api_object)
        move_modes.append(follow_raw_api_object)

        move_raw_api_object.add_raw_member("modes", move_modes, "engine.ability.type.Move")

        # Diplomacy settings
        move_raw_api_object.add_raw_parent("engine.ability.specialization.DiplomaticAbility")
        diplomatic_stances = [dataset.nyan_api_objects["engine.aux.diplomatic_stance.type.Self"]]
        move_raw_api_object.add_raw_member("stances", diplomatic_stances,
                                           "engine.ability.specialization.DiplomaticAbility")

        abilities_set.append(move_raw_api_object)

        unit_line.add_raw_api_object(move_raw_api_object)

        # =======================================================================
        # Turn ability
        # =======================================================================
        obj_name = "%s.Turn" % (game_entity_name)
        turn_raw_api_object = RawAPIObject(obj_name, "Turn", dataset.nyan_api_objects)
        turn_raw_api_object.add_raw_parent("engine.ability.type.Turn")
        turn_location = ExpectedPointer(unit_line, game_entity_name)
        turn_raw_api_object.set_location(turn_location)

        # Speed
        turn_speed_unmodified = current_unit.get_member("turn_speed").get_value()

        # Default case: Instant turning
        turn_speed = MemberSpecialValue.NYAN_INF

        # Ships/Trebuchets turn slower
        if turn_speed_unmodified >= 0:
            # TODO: Calculate this
            pass

        turn_raw_api_object.add_raw_member("turn_speed", turn_speed, "engine.ability.type.Turn")

        # Diplomacy settings
        turn_raw_api_object.add_raw_parent("engine.ability.specialization.DiplomaticAbility")
        diplomatic_stances = [dataset.nyan_api_objects["engine.aux.diplomatic_stance.type.Self"]]
        turn_raw_api_object.add_raw_member("stances", diplomatic_stances,
                                           "engine.ability.specialization.DiplomaticAbility")

        abilities_set.append(turn_raw_api_object)

        unit_line.add_raw_api_object(turn_raw_api_object)

        # =======================================================================
        # LineOfSight ability
        # =======================================================================
        obj_name = "%s.LineOfSight" % (game_entity_name)
        los_raw_api_object = RawAPIObject(obj_name, "LineOfSight", dataset.nyan_api_objects)
        los_raw_api_object.add_raw_parent("engine.ability.type.LineOfSight")
        los_location = ExpectedPointer(unit_line, game_entity_name)
        los_raw_api_object.set_location(los_location)

        # Line of sight
        line_of_sight = current_unit.get_member("line_of_sight").get_value()
        los_raw_api_object.add_raw_member("range", line_of_sight,
                                          "engine.ability.type.LineOfSight")

        # Diplomacy settings
        los_raw_api_object.add_raw_parent("engine.ability.specialization.DiplomaticAbility")
        diplomatic_stances = [dataset.nyan_api_objects["engine.aux.diplomatic_stance.type.Self"]]
        los_raw_api_object.add_raw_member("stances", diplomatic_stances,
                                          "engine.ability.specialization.DiplomaticAbility")

        abilities_set.append(los_raw_api_object)

        unit_line.add_raw_api_object(los_raw_api_object)

        # =======================================================================
        # Visibility ability
        # =======================================================================
        obj_name = "%s.Visibility" % (game_entity_name)
        visibility_raw_api_object = RawAPIObject(obj_name, "Visibility", dataset.nyan_api_objects)
        visibility_raw_api_object.add_raw_parent("engine.ability.type.Visibility")
        visibility_location = ExpectedPointer(unit_line, game_entity_name)
        visibility_raw_api_object.set_location(visibility_location)

        # Units are not visible in fog
        visibility_raw_api_object.add_raw_member("visible_in_fog", False,
                                                 "engine.ability.type.Visibility")

        abilities_set.append(visibility_raw_api_object)

        unit_line.add_raw_api_object(visibility_raw_api_object)

        # =======================================================================
        # Live ability
        # =======================================================================
        obj_name = "%s.Live" % (game_entity_name)
        live_raw_api_object = RawAPIObject(obj_name, "Live", dataset.nyan_api_objects)
        live_raw_api_object.add_raw_parent("engine.ability.type.Live")
        live_location = ExpectedPointer(unit_line, game_entity_name)
        live_raw_api_object.set_location(live_location)

        attributes_set = []

        obj_name = "%s.Live.Health" % (game_entity_name)
        health_raw_api_object = RawAPIObject(obj_name, "Health", dataset.nyan_api_objects)
        health_raw_api_object.add_raw_parent("engine.aux.attribute.AttributeSetting")
        health_location = ExpectedPointer(unit_line, "%s.Live" % (game_entity_name))
        health_raw_api_object.set_location(health_location)

        attribute_value = dataset.pregen_nyan_objects["aux.attribute.types.Health"].get_nyan_object()
        health_raw_api_object.add_raw_member("attribute", attribute_value,
                                             "engine.aux.attribute.AttributeSetting")

        # Lowest HP can go
        health_raw_api_object.add_raw_member("min_value", -1,
                                             "engine.aux.attribute.AttributeSetting")

        # Max HP and starting HP
        max_hp_value = current_unit.get_member("hit_points").get_value()
        health_raw_api_object.add_raw_member("max_value", max_hp_value,
                                             "engine.aux.attribute.AttributeSetting")
        health_raw_api_object.add_raw_member("starting_value", max_hp_value,
                                             "engine.aux.attribute.AttributeSetting")

        attributes_set.append(health_raw_api_object)
        live_raw_api_object.add_raw_member("attributes", attributes_set,
                                           "engine.ability.type.Live")

        abilities_set.append(live_raw_api_object)

        unit_line.add_raw_api_object(health_raw_api_object)
        unit_line.add_raw_api_object(live_raw_api_object)

        # =======================================================================
        # Stop ability
        # =======================================================================
        obj_name = "%s.Stop" % (game_entity_name)
        stop_raw_api_object = RawAPIObject(obj_name, "Stop", dataset.nyan_api_objects)
        stop_raw_api_object.add_raw_parent("engine.ability.type.Stop")
        stop_location = ExpectedPointer(unit_line, game_entity_name)
        stop_raw_api_object.set_location(stop_location)

        # Diplomacy settings
        stop_raw_api_object.add_raw_parent("engine.ability.specialization.DiplomaticAbility")
        diplomatic_stances = [dataset.nyan_api_objects["engine.aux.diplomatic_stance.type.Self"]]
        stop_raw_api_object.add_raw_member("stances", diplomatic_stances,
                                           "engine.ability.specialization.DiplomaticAbility")

        abilities_set.append(stop_raw_api_object)

        unit_line.add_raw_api_object(stop_raw_api_object)

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

    @staticmethod
    def _building_line_to_game_entity(building_line):
        """
        Creates raw API objects for a building line.

        :param unit_line: Unit line that gets converted to a game entity.
        :type unit_line: ..dataformat.converter_object.ConverterObjectGroup
        """
        pass
