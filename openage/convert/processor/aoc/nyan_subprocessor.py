# Copyright 2019-2019 the openage authors. See copying.md for legal info.

"""
Convert API-like objects to nyan objects.
"""
from ...dataformat.aoc.internal_nyan_names import unit_line_lookups, class_id_lookups
from openage.convert.dataformat.converter_object import RawAPIObject
from openage.nyan.nyan_structs import NyanObject, MemberSpecialValue
from openage.convert.dataformat.aoc.genie_graphic import CombinedSprite
from openage.convert.dataformat.aoc.expected_pointer import ExpectedPointer
from openage.convert.dataformat.aoc.genie_unit import GenieVillagerGroup


class AoCNyanSubprocessor:

    @classmethod
    def convert(cls, gamedata):

        cls._process_game_entities(gamedata)

    @classmethod
    def _process_game_entities(cls, full_data_set):

        for unit_line in full_data_set.unit_lines.values():
            cls._unit_line_to_game_entity(unit_line)

        for building_line in full_data_set.building_lines.values():
            cls._building_line_to_game_entity(building_line)

        # TODO: Techs, civs, complex game entities

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
        game_entity_name = unit_line_lookups[current_unit_id][0]
        obj_location = "data/game_entity/unique/%s.nyan" % (unit_line_lookups[current_unit_id][1])
        raw_api_object = RawAPIObject(game_entity_name, game_entity_name,
                                      dataset.nyan_api_objects)
        raw_api_object.add_raw_parent("engine.aux.game_entity.GameEntity")
        raw_api_object.set_location(obj_location)
        unit_line.add_raw_api_object(raw_api_object)

        #=======================================================================
        # Game Entity Types
        #------------------
        # we give a unit two types
        #    - aux.game_entity_type.types.Unit (if unit_type >= 70)
        #    - aux.game_entity_type.types.<Class> (depending on the class)
        #=======================================================================
        # Create or use existing auxiliary types
        types_set = []
        unit_type = current_unit.get_member("unit_type").get_value()

        if unit_type >= 70:
            type_obj = dataset.pregen_nyan_objects["aux.game_entity_type.types.Unit"]
            types_set.append(type_obj)

        unit_class = current_unit.get_member("unit_class").get_value()
        class_name = class_id_lookups[unit_class]
        class_obj_name = "aux.game_entity_type.types.%s" % (class_name)

        # Create the game entity type on-the-fly if it not already exists
        if class_obj_name not in dataset.pregen_nyan_objects.keys():
            parents = [dataset.nyan_api_objects["engine.aux.game_entity_type.GameEntityType"]]
            new_game_entity_type = NyanObject(class_name, parents, None, None)
            dataset.pregen_nyan_objects.update({class_obj_name: new_game_entity_type})

        type_obj = dataset.pregen_nyan_objects[class_obj_name]
        types_set.append(type_obj)

        raw_api_object.add_raw_member("types", types_set)

        #=======================================================================
        # Abilities
        #=======================================================================
        abilities_set = []

        #=======================================================================
        # Idle ability
        #=======================================================================
        obj_name = "%s.Idle" % (game_entity_name)
        idle_raw_api_object = RawAPIObject(obj_name, "Idle", dataset.nyan_api_objects)
        idle_raw_api_object.add_raw_parent("engine.ability.type.Idle")
        idle_raw_api_object.set_location(obj_location)

        idle_animation_id = current_unit.get_member("idle_graphic0").get_value()

        if idle_animation_id > -1:
            # Make the ability animated
            idle_raw_api_object.add_raw_parent("engine.ability.specialization.AnimatedAbility")

            animations_set = []

            # Create animation object
            obj_name = "%s.Idle.IdleAnimation" % (game_entity_name)
            animation_raw_api_object = RawAPIObject(obj_name, "IdleAnimation", dataset.nyan_api_objects)
            animation_raw_api_object.add_raw_parent("engine.aux.graphics.Animation")
            animation_raw_api_object.set_location(obj_location)

            idle_sprite = CombinedSprite(idle_animation_id, dataset)
            animation_raw_api_object.add_raw_member("sprite", idle_sprite)

            animation_expected_pointer = ExpectedPointer(unit_line, obj_name)
            animations_set.append(animation_expected_pointer)

            idle_raw_api_object.add_raw_member("animations", animations_set)

            unit_line.add_raw_api_object(animation_raw_api_object)

        abilities_set.append(idle_raw_api_object)

        unit_line.add_raw_api_object(idle_raw_api_object)

        #=======================================================================
        # Move ability
        #=======================================================================
        obj_name = "%s.Move" % (game_entity_name)
        move_raw_api_object = RawAPIObject(obj_name, "Move", dataset.nyan_api_objects)
        move_raw_api_object.add_raw_parent("engine.ability.type.Move")
        move_raw_api_object.set_location(obj_location)

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
            animation_raw_api_object.set_location(obj_location)

            move_sprite = CombinedSprite(move_animation_id, dataset)
            animation_raw_api_object.add_raw_member("sprite", move_sprite)

            animation_expected_pointer = ExpectedPointer(unit_line, obj_name)
            animations_set.append(animation_expected_pointer)

            move_raw_api_object.add_raw_member("animations", animations_set)

            unit_line.add_raw_api_object(animation_raw_api_object)

        # Speed
        move_speed = current_unit.get_member("speed").get_value()
        move_raw_api_object.add_raw_member("speed", move_speed)

        # Diplomacy settings
        move_raw_api_object.add_raw_parent("engine.ability.specialization.DiplomaticAbility")
        diplomatic_stances = [dataset.nyan_api_objects["engine.aux.diplomatic_stance.type.Self"]]
        move_raw_api_object.add_raw_member("diplomatic_stances", diplomatic_stances)

        abilities_set.append(move_raw_api_object)

        unit_line.add_raw_api_object(move_raw_api_object)

        #=======================================================================
        # Turn ability
        #=======================================================================
        obj_name = "%s.Turn" % (game_entity_name)
        turn_raw_api_object = RawAPIObject(obj_name, "Turn", dataset.nyan_api_objects)
        turn_raw_api_object.add_raw_parent("engine.ability.type.Turn")
        turn_raw_api_object.set_location(obj_location)

        # Speed
        turn_speed_unmodified = current_unit.get_member("turn_speed").get_value()

        # Default case: Instant turning
        turn_speed = MemberSpecialValue.NYAN_INF

        # Ships/Trebuchets turn slower
        if turn_speed_unmodified >= 0:
            # TODO: Calculate this
            pass

        turn_raw_api_object.add_raw_member("turn_speed", turn_speed)

        # Diplomacy settings
        turn_raw_api_object.add_raw_parent("engine.ability.specialization.DiplomaticAbility")
        diplomatic_stances = [dataset.nyan_api_objects["engine.aux.diplomatic_stance.type.Self"]]
        turn_raw_api_object.add_raw_member("diplomatic_stances", diplomatic_stances)

        abilities_set.append(turn_raw_api_object)

        unit_line.add_raw_api_object(turn_raw_api_object)

        #=======================================================================
        # LineOfSight ability
        #=======================================================================
        obj_name = "%s.LineOfSight" % (game_entity_name)
        los_raw_api_object = RawAPIObject(obj_name, "LineOfSight", dataset.nyan_api_objects)
        los_raw_api_object.add_raw_parent("engine.ability.type.LineOfSight")
        los_raw_api_object.set_location(obj_location)

        # Line of sight
        line_of_sight = current_unit.get_member("line_of_sight").get_value()
        los_raw_api_object.add_raw_member("range", line_of_sight)

        # Diplomacy settings
        los_raw_api_object.add_raw_parent("engine.ability.specialization.DiplomaticAbility")
        diplomatic_stances = [dataset.nyan_api_objects["engine.aux.diplomatic_stance.type.Self"]]
        los_raw_api_object.add_raw_member("diplomatic_stances", diplomatic_stances)

        abilities_set.append(los_raw_api_object)

        unit_line.add_raw_api_object(los_raw_api_object)

        #=======================================================================
        # Visibility ability
        #=======================================================================
        obj_name = "%s.Visibility" % (game_entity_name)
        visibility_raw_api_object = RawAPIObject(obj_name, "Visibility", dataset.nyan_api_objects)
        visibility_raw_api_object.add_raw_parent("engine.ability.type.Visibility")
        visibility_raw_api_object.set_location(obj_location)

        # Units are not visible in fog
        visibility_raw_api_object.add_raw_member("visible_in_fog", False)

        abilities_set.append(visibility_raw_api_object)

        unit_line.add_raw_api_object(visibility_raw_api_object)

        #=======================================================================
        # Live ability
        #=======================================================================
        obj_name = "%s.Live" % (game_entity_name)
        live_raw_api_object = RawAPIObject(obj_name, "Live", dataset.nyan_api_objects)
        live_raw_api_object.add_raw_parent("engine.ability.type.Live")
        live_raw_api_object.set_location(obj_location)

        attributes_set = []

        health_raw_api_object = RawAPIObject(obj_name, "Health", dataset.nyan_api_objects)
        health_raw_api_object.add_raw_parent("engine.aux.attribute.AttributeSetting")
        health_raw_api_object.set_location(obj_location)

        attribute_value = dataset.pregen_nyan_objects["aux.attribute.types.Health"]
        health_raw_api_object.add_raw_member("attribute", attribute_value)

        # Lowest HP can go
        health_raw_api_object.add_raw_member("min_value", -1)

        # Max HP and starting HP
        max_hp_value = current_unit.get_member("hit_points").get_value()
        health_raw_api_object.add_raw_member("max_value", max_hp_value)
        health_raw_api_object.add_raw_member("starting_value", max_hp_value)

        attributes_set.append(health_raw_api_object)
        live_raw_api_object.add_raw_member("attributes", attributes_set)

        abilities_set.append(live_raw_api_object)

        unit_line.add_raw_api_object(health_raw_api_object)
        unit_line.add_raw_api_object(live_raw_api_object)

        #=======================================================================
        # Stop ability
        #=======================================================================
        obj_name = "%s.Stop" % (game_entity_name)
        stop_raw_api_object = RawAPIObject(obj_name, "Stop", dataset.nyan_api_objects)
        stop_raw_api_object.add_raw_parent("engine.ability.type.Stop")
        stop_raw_api_object.set_location(obj_location)

        # Diplomacy settings
        stop_raw_api_object.add_raw_parent("engine.ability.specialization.DiplomaticAbility")
        diplomatic_stances = [dataset.nyan_api_objects["engine.aux.diplomatic_stance.type.Self"]]
        stop_raw_api_object.add_raw_member("diplomatic_stances", diplomatic_stances)

        abilities_set.append(stop_raw_api_object)

        unit_line.add_raw_api_object(stop_raw_api_object)

        #=======================================================================
        # TODO: Bunch of other abilities
        #       Death, Selectable, Hitbox, Despawn, ApplyEffect, Resistance, ...
        #=======================================================================
        raw_api_object.add_raw_member("abilities", abilities_set)

        #=======================================================================
        # TODO: Modifiers
        #=======================================================================
        modifiers_set = []

        raw_api_object.add_raw_member("modifiers", modifiers_set)

        #=======================================================================
        # TODO: Variants
        #=======================================================================
        variants_set = []

        raw_api_object.add_raw_member("variants", variants_set)

    @staticmethod
    def _building_line_to_game_entity(building_line):
        """
        Creates raw API objects for a building line.

        :param unit_line: Unit line that gets converted to a game entity.
        :type unit_line: ..dataformat.converter_object.ConverterObjectGroup
        """
        pass
