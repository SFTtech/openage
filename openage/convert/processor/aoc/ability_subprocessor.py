# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Derives and adds abilities for to unit lines. Subroutine of the
nyan subprocessor.
"""
from ...dataformat.converter_object import RawAPIObject
from ...dataformat.aoc.expected_pointer import ExpectedPointer
from ...dataformat.aoc.internal_nyan_names import UNIT_LINE_LOOKUPS, BUILDING_LINE_LOOKUPS
from ...dataformat.aoc.genie_unit import GenieVillagerGroup
from ...dataformat.aoc.combined_sprite import CombinedSprite
from openage.nyan.nyan_structs import MemberSpecialValue
from openage.convert.dataformat.aoc.genie_unit import GenieBuildingLineGroup,\
    GenieAmbientGroup, GenieGarrisonMode, GenieStackBuildingGroup,\
    GenieUnitLineGroup
from openage.convert.dataformat.aoc.internal_nyan_names import TECH_GROUP_LOOKUPS,\
    AMBIENT_GROUP_LOOKUPS, GATHER_TASK_LOOKUPS, RESTOCK_TARGET_LOOKUPS,\
    TERRAIN_GROUP_LOOKUPS, TERRAIN_TYPE_LOOKUPS, COMMAND_TYPE_LOOKUPS
from openage.util.ordered_set import OrderedSet
from openage.convert.processor.aoc.effect_resistance_subprocessor import AoCEffectResistanceSubprocessor


class AoCAbilitySubprocessor:

    @staticmethod
    def apply_continuous_effect_ability(line, command_id, ranged=False):
        """
        Adds the ApplyContinuousEffect ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        current_unit = line.get_head_unit()
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_name = COMMAND_TYPE_LOOKUPS[command_id]

        if ranged:
            ability_parent = "engine.ability.type.RangedContinuousEffect"

        else:
            ability_parent = "engine.ability.type.ApplyContinuousEffect"

        ability_ref = "%s.%s" % (game_entity_name, ability_name)
        ability_raw_api_object = RawAPIObject(ability_ref, ability_name, dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent(ability_parent)
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        if ranged:
            # Min range
            min_range = current_unit["weapon_range_min"].get_value()
            ability_raw_api_object.add_raw_member("min_range",
                                                  min_range,
                                                  "engine.ability.type.RangedContinuousEffect")

            # Max range
            max_range = current_unit["weapon_range_max"].get_value()
            ability_raw_api_object.add_raw_member("max_range",
                                                  max_range,
                                                  "engine.ability.type.RangedContinuousEffect")

        # Effects
        if command_id == 101:
            # TODO: Construct
            effects = []
            allowed_types = [dataset.pregen_nyan_objects["aux.game_entity_type.types.Unit"].get_nyan_object(),
                             dataset.pregen_nyan_objects["aux.game_entity_type.types.Building"].get_nyan_object()]

        elif command_id == 105:
            # Heal
            effects = AoCEffectResistanceSubprocessor.get_heal_effects(line, ability_ref)
            allowed_types = [dataset.pregen_nyan_objects["aux.game_entity_type.types.Unit"].get_nyan_object()]

        elif command_id == 106:
            # TODO: Repair
            effects = []
            allowed_types = [dataset.pregen_nyan_objects["aux.game_entity_type.types.Building"].get_nyan_object()]

        ability_raw_api_object.add_raw_member("effects",
                                              effects,
                                              "engine.ability.type.ApplyContinuousEffect")

        # Application delay
        attack_graphic_id = current_unit["attack_sprite_id"].get_value()
        attack_graphic = dataset.genie_graphics[attack_graphic_id]
        frame_rate = attack_graphic.get_frame_rate()
        frame_delay = current_unit["frame_delay"].get_value()
        application_delay = frame_rate * frame_delay
        ability_raw_api_object.add_raw_member("application_delay",
                                              application_delay,
                                              "engine.ability.type.ApplyContinuousEffect")

        # Allowed types (all buildings/units)

        ability_raw_api_object.add_raw_member("allowed_types",
                                              allowed_types,
                                              "engine.ability.type.ApplyContinuousEffect")
        ability_raw_api_object.add_raw_member("blacklisted_game_entities",
                                              [],
                                              "engine.ability.type.ApplyContinuousEffect")

        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def apply_discrete_effect_ability(line, command_id, ranged=False):
        """
        Adds the ApplyDiscreteEffect ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        current_unit = line.get_head_unit()
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_name = COMMAND_TYPE_LOOKUPS[command_id]

        if ranged:
            ability_parent = "engine.ability.type.RangedDiscreteEffect"

        else:
            ability_parent = "engine.ability.type.ApplyDiscreteEffect"

        ability_ref = "%s.%s" % (game_entity_name, ability_name)
        ability_raw_api_object = RawAPIObject(ability_ref, ability_name, dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent(ability_parent)
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        if ranged:
            # Min range
            min_range = current_unit["weapon_range_min"].get_value()
            ability_raw_api_object.add_raw_member("min_range",
                                                  min_range,
                                                  "engine.ability.type.RangedDiscreteEffect")

            # Max range
            max_range = current_unit["weapon_range_max"].get_value()
            ability_raw_api_object.add_raw_member("max_range",
                                                  max_range,
                                                  "engine.ability.type.RangedDiscreteEffect")

        # Effects
        if command_id == 7:
            # Attack
            effects = AoCEffectResistanceSubprocessor.get_attack_effects(line, ability_ref)

        elif command_id == 104:
            # Convert
            effects = AoCEffectResistanceSubprocessor.get_convert_effects(line, ability_ref)

        ability_raw_api_object.add_raw_member("effects",
                                              effects,
                                              "engine.ability.type.ApplyDiscreteEffect")

        # Reload time
        reload_time = current_unit["attack_speed"].get_value()
        ability_raw_api_object.add_raw_member("reload_time",
                                              reload_time,
                                              "engine.ability.type.ApplyDiscreteEffect")

        # Application delay
        attack_graphic_id = current_unit["attack_sprite_id"].get_value()
        attack_graphic = dataset.genie_graphics[attack_graphic_id]
        frame_rate = attack_graphic.get_frame_rate()
        frame_delay = current_unit["frame_delay"].get_value()
        application_delay = frame_rate * frame_delay
        ability_raw_api_object.add_raw_member("application_delay",
                                              application_delay,
                                              "engine.ability.type.ApplyDiscreteEffect")

        # Allowed types (all buildings/units)
        allowed_types = [dataset.pregen_nyan_objects["aux.game_entity_type.types.Unit"].get_nyan_object(),
                         dataset.pregen_nyan_objects["aux.game_entity_type.types.Building"].get_nyan_object()]

        ability_raw_api_object.add_raw_member("allowed_types",
                                              allowed_types,
                                              "engine.ability.type.ApplyDiscreteEffect")
        ability_raw_api_object.add_raw_member("blacklisted_game_entities",
                                              [],
                                              "engine.ability.type.ApplyDiscreteEffect")

        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def attribute_change_tracker_ability(line):
        """
        Adds the AttributeChangeTracker ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = "%s.AttributeChangeTracker" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(ability_ref, "AttributeChangeTracker", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.AttributeChangeTracker")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Attribute
        attribute = dataset.pregen_nyan_objects["aux.attribute.types.Health"].get_nyan_object()
        ability_raw_api_object.add_raw_member("attribute",
                                              attribute,
                                              "engine.ability.type.AttributeChangeTracker")

        # TODO: Change progress
        ability_raw_api_object.add_raw_member("change_progress",
                                              [],
                                              "engine.ability.type.AttributeChangeTracker")

        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def collect_storage_ability(line):
        """
        Adds the CollectStorage ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = "%s.CollectStorage" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(ability_ref, "CollectStorage", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.CollectStorage")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Container
        container_ref = "%s.Storage.%sContainer" % (game_entity_name, game_entity_name)
        container_expected_pointer = ExpectedPointer(line, container_ref)
        ability_raw_api_object.add_raw_member("container",
                                              container_expected_pointer,
                                              "engine.ability.type.CollectStorage")

        # Storage elements
        elements = []
        entity_lookups = {}
        entity_lookups.update(UNIT_LINE_LOOKUPS)
        entity_lookups.update(AMBIENT_GROUP_LOOKUPS)
        for entity in line.garrison_entities:
            entity_ref = entity_lookups[entity.get_head_unit_id()][0]
            entity_expected_pointer = ExpectedPointer(entity, entity_ref)
            elements.append(entity_expected_pointer)

        ability_raw_api_object.add_raw_member("storage_elements",
                                              elements,
                                              "engine.ability.type.CollectStorage")

        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def constructable_ability(line):
        """
        Adds the Constructable ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = "%s.Constructable" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(ability_ref, "Constructable", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Constructable")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Starting progress (always 0)
        ability_raw_api_object.add_raw_member("starting_progress",
                                              0,
                                              "engine.ability.type.Constructable")

        # TODO: Construction progress
        ability_raw_api_object.add_raw_member("construction_progress",
                                              [],
                                              "engine.ability.type.Constructable")

        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def create_ability(line):
        """
        Adds the Create ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS
            creatable_lookup_dict = UNIT_LINE_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS
            creatable_lookup_dict = BUILDING_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]
        ability_ref = "%s.Create" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(ability_ref, "Create", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Create")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        creatables_set = []

        for creatable in line.creates:
            if creatable.is_unique():
                # Skip this because unique units are handled by civs
                continue

            # CreatableGameEntity objects are created for each unit/building
            # line individually to avoid duplicates. We just point to the
            # raw API objects here.
            creatable_id = creatable.get_head_unit_id()
            creatable_name = creatable_lookup_dict[creatable_id][0]

            raw_api_object_ref = "%s.CreatableGameEntity" % creatable_name
            creatable_expected_pointer = ExpectedPointer(creatable,
                                                         raw_api_object_ref)
            creatables_set.append(creatable_expected_pointer)

        ability_raw_api_object.add_raw_member("creatables", creatables_set,
                                              "engine.ability.type.Create")
        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def death_ability(line):
        """
        Adds a PassiveTransformTo ability to a line that is used to make entities die.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        current_unit = line.get_head_unit()
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        elif isinstance(line, GenieAmbientGroup):
            name_lookup_dict = AMBIENT_GROUP_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = "%s.Death" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(ability_ref, "Death", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.PassiveTransformTo")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        ability_animation_id = current_unit.get_member("dying_graphic").get_value()

        if ability_animation_id > -1:
            # Make the ability animated
            ability_raw_api_object.add_raw_parent("engine.ability.specialization.AnimatedAbility")

            animations_set = []

            # Create animation object
            animation_ref = "%s.Death.DeathAnimation" % (game_entity_name)
            animation_raw_api_object = RawAPIObject(animation_ref, "DeathAnimation",
                                                    dataset.nyan_api_objects)
            animation_raw_api_object.add_raw_parent("engine.aux.graphics.Animation")
            animation_location = ExpectedPointer(line, ability_ref)
            animation_raw_api_object.set_location(animation_location)

            ability_sprite = CombinedSprite(ability_animation_id,
                                            "death_%s" % (name_lookup_dict[current_unit_id][1]),
                                            dataset)
            dataset.combined_sprites.update({ability_sprite.get_id(): ability_sprite})
            ability_sprite.add_reference(animation_raw_api_object)

            animation_raw_api_object.add_raw_member("sprite", ability_sprite,
                                                    "engine.aux.graphics.Animation")

            animation_expected_pointer = ExpectedPointer(line, animation_ref)
            animations_set.append(animation_expected_pointer)

            ability_raw_api_object.add_raw_member("animations", animations_set,
                                                  "engine.ability.specialization.AnimatedAbility")

            line.add_raw_api_object(animation_raw_api_object)

        # Death condition
        death_condition = [dataset.pregen_nyan_objects["aux.boolean.clause.death.StandardHealthDeath"].get_nyan_object()]
        ability_raw_api_object.add_raw_member("condition",
                                              death_condition,
                                              "engine.ability.type.PassiveTransformTo")

        # Transform time
        # Not setting this to 0.0 allows abilities to use the death condition
        # for stuff before they are deactivated (TODO: is this a good idea?)
        ability_raw_api_object.add_raw_member("transform_time",
                                              0.01,
                                              "engine.ability.type.PassiveTransformTo")

        # Target state
        # =====================================================================================
        target_state_name = "%s.Death.DeadState" % (game_entity_name)
        target_state_raw_api_object = RawAPIObject(target_state_name, "DeadState", dataset.nyan_api_objects)
        target_state_raw_api_object.add_raw_parent("engine.aux.state_machine.StateChanger")
        target_state_location = ExpectedPointer(line, ability_ref)
        target_state_raw_api_object.set_location(target_state_location)

        # Priority
        target_state_raw_api_object.add_raw_member("priority",
                                                   1000,
                                                   "engine.aux.state_machine.StateChanger")

        # Enabled abilities
        target_state_raw_api_object.add_raw_member("enable_abilities",
                                                   [],
                                                   "engine.aux.state_machine.StateChanger")

        # TODO: Disabled abilities
        target_state_raw_api_object.add_raw_member("disable_abilities",
                                                   [],
                                                   "engine.aux.state_machine.StateChanger")

        # Enabled modifiers
        target_state_raw_api_object.add_raw_member("enable_modifiers",
                                                   [],
                                                   "engine.aux.state_machine.StateChanger")

        # Disabled modifiers
        target_state_raw_api_object.add_raw_member("disable_modifiers",
                                                   [],
                                                   "engine.aux.state_machine.StateChanger")

        line.add_raw_api_object(target_state_raw_api_object)
        # =====================================================================================
        target_state_expected_pointer = ExpectedPointer(line, target_state_name)
        ability_raw_api_object.add_raw_member("target_state",
                                              target_state_expected_pointer,
                                              "engine.ability.type.PassiveTransformTo")

        # Transform progress
        ability_raw_api_object.add_raw_member("transform_progress",
                                              [],
                                              "engine.ability.type.PassiveTransformTo")

        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def despawn_ability(line):
        """
        Adds the Despawn ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        current_unit = line.get_head_unit()
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        # Animation and time come from dead unit
        death_animation_id = current_unit.get_member("dying_graphic").get_value()
        dead_unit_id = current_unit.get_member("dead_unit_id").get_value()
        dead_unit = None
        if dead_unit_id > -1:
            dead_unit = dataset.genie_units[dead_unit_id]

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        elif isinstance(line, GenieAmbientGroup):
            name_lookup_dict = AMBIENT_GROUP_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = "%s.Despawn" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(ability_ref, "Despawn", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Despawn")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        ability_animation_id = -1
        if dead_unit:
            ability_animation_id = dead_unit.get_member("idle_graphic0").get_value()

        if ability_animation_id > -1:
            # Make the ability animated
            ability_raw_api_object.add_raw_parent("engine.ability.specialization.AnimatedAbility")

            animations_set = []

            # Create animation object
            animation_ref = "%s.Despawn.DespawnAnimation" % (game_entity_name)
            animation_raw_api_object = RawAPIObject(animation_ref, "DespawnAnimation",
                                                    dataset.nyan_api_objects)
            animation_raw_api_object.add_raw_parent("engine.aux.graphics.Animation")
            animation_location = ExpectedPointer(line, ability_ref)
            animation_raw_api_object.set_location(animation_location)

            ability_sprite = CombinedSprite(ability_animation_id,
                                            "despawn_%s" % (name_lookup_dict[current_unit_id][1]),
                                            dataset)
            dataset.combined_sprites.update({ability_sprite.get_id(): ability_sprite})
            ability_sprite.add_reference(animation_raw_api_object)

            animation_raw_api_object.add_raw_member("sprite", ability_sprite,
                                                    "engine.aux.graphics.Animation")

            animation_expected_pointer = ExpectedPointer(line, animation_ref)
            animations_set.append(animation_expected_pointer)

            ability_raw_api_object.add_raw_member("animations", animations_set,
                                                  "engine.ability.specialization.AnimatedAbility")

            line.add_raw_api_object(animation_raw_api_object)

        # Activation condition
        # Uses the death condition of the units
        activation_condition = [dataset.pregen_nyan_objects["aux.boolean.clause.death.StandardHealthDeath"].get_nyan_object()]
        ability_raw_api_object.add_raw_member("activation_condition",
                                              activation_condition,
                                              "engine.ability.type.Despawn")

        # Despawn condition
        # TODO: implement
        ability_raw_api_object.add_raw_member("despawn_condition",
                                              [],
                                              "engine.ability.type.Despawn")

        # Despawn time = corpse decay time (dead unit) or Death animation time (if no dead unit exist)
        despawn_time = 0
        if dead_unit:
            resource_storage = dead_unit.get_member("resource_storage").get_value()
            for storage in resource_storage:
                resource_id = storage.get_value()["type"].get_value()

                if resource_id == 12:
                    despawn_time = storage.get_value()["amount"].get_value()

        elif death_animation_id > -1:
            despawn_time = dataset.genie_graphics[death_animation_id].get_animation_length()

        ability_raw_api_object.add_raw_member("despawn_time",
                                              despawn_time,
                                              "engine.ability.type.Despawn")

        # State change (let Death handle state change?)
        # Uses the same state changer as Death
        state_change_expected_pointer = ExpectedPointer(line, "%s.Death.DeadState" % (game_entity_name))
        ability_raw_api_object.add_raw_member("state_change",
                                              state_change_expected_pointer,
                                              "engine.ability.type.Despawn")

        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def drop_resources_ability(line):
        """
        Adds the DropResources ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = "%s.DropResources" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(ability_ref, "DropResources", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.DropResources")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Search range
        ability_raw_api_object.add_raw_member("search_range",
                                              MemberSpecialValue.NYAN_INF,
                                              "engine.ability.type.DropResources")

        # Allowed types
        allowed_types = [dataset.pregen_nyan_objects["aux.game_entity_type.types.DropSite"].get_nyan_object()]
        ability_raw_api_object.add_raw_member("allowed_types",
                                              allowed_types,
                                              "engine.ability.type.DropResources")
        # Blacklisted enties
        ability_raw_api_object.add_raw_member("blacklisted_game_entities",
                                              [],
                                              "engine.ability.type.DropResources")

        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def drop_site_ability(line):
        """
        Adds the DropSite ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = "%s.DropSite" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(ability_ref, "DropSite", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.DropSite")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        resources = []
        for resource_id in line.get_accepted_resources():
            if resource_id == 0:
                resource = dataset.pregen_nyan_objects["aux.resource.types.Food"].get_nyan_object()

            elif resource_id == 1:
                resource = dataset.pregen_nyan_objects["aux.resource.types.Wood"].get_nyan_object()

            elif resource_id == 2:
                resource = dataset.pregen_nyan_objects["aux.resource.types.Stone"].get_nyan_object()

            elif resource_id == 3:
                resource = dataset.pregen_nyan_objects["aux.resource.types.Gold"].get_nyan_object()

            else:
                continue

            resources.append(resource)

        ability_raw_api_object.add_raw_member("accepts", resources, "engine.ability.type.DropSite")

        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def enter_container_ability(line):
        """
        Adds the EnterContainer ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability. None if no valid containers were found.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer, None
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = "%s.EnterContainer" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(ability_ref, "EnterContainer", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.EnterContainer")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Containers
        containers = []
        entity_lookups = {}
        entity_lookups.update(UNIT_LINE_LOOKUPS)
        entity_lookups.update(BUILDING_LINE_LOOKUPS)

        for garrison in line.garrison_locations:
            garrison_mode = garrison.get_garrison_mode()

            # Cannot enter production buildings or monk inventories
            if garrison_mode in (GenieGarrisonMode.SELF_PRODUCED, GenieGarrisonMode.MONK):
                continue

            garrison_name = entity_lookups[garrison.get_head_unit_id()][0]

            container_ref = "%s.Storage.%sContainer" % (garrison_name, garrison_name)
            container_expected_pointer = ExpectedPointer(garrison, container_ref)
            containers.append(container_expected_pointer)

        if not containers:
            return None

        ability_raw_api_object.add_raw_member("allowed_containers",
                                              containers,
                                              "engine.ability.type.EnterContainer")

        # Allowed types (all buildings/units)
        allowed_types = [dataset.pregen_nyan_objects["aux.game_entity_type.types.Unit"].get_nyan_object(),
                         dataset.pregen_nyan_objects["aux.game_entity_type.types.Building"].get_nyan_object()]

        ability_raw_api_object.add_raw_member("allowed_types",
                                              allowed_types,
                                              "engine.ability.type.EnterContainer")
        ability_raw_api_object.add_raw_member("blacklisted_game_entities",
                                              [],
                                              "engine.ability.type.EnterContainer")

        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def exit_container_ability(line):
        """
        Adds the ExitContainer ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability. None if no valid containers were found.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer, None
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = "%s.ExitContainer" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(ability_ref, "ExitContainer", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.ExitContainer")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Containers
        containers = []
        entity_lookups = {}
        entity_lookups.update(UNIT_LINE_LOOKUPS)
        entity_lookups.update(BUILDING_LINE_LOOKUPS)

        for garrison in line.garrison_locations:
            garrison_mode = garrison.get_garrison_mode()

            # Cannot enter production buildings or monk inventories
            if garrison_mode == GenieGarrisonMode.MONK:
                continue

            garrison_name = entity_lookups[garrison.get_head_unit_id()][0]

            container_ref = "%s.Storage.%sContainer" % (garrison_name, garrison_name)
            container_expected_pointer = ExpectedPointer(garrison, container_ref)
            containers.append(container_expected_pointer)

        if not containers:
            return None

        ability_raw_api_object.add_raw_member("allowed_containers",
                                              containers,
                                              "engine.ability.type.ExitContainer")

        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def foundation_ability(line, terrain_id=-1):
        """
        Adds the Foundation abilities to a line. Optionally chooses the specified
        terrain ID.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param terrain_id: Force this terrain ID as foundation
        :type terrain_id: int
        :returns: The expected pointers for the abilities.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        current_unit = line.get_head_unit()
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        elif isinstance(line, GenieAmbientGroup):
            name_lookup_dict = AMBIENT_GROUP_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = "%s.Foundation" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(ability_ref, "Foundation", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Foundation")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Terrain
        if terrain_id == -1:
            terrain_id = current_unit["foundation_terrain_id"].get_value()

        terrain = dataset.terrain_groups[terrain_id]
        terrain_expected_pointer = ExpectedPointer(terrain, TERRAIN_GROUP_LOOKUPS[terrain_id][1])
        ability_raw_api_object.add_raw_member("foundation_terrain",
                                              terrain_expected_pointer,
                                              "engine.ability.type.Foundation")

        # Flatten ground (TODO: always true?)
        ability_raw_api_object.add_raw_member("flatten_ground",
                                              True,
                                              "engine.ability.type.Foundation")

        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def gather_ability(line):
        """
        Adds the Gather abilities to a line. Unlike the other methods, this
        creates multiple abilities.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointers for the abilities.
        :rtype: list
        """
        if isinstance(line, GenieVillagerGroup):
            gatherers = line.variants[1].line

        else:
            gatherers = [line.line[0]]

        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        elif isinstance(line, GenieAmbientGroup):
            name_lookup_dict = AMBIENT_GROUP_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        abilities = []
        for gatherer in gatherers:
            unit_commands = gatherer.get_member("unit_commands").get_value()
            resource = None
            ability_animation_id = -1
            harvestable_class_ids = OrderedSet()
            harvestable_unit_ids = OrderedSet()

            for command in unit_commands:
                # Find a gather ability. It doesn't matter which one because
                # they should all produce the same resource for one genie unit.
                type_id = command.get_value()["type"].get_value()

                if type_id not in (5, 110):
                    continue

                target_class_id = command.get_value()["class_id"].get_value()
                if target_class_id > -1:
                    harvestable_class_ids.add(target_class_id)

                target_unit_id = command.get_value()["unit_id"].get_value()
                if target_unit_id > -1:
                    harvestable_unit_ids.add(target_unit_id)

                resource_id = command.get_value()["resource_out"].get_value()

                # If resource_out is not specified, the gatherer harvests resource_in
                if resource_id == -1:
                    resource_id = command.get_value()["resource_in"].get_value()

                if resource_id == 0:
                    resource = dataset.pregen_nyan_objects["aux.resource.types.Food"].get_nyan_object()

                elif resource_id == 1:
                    resource = dataset.pregen_nyan_objects["aux.resource.types.Wood"].get_nyan_object()

                elif resource_id == 2:
                    resource = dataset.pregen_nyan_objects["aux.resource.types.Stone"].get_nyan_object()

                elif resource_id == 3:
                    resource = dataset.pregen_nyan_objects["aux.resource.types.Gold"].get_nyan_object()

                else:
                    continue

                if type_id == 110:
                    ability_animation_id = command.get_value()["work_sprite_id"].get_value()

                else:
                    ability_animation_id = command.get_value()["proceed_sprite_id"].get_value()

            # Look for the harvestable groups that match the class IDs and unit IDs
            check_groups = []
            check_groups.extend(dataset.unit_lines.values())
            check_groups.extend(dataset.building_lines.values())
            check_groups.extend(dataset.ambient_groups.values())

            harvestable_groups = []
            for group in check_groups:
                if not group.is_harvestable():
                    continue

                if group.get_class_id() in harvestable_class_ids:
                    harvestable_groups.append(group)
                    continue

                for unit_id in harvestable_unit_ids:
                    if group.contains_entity(unit_id):
                        harvestable_groups.append(group)

            if len(harvestable_groups) == 0:
                # If no matching groups are found, then we don't
                # need to create an ability.
                continue

            gatherer_unit_id = gatherer.get_id()
            if gatherer_unit_id not in GATHER_TASK_LOOKUPS.keys():
                # Skips hunting wolves
                continue

            ability_name = GATHER_TASK_LOOKUPS[gatherer_unit_id]

            ability_ref = "%s.%s" % (game_entity_name, ability_name)
            ability_raw_api_object = RawAPIObject(ability_ref, ability_name, dataset.nyan_api_objects)
            ability_raw_api_object.add_raw_parent("engine.ability.type.Gather")
            ability_location = ExpectedPointer(line, game_entity_name)
            ability_raw_api_object.set_location(ability_location)

            if ability_animation_id > -1:
                # Make the ability animated
                ability_raw_api_object.add_raw_parent("engine.ability.specialization.AnimatedAbility")

                animations_set = []

                # Create animation object
                animation_ref = "%s.%s.%sAnimation" % (game_entity_name, ability_name, ability_name)
                animation_raw_api_object = RawAPIObject(animation_ref, "%sAnimation" % (ability_name),
                                                        dataset.nyan_api_objects)
                animation_raw_api_object.add_raw_parent("engine.aux.graphics.Animation")
                animation_location = ExpectedPointer(line, ability_ref)
                animation_raw_api_object.set_location(animation_location)

                ability_sprite = CombinedSprite(ability_animation_id,
                                                "%s_%s" % (ability_name,
                                                           name_lookup_dict[current_unit_id][1]),
                                                dataset)
                dataset.combined_sprites.update({ability_sprite.get_id(): ability_sprite})
                ability_sprite.add_reference(animation_raw_api_object)

                animation_raw_api_object.add_raw_member("sprite", ability_sprite,
                                                        "engine.aux.graphics.Animation")

                animation_expected_pointer = ExpectedPointer(line, animation_ref)
                animations_set.append(animation_expected_pointer)

                ability_raw_api_object.add_raw_member("animations", animations_set,
                                                      "engine.ability.specialization.AnimatedAbility")

                line.add_raw_api_object(animation_raw_api_object)

            # Auto resume
            ability_raw_api_object.add_raw_member("auto_resume",
                                                  True,
                                                  "engine.ability.type.Gather")

            # search range
            ability_raw_api_object.add_raw_member("resume_search_range",
                                                  MemberSpecialValue.NYAN_INF,
                                                  "engine.ability.type.Gather")

            # Carry capacity
            carry_capacity = gatherer.get_member("resource_capacity").get_value()
            ability_raw_api_object.add_raw_member("carry_capacity",
                                                  carry_capacity,
                                                  "engine.ability.type.Gather")

            # Gather rate
            # TODO: The work_rate attribute must be turned into a MultiplierModifier
            rate_name = "%s.%s.GatherRate" % (game_entity_name, ability_name)
            rate_raw_api_object = RawAPIObject(rate_name, "GatherRate", dataset.nyan_api_objects)
            rate_raw_api_object.add_raw_parent("engine.aux.resource.ResourceRate")
            rate_location = ExpectedPointer(line, ability_ref)
            rate_raw_api_object.set_location(rate_location)

            rate_raw_api_object.add_raw_member("type", resource, "engine.aux.resource.ResourceRate")

            gather_rate = gatherer.get_member("work_rate").get_value()
            rate_raw_api_object.add_raw_member("rate", gather_rate, "engine.aux.resource.ResourceRate")

            line.add_raw_api_object(rate_raw_api_object)

            rate_expected_pointer = ExpectedPointer(line, rate_name)
            ability_raw_api_object.add_raw_member("gather_rate",
                                                  rate_expected_pointer,
                                                  "engine.ability.type.Gather")

            # TODO: Carry progress
            ability_raw_api_object.add_raw_member("carry_progress",
                                                  [],
                                                  "engine.ability.type.Gather")

            # Targets (resource spots)
            entity_lookups = {}
            entity_lookups.update(UNIT_LINE_LOOKUPS)
            entity_lookups.update(BUILDING_LINE_LOOKUPS)
            entity_lookups.update(AMBIENT_GROUP_LOOKUPS)

            spot_expected_pointers = []
            for group in harvestable_groups:
                group_id = group.get_head_unit_id()
                group_name = entity_lookups[group_id][0]

                spot_expected_pointer = ExpectedPointer(group,
                                                        "%s.Harvestable.%sResourceSpot"
                                                        % (group_name, group_name))
                spot_expected_pointers.append(spot_expected_pointer)

            ability_raw_api_object.add_raw_member("targets",
                                                  spot_expected_pointers,
                                                  "engine.ability.type.Gather")

            line.add_raw_api_object(ability_raw_api_object)

            ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())
            abilities.append(ability_expected_pointer)

        return abilities

    @staticmethod
    def harvestable_ability(line):
        """
        Adds the Harvestable ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        current_unit = line.get_head_unit()
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        elif isinstance(line, GenieAmbientGroup):
            name_lookup_dict = AMBIENT_GROUP_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = "%s.Harvestable" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(ability_ref, "Harvestable", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Harvestable")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Resource spot
        resource_storage = current_unit.get_member("resource_storage").get_value()

        for storage in resource_storage:
            resource_id = storage.get_value()["type"].get_value()

            # IDs 15, 16, 17 are other types of food (meat, berries, fish)
            if resource_id in (0, 15, 16, 17):
                resource = dataset.pregen_nyan_objects["aux.resource.types.Food"].get_nyan_object()

            elif resource_id == 1:
                resource = dataset.pregen_nyan_objects["aux.resource.types.Wood"].get_nyan_object()

            elif resource_id == 2:
                resource = dataset.pregen_nyan_objects["aux.resource.types.Stone"].get_nyan_object()

            elif resource_id == 3:
                resource = dataset.pregen_nyan_objects["aux.resource.types.Gold"].get_nyan_object()

            else:
                continue

            spot_name = "%s.Harvestable.%sResourceSpot" % (game_entity_name, game_entity_name)
            spot_raw_api_object = RawAPIObject(spot_name,
                                               "%sResourceSpot" % (game_entity_name),
                                               dataset.nyan_api_objects)
            spot_raw_api_object.add_raw_parent("engine.aux.resource_spot.ResourceSpot")
            spot_location = ExpectedPointer(line, ability_ref)
            spot_raw_api_object.set_location(spot_location)

            # Type
            spot_raw_api_object.add_raw_member("resource",
                                               resource,
                                               "engine.aux.resource_spot.ResourceSpot")

            # Start amount (equals max amount)

            if line.get_id() == 50:
                # Farm food amount (hardcoded in civ)
                starting_amount = dataset.genie_civs[1].get_member("resources").get_value()[36].get_value()

            elif line.get_id() == 199:
                # Fish trap food amount (hardcoded in civ)
                starting_amount = storage.get_value()["amount"].get_value()
                starting_amount += dataset.genie_civs[1].get_member("resources").get_value()[88].get_value()

            else:
                starting_amount = storage.get_value()["amount"].get_value()

            spot_raw_api_object.add_raw_member("starting_amount",
                                               starting_amount,
                                               "engine.aux.resource_spot.ResourceSpot")

            # Max amount
            spot_raw_api_object.add_raw_member("max_amount",
                                               starting_amount,
                                               "engine.aux.resource_spot.ResourceSpot")

            # Decay rate
            decay_rate = current_unit.get_member("resource_decay").get_value()
            spot_raw_api_object.add_raw_member("decay_rate",
                                               decay_rate,
                                               "engine.aux.resource_spot.ResourceSpot")

            spot_expected_pointer = ExpectedPointer(line, spot_name)
            ability_raw_api_object.add_raw_member("resources",
                                                  spot_expected_pointer,
                                                  "engine.ability.type.Harvestable")
            line.add_raw_api_object(spot_raw_api_object)

            # Only one resource spot per ability
            break

        # Harvest Progress
        ability_raw_api_object.add_raw_member("harvest_progress",
                                              [],
                                              "engine.ability.type.Harvestable")

        # Restock Progress (TODO: Farms are different)
        ability_raw_api_object.add_raw_member("restock_progress",
                                              [],
                                              "engine.ability.type.Harvestable")

        # Gatherer limit (infinite in AoC except for farms)
        gatherer_limit = MemberSpecialValue.NYAN_INF
        if line.get_class_id() == 49:
            gatherer_limit = 1

        ability_raw_api_object.add_raw_member("gatherer_limit",
                                              gatherer_limit,
                                              "engine.ability.type.Harvestable")

        # Unit have to die before they are harvestable (except for farms)
        harvestable_by_default = current_unit.get_member("hit_points").get_value() == 0
        if line.get_class_id() == 49:
            harvestable_by_default = True

        ability_raw_api_object.add_raw_member("harvestable_by_default",
                                              harvestable_by_default,
                                              "engine.ability.type.Harvestable")

        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def herd_ability(line):
        """
        Adds the Herd ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        elif isinstance(line, GenieAmbientGroup):
            name_lookup_dict = AMBIENT_GROUP_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = "%s.Herd" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(ability_ref, "Herd", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Herd")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Range
        ability_raw_api_object.add_raw_member("range",
                                              3.0,
                                              "engine.ability.type.Herd")

        # Strength
        ability_raw_api_object.add_raw_member("strength",
                                              0,
                                              "engine.ability.type.Herd")

        # Allowed types
        allowed_types = [dataset.pregen_nyan_objects["aux.game_entity_type.types.Herdable"].get_nyan_object()]
        ability_raw_api_object.add_raw_member("allowed_types",
                                              allowed_types,
                                              "engine.ability.type.Herd")

        # Blacklisted entities
        ability_raw_api_object.add_raw_member("blacklisted_game_entities",
                                              [],
                                              "engine.ability.type.Herd")

        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def herdable_ability(line):
        """
        Adds the Herdable ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        elif isinstance(line, GenieAmbientGroup):
            name_lookup_dict = AMBIENT_GROUP_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = "%s.Herdable" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(ability_ref, "Herdable", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Herdable")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Mode
        mode = dataset.nyan_api_objects["engine.aux.herdable_mode.type.LongestTimeInRange"]
        ability_raw_api_object.add_raw_member("mode", mode, "engine.ability.type.Herdable")

        # Discover range
        ability_raw_api_object.add_raw_member("adjacent_discover_range",
                                              1.0,
                                              "engine.ability.type.Herdable")

        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def hitbox_ability(line):
        """
        Adds the Hitbox ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        current_unit = line.get_head_unit()
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        elif isinstance(line, GenieAmbientGroup):
            name_lookup_dict = AMBIENT_GROUP_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = "%s.Hitbox" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(ability_ref, "Hitbox", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Hitbox")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Hitbox object
        hitbox_name = "%s.Hitbox.%sHitbox" % (game_entity_name, game_entity_name)
        hitbox_raw_api_object = RawAPIObject(hitbox_name,
                                             "%sHitbox" % (game_entity_name),
                                             dataset.nyan_api_objects)
        hitbox_raw_api_object.add_raw_parent("engine.aux.hitbox.Hitbox")
        hitbox_location = ExpectedPointer(line, ability_ref)
        hitbox_raw_api_object.set_location(hitbox_location)

        radius_x = current_unit.get_member("radius_x").get_value()
        radius_y = current_unit.get_member("radius_y").get_value()
        radius_z = current_unit.get_member("radius_z").get_value()

        hitbox_raw_api_object.add_raw_member("radius_x",
                                             radius_x,
                                             "engine.aux.hitbox.Hitbox")
        hitbox_raw_api_object.add_raw_member("radius_y",
                                             radius_y,
                                             "engine.aux.hitbox.Hitbox")
        hitbox_raw_api_object.add_raw_member("radius_z",
                                             radius_z,
                                             "engine.aux.hitbox.Hitbox")

        hitbox_expected_pointer = ExpectedPointer(line, hitbox_name)
        ability_raw_api_object.add_raw_member("hitbox",
                                              hitbox_expected_pointer,
                                              "engine.ability.type.Hitbox")

        line.add_raw_api_object(hitbox_raw_api_object)
        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def idle_ability(line):
        """
        Adds the Idle ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        current_unit = line.get_head_unit()
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        elif isinstance(line, GenieAmbientGroup):
            name_lookup_dict = AMBIENT_GROUP_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = "%s.Idle" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(ability_ref, "Idle", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Idle")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        ability_animation_id = current_unit.get_member("idle_graphic0").get_value()

        if ability_animation_id > -1:
            # Make the ability animated
            ability_raw_api_object.add_raw_parent("engine.ability.specialization.AnimatedAbility")

            animations_set = []

            # Create animation object
            animation_ref = "%s.Idle.IdleAnimation" % (game_entity_name)
            animation_raw_api_object = RawAPIObject(animation_ref, "IdleAnimation",
                                                    dataset.nyan_api_objects)
            animation_raw_api_object.add_raw_parent("engine.aux.graphics.Animation")
            animation_location = ExpectedPointer(line, "%s.Idle" % (game_entity_name))
            animation_raw_api_object.set_location(animation_location)

            ability_sprite = CombinedSprite(ability_animation_id,
                                            "idle_%s" % (name_lookup_dict[current_unit_id][1]),
                                            dataset)
            dataset.combined_sprites.update({ability_sprite.get_id(): ability_sprite})
            ability_sprite.add_reference(animation_raw_api_object)

            animation_raw_api_object.add_raw_member("sprite", ability_sprite,
                                                    "engine.aux.graphics.Animation")

            animation_expected_pointer = ExpectedPointer(line, animation_ref)
            animations_set.append(animation_expected_pointer)

            ability_raw_api_object.add_raw_member("animations", animations_set,
                                                  "engine.ability.specialization.AnimatedAbility")

            line.add_raw_api_object(animation_raw_api_object)

        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def live_ability(line):
        """
        Adds the Live ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        current_unit = line.get_head_unit()
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        elif isinstance(line, GenieAmbientGroup):
            name_lookup_dict = AMBIENT_GROUP_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = "%s.Live" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(ability_ref, "Live", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Live")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        attributes_set = []

        ability_ref = "%s.Live.Health" % (game_entity_name)
        health_raw_api_object = RawAPIObject(ability_ref, "Health", dataset.nyan_api_objects)
        health_raw_api_object.add_raw_parent("engine.aux.attribute.AttributeSetting")
        health_location = ExpectedPointer(line, "%s.Live" % (game_entity_name))
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

        health_expected_pointer = ExpectedPointer(line, health_raw_api_object.get_id())
        attributes_set.append(health_expected_pointer)
        ability_raw_api_object.add_raw_member("attributes", attributes_set,
                                              "engine.ability.type.Live")

        line.add_raw_api_object(health_raw_api_object)
        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def los_ability(line):
        """
        Adds the LineOfSight ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        current_unit = line.get_head_unit()
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = "%s.LineOfSight" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(ability_ref, "LineOfSight", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.LineOfSight")
        ability_location = ExpectedPointer(line, game_entity_name)
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

        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def move_ability(line, projectile=-1):
        """
        Adds the Move ability to a line or to a projectile of that line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        dataset = line.data

        if projectile == -1:
            current_unit = line.get_head_unit()
            current_unit_id = line.get_head_unit_id()

        elif projectile == 0:
            current_unit_id = line.get_head_unit_id()
            projectile_id = line.get_head_unit()["attack_projectile_primary_unit_id"].get_value()
            current_unit = dataset.genie_units[projectile_id]

        elif projectile == 1:
            current_unit_id = line.get_head_unit_id()
            projectile_id = line.get_head_unit()["attack_projectile_secondary_unit_id"].get_value()
            current_unit = dataset.genie_units[projectile_id]

        else:
            raise Exception("Invalid projectile number: %s" % (projectile))

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        if projectile == -1:
            ability_ref = "%s.Move" % (game_entity_name)
            ability_raw_api_object = RawAPIObject(ability_ref, "Move", dataset.nyan_api_objects)
            ability_raw_api_object.add_raw_parent("engine.ability.type.Move")
            ability_location = ExpectedPointer(line, game_entity_name)
            ability_raw_api_object.set_location(ability_location)

        else:
            ability_ref = "Projectile%s.Move" % (projectile)
            ability_raw_api_object = RawAPIObject(ability_ref, "Move", dataset.nyan_api_objects)
            ability_raw_api_object.add_raw_parent("engine.ability.type.Move")
            ability_location = ExpectedPointer(line,
                                               "%s.ShootProjectile.Projectile%s"
                                               % (game_entity_name, projectile))
            ability_raw_api_object.set_location(ability_location)

        # Animation
        ability_animation_id = current_unit.get_member("move_graphics").get_value()

        if ability_animation_id > -1:
            # Make the ability animated
            ability_raw_api_object.add_raw_parent("engine.ability.specialization.AnimatedAbility")

            animations_set = []

            # Create animation object
            animation_ref = "%s.MoveAnimation" % (ability_ref)
            animation_raw_api_object = RawAPIObject(animation_ref, "MoveAnimation",
                                                    dataset.nyan_api_objects)
            animation_raw_api_object.add_raw_parent("engine.aux.graphics.Animation")
            animation_location = ExpectedPointer(line, ability_ref)
            animation_raw_api_object.set_location(animation_location)

            ability_sprite = CombinedSprite(ability_animation_id,
                                            "move_%s" % (name_lookup_dict[current_unit_id][1]),
                                            dataset)
            dataset.combined_sprites.update({ability_sprite.get_id(): ability_sprite})
            ability_sprite.add_reference(animation_raw_api_object)

            animation_raw_api_object.add_raw_member("sprite", ability_sprite,
                                                    "engine.aux.graphics.Animation")

            animation_expected_pointer = ExpectedPointer(line, animation_ref)
            animations_set.append(animation_expected_pointer)

            ability_raw_api_object.add_raw_member("animations", animations_set,
                                                  "engine.ability.specialization.AnimatedAbility")

            line.add_raw_api_object(animation_raw_api_object)

        # Speed
        speed = current_unit.get_member("speed").get_value()
        ability_raw_api_object.add_raw_member("speed", speed, "engine.ability.type.Move")

        if projectile == -1:
            # Standard move modes
            move_modes = [dataset.nyan_api_objects["engine.aux.move_mode.type.AttackMove"],
                          dataset.nyan_api_objects["engine.aux.move_mode.type.Normal"],
                          dataset.nyan_api_objects["engine.aux.move_mode.type.Patrol"]]

            # Follow
            ability_ref = "%s.Move.Follow" % (game_entity_name)
            follow_raw_api_object = RawAPIObject(ability_ref, "Follow", dataset.nyan_api_objects)
            follow_raw_api_object.add_raw_parent("engine.aux.move_mode.type.Follow")
            follow_location = ExpectedPointer(line, "%s.Move" % (game_entity_name))
            follow_raw_api_object.set_location(follow_location)

            follow_range = current_unit.get_member("line_of_sight").get_value() - 1
            follow_raw_api_object.add_raw_member("range", follow_range,
                                                 "engine.aux.move_mode.type.Follow")

            line.add_raw_api_object(follow_raw_api_object)
            follow_expected_pointer = ExpectedPointer(line, follow_raw_api_object.get_id())
            move_modes.append(follow_expected_pointer)

        else:
            move_modes = [dataset.nyan_api_objects["engine.aux.move_mode.type.Normal"], ]

        ability_raw_api_object.add_raw_member("modes", move_modes, "engine.ability.type.Move")

        # Diplomacy settings
        if projectile == -1:
            ability_raw_api_object.add_raw_parent("engine.ability.specialization.DiplomaticAbility")
            diplomatic_stances = [dataset.nyan_api_objects["engine.aux.diplomatic_stance.type.Self"]]
            ability_raw_api_object.add_raw_member("stances", diplomatic_stances,
                                                  "engine.ability.specialization.DiplomaticAbility")

        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def named_ability(line):
        """
        Adds the Named ability to a line.

        TODO: Lookup names from language.dll

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        elif isinstance(line, GenieAmbientGroup):
            name_lookup_dict = AMBIENT_GROUP_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = "%s.Named" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(ability_ref, "Named", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Named")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Name
        name_ref = "%s.Named.%sName" % (game_entity_name, game_entity_name)
        name_raw_api_object = RawAPIObject(name_ref,
                                           "%sName"  % (game_entity_name),
                                           dataset.nyan_api_objects)
        name_raw_api_object.add_raw_parent("engine.aux.translated.type.TranslatedString")
        name_location = ExpectedPointer(line, ability_ref)
        name_raw_api_object.set_location(name_location)

        name_raw_api_object.add_raw_member("translations",
                                           [],
                                           "engine.aux.translated.type.TranslatedString")

        name_expected_pointer = ExpectedPointer(line, name_ref)
        ability_raw_api_object.add_raw_member("name", name_expected_pointer, "engine.ability.type.Named")
        line.add_raw_api_object(name_raw_api_object)

        # Description
        description_ref = "%s.Named.%sDescription" % (game_entity_name, game_entity_name)
        description_raw_api_object = RawAPIObject(description_ref,
                                                  "%sDescription"  % (game_entity_name),
                                                  dataset.nyan_api_objects)
        description_raw_api_object.add_raw_parent("engine.aux.translated.type.TranslatedMarkupFile")
        description_location = ExpectedPointer(line, ability_ref)
        description_raw_api_object.set_location(description_location)

        description_raw_api_object.add_raw_member("translations",
                                                  [],
                                                  "engine.aux.translated.type.TranslatedMarkupFile")

        description_expected_pointer = ExpectedPointer(line, description_ref)
        ability_raw_api_object.add_raw_member("description",
                                              description_expected_pointer,
                                              "engine.ability.type.Named")
        line.add_raw_api_object(description_raw_api_object)

        # Long description
        long_description_ref = "%s.Named.%sLongDescription" % (game_entity_name, game_entity_name)
        long_description_raw_api_object = RawAPIObject(long_description_ref,
                                                       "%sLongDescription"  % (game_entity_name),
                                                       dataset.nyan_api_objects)
        long_description_raw_api_object.add_raw_parent("engine.aux.translated.type.TranslatedMarkupFile")
        long_description_location = ExpectedPointer(line, ability_ref)
        long_description_raw_api_object.set_location(long_description_location)

        long_description_raw_api_object.add_raw_member("translations",
                                                       [],
                                                       "engine.aux.translated.type.TranslatedMarkupFile")

        long_description_expected_pointer = ExpectedPointer(line, long_description_ref)
        ability_raw_api_object.add_raw_member("long_description",
                                              long_description_expected_pointer,
                                              "engine.ability.type.Named")
        line.add_raw_api_object(long_description_raw_api_object)

        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def overlay_terrain_ability(line):
        """
        Adds the OverlayTerrain to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointers for the abilities.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        current_unit = line.get_head_unit()
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        elif isinstance(line, GenieAmbientGroup):
            name_lookup_dict = AMBIENT_GROUP_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = "%s.OverlayTerrain" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(ability_ref, "OverlayTerrain", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.OverlayTerrain")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Terrain (Use foundation terrain)
        terrain_id = current_unit["foundation_terrain_id"].get_value()
        terrain = dataset.terrain_groups[terrain_id]
        terrain_expected_pointer = ExpectedPointer(terrain, TERRAIN_GROUP_LOOKUPS[terrain_id][1])
        ability_raw_api_object.add_raw_member("terrain_overlay",
                                              terrain_expected_pointer,
                                              "engine.ability.type.OverlayTerrain")

        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def passable_ability(line):
        """
        Adds the Passable ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        elif isinstance(line, GenieAmbientGroup):
            name_lookup_dict = AMBIENT_GROUP_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = "%s.Passable" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(ability_ref, "Passable", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Passable")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Hitbox
        hitbox_ref = "%s.Hitbox.%sHitbox" % (game_entity_name, game_entity_name)
        hitbox_expected_pointer = ExpectedPointer(line, hitbox_ref)
        ability_raw_api_object.add_raw_member("hitbox",
                                              hitbox_expected_pointer,
                                              "engine.ability.type.Passable")

        # Passable mode
        # =====================================================================================
        mode_name = "%s.Passable.PassableMode" % (game_entity_name)
        mode_raw_api_object = RawAPIObject(mode_name, "PassableMode", dataset.nyan_api_objects)
        mode_parent = "engine.aux.passable_mode.type.Normal"
        if isinstance(line, GenieStackBuildingGroup):
            if line.is_gate():
                mode_parent = "engine.aux.passable_mode.type.Gate"

        mode_raw_api_object.add_raw_parent(mode_parent)
        mode_location = ExpectedPointer(line, ability_ref)
        mode_raw_api_object.set_location(mode_location)

        # Allowed types
        allowed_types = [dataset.pregen_nyan_objects["aux.game_entity_type.types.Unit"].get_nyan_object(),
                         dataset.pregen_nyan_objects["aux.game_entity_type.types.Building"].get_nyan_object(),
                         dataset.pregen_nyan_objects["aux.game_entity_type.types.Projectile"].get_nyan_object()]
        mode_raw_api_object.add_raw_member("allowed_types",
                                           allowed_types,
                                           "engine.aux.passable_mode.PassableMode")

        # Blacklisted entities
        mode_raw_api_object.add_raw_member("blacklisted_game_entities",
                                           [],
                                           "engine.aux.passable_mode.PassableMode")

        line.add_raw_api_object(mode_raw_api_object)
        # =====================================================================================
        mode_expected_pointer = ExpectedPointer(line, mode_name)
        ability_raw_api_object.add_raw_member("mode",
                                              mode_expected_pointer,
                                              "engine.ability.type.Passable")

        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def production_queue_ability(line):
        """
        Adds the ProductionQueue ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = "%s.ProductionQueue" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(ability_ref, "ProductionQueue", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.ProductionQueue")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Size
        size = 14

        ability_raw_api_object.add_raw_member("size", size, "engine.ability.type.ProductionQueue")

        # Production modes
        modes = []

        mode_name = "%s.ProvideContingent.CreatablesMode" % (game_entity_name)
        mode_raw_api_object = RawAPIObject(mode_name, "CreatablesMode", dataset.nyan_api_objects)
        mode_raw_api_object.add_raw_parent("engine.aux.production_mode.type.Creatables")
        mode_location = ExpectedPointer(line, ability_ref)
        mode_raw_api_object.set_location(mode_location)

        # AoE2 allows all creatables in production queue
        mode_raw_api_object.add_raw_member("exclude", [], "engine.aux.production_mode.type.Creatables")

        mode_expected_pointer = ExpectedPointer(line, mode_name)
        modes.append(mode_expected_pointer)

        ability_raw_api_object.add_raw_member("production_modes",
                                              modes,
                                              "engine.ability.type.ProductionQueue")

        line.add_raw_api_object(mode_raw_api_object)
        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def projectile_ability(line, position=0):
        """
        Adds a Projectile ability to projectiles in a line. Which projectile should
        be added is determined by the 'position' argument.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param position: When 0, gives the first projectile its ability. When 1, the second...
        :type position: int
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        current_unit = line.get_head_unit()
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        # First projectile is mandatory
        obj_ref  = "%s.ShootProjectile.Projectile%s" % (game_entity_name, str(position))
        ability_ref = "%s.ShootProjectile.Projectile%s.Projectile"\
            % (game_entity_name, str(position))
        ability_raw_api_object = RawAPIObject(ability_ref, "Projectile", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Projectile")
        ability_location = ExpectedPointer(line, obj_ref)
        ability_raw_api_object.set_location(ability_location)

        # Arc
        if position == 0:
            projectile_id = current_unit.get_member("attack_projectile_primary_unit_id").get_value()

        elif position == 1:
            projectile_id = current_unit.get_member("attack_projectile_secondary_unit_id").get_value()

        else:
            raise Exception("Invalid position")

        projectile = dataset.genie_units[projectile_id]
        # TODO: radiant?
        arc = projectile.get_member("projectile_arc").get_value() * 180 / 3.14
        ability_raw_api_object.add_raw_member("arc",
                                              arc,
                                              "engine.ability.type.Projectile")

        # Accuracy
        accuracy_name = "%s.ShootProjectile.Projectile%s.Projectile.Accuracy"\
                        % (game_entity_name, str(position))
        accuracy_raw_api_object = RawAPIObject(accuracy_name, "Accuracy", dataset.nyan_api_objects)
        accuracy_raw_api_object.add_raw_parent("engine.aux.accuracy.Accuracy")
        accuracy_location = ExpectedPointer(line, ability_ref)
        accuracy_raw_api_object.set_location(accuracy_location)

        accuracy_value = current_unit.get_member("accuracy").get_value()
        accuracy_raw_api_object.add_raw_member("accuracy",
                                               accuracy_value,
                                               "engine.aux.accuracy.Accuracy")

        accuracy_dispersion = current_unit.get_member("accuracy_dispersion").get_value()
        accuracy_raw_api_object.add_raw_member("accuracy_dispersion",
                                               accuracy_dispersion,
                                               "engine.aux.accuracy.Accuracy")
        dropoff_type = dataset.nyan_api_objects["engine.aux.dropoff_type.type.InverseLinear"]
        accuracy_raw_api_object.add_raw_member("dispersion_dropoff",
                                               dropoff_type,
                                               "engine.aux.accuracy.Accuracy")

        allowed_types = [dataset.pregen_nyan_objects["aux.game_entity_type.types.Building"].get_nyan_object(),
                         dataset.pregen_nyan_objects["aux.game_entity_type.types.Unit"].get_nyan_object()]
        accuracy_raw_api_object.add_raw_member("target_types",
                                               allowed_types,
                                               "engine.aux.accuracy.Accuracy")
        accuracy_raw_api_object.add_raw_member("blacklisted_entities",
                                               [],
                                               "engine.aux.accuracy.Accuracy")

        line.add_raw_api_object(accuracy_raw_api_object)
        accuracy_expected_pointer = ExpectedPointer(line, accuracy_name)
        ability_raw_api_object.add_raw_member("accuracy",
                                              [accuracy_expected_pointer],
                                              "engine.ability.type.Projectile")

        # Target mode
        target_mode = dataset.nyan_api_objects["engine.aux.target_mode.type.CurrentPosition"]
        ability_raw_api_object.add_raw_member("target_mode",
                                              target_mode,
                                              "engine.ability.type.Projectile")

        # TODO: Ingore types
        ability_raw_api_object.add_raw_member("ignored_types",
                                              [],
                                              "engine.ability.type.Projectile")
        ability_raw_api_object.add_raw_member("unignored_entities",
                                              [],
                                              "engine.ability.type.Projectile")

        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def provide_contingent_ability(line):
        """
        Adds the ProvideContingent ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        current_unit = line.get_head_unit()
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = "%s.ProvideContingent" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(ability_ref, "ProvideContingent", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.ProvideContingent")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Also stores the pop space
        resource_storage = current_unit.get_member("resource_storage").get_value()

        contingents = []
        for storage in resource_storage:
            type_id = storage.get_value()["type"].get_value()

            if type_id == 4:
                resource = dataset.pregen_nyan_objects["aux.resource.types.PopulationSpace"].get_nyan_object()
                resource_name = "PopSpace"

            else:
                continue

            amount = storage.get_value()["amount"].get_value()

            contingent_amount_name = "%s.ProvideContingent.%s" % (game_entity_name, resource_name)
            contingent_amount = RawAPIObject(contingent_amount_name, resource_name,
                                             dataset.nyan_api_objects)
            contingent_amount.add_raw_parent("engine.aux.resource.ResourceAmount")
            ability_expected_pointer = ExpectedPointer(line, ability_ref)
            contingent_amount.set_location(ability_expected_pointer)

            contingent_amount.add_raw_member("type",
                                             resource,
                                             "engine.aux.resource.ResourceAmount")
            contingent_amount.add_raw_member("amount",
                                             amount,
                                             "engine.aux.resource.ResourceAmount")

            line.add_raw_api_object(contingent_amount)
            contingent_amount_expected_pointer = ExpectedPointer(line,
                                                                 contingent_amount_name)
            contingents.append(contingent_amount_expected_pointer)

        if not contingents:
            # Do not create the ability if its values are empty
            return None

        ability_raw_api_object.add_raw_member("amount",
                                              contingents,
                                              "engine.ability.type.ProvideContingent")
        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def rally_point_ability(line):
        """
        Adds the RallyPoint ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = "%s.RallyPoint" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(ability_ref, "RallyPoint", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.RallyPoint")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def remove_storage_ability(line):
        """
        Adds the RemoveStorage ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = "%s.RemoveStorage" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(ability_ref, "RemoveStorage", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.RemoveStorage")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Container
        container_ref = "%s.Storage.%sContainer" % (game_entity_name, game_entity_name)
        container_expected_pointer = ExpectedPointer(line, container_ref)
        ability_raw_api_object.add_raw_member("container",
                                              container_expected_pointer,
                                              "engine.ability.type.RemoveStorage")

        # Storage elements
        elements = []
        entity_lookups = {}
        entity_lookups.update(UNIT_LINE_LOOKUPS)
        entity_lookups.update(AMBIENT_GROUP_LOOKUPS)
        for entity in line.garrison_entities:
            entity_ref = entity_lookups[entity.get_head_unit_id()][0]
            entity_expected_pointer = ExpectedPointer(entity, entity_ref)
            elements.append(entity_expected_pointer)

        ability_raw_api_object.add_raw_member("storage_elements",
                                              elements,
                                              "engine.ability.type.RemoveStorage")

        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def restock_ability(line, restock_target_id):
        """
        Adds the Restock ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        # get the restock target
        converter_groups = {}
        converter_groups.update(dataset.unit_lines)
        converter_groups.update(dataset.building_lines)
        converter_groups.update(dataset.ambient_groups)

        restock_target = converter_groups[restock_target_id]

        if not restock_target.is_harvestable():
            raise Exception("%s cannot be restocked: is not harvestable"
                            % (restock_target))

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]
        ability_ref = "%s.%s" % (game_entity_name, RESTOCK_TARGET_LOOKUPS[restock_target_id])
        ability_raw_api_object = RawAPIObject(ability_ref,
                                              RESTOCK_TARGET_LOOKUPS[restock_target_id],
                                              dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Restock")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        ability_animation_id  = -1

        if isinstance(line, GenieVillagerGroup) and restock_target_id == 50:
            # Search for the build graphic of farms
            restock_unit = line.get_units_with_command(101)[0]
            commands = restock_unit.get_member("unit_commands").get_value()
            for command in commands:
                type_id = command.get_value()["type"].get_value()

                if type_id == 101:
                    ability_animation_id = command.get_value()["work_sprite_id"].get_value()

        if ability_animation_id > -1:
            # Make the ability animated
            ability_raw_api_object.add_raw_parent("engine.ability.specialization.AnimatedAbility")

            animations_set = []

            # Create animation object
            animation_ref = "%s.%s.RestockAnimation" % (game_entity_name,
                                                        RESTOCK_TARGET_LOOKUPS[restock_target_id])
            animation_raw_api_object = RawAPIObject(animation_ref,
                                                    "%sAnimation" % (RESTOCK_TARGET_LOOKUPS[restock_target_id]),
                                                    dataset.nyan_api_objects)
            animation_raw_api_object.add_raw_parent("engine.aux.graphics.Animation")
            animation_location = ExpectedPointer(line, ability_ref)
            animation_raw_api_object.set_location(animation_location)

            ability_sprite = CombinedSprite(ability_animation_id,
                                            "restock_%s" % (name_lookup_dict[current_unit_id][1]),
                                            dataset)
            dataset.combined_sprites.update({ability_sprite.get_id(): ability_sprite})
            ability_sprite.add_reference(animation_raw_api_object)

            animation_raw_api_object.add_raw_member("sprite", ability_sprite,
                                                    "engine.aux.graphics.Animation")

            animation_expected_pointer = ExpectedPointer(line, animation_ref)
            animations_set.append(animation_expected_pointer)

            ability_raw_api_object.add_raw_member("animations", animations_set,
                                                  "engine.ability.specialization.AnimatedAbility")

            line.add_raw_api_object(animation_raw_api_object)

        # Auto restock
        ability_raw_api_object.add_raw_member("auto_restock",
                                              True,  # always True since AoC
                                              "engine.ability.type.Restock")

        # Target
        restock_target_lookup_dict = {}
        restock_target_lookup_dict.update(UNIT_LINE_LOOKUPS)
        restock_target_lookup_dict.update(BUILDING_LINE_LOOKUPS)
        restock_target_lookup_dict.update(AMBIENT_GROUP_LOOKUPS)
        restock_target_name = restock_target_lookup_dict[restock_target_id][0]
        spot_expected_pointer = ExpectedPointer(restock_target,
                                                "%s.Harvestable.%sResourceSpot"
                                                % (restock_target_name,
                                                   restock_target_name))
        ability_raw_api_object.add_raw_member("target",
                                              spot_expected_pointer,
                                              "engine.ability.type.Restock")

        # restock time
        restock_time = restock_target.get_head_unit().get_member("creation_time").get_value()
        ability_raw_api_object.add_raw_member("restock_time",
                                              restock_time,
                                              "engine.ability.type.Restock")

        # Manual/Auto Cost
        # Link to the same Cost object as Create
        cost_expected_pointer = ExpectedPointer(restock_target,
                                                "%s.CreatableGameEntity.%sCost"
                                                % (restock_target_name, restock_target_name))
        ability_raw_api_object.add_raw_member("manual_cost",
                                              cost_expected_pointer,
                                              "engine.ability.type.Restock")
        ability_raw_api_object.add_raw_member("auto_cost",
                                              cost_expected_pointer,
                                              "engine.ability.type.Restock")

        # Amount
        restock_amount = restock_target.get_head_unit().get_member("resource_capacity").get_value()
        if restock_target_id == 50:
            # Farm food amount (hardcoded in civ)
            restock_amount = dataset.genie_civs[1].get_member("resources").get_value()[36].get_value()

        elif restock_target_id == 199:
            # Fish trap added food amount (hardcoded in civ)
            restock_amount += dataset.genie_civs[1].get_member("resources").get_value()[88].get_value()

        ability_raw_api_object.add_raw_member("amount",
                                              restock_amount,
                                              "engine.ability.type.Restock")

        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def research_ability(line):
        """
        Adds the Research ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]
        ability_ref = "%s.Research" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(ability_ref, "Research", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Research")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        researchables_set = []

        for researchable in line.researches:
            if researchable.is_unique():
                # Skip this because unique techs are handled by civs
                continue

            # ResearchableTech objects are created for each unit/building
            # line individually to avoid duplicates. We just point to the
            # raw API objects here.
            researchable_id = researchable.get_id()
            researchable_name = TECH_GROUP_LOOKUPS[researchable_id][0]

            raw_api_object_ref = "%s.ResearchableTech" % researchable_name
            researchable_expected_pointer = ExpectedPointer(researchable,
                                                            raw_api_object_ref)
            researchables_set.append(researchable_expected_pointer)

        ability_raw_api_object.add_raw_member("researchables", researchables_set,
                                              "engine.ability.type.Research")
        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def resistance_ability(line):
        """
        Adds the Resistance ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        elif isinstance(line, GenieAmbientGroup):
            name_lookup_dict = AMBIENT_GROUP_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]
        ability_ref = "%s.Resistance" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(ability_ref, "Resistance", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Resistance")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        resistances = []
        resistances.extend(AoCEffectResistanceSubprocessor.get_attack_resistances(line, ability_ref))
        # TODO: Other resistance types

        # Resistances
        ability_raw_api_object.add_raw_member("resistances",
                                              resistances,
                                              "engine.ability.type.Resistance")

        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def selectable_ability(line):
        """
        Adds Selectable abilities to a line. Units will get two of these,
        one Rectangle box for the Self stance and one MatchToSprite box
        for other stances.

        :param line: Unit/Building line that gets the abilities.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the abilities.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        current_unit = line.get_head_unit()
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        elif isinstance(line, GenieAmbientGroup):
            name_lookup_dict = AMBIENT_GROUP_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        obj_refs = ("%s.Selectable" % (game_entity_name),)
        ability_refs = ("Selectable",)

        if isinstance(line, GenieUnitLineGroup):
            obj_refs = ("%s.SelectableOthers" % (game_entity_name),
                        "%s.SelectableSelf" % (game_entity_name))
            ability_refs = ("SelectableOthers",
                            "SelectableSelf")

        abilities = []

        # First box (MatchToSprite)
        obj_ref = obj_refs[0]
        ability_ref = ability_refs[0]

        ability_raw_api_object = RawAPIObject(obj_ref, ability_ref, dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Selectable")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Selection box
        box_ref = dataset.nyan_api_objects["engine.aux.selection_box.type.MatchToSprite"]
        ability_raw_api_object.add_raw_member("selection_box",
                                              box_ref,
                                              "engine.ability.type.Selectable")

        # Diplomacy setting (for units)
        if isinstance(line, GenieUnitLineGroup):
            ability_raw_api_object.add_raw_parent("engine.ability.specialization.DiplomaticAbility")

            stances = [dataset.pregen_nyan_objects["aux.diplomatic_stance.types.Enemy"].get_nyan_object(),
                       dataset.pregen_nyan_objects["aux.diplomatic_stance.types.Neutral"].get_nyan_object(),
                       dataset.pregen_nyan_objects["aux.diplomatic_stance.types.Friendly"].get_nyan_object()]
            ability_raw_api_object.add_raw_member("stances",
                                                  stances,
                                                  "engine.ability.specialization.DiplomaticAbility")

        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        abilities.append(ability_expected_pointer)

        if not isinstance(line, GenieUnitLineGroup):
            return abilities

        # Second box (Rectangle)
        obj_ref = obj_refs[1]
        ability_ref = ability_refs[1]

        ability_raw_api_object = RawAPIObject(obj_ref, ability_ref, dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Selectable")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Selection box
        box_name = "%s.SelectableSelf.Rectangle" % (game_entity_name)
        box_raw_api_object = RawAPIObject(box_name, "Rectangle", dataset.nyan_api_objects)
        box_raw_api_object.add_raw_parent("engine.aux.selection_box.type.Rectangle")
        box_location = ExpectedPointer(line, obj_ref)
        box_raw_api_object.set_location(box_location)

        radius_x = current_unit.get_member("selection_shape_x").get_value()
        box_raw_api_object.add_raw_member("radius_x",
                                          radius_x,
                                          "engine.aux.selection_box.type.Rectangle")

        radius_y = current_unit.get_member("selection_shape_y").get_value()
        box_raw_api_object.add_raw_member("radius_y",
                                          radius_y,
                                          "engine.aux.selection_box.type.Rectangle")

        line.add_raw_api_object(box_raw_api_object)

        box_expected_pointer = ExpectedPointer(line, box_name)
        ability_raw_api_object.add_raw_member("selection_box",
                                              box_expected_pointer,
                                              "engine.ability.type.Selectable")

        # Diplomacy settings
        ability_raw_api_object.add_raw_parent("engine.ability.specialization.DiplomaticAbility")

        stances = [dataset.nyan_api_objects["engine.aux.diplomatic_stance.type.Self"]]
        ability_raw_api_object.add_raw_member("stances",
                                              stances,
                                              "engine.ability.specialization.DiplomaticAbility")

        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        abilities.append(ability_expected_pointer)

        return abilities

    @staticmethod
    def send_back_to_task_ability(line):
        """
        Adds the SendBackToTask ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]
        ability_ref = "%s.SendBackToTask" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(ability_ref, "SendBackToTask", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.SendBackToTask")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Only works on villagers
        allowed_types = [dataset.pregen_nyan_objects["aux.game_entity_type.types.Villager"].get_nyan_object()]
        ability_raw_api_object.add_raw_member("allowed_types",
                                              allowed_types,
                                              "engine.ability.type.SendBackToTask")
        ability_raw_api_object.add_raw_member("blacklisted_game_entities",
                                              [],
                                              "engine.ability.type.SendBackToTask")

        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def shoot_projectile_ability(line, command_id):
        """
        Adds the ShootProjectile ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        current_unit = line.get_head_unit()
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        ability_name = COMMAND_TYPE_LOOKUPS[command_id]

        game_entity_name = name_lookup_dict[current_unit_id][0]
        ability_ref = "%s.%s" % (game_entity_name, ability_name)
        ability_raw_api_object = RawAPIObject(ability_ref, ability_name, dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.ShootProjectile")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        ability_animation_id = current_unit.get_member("attack_sprite_id").get_value()

        if ability_animation_id > -1:
            # Make the ability animated
            ability_raw_api_object.add_raw_parent("engine.ability.specialization.AnimatedAbility")

            animations_set = []

            # Create animation object
            animation_ref = "%s.%s.ShootAnimation" % (game_entity_name, ability_name)
            animation_raw_api_object = RawAPIObject(animation_ref, "ShootAnimation",
                                                    dataset.nyan_api_objects)
            animation_raw_api_object.add_raw_parent("engine.aux.graphics.Animation")
            animation_location = ExpectedPointer(line, ability_ref)
            animation_raw_api_object.set_location(animation_location)

            ability_sprite = CombinedSprite(ability_animation_id,
                                            "attack_%s" % (name_lookup_dict[current_unit_id][1]),
                                            dataset)
            dataset.combined_sprites.update({ability_sprite.get_id(): ability_sprite})
            ability_sprite.add_reference(animation_raw_api_object)

            animation_raw_api_object.add_raw_member("sprite", ability_sprite,
                                                    "engine.aux.graphics.Animation")

            animation_expected_pointer = ExpectedPointer(line, animation_ref)
            animations_set.append(animation_expected_pointer)

            ability_raw_api_object.add_raw_member("animations", animations_set,
                                                  "engine.ability.specialization.AnimatedAbility")

            line.add_raw_api_object(animation_raw_api_object)

        # Projectile
        projectiles = []
        projectile_primary = current_unit.get_member("attack_projectile_primary_unit_id").get_value()
        if projectile_primary > -1:
            projectiles.append(ExpectedPointer(line,
                                               "%s.ShootProjectile.Projectile0" % (game_entity_name)))

        projectile_secondary = current_unit.get_member("attack_projectile_secondary_unit_id").get_value()
        if projectile_secondary > -1:
            projectiles.append(ExpectedPointer(line,
                                               "%s.ShootProjectile.Projectile1" % (game_entity_name)))

        ability_raw_api_object.add_raw_member("projectiles",
                                              projectiles,
                                              "engine.ability.type.ShootProjectile")

        # Projectile count
        min_projectiles = current_unit.get_member("attack_projectile_count").get_value()
        max_projectiles = current_unit.get_member("attack_projectile_max_count").get_value()

        # Special case where only the second projectile is defined (town center)
        # The min/max projectile count is lowered by 1 in this case
        if projectile_primary == -1:
            min_projectiles -= 1
            max_projectiles -= 1

        ability_raw_api_object.add_raw_member("min_projectiles",
                                              min_projectiles,
                                              "engine.ability.type.ShootProjectile")
        ability_raw_api_object.add_raw_member("max_projectiles",
                                              max_projectiles,
                                              "engine.ability.type.ShootProjectile")

        # Range
        min_range = current_unit.get_member("weapon_range_min").get_value()
        ability_raw_api_object.add_raw_member("min_range",
                                              min_range,
                                              "engine.ability.type.ShootProjectile")

        max_range = current_unit.get_member("weapon_range_max").get_value()
        ability_raw_api_object.add_raw_member("max_range",
                                              max_range,
                                              "engine.ability.type.ShootProjectile")

        # Reload time and delay
        reload_time = current_unit.get_member("attack_speed").get_value()
        ability_raw_api_object.add_raw_member("reload_time",
                                              reload_time,
                                              "engine.ability.type.ShootProjectile")

        if ability_animation_id > -1:
            animation = dataset.genie_graphics[ability_animation_id]
            frame_rate = animation.get_frame_rate()

        else:
            frame_rate = 0

        spawn_delay_frames = current_unit.get_member("frame_delay").get_value()
        spawn_delay = frame_rate * spawn_delay_frames
        ability_raw_api_object.add_raw_member("spawn_delay",
                                              spawn_delay,
                                              "engine.ability.type.ShootProjectile")

        # TODO: Hardcoded?
        ability_raw_api_object.add_raw_member("projectile_delay",
                                              0.1,
                                              "engine.ability.type.ShootProjectile")

        # Turning
        if isinstance(line, GenieBuildingLineGroup):
            require_turning = False

        else:
            require_turning = True

        ability_raw_api_object.add_raw_member("require_turning",
                                              require_turning,
                                              "engine.ability.type.ShootProjectile")

        # Manual Aiming (Mangonel + Trebuchet)
        if line.get_head_unit_id() in (280, 331):
            manual_aiming_allowed = True

        else:
            manual_aiming_allowed = False

        ability_raw_api_object.add_raw_member("manual_aiming_allowed",
                                              manual_aiming_allowed,
                                              "engine.ability.type.ShootProjectile")

        # Spawning area
        spawning_area_offset_x = current_unit.get_member("weapon_offset")[0].get_value()
        spawning_area_offset_y = current_unit.get_member("weapon_offset")[1].get_value()
        spawning_area_offset_z = current_unit.get_member("weapon_offset")[2].get_value()

        ability_raw_api_object.add_raw_member("spawning_area_offset_x",
                                              spawning_area_offset_x,
                                              "engine.ability.type.ShootProjectile")
        ability_raw_api_object.add_raw_member("spawning_area_offset_y",
                                              spawning_area_offset_y,
                                              "engine.ability.type.ShootProjectile")
        ability_raw_api_object.add_raw_member("spawning_area_offset_z",
                                              spawning_area_offset_z,
                                              "engine.ability.type.ShootProjectile")

        spawning_area_width = current_unit.get_member("attack_projectile_spawning_area_width").get_value()
        spawning_area_height = current_unit.get_member("attack_projectile_spawning_area_length").get_value()
        spawning_area_randomness = current_unit.get_member("attack_projectile_spawning_area_randomness").get_value()

        ability_raw_api_object.add_raw_member("spawning_area_width",
                                              spawning_area_width,
                                              "engine.ability.type.ShootProjectile")
        ability_raw_api_object.add_raw_member("spawning_area_height",
                                              spawning_area_height,
                                              "engine.ability.type.ShootProjectile")
        ability_raw_api_object.add_raw_member("spawning_area_randomness",
                                              spawning_area_randomness,
                                              "engine.ability.type.ShootProjectile")

        # Restrictions on targets (only units and buildings allowed)
        allowed_types = [dataset.pregen_nyan_objects["aux.game_entity_type.types.Building"].get_nyan_object(),
                         dataset.pregen_nyan_objects["aux.game_entity_type.types.Unit"].get_nyan_object()]
        ability_raw_api_object.add_raw_member("allowed_types",
                                              allowed_types,
                                              "engine.ability.type.ShootProjectile")
        ability_raw_api_object.add_raw_member("blacklisted_game_entities",
                                              [],
                                              "engine.ability.type.ShootProjectile")

        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def stop_ability(line):
        """
        Adds the Stop ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = "%s.Stop" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(ability_ref, "Stop", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Stop")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Diplomacy settings
        ability_raw_api_object.add_raw_parent("engine.ability.specialization.DiplomaticAbility")
        diplomatic_stances = [dataset.nyan_api_objects["engine.aux.diplomatic_stance.type.Self"]]
        ability_raw_api_object.add_raw_member("stances", diplomatic_stances,
                                              "engine.ability.specialization.DiplomaticAbility")

        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def storage_ability(line):
        """
        Adds the Storage ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        current_unit = line.get_head_unit()
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = "%s.Storage" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(ability_ref, "Storage", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Storage")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Container
        # ==============================================================================
        container_name = "%s.Storage.%sContainer" % (game_entity_name, game_entity_name)
        container_raw_api_object = RawAPIObject(container_name,
                                                "%sContainer" % (game_entity_name),
                                                dataset.nyan_api_objects)
        container_raw_api_object.add_raw_parent("engine.aux.storage.Container")
        container_location = ExpectedPointer(line, ability_ref)
        container_raw_api_object.set_location(container_location)

        # TODO: Define storage elements
        container_raw_api_object.add_raw_member("storage_elements",
                                                [],
                                                "engine.aux.storage.Container")

        # Container slots
        slots = current_unit.get_member("garrison_capacity").get_value()
        container_raw_api_object.add_raw_member("slots",
                                                slots,
                                                "engine.aux.storage.Container")

        # TODO: Carry progress
        container_raw_api_object.add_raw_member("carry_progress",
                                                [],
                                                "engine.aux.storage.Container")

        line.add_raw_api_object(container_raw_api_object)
        # ==============================================================================
        container_expected_pointer = ExpectedPointer(line, container_name)
        ability_raw_api_object.add_raw_member("container",
                                              container_expected_pointer,
                                              "engine.ability.type.Storage")

        # TODO: Empty condition
        ability_raw_api_object.add_raw_member("empty_condition",
                                              [],
                                              "engine.ability.type.Storage")

        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def terrain_requirement_ability(line):
        """
        Adds the TerrainRequirement to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointers for the abilities.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        current_unit = line.get_head_unit()
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        elif isinstance(line, GenieAmbientGroup):
            name_lookup_dict = AMBIENT_GROUP_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = "%s.TerrainRequirement" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(ability_ref, "TerrainRequirement", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.TerrainRequirement")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Allowed types
        allowed_types = []
        terrain_restriction = current_unit["terrain_restriction"].get_value()
        for terrain_type in TERRAIN_TYPE_LOOKUPS.values():
            # Check if terrain type is covered by terrain restriction
            if terrain_restriction in terrain_type[1]:
                type_name = "aux.terrain_type.types.%s" % (terrain_type[2])
                type_obj = dataset.pregen_nyan_objects[type_name].get_nyan_object()
                allowed_types.append(type_obj)

        ability_raw_api_object.add_raw_member("allowed_types",
                                              allowed_types,
                                              "engine.ability.type.TerrainRequirement")

        # Blacklisted terrains
        ability_raw_api_object.add_raw_member("blacklisted_terrains",
                                              [],
                                              "engine.ability.type.TerrainRequirement")

        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def transfer_storage_ability(line):
        """
        Adds the TransferStorage ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer, None
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = "%s.TransferStorage" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(ability_ref, "TransferStorage", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.TransferStorage")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # storage element
        storage_entity = None
        garrisoned_expected_pointer = None
        for garrisoned in line.garrison_entities:
            creatable_type = garrisoned.get_head_unit().get_member("creatable_type").get_value()

            if creatable_type == 4:
                storage_name = AMBIENT_GROUP_LOOKUPS[garrisoned.get_id()][0]
                storage_entity = garrisoned
                garrisoned_expected_pointer = ExpectedPointer(storage_entity, storage_name)

                # TODO: There are other relics in the .dat
                break

        ability_raw_api_object.add_raw_member("storage_element",
                                              garrisoned_expected_pointer,
                                              "engine.ability.type.TransferStorage")

        # Source container
        source_ref = "%s.Storage.%sContainer" % (game_entity_name, game_entity_name)
        source_expected_pointer = ExpectedPointer(line, source_ref)
        ability_raw_api_object.add_raw_member("source_container",
                                              source_expected_pointer,
                                              "engine.ability.type.TransferStorage")

        # Target container
        target = None
        unit_commands = line.get_switch_unit().get_member("unit_commands").get_value()
        for command in unit_commands:
            type_id = command.get_value()["type"].get_value()

            # Deposit
            if type_id == 136:
                target_id = command.get_value()["unit_id"].get_value()
                target = dataset.building_lines[target_id]

        target_name = BUILDING_LINE_LOOKUPS[target.get_id()][0]
        target_ref = "%s.Storage.%sContainer" % (target_name, target_name)
        target_expected_pointer = ExpectedPointer(target, target_ref)
        ability_raw_api_object.add_raw_member("target_container",
                                              target_expected_pointer,
                                              "engine.ability.type.TransferStorage")

        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def turn_ability(line):
        """
        Adds the Turn ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        current_unit = line.get_head_unit()
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = "%s.Turn" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(ability_ref, "Turn", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Turn")
        ability_location = ExpectedPointer(line, game_entity_name)
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

        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def use_contingent_ability(line):
        """
        Adds the UseContingent ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        current_unit = line.get_head_unit()
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = "%s.UseContingent" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(ability_ref, "UseContingent", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.UseContingent")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Also stores the pop space
        resource_storage = current_unit.get_member("resource_storage").get_value()

        contingents = []
        for storage in resource_storage:
            type_id = storage.get_value()["type"].get_value()

            if type_id == 11:
                resource = dataset.pregen_nyan_objects["aux.resource.types.PopulationSpace"].get_nyan_object()
                resource_name = "PopSpace"

            else:
                continue

            amount = storage.get_value()["amount"].get_value()

            contingent_amount_name = "%s.UseContingent.%s" % (game_entity_name, resource_name)
            contingent_amount = RawAPIObject(contingent_amount_name, resource_name,
                                             dataset.nyan_api_objects)
            contingent_amount.add_raw_parent("engine.aux.resource.ResourceAmount")
            ability_expected_pointer = ExpectedPointer(line, ability_ref)
            contingent_amount.set_location(ability_expected_pointer)

            contingent_amount.add_raw_member("type",
                                             resource,
                                             "engine.aux.resource.ResourceAmount")
            contingent_amount.add_raw_member("amount",
                                             amount,
                                             "engine.aux.resource.ResourceAmount")

            line.add_raw_api_object(contingent_amount)
            contingent_amount_expected_pointer = ExpectedPointer(line,
                                                                 contingent_amount_name)
            contingents.append(contingent_amount_expected_pointer)

        if not contingents:
            # Do not create the ability if its values are empty
            return None

        ability_raw_api_object.add_raw_member("amount",
                                              contingents,
                                              "engine.ability.type.UseContingent")
        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def visibility_ability(line):
        """
        Adds the Visibility ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        elif isinstance(line, GenieAmbientGroup):
            name_lookup_dict = AMBIENT_GROUP_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = "%s.Visibility" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(ability_ref, "Visibility", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Visibility")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Units are not visible in fog
        visible = False

        # Buidings and scenery is though
        if isinstance(line, (GenieBuildingLineGroup, GenieAmbientGroup)):
            visible = True

        ability_raw_api_object.add_raw_member("visible_in_fog", visible,
                                              "engine.ability.type.Visibility")

        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer
