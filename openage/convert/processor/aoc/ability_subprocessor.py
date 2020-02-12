# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Derives and adds abilities for to unit lines. Subroutine of the
nyan subprocessor.
"""
from ...dataformat.converter_object import RawAPIObject
from ...dataformat.aoc.expected_pointer import ExpectedPointer
from ...dataformat.aoc.internal_nyan_names import UNIT_LINE_LOOKUPS
from ...dataformat.aoc.genie_unit import GenieVillagerGroup
from ...dataformat.aoc.combined_sprite import CombinedSprite
from openage.nyan.nyan_structs import MemberSpecialValue


class AoCAbilitySubprocessor:

    @staticmethod
    def idle_ability(unit_line):
        """
        Adds the Idle ability to a unit line.

        :param unit_line: Unit line that gets the ability.
        :type unit_line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        if isinstance(unit_line, GenieVillagerGroup):
            # TODO: Requires special treatment?
            current_unit = unit_line.variants[0].line[0]

        else:
            current_unit = unit_line.line[0]

        current_unit_id = unit_line.get_head_unit_id()
        dataset = unit_line.data
        game_entity_name = UNIT_LINE_LOOKUPS[current_unit_id][0]

        obj_name = "%s.Idle" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(obj_name, "Idle", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Idle")
        ability_location = ExpectedPointer(unit_line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        ability_animation_id = current_unit.get_member("idle_graphic0").get_value()

        if ability_animation_id > -1:
            # Make the ability animated
            ability_raw_api_object.add_raw_parent("engine.ability.specialization.AnimatedAbility")

            animations_set = []

            # Create animation object
            obj_name = "%s.Idle.IdleAnimation" % (game_entity_name)
            animation_raw_api_object = RawAPIObject(obj_name, "IdleAnimation",
                                                    dataset.nyan_api_objects)
            animation_raw_api_object.add_raw_parent("engine.aux.graphics.Animation")
            animation_location = ExpectedPointer(unit_line, "%s.Idle" % (game_entity_name))
            animation_raw_api_object.set_location(animation_location)

            ability_sprite = CombinedSprite(ability_animation_id,
                                            "idle_%s" % (UNIT_LINE_LOOKUPS[current_unit_id][1]),
                                            dataset)
            dataset.combined_sprites.update({ability_sprite.get_id(): ability_sprite})
            ability_sprite.add_reference(animation_raw_api_object)

            animation_raw_api_object.add_raw_member("sprite", ability_sprite,
                                                    "engine.aux.graphics.Animation")

            animation_expected_pointer = ExpectedPointer(unit_line, obj_name)
            animations_set.append(animation_expected_pointer)

            ability_raw_api_object.add_raw_member("animations", animations_set,
                                                  "engine.ability.specialization.AnimatedAbility")

            unit_line.add_raw_api_object(animation_raw_api_object)

        unit_line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(unit_line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def live_ability(unit_line):
        """
        Adds the Live ability to a unit line.

        :param unit_line: Unit line that gets the ability.
        :type unit_line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        if isinstance(unit_line, GenieVillagerGroup):
            # TODO: Requires special treatment?
            current_unit = unit_line.variants[0].line[0]

        else:
            current_unit = unit_line.line[0]

        current_unit_id = unit_line.get_head_unit_id()
        dataset = unit_line.data
        game_entity_name = UNIT_LINE_LOOKUPS[current_unit_id][0]

        obj_name = "%s.Live" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(obj_name, "Live", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Live")
        ability_location = ExpectedPointer(unit_line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

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

        health_expected_pointer = ExpectedPointer(unit_line, health_raw_api_object.get_id())
        attributes_set.append(health_expected_pointer)
        ability_raw_api_object.add_raw_member("attributes", attributes_set,
                                              "engine.ability.type.Live")

        unit_line.add_raw_api_object(health_raw_api_object)
        unit_line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(unit_line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def los_ability(unit_line):
        """
        Adds the LineOfSight ability to a unit line.

        :param unit_line: Unit line that gets the ability.
        :type unit_line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        if isinstance(unit_line, GenieVillagerGroup):
            # TODO: Requires special treatment?
            current_unit = unit_line.variants[0].line[0]

        else:
            current_unit = unit_line.line[0]

        current_unit_id = unit_line.get_head_unit_id()
        dataset = unit_line.data
        game_entity_name = UNIT_LINE_LOOKUPS[current_unit_id][0]

        obj_name = "%s.LineOfSight" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(obj_name, "LineOfSight", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.LineOfSight")
        ability_location = ExpectedPointer(unit_line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Line of sight
        line_of_sight = current_unit.get_member("line_of_sight").get_value()
        ability_raw_api_object.add_raw_member("range", line_of_sight,
                                              "engine.ability.type.LineOfSight")

        # Diplomacy settings
        ability_raw_api_object.add_raw_parent("engine.ability.specialization.DiplomaticAbility")
        diplomatic_stances = [dataset.nyan_api_objects["engine.aux.diplomatic_stance.type.Self"]]
        ability_raw_api_object.add_raw_member("stances", diplomatic_stances,
                                              "engine.ability.specialization.DiplomaticAbility")

        unit_line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(unit_line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def move_ability(unit_line):
        """
        Adds the Move ability to a unit line.

        :param unit_line: Unit line that gets the ability.
        :type unit_line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        if isinstance(unit_line, GenieVillagerGroup):
            # TODO: Requires special treatment?
            current_unit = unit_line.variants[0].line[0]

        else:
            current_unit = unit_line.line[0]

        current_unit_id = unit_line.get_head_unit_id()
        dataset = unit_line.data
        game_entity_name = UNIT_LINE_LOOKUPS[current_unit_id][0]

        obj_name = "%s.Move" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(obj_name, "Move", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Move")
        ability_location = ExpectedPointer(unit_line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Animation
        ability_animation_id = current_unit.get_member("move_graphics").get_value()

        if ability_animation_id > -1:
            # Make the ability animated
            ability_raw_api_object.add_raw_parent("engine.ability.specialization.AnimatedAbility")

            animations_set = []

            # Create animation object
            obj_name = "%s.Move.MoveAnimation" % (game_entity_name)
            animation_raw_api_object = RawAPIObject(obj_name, "MoveAnimation",
                                                    dataset.nyan_api_objects)
            animation_raw_api_object.add_raw_parent("engine.aux.graphics.Animation")
            animation_location = ExpectedPointer(unit_line, "%s.Move" % (game_entity_name))
            animation_raw_api_object.set_location(animation_location)

            ability_sprite = CombinedSprite(ability_animation_id,
                                            "move_%s" % (UNIT_LINE_LOOKUPS[current_unit_id][1]),
                                            dataset)
            dataset.combined_sprites.update({ability_sprite.get_id(): ability_sprite})
            ability_sprite.add_reference(animation_raw_api_object)

            animation_raw_api_object.add_raw_member("sprite", ability_sprite,
                                                    "engine.aux.graphics.Animation")

            animation_expected_pointer = ExpectedPointer(unit_line, obj_name)
            animations_set.append(animation_expected_pointer)

            ability_raw_api_object.add_raw_member("animations", animations_set,
                                                  "engine.ability.specialization.AnimatedAbility")

            unit_line.add_raw_api_object(animation_raw_api_object)

        # Speed
        speed = current_unit.get_member("speed").get_value()
        ability_raw_api_object.add_raw_member("speed", speed, "engine.ability.type.Move")

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
        follow_raw_api_object.add_raw_member("range", follow_range,
                                             "engine.aux.move_mode.type.Follow")

        unit_line.add_raw_api_object(follow_raw_api_object)
        follow_expected_pointer = ExpectedPointer(unit_line, follow_raw_api_object.get_id())
        move_modes.append(follow_expected_pointer)

        ability_raw_api_object.add_raw_member("modes", move_modes, "engine.ability.type.Move")

        # Diplomacy settings
        ability_raw_api_object.add_raw_parent("engine.ability.specialization.DiplomaticAbility")
        diplomatic_stances = [dataset.nyan_api_objects["engine.aux.diplomatic_stance.type.Self"]]
        ability_raw_api_object.add_raw_member("stances", diplomatic_stances,
                                              "engine.ability.specialization.DiplomaticAbility")

        unit_line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(unit_line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def stop_ability(unit_line):
        """
        Adds the Stop ability to a unit line.

        :param unit_line: Unit line that gets the ability.
        :type unit_line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        current_unit_id = unit_line.get_head_unit_id()
        dataset = unit_line.data
        game_entity_name = UNIT_LINE_LOOKUPS[current_unit_id][0]

        obj_name = "%s.Stop" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(obj_name, "Stop", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Stop")
        ability_location = ExpectedPointer(unit_line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Diplomacy settings
        ability_raw_api_object.add_raw_parent("engine.ability.specialization.DiplomaticAbility")
        diplomatic_stances = [dataset.nyan_api_objects["engine.aux.diplomatic_stance.type.Self"]]
        ability_raw_api_object.add_raw_member("stances", diplomatic_stances,
                                              "engine.ability.specialization.DiplomaticAbility")

        unit_line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(unit_line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def turn_ability(unit_line):
        """
        Adds the Turn ability to a unit line.

        :param unit_line: Unit line that gets the ability.
        :type unit_line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        if isinstance(unit_line, GenieVillagerGroup):
            # TODO: Requires special treatment?
            current_unit = unit_line.variants[0].line[0]

        else:
            current_unit = unit_line.line[0]

        current_unit_id = unit_line.get_head_unit_id()
        dataset = unit_line.data
        game_entity_name = UNIT_LINE_LOOKUPS[current_unit_id][0]

        obj_name = "%s.Turn" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(obj_name, "Turn", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Turn")
        ability_location = ExpectedPointer(unit_line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Speed
        turn_speed_unmodified = current_unit.get_member("turn_speed").get_value()

        # Default case: Instant turning
        turn_speed = MemberSpecialValue.NYAN_INF

        # Ships/Trebuchets turn slower
        if turn_speed_unmodified >= 0:
            # TODO: Calculate this
            pass

        ability_raw_api_object.add_raw_member("turn_speed", turn_speed, "engine.ability.type.Turn")

        # Diplomacy settings
        ability_raw_api_object.add_raw_parent("engine.ability.specialization.DiplomaticAbility")
        diplomatic_stances = [dataset.nyan_api_objects["engine.aux.diplomatic_stance.type.Self"]]
        ability_raw_api_object.add_raw_member("stances", diplomatic_stances,
                                              "engine.ability.specialization.DiplomaticAbility")

        unit_line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(unit_line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def visibility_ability(unit_line):
        """
        Adds the Visibility ability to a unit line.

        :param unit_line: Unit line that gets the ability.
        :type unit_line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        current_unit_id = unit_line.get_head_unit_id()
        dataset = unit_line.data
        game_entity_name = UNIT_LINE_LOOKUPS[current_unit_id][0]

        obj_name = "%s.Visibility" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(obj_name, "Visibility", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Visibility")
        ability_location = ExpectedPointer(unit_line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Units are not visible in fog
        ability_raw_api_object.add_raw_member("visible_in_fog", False,
                                              "engine.ability.type.Visibility")

        unit_line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(unit_line, ability_raw_api_object.get_id())

        return ability_expected_pointer
