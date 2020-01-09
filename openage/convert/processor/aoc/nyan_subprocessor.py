# Copyright 2019-2019 the openage authors. See copying.md for legal info.

"""
Convert API-like objects to nyan objects.
"""
from ...dataformat.aoc.internal_nyan_names import unit_line_lookups, class_id_lookups
from openage.convert.dataformat.converter_object import RawAPIObject
from openage.nyan.nyan_structs import NyanObject
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

        # Create the game entity type if it not already exists
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

            sprite = CombinedSprite(idle_animation_id, dataset)
            animation_raw_api_object.add_raw_member("sprite", sprite)

            animation_expected_pointer = ExpectedPointer(unit_line, obj_name)
            animations_set.append(animation_expected_pointer)

            idle_raw_api_object.add_raw_member("animations", animations_set)

            unit_line.add_raw_api_object(animation_raw_api_object)

        abilities_set.append(idle_raw_api_object)

        unit_line.add_raw_api_object(idle_raw_api_object)

        #=======================================================================
        # Move ability
        #=======================================================================

    @staticmethod
    def _building_line_to_game_entity(building_line):
        """
        Creates raw API objects for a building line.

        :param unit_line: Unit line that gets converted to a game entity.
        :type unit_line: ..dataformat.converter_object.ConverterObjectGroup
        """
        pass
