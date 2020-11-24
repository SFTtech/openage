# Copyright 2020-2020 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-public-methods,too-many-lines,too-many-locals
# pylint: disable=too-many-branches,too-many-statements,too-many-arguments
# pylint: disable=invalid-name
#
# TODO:
# pylint: disable=unused-argument,line-too-long

"""
Derives and adds abilities to lines. Subroutine of the
nyan subprocessor.
"""
from math import degrees

from .....nyan.nyan_structs import MemberSpecialValue, MemberOperator
from .....util.ordered_set import OrderedSet
from ....entity_object.conversion.aoc.genie_unit import GenieBuildingLineGroup,\
    GenieAmbientGroup, GenieGarrisonMode, GenieStackBuildingGroup,\
    GenieUnitLineGroup, GenieMonkGroup
from ....entity_object.conversion.aoc.genie_unit import GenieVillagerGroup
from ....entity_object.conversion.combined_sound import CombinedSound
from ....entity_object.conversion.combined_sprite import CombinedSprite
from ....entity_object.conversion.converter_object import RawAPIObject
from ....entity_object.conversion.converter_object import RawMemberPush
from ....service.conversion import internal_name_lookups
from ....value_object.conversion.forward_ref import ForwardRef
from .effect_subprocessor import AoCEffectSubprocessor


class AoCAbilitySubprocessor:
    """
    Creates raw API objects for abilities in AoC.
    """

    @staticmethod
    def active_transform_to_ability(line):
        """
        Adds the ActiveTransformTo ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        # TODO: Implement
        return None

    @staticmethod
    def apply_continuous_effect_ability(line, command_id, ranged=False):
        """
        Adds the ApplyContinuousEffect ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        if isinstance(line, GenieVillagerGroup):
            current_unit = line.get_units_with_command(command_id)[0]

        else:
            current_unit = line.get_head_unit()

        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
        command_lookup_dict = internal_name_lookups.get_command_lookups(dataset.game_version)
        gset_lookup_dict = internal_name_lookups.get_graphic_set_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_name = command_lookup_dict[command_id][0]

        if ranged:
            ability_parent = "engine.ability.type.RangedContinuousEffect"

        else:
            ability_parent = "engine.ability.type.ApplyContinuousEffect"

        ability_ref = f"{game_entity_name}.{ability_name}"
        ability_raw_api_object = RawAPIObject(ability_ref, ability_name, dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent(ability_parent)
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Get animation from commands proceed sprite
        unit_commands = current_unit["unit_commands"].get_value()
        for command in unit_commands:
            type_id = command["type"].get_value()

            if type_id != command_id:
                continue

            ability_animation_id = command["proceed_sprite_id"].get_value()
            break

        else:
            ability_animation_id = -1

        if ability_animation_id > -1:
            # Make the ability animated
            ability_raw_api_object.add_raw_parent("engine.ability.specialization.AnimatedAbility")

            animations_set = []
            animation_forward_ref = AoCAbilitySubprocessor.create_animation(line,
                                                                            ability_animation_id,
                                                                            ability_ref,
                                                                            ability_name,
                                                                            "%s_"
                                                                            % command_lookup_dict[command_id][1])
            animations_set.append(animation_forward_ref)
            ability_raw_api_object.add_raw_member("animations", animations_set,
                                                  "engine.ability.specialization.AnimatedAbility")

            # Create custom civ graphics
            handled_graphics_set_ids = set()
            for civ_group in dataset.civ_groups.values():
                civ = civ_group.civ
                civ_id = civ_group.get_id()

                # Only proceed if the civ stores the unit in the line
                if current_unit_id not in civ["units"].get_value().keys():
                    continue

                civ_animation_id = civ["units"][current_unit_id]["attack_sprite_id"].get_value()

                if civ_animation_id != ability_animation_id:
                    # Find the corresponding graphics set
                    graphics_set_id = -1
                    for set_id, items in gset_lookup_dict.items():
                        if civ_id in items[0]:
                            graphics_set_id = set_id
                            break

                    # Check if the object for the animation has been created before
                    obj_exists = graphics_set_id in handled_graphics_set_ids
                    if not obj_exists:
                        handled_graphics_set_ids.add(graphics_set_id)

                    obj_prefix = f"{gset_lookup_dict[graphics_set_id][1]}{ability_name}"
                    filename_prefix = "%s_%s_" % (command_lookup_dict[command_id][1],
                                                  gset_lookup_dict[graphics_set_id][2],)
                    AoCAbilitySubprocessor.create_civ_animation(line,
                                                                civ_group,
                                                                civ_animation_id,
                                                                ability_ref,
                                                                obj_prefix,
                                                                filename_prefix,
                                                                obj_exists)

        # Command Sound
        ability_comm_sound_id = current_unit["command_sound_id"].get_value()
        if ability_comm_sound_id > -1:
            # Make the ability animated
            ability_raw_api_object.add_raw_parent("engine.ability.specialization.CommandSoundAbility")

            sounds_set = []
            sound_forward_ref = AoCAbilitySubprocessor.create_sound(line,
                                                                    ability_comm_sound_id,
                                                                    ability_ref,
                                                                    ability_name,
                                                                    "command_")
            sounds_set.append(sound_forward_ref)
            ability_raw_api_object.add_raw_member("sounds", sounds_set,
                                                  "engine.ability.specialization.CommandSoundAbility")

        if ranged:
            # Min range
            min_range = current_unit["weapon_range_min"].get_value()
            ability_raw_api_object.add_raw_member("min_range",
                                                  min_range,
                                                  "engine.ability.type.RangedContinuousEffect")

            # Max range
            if command_id == 105:
                # Heal
                max_range = 4

            else:
                max_range = current_unit["weapon_range_max"].get_value()

            ability_raw_api_object.add_raw_member("max_range",
                                                  max_range,
                                                  "engine.ability.type.RangedContinuousEffect")

        # Effects
        if command_id == 101:
            # Construct
            effects = AoCEffectSubprocessor.get_construct_effects(line, ability_ref)
            allowed_types = [
                dataset.pregen_nyan_objects["aux.game_entity_type.types.Building"].get_nyan_object()
            ]

        elif command_id == 105:
            # Heal
            effects = AoCEffectSubprocessor.get_heal_effects(line, ability_ref)
            allowed_types = [
                dataset.pregen_nyan_objects["aux.game_entity_type.types.Unit"].get_nyan_object()
            ]

        elif command_id == 106:
            # Repair
            effects = AoCEffectSubprocessor.get_repair_effects(line, ability_ref)
            allowed_types = [
                dataset.pregen_nyan_objects["aux.game_entity_type.types.Building"].get_nyan_object()
            ]

        ability_raw_api_object.add_raw_member("effects",
                                              effects,
                                              "engine.ability.type.ApplyContinuousEffect")

        # Application delay
        apply_graphic = dataset.genie_graphics[ability_animation_id]
        frame_rate = apply_graphic.get_frame_rate()
        frame_delay = current_unit["frame_delay"].get_value()
        application_delay = frame_rate * frame_delay
        ability_raw_api_object.add_raw_member("application_delay",
                                              application_delay,
                                              "engine.ability.type.ApplyContinuousEffect")

        # Allowed types
        ability_raw_api_object.add_raw_member("allowed_types",
                                              allowed_types,
                                              "engine.ability.type.ApplyContinuousEffect")
        ability_raw_api_object.add_raw_member("blacklisted_entities",
                                              [],
                                              "engine.ability.type.ApplyContinuousEffect")

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def apply_discrete_effect_ability(line, command_id, ranged=False, projectile=-1):
        """
        Adds the ApplyDiscreteEffect ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        if isinstance(line, GenieVillagerGroup):
            current_unit = line.get_units_with_command(command_id)[0]
            current_unit_id = current_unit["id0"].get_value()

        else:
            current_unit = line.get_head_unit()
            current_unit_id = line.get_head_unit_id()

        head_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
        command_lookup_dict = internal_name_lookups.get_command_lookups(dataset.game_version)
        gset_lookup_dict = internal_name_lookups.get_graphic_set_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[head_unit_id][0]

        ability_name = command_lookup_dict[command_id][0]

        if ranged:
            ability_parent = "engine.ability.type.RangedDiscreteEffect"

        else:
            ability_parent = "engine.ability.type.ApplyDiscreteEffect"

        if projectile == -1:
            ability_ref = f"{game_entity_name}.{ability_name}"
            ability_raw_api_object = RawAPIObject(ability_ref,
                                                  ability_name,
                                                  dataset.nyan_api_objects)
            ability_raw_api_object.add_raw_parent(ability_parent)
            ability_location = ForwardRef(line, game_entity_name)
            ability_raw_api_object.set_location(ability_location)

            ability_animation_id = current_unit["attack_sprite_id"].get_value()

        else:
            ability_ref = "%s.ShootProjectile.Projectile%s.%s" % (game_entity_name,
                                                                  str(projectile),
                                                                  ability_name)
            ability_raw_api_object = RawAPIObject(ability_ref, ability_name, dataset.nyan_api_objects)
            ability_raw_api_object.add_raw_parent(ability_parent)
            ability_location = ForwardRef(line,
                                          "%s.ShootProjectile.Projectile%s"
                                          % (game_entity_name, str(projectile)))
            ability_raw_api_object.set_location(ability_location)

            ability_animation_id = -1

        if ability_animation_id > -1:
            # Make the ability animated
            ability_raw_api_object.add_raw_parent("engine.ability.specialization.AnimatedAbility")

            animations_set = []
            animation_forward_ref = AoCAbilitySubprocessor.create_animation(line,
                                                                            ability_animation_id,
                                                                            ability_ref,
                                                                            ability_name,
                                                                            "%s_"
                                                                            % command_lookup_dict[command_id][1])
            animations_set.append(animation_forward_ref)
            ability_raw_api_object.add_raw_member("animations", animations_set,
                                                  "engine.ability.specialization.AnimatedAbility")

            # Create custom civ graphics
            handled_graphics_set_ids = set()
            for civ_group in dataset.civ_groups.values():
                civ = civ_group.civ
                civ_id = civ_group.get_id()

                # Only proceed if the civ stores the unit in the line
                if current_unit_id not in civ["units"].get_value().keys():
                    continue

                civ_animation_id = civ["units"][current_unit_id]["attack_sprite_id"].get_value()

                if civ_animation_id != ability_animation_id:
                    # Find the corresponding graphics set
                    graphics_set_id = -1
                    for set_id, items in gset_lookup_dict.items():
                        if civ_id in items[0]:
                            graphics_set_id = set_id
                            break

                    # Check if the object for the animation has been created before
                    obj_exists = graphics_set_id in handled_graphics_set_ids
                    if not obj_exists:
                        handled_graphics_set_ids.add(graphics_set_id)

                    obj_prefix = f"{gset_lookup_dict[graphics_set_id][1]}{ability_name}"
                    filename_prefix = "%s_%s_" % (command_lookup_dict[command_id][1],
                                                  gset_lookup_dict[graphics_set_id][2],)
                    AoCAbilitySubprocessor.create_civ_animation(line,
                                                                civ_group,
                                                                civ_animation_id,
                                                                ability_ref,
                                                                obj_prefix,
                                                                filename_prefix,
                                                                obj_exists)

        # Command Sound
        if projectile == -1:
            ability_comm_sound_id = current_unit["command_sound_id"].get_value()

        else:
            ability_comm_sound_id = -1

        if ability_comm_sound_id > -1:
            # Make the ability animated
            ability_raw_api_object.add_raw_parent("engine.ability.specialization.CommandSoundAbility")

            sounds_set = []

            if projectile == -1:
                sound_obj_prefix = ability_name

            else:
                sound_obj_prefix = "ProjectileAttack"

            sound_forward_ref = AoCAbilitySubprocessor.create_sound(line,
                                                                    ability_comm_sound_id,
                                                                    ability_ref,
                                                                    sound_obj_prefix,
                                                                    "command_")
            sounds_set.append(sound_forward_ref)
            ability_raw_api_object.add_raw_member("sounds", sounds_set,
                                                  "engine.ability.specialization.CommandSoundAbility")

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
            if projectile != 1:
                effects = AoCEffectSubprocessor.get_attack_effects(line, ability_ref)

            else:
                effects = AoCEffectSubprocessor.get_attack_effects(line, ability_ref, projectile=1)

        elif command_id == 104:
            # Convert
            effects = AoCEffectSubprocessor.get_convert_effects(line, ability_ref)

        ability_raw_api_object.add_raw_member("effects",
                                              effects,
                                              "engine.ability.type.ApplyDiscreteEffect")

        # Reload time
        if projectile == -1:
            reload_time = current_unit["attack_speed"].get_value()

        else:
            reload_time = 0

        ability_raw_api_object.add_raw_member("reload_time",
                                              reload_time,
                                              "engine.ability.type.ApplyDiscreteEffect")

        # Application delay
        if projectile == -1:
            attack_graphic_id = current_unit["attack_sprite_id"].get_value()
            attack_graphic = dataset.genie_graphics[attack_graphic_id]
            frame_rate = attack_graphic.get_frame_rate()
            frame_delay = current_unit["frame_delay"].get_value()
            application_delay = frame_rate * frame_delay

        else:
            application_delay = 0

        ability_raw_api_object.add_raw_member("application_delay",
                                              application_delay,
                                              "engine.ability.type.ApplyDiscreteEffect")

        # Allowed types (all buildings/units)
        if command_id == 104:
            # Convert
            allowed_types = [
                dataset.pregen_nyan_objects["aux.game_entity_type.types.Unit"].get_nyan_object()
            ]

        else:
            allowed_types = [
                dataset.pregen_nyan_objects["aux.game_entity_type.types.Unit"].get_nyan_object(),
                dataset.pregen_nyan_objects["aux.game_entity_type.types.Building"].get_nyan_object()
            ]

        ability_raw_api_object.add_raw_member("allowed_types",
                                              allowed_types,
                                              "engine.ability.type.ApplyDiscreteEffect")

        if command_id == 104:
            # Convert
            blacklisted_entities = []
            for unit_line in dataset.unit_lines.values():
                if unit_line.has_command(104):
                    # Blacklist other monks
                    blacklisted_name = name_lookup_dict[unit_line.get_head_unit_id()][0]
                    blacklisted_entities.append(ForwardRef(unit_line, blacklisted_name))

                elif unit_line.get_class_id() in (13, 55):
                    # Blacklist siege
                    blacklisted_name = name_lookup_dict[unit_line.get_head_unit_id()][0]
                    blacklisted_entities.append(ForwardRef(unit_line, blacklisted_name))

        else:
            blacklisted_entities = []

        ability_raw_api_object.add_raw_member("blacklisted_entities",
                                              blacklisted_entities,
                                              "engine.ability.type.ApplyDiscreteEffect")

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def attribute_change_tracker_ability(line):
        """
        Adds the AttributeChangeTracker ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        current_unit = line.get_head_unit()
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = f"{game_entity_name}.AttributeChangeTracker"
        ability_raw_api_object = RawAPIObject(ability_ref,
                                              "AttributeChangeTracker",
                                              dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.AttributeChangeTracker")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Attribute
        attribute = dataset.pregen_nyan_objects["aux.attribute.types.Health"].get_nyan_object()
        ability_raw_api_object.add_raw_member("attribute",
                                              attribute,
                                              "engine.ability.type.AttributeChangeTracker")

        # Change progress
        damage_graphics = current_unit["damage_graphics"].get_value()
        progress_forward_refs = []

        # Damage graphics are ordered ascending, so we start from 0
        interval_left_bound = 0
        for damage_graphic_member in damage_graphics:
            interval_right_bound = damage_graphic_member["damage_percent"].get_value()
            progress_name = "%s.AttributeChangeTracker.ChangeProgress%s" % (game_entity_name,
                                                                            interval_right_bound)
            progress_raw_api_object = RawAPIObject(progress_name,
                                                   f"ChangeProgress{interval_right_bound}",
                                                   dataset.nyan_api_objects)
            progress_raw_api_object.add_raw_parent("engine.aux.progress.type.AttributeChangeProgress")
            progress_location = ForwardRef(line, ability_ref)
            progress_raw_api_object.set_location(progress_location)

            # Interval
            progress_raw_api_object.add_raw_member("left_boundary",
                                                   interval_left_bound,
                                                   "engine.aux.progress.Progress")
            progress_raw_api_object.add_raw_member("right_boundary",
                                                   interval_right_bound,
                                                   "engine.aux.progress.Progress")

            progress_animation_id = damage_graphic_member["graphic_id"].get_value()
            if progress_animation_id > -1:
                progress_raw_api_object.add_raw_parent("engine.aux.progress.specialization.AnimationOverlayProgress")

                # Animation
                animations_set = []
                animation_forward_ref = AoCAbilitySubprocessor.create_animation(line,
                                                                                progress_animation_id,
                                                                                progress_name,
                                                                                "Idle",
                                                                                "idle_damage_override_%s_"
                                                                                % (interval_right_bound))
                animations_set.append(animation_forward_ref)
                progress_raw_api_object.add_raw_member("overlays",
                                                       animations_set,
                                                       "engine.aux.progress.specialization.AnimationOverlayProgress")

            progress_forward_refs.append(ForwardRef(line, progress_name))
            line.add_raw_api_object(progress_raw_api_object)
            interval_left_bound = interval_right_bound

        ability_raw_api_object.add_raw_member("change_progress",
                                              progress_forward_refs,
                                              "engine.ability.type.AttributeChangeTracker")

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def collect_storage_ability(line):
        """
        Adds the CollectStorage ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = f"{game_entity_name}.CollectStorage"
        ability_raw_api_object = RawAPIObject(ability_ref,
                                              "CollectStorage",
                                              dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.CollectStorage")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Container
        container_ref = f"{game_entity_name}.Storage.{game_entity_name}Container"
        container_forward_ref = ForwardRef(line, container_ref)
        ability_raw_api_object.add_raw_member("container",
                                              container_forward_ref,
                                              "engine.ability.type.CollectStorage")

        # Storage elements
        elements = []
        entity_lookups = internal_name_lookups.get_entity_lookups(dataset.game_version)
        for entity in line.garrison_entities:
            entity_ref = entity_lookups[entity.get_head_unit_id()][0]
            entity_forward_ref = ForwardRef(entity, entity_ref)
            elements.append(entity_forward_ref)

        ability_raw_api_object.add_raw_member("storage_elements",
                                              elements,
                                              "engine.ability.type.CollectStorage")

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def constructable_ability(line):
        """
        Adds the Constructable ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        current_unit = line.get_head_unit()
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = f"{game_entity_name}.Constructable"
        ability_raw_api_object = RawAPIObject(ability_ref, "Constructable", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Constructable")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Starting progress (always 0)
        ability_raw_api_object.add_raw_member("starting_progress",
                                              0,
                                              "engine.ability.type.Constructable")

        construction_animation_id = current_unit["construction_graphic_id"].get_value()

        # Construction progress
        progress_forward_refs = []
        if line.get_class_id() == 49:
            # Farms
            # =====================================================================================
            progress_name = f"{game_entity_name}.Constructable.ConstructionProgress0"
            progress_raw_api_object = RawAPIObject(progress_name,
                                                   "ConstructionProgress0",
                                                   dataset.nyan_api_objects)
            progress_raw_api_object.add_raw_parent("engine.aux.progress.type.ConstructionProgress")
            progress_location = ForwardRef(line, ability_ref)
            progress_raw_api_object.set_location(progress_location)

            # Interval = (0.0, 0.0)
            progress_raw_api_object.add_raw_member("left_boundary",
                                                   0.0,
                                                   "engine.aux.progress.Progress")
            progress_raw_api_object.add_raw_member("right_boundary",
                                                   0.0,
                                                   "engine.aux.progress.Progress")

            progress_raw_api_object.add_raw_parent("engine.aux.progress.specialization.TerrainOverlayProgress")

            # Terrain overlay
            terrain_ref = "FarmConstruction1"
            terrain_group = dataset.terrain_groups[29]
            terrain_forward_ref = ForwardRef(terrain_group, terrain_ref)
            progress_raw_api_object.add_raw_member("terrain_overlay",
                                                   terrain_forward_ref,
                                                   "engine.aux.progress.specialization.TerrainOverlayProgress")

            progress_raw_api_object.add_raw_parent("engine.aux.progress.specialization.StateChangeProgress")

            # State change
            # =====================================================================================
            init_state_name = f"{ability_ref}.InitState"
            init_state_raw_api_object = RawAPIObject(init_state_name,
                                                     "InitState",
                                                     dataset.nyan_api_objects)
            init_state_raw_api_object.add_raw_parent("engine.aux.state_machine.StateChanger")
            init_state_location = ForwardRef(line, ability_ref)
            init_state_raw_api_object.set_location(init_state_location)

            # Priority
            init_state_raw_api_object.add_raw_member("priority",
                                                     1,
                                                     "engine.aux.state_machine.StateChanger")

            # Enabled abilities
            enabled_forward_refs = [
                ForwardRef(line,
                           f"{game_entity_name}.VisibilityConstruct0")
            ]
            init_state_raw_api_object.add_raw_member("enable_abilities",
                                                     enabled_forward_refs,
                                                     "engine.aux.state_machine.StateChanger")

            # Disabled abilities
            disabled_forward_refs = [
                ForwardRef(line,
                           f"{game_entity_name}.AttributeChangeTracker"),
                ForwardRef(line,
                           f"{game_entity_name}.LineOfSight"),
                ForwardRef(line,
                           f"{game_entity_name}.Visibility")
            ]
            if len(line.creates) > 0:
                disabled_forward_refs.append(ForwardRef(line,
                                                        f"{game_entity_name}.Create"))
                disabled_forward_refs.append(ForwardRef(line,
                                                        "%s.ProductionQueue"
                                                        % (game_entity_name)))
            if len(line.researches) > 0:
                disabled_forward_refs.append(ForwardRef(line,
                                                        f"{game_entity_name}.Research"))

            if line.is_projectile_shooter():
                disabled_forward_refs.append(ForwardRef(line,
                                                        f"{game_entity_name}.Attack"))

            if line.is_garrison():
                disabled_forward_refs.append(ForwardRef(line,
                                                        f"{game_entity_name}.Storage"))
                disabled_forward_refs.append(ForwardRef(line,
                                                        "%s.RemoveStorage"
                                                        % (game_entity_name)))

                garrison_mode = line.get_garrison_mode()

                if garrison_mode == GenieGarrisonMode.NATURAL:
                    disabled_forward_refs.append(ForwardRef(line,
                                                            "%s.SendBackToTask"
                                                            % (game_entity_name)))

                if garrison_mode in (GenieGarrisonMode.NATURAL, GenieGarrisonMode.SELF_PRODUCED):
                    disabled_forward_refs.append(ForwardRef(line,
                                                            "%s.RallyPoint"
                                                            % (game_entity_name)))

            if line.is_harvestable():
                disabled_forward_refs.append(ForwardRef(line,
                                                        "%s.Harvestable"
                                                        % (game_entity_name)))

            if line.is_dropsite():
                disabled_forward_refs.append(ForwardRef(line,
                                                        f"{game_entity_name}.DropSite"))

            if line.is_trade_post():
                disabled_forward_refs.append(ForwardRef(line,
                                                        "%s.TradePost"
                                                        % (game_entity_name)))

            init_state_raw_api_object.add_raw_member("disable_abilities",
                                                     disabled_forward_refs,
                                                     "engine.aux.state_machine.StateChanger")

            # Enabled modifiers
            init_state_raw_api_object.add_raw_member("enable_modifiers",
                                                     [],
                                                     "engine.aux.state_machine.StateChanger")

            # Disabled modifiers
            init_state_raw_api_object.add_raw_member("disable_modifiers",
                                                     [],
                                                     "engine.aux.state_machine.StateChanger")

            line.add_raw_api_object(init_state_raw_api_object)
            # =====================================================================================
            init_state_forward_ref = ForwardRef(line, init_state_name)
            progress_raw_api_object.add_raw_member("state_change",
                                                   init_state_forward_ref,
                                                   "engine.aux.progress.specialization.StateChangeProgress")
            # =====================================================================================
            progress_forward_refs.append(ForwardRef(line, progress_name))
            line.add_raw_api_object(progress_raw_api_object)
            # =====================================================================================
            progress_name = f"{game_entity_name}.Constructable.ConstructionProgress33"
            progress_raw_api_object = RawAPIObject(progress_name,
                                                   "ConstructionProgress33",
                                                   dataset.nyan_api_objects)
            progress_raw_api_object.add_raw_parent("engine.aux.progress.type.ConstructionProgress")
            progress_location = ForwardRef(line, ability_ref)
            progress_raw_api_object.set_location(progress_location)

            # Interval = (0.0, 33.0)
            progress_raw_api_object.add_raw_member("left_boundary",
                                                   0.0,
                                                   "engine.aux.progress.Progress")
            progress_raw_api_object.add_raw_member("right_boundary",
                                                   33.0,
                                                   "engine.aux.progress.Progress")

            progress_raw_api_object.add_raw_parent("engine.aux.progress.specialization.TerrainOverlayProgress")

            # Terrain overlay
            terrain_ref = "FarmConstruction1"
            terrain_group = dataset.terrain_groups[29]
            terrain_forward_ref = ForwardRef(terrain_group, terrain_ref)
            progress_raw_api_object.add_raw_member("terrain_overlay",
                                                   terrain_forward_ref,
                                                   "engine.aux.progress.specialization.TerrainOverlayProgress")

            progress_raw_api_object.add_raw_parent("engine.aux.progress.specialization.StateChangeProgress")

            # State change
            # =====================================================================================
            construct_state_name = f"{ability_ref}.ConstructState"
            construct_state_raw_api_object = RawAPIObject(construct_state_name,
                                                          "ConstructState",
                                                          dataset.nyan_api_objects)
            construct_state_raw_api_object.add_raw_parent("engine.aux.state_machine.StateChanger")
            construct_state_location = ForwardRef(line, ability_ref)
            construct_state_raw_api_object.set_location(construct_state_location)

            # Priority
            construct_state_raw_api_object.add_raw_member("priority",
                                                          1,
                                                          "engine.aux.state_machine.StateChanger")

            # Enabled abilities
            construct_state_raw_api_object.add_raw_member("enable_abilities",
                                                          [],
                                                          "engine.aux.state_machine.StateChanger")

            # Disabled abilities
            disabled_forward_refs = [ForwardRef(line,
                                                "%s.AttributeChangeTracker"
                                                % (game_entity_name))]
            if len(line.creates) > 0:
                disabled_forward_refs.append(ForwardRef(line,
                                                        f"{game_entity_name}.Create"))
                disabled_forward_refs.append(ForwardRef(line,
                                                        "%s.ProductionQueue"
                                                        % (game_entity_name)))
            if len(line.researches) > 0:
                disabled_forward_refs.append(ForwardRef(line,
                                                        f"{game_entity_name}.Research"))

            if line.is_projectile_shooter():
                disabled_forward_refs.append(ForwardRef(line,
                                                        f"{game_entity_name}.Attack"))

            if line.is_garrison():
                disabled_forward_refs.append(ForwardRef(line,
                                                        f"{game_entity_name}.Storage"))
                disabled_forward_refs.append(ForwardRef(line,
                                                        "%s.RemoveStorage"
                                                        % (game_entity_name)))

                garrison_mode = line.get_garrison_mode()

                if garrison_mode == GenieGarrisonMode.NATURAL:
                    disabled_forward_refs.append(ForwardRef(line,
                                                            "%s.SendBackToTask"
                                                            % (game_entity_name)))

                if garrison_mode in (GenieGarrisonMode.NATURAL, GenieGarrisonMode.SELF_PRODUCED):
                    disabled_forward_refs.append(ForwardRef(line,
                                                            "%s.RallyPoint"
                                                            % (game_entity_name)))

            if line.is_harvestable():
                disabled_forward_refs.append(ForwardRef(line,
                                                        "%s.Harvestable"
                                                        % (game_entity_name)))

            if line.is_dropsite():
                disabled_forward_refs.append(ForwardRef(line,
                                                        f"{game_entity_name}.DropSite"))

            if line.is_trade_post():
                disabled_forward_refs.append(ForwardRef(line,
                                                        "%s.TradePost"
                                                        % (game_entity_name)))

            construct_state_raw_api_object.add_raw_member("disable_abilities",
                                                          disabled_forward_refs,
                                                          "engine.aux.state_machine.StateChanger")

            # Enabled modifiers
            construct_state_raw_api_object.add_raw_member("enable_modifiers",
                                                          [],
                                                          "engine.aux.state_machine.StateChanger")

            # Disabled modifiers
            construct_state_raw_api_object.add_raw_member("disable_modifiers",
                                                          [],
                                                          "engine.aux.state_machine.StateChanger")

            line.add_raw_api_object(construct_state_raw_api_object)
            # =====================================================================================
            construct_state_forward_ref = ForwardRef(line, construct_state_name)
            progress_raw_api_object.add_raw_member("state_change",
                                                   construct_state_forward_ref,
                                                   "engine.aux.progress.specialization.StateChangeProgress")
            # =====================================================================================
            progress_forward_refs.append(ForwardRef(line, progress_name))
            line.add_raw_api_object(progress_raw_api_object)
            # =====================================================================================
            progress_name = f"{game_entity_name}.Constructable.ConstructionProgress66"
            progress_raw_api_object = RawAPIObject(progress_name,
                                                   "ConstructionProgress66",
                                                   dataset.nyan_api_objects)
            progress_raw_api_object.add_raw_parent("engine.aux.progress.type.ConstructionProgress")
            progress_location = ForwardRef(line, ability_ref)
            progress_raw_api_object.set_location(progress_location)

            # Interval = (33.0, 66.0)
            progress_raw_api_object.add_raw_member("left_boundary",
                                                   33.0,
                                                   "engine.aux.progress.Progress")
            progress_raw_api_object.add_raw_member("right_boundary",
                                                   66.0,
                                                   "engine.aux.progress.Progress")

            progress_raw_api_object.add_raw_parent("engine.aux.progress.specialization.TerrainOverlayProgress")

            # Terrain overlay
            terrain_ref = "FarmConstruction2"
            terrain_group = dataset.terrain_groups[30]
            terrain_forward_ref = ForwardRef(terrain_group, terrain_ref)
            progress_raw_api_object.add_raw_member("terrain_overlay",
                                                   terrain_forward_ref,
                                                   "engine.aux.progress.specialization.TerrainOverlayProgress")

            # State change
            progress_raw_api_object.add_raw_parent("engine.aux.progress.specialization.StateChangeProgress")
            progress_raw_api_object.add_raw_member("state_change",
                                                   construct_state_forward_ref,
                                                   "engine.aux.progress.specialization.StateChangeProgress")
            # =====================================================================================
            progress_forward_refs.append(ForwardRef(line, progress_name))
            line.add_raw_api_object(progress_raw_api_object)
            # =====================================================================================
            progress_name = f"{game_entity_name}.Constructable.ConstructionProgress100"
            progress_raw_api_object = RawAPIObject(progress_name,
                                                   "ConstructionProgress100",
                                                   dataset.nyan_api_objects)
            progress_raw_api_object.add_raw_parent("engine.aux.progress.type.ConstructionProgress")
            progress_location = ForwardRef(line, ability_ref)
            progress_raw_api_object.set_location(progress_location)

            # Interval = (66.0, 100.0)
            progress_raw_api_object.add_raw_member("left_boundary",
                                                   66.0,
                                                   "engine.aux.progress.Progress")
            progress_raw_api_object.add_raw_member("right_boundary",
                                                   100.0,
                                                   "engine.aux.progress.Progress")

            progress_raw_api_object.add_raw_parent("engine.aux.progress.specialization.TerrainOverlayProgress")

            # Terrain overlay
            terrain_ref = "FarmConstruction3"
            terrain_group = dataset.terrain_groups[31]
            terrain_forward_ref = ForwardRef(terrain_group, terrain_ref)
            progress_raw_api_object.add_raw_member("terrain_overlay",
                                                   terrain_forward_ref,
                                                   "engine.aux.progress.specialization.TerrainOverlayProgress")

            # State change
            progress_raw_api_object.add_raw_parent("engine.aux.progress.specialization.StateChangeProgress")
            progress_raw_api_object.add_raw_member("state_change",
                                                   construct_state_forward_ref,
                                                   "engine.aux.progress.specialization.StateChangeProgress")
            # =====================================================================================
            progress_forward_refs.append(ForwardRef(line, progress_name))
            line.add_raw_api_object(progress_raw_api_object)

        else:
            progress_name = f"{game_entity_name}.Constructable.ConstructionProgress0"
            progress_raw_api_object = RawAPIObject(progress_name,
                                                   "ConstructionProgress0",
                                                   dataset.nyan_api_objects)
            progress_raw_api_object.add_raw_parent("engine.aux.progress.type.ConstructionProgress")
            progress_location = ForwardRef(line, ability_ref)
            progress_raw_api_object.set_location(progress_location)

            # Interval = (0.0, 0.0)
            progress_raw_api_object.add_raw_member("left_boundary",
                                                   0.0,
                                                   "engine.aux.progress.Progress")
            progress_raw_api_object.add_raw_member("right_boundary",
                                                   0.0,
                                                   "engine.aux.progress.Progress")

            if construction_animation_id > -1:
                # =================================================================================
                # Idle override
                # =================================================================================
                progress_raw_api_object.add_raw_parent("engine.aux.progress.specialization.AnimatedProgress")

                overrides = []
                override_ref = f"{game_entity_name}.Constructable.ConstructionProgress0.IdleOverride"
                override_raw_api_object = RawAPIObject(override_ref,
                                                       "IdleOverride",
                                                       dataset.nyan_api_objects)
                override_raw_api_object.add_raw_parent("engine.aux.animation_override.AnimationOverride")
                override_location = ForwardRef(line, progress_name)
                override_raw_api_object.set_location(override_location)

                idle_forward_ref = ForwardRef(line, f"{game_entity_name}.Idle")
                override_raw_api_object.add_raw_member("ability",
                                                       idle_forward_ref,
                                                       "engine.aux.animation_override.AnimationOverride")

                # Animation
                animations_set = []
                animation_forward_ref = AoCAbilitySubprocessor.create_animation(line,
                                                                                construction_animation_id,
                                                                                override_ref,
                                                                                "Idle",
                                                                                "idle_construct0_override_")

                animations_set.append(animation_forward_ref)
                override_raw_api_object.add_raw_member("animations",
                                                       animations_set,
                                                       "engine.aux.animation_override.AnimationOverride")

                override_raw_api_object.add_raw_member("priority",
                                                       1,
                                                       "engine.aux.animation_override.AnimationOverride")

                override_forward_ref = ForwardRef(line, override_ref)
                overrides.append(override_forward_ref)
                line.add_raw_api_object(override_raw_api_object)
                # =================================================================================
                progress_raw_api_object.add_raw_member("overrides",
                                                       overrides,
                                                       "engine.aux.progress.specialization.AnimatedProgress")

            progress_raw_api_object.add_raw_parent("engine.aux.progress.specialization.StateChangeProgress")

            # State change
            # =====================================================================================
            init_state_name = f"{ability_ref}.InitState"
            init_state_raw_api_object = RawAPIObject(init_state_name,
                                                     "InitState",
                                                     dataset.nyan_api_objects)
            init_state_raw_api_object.add_raw_parent("engine.aux.state_machine.StateChanger")
            init_state_location = ForwardRef(line, ability_ref)
            init_state_raw_api_object.set_location(init_state_location)

            # Priority
            init_state_raw_api_object.add_raw_member("priority",
                                                     1,
                                                     "engine.aux.state_machine.StateChanger")

            # Enabled abilities
            enabled_forward_refs = [
                ForwardRef(line,
                           f"{game_entity_name}.VisibilityConstruct0")
            ]
            init_state_raw_api_object.add_raw_member("enable_abilities",
                                                     enabled_forward_refs,
                                                     "engine.aux.state_machine.StateChanger")

            # Disabled abilities
            disabled_forward_refs = [
                ForwardRef(line,
                           f"{game_entity_name}.AttributeChangeTracker"),
                ForwardRef(line,
                           f"{game_entity_name}.LineOfSight"),
                ForwardRef(line,
                           f"{game_entity_name}.Visibility")
            ]
            if len(line.creates) > 0:
                disabled_forward_refs.append(ForwardRef(line,
                                                        f"{game_entity_name}.Create"))
                disabled_forward_refs.append(ForwardRef(line,
                                                        "%s.ProductionQueue"
                                                        % (game_entity_name)))
            if len(line.researches) > 0:
                disabled_forward_refs.append(ForwardRef(line,
                                                        f"{game_entity_name}.Research"))

            if line.is_projectile_shooter():
                disabled_forward_refs.append(ForwardRef(line,
                                                        f"{game_entity_name}.Attack"))

            if line.is_garrison():
                disabled_forward_refs.append(ForwardRef(line,
                                                        f"{game_entity_name}.Storage"))
                disabled_forward_refs.append(ForwardRef(line,
                                                        "%s.RemoveStorage"
                                                        % (game_entity_name)))

                garrison_mode = line.get_garrison_mode()

                if garrison_mode == GenieGarrisonMode.NATURAL:
                    disabled_forward_refs.append(ForwardRef(line,
                                                            "%s.SendBackToTask"
                                                            % (game_entity_name)))

                if garrison_mode in (GenieGarrisonMode.NATURAL, GenieGarrisonMode.SELF_PRODUCED):
                    disabled_forward_refs.append(ForwardRef(line,
                                                            "%s.RallyPoint"
                                                            % (game_entity_name)))

            if line.is_harvestable():
                disabled_forward_refs.append(ForwardRef(line,
                                                        "%s.Harvestable"
                                                        % (game_entity_name)))

            if line.is_dropsite():
                disabled_forward_refs.append(ForwardRef(line,
                                                        f"{game_entity_name}.DropSite"))

            if line.is_trade_post():
                disabled_forward_refs.append(ForwardRef(line,
                                                        "%s.TradePost"
                                                        % (game_entity_name)))

            init_state_raw_api_object.add_raw_member("disable_abilities",
                                                     disabled_forward_refs,
                                                     "engine.aux.state_machine.StateChanger")

            # Enabled modifiers
            init_state_raw_api_object.add_raw_member("enable_modifiers",
                                                     [],
                                                     "engine.aux.state_machine.StateChanger")

            # Disabled modifiers
            init_state_raw_api_object.add_raw_member("disable_modifiers",
                                                     [],
                                                     "engine.aux.state_machine.StateChanger")

            line.add_raw_api_object(init_state_raw_api_object)
            # =====================================================================================
            init_state_forward_ref = ForwardRef(line, init_state_name)
            progress_raw_api_object.add_raw_member("state_change",
                                                   init_state_forward_ref,
                                                   "engine.aux.progress.specialization.StateChangeProgress")
            # =====================================================================================
            progress_forward_refs.append(ForwardRef(line, progress_name))
            line.add_raw_api_object(progress_raw_api_object)
            # =====================================================================================
            progress_name = f"{game_entity_name}.Constructable.ConstructionProgress25"
            progress_raw_api_object = RawAPIObject(progress_name,
                                                   "ConstructionProgress25",
                                                   dataset.nyan_api_objects)
            progress_raw_api_object.add_raw_parent("engine.aux.progress.type.ConstructionProgress")
            progress_location = ForwardRef(line, ability_ref)
            progress_raw_api_object.set_location(progress_location)

            # Interval = (0.0, 25.0)
            progress_raw_api_object.add_raw_member("left_boundary",
                                                   0.0,
                                                   "engine.aux.progress.Progress")
            progress_raw_api_object.add_raw_member("right_boundary",
                                                   25.0,
                                                   "engine.aux.progress.Progress")

            if construction_animation_id > -1:
                # =================================================================================
                # Idle override
                # =================================================================================
                progress_raw_api_object.add_raw_parent("engine.aux.progress.specialization.AnimatedProgress")

                overrides = []
                override_ref = f"{game_entity_name}.Constructable.ConstructionProgress25.IdleOverride"
                override_raw_api_object = RawAPIObject(override_ref,
                                                       "IdleOverride",
                                                       dataset.nyan_api_objects)
                override_raw_api_object.add_raw_parent("engine.aux.animation_override.AnimationOverride")
                override_location = ForwardRef(line, progress_name)
                override_raw_api_object.set_location(override_location)

                idle_forward_ref = ForwardRef(line, f"{game_entity_name}.Idle")
                override_raw_api_object.add_raw_member("ability",
                                                       idle_forward_ref,
                                                       "engine.aux.animation_override.AnimationOverride")

                # Animation
                animations_set = []
                animation_forward_ref = AoCAbilitySubprocessor.create_animation(line,
                                                                                construction_animation_id,
                                                                                override_ref,
                                                                                "Idle",
                                                                                "idle_construct25_override_")

                animations_set.append(animation_forward_ref)
                override_raw_api_object.add_raw_member("animations",
                                                       animations_set,
                                                       "engine.aux.animation_override.AnimationOverride")

                override_raw_api_object.add_raw_member("priority",
                                                       1,
                                                       "engine.aux.animation_override.AnimationOverride")

                override_forward_ref = ForwardRef(line, override_ref)
                overrides.append(override_forward_ref)
                line.add_raw_api_object(override_raw_api_object)
                # =================================================================================
                progress_raw_api_object.add_raw_member("overrides",
                                                       overrides,
                                                       "engine.aux.progress.specialization.AnimatedProgress")

            progress_raw_api_object.add_raw_parent("engine.aux.progress.specialization.StateChangeProgress")

            # State change
            # =====================================================================================
            construct_state_name = f"{ability_ref}.ConstructState"
            construct_state_raw_api_object = RawAPIObject(construct_state_name,
                                                          "ConstructState",
                                                          dataset.nyan_api_objects)
            construct_state_raw_api_object.add_raw_parent("engine.aux.state_machine.StateChanger")
            construct_state_location = ForwardRef(line, ability_ref)
            construct_state_raw_api_object.set_location(construct_state_location)

            # Priority
            construct_state_raw_api_object.add_raw_member("priority",
                                                          1,
                                                          "engine.aux.state_machine.StateChanger")

            # Enabled abilities
            construct_state_raw_api_object.add_raw_member("enable_abilities",
                                                          [],
                                                          "engine.aux.state_machine.StateChanger")

            # Disabled abilities
            disabled_forward_refs = [ForwardRef(line,
                                                "%s.AttributeChangeTracker"
                                                % (game_entity_name))]
            if len(line.creates) > 0:
                disabled_forward_refs.append(ForwardRef(line,
                                                        f"{game_entity_name}.Create"))
                disabled_forward_refs.append(ForwardRef(line,
                                                        "%s.ProductionQueue"
                                                        % (game_entity_name)))
            if len(line.researches) > 0:
                disabled_forward_refs.append(ForwardRef(line,
                                                        f"{game_entity_name}.Research"))

            if line.is_projectile_shooter():
                disabled_forward_refs.append(ForwardRef(line,
                                                        f"{game_entity_name}.Attack"))

            if line.is_garrison():
                disabled_forward_refs.append(ForwardRef(line,
                                                        f"{game_entity_name}.Storage"))
                disabled_forward_refs.append(ForwardRef(line,
                                                        "%s.RemoveStorage"
                                                        % (game_entity_name)))

                garrison_mode = line.get_garrison_mode()

                if garrison_mode == GenieGarrisonMode.NATURAL:
                    disabled_forward_refs.append(ForwardRef(line,
                                                            "%s.SendBackToTask"
                                                            % (game_entity_name)))

                if garrison_mode in (GenieGarrisonMode.NATURAL, GenieGarrisonMode.SELF_PRODUCED):
                    disabled_forward_refs.append(ForwardRef(line,
                                                            "%s.RallyPoint"
                                                            % (game_entity_name)))

            if line.is_harvestable():
                disabled_forward_refs.append(ForwardRef(line,
                                                        "%s.Harvestable"
                                                        % (game_entity_name)))

            if line.is_dropsite():
                disabled_forward_refs.append(ForwardRef(line,
                                                        f"{game_entity_name}.DropSite"))

            if line.is_trade_post():
                disabled_forward_refs.append(ForwardRef(line,
                                                        "%s.TradePost"
                                                        % (game_entity_name)))

            construct_state_raw_api_object.add_raw_member("disable_abilities",
                                                          disabled_forward_refs,
                                                          "engine.aux.state_machine.StateChanger")

            # Enabled modifiers
            construct_state_raw_api_object.add_raw_member("enable_modifiers",
                                                          [],
                                                          "engine.aux.state_machine.StateChanger")

            # Disabled modifiers
            construct_state_raw_api_object.add_raw_member("disable_modifiers",
                                                          [],
                                                          "engine.aux.state_machine.StateChanger")

            line.add_raw_api_object(construct_state_raw_api_object)
            # =====================================================================================
            construct_state_forward_ref = ForwardRef(line, construct_state_name)
            progress_raw_api_object.add_raw_member("state_change",
                                                   construct_state_forward_ref,
                                                   "engine.aux.progress.specialization.StateChangeProgress")
            # =====================================================================================
            progress_forward_refs.append(ForwardRef(line, progress_name))
            line.add_raw_api_object(progress_raw_api_object)
            # =====================================================================================
            progress_name = f"{game_entity_name}.Constructable.ConstructionProgress50"
            progress_raw_api_object = RawAPIObject(progress_name,
                                                   "ConstructionProgress50",
                                                   dataset.nyan_api_objects)
            progress_raw_api_object.add_raw_parent("engine.aux.progress.type.ConstructionProgress")
            progress_location = ForwardRef(line, ability_ref)
            progress_raw_api_object.set_location(progress_location)

            # Interval = (25.0, 50.0)
            progress_raw_api_object.add_raw_member("left_boundary",
                                                   25.0,
                                                   "engine.aux.progress.Progress")
            progress_raw_api_object.add_raw_member("right_boundary",
                                                   50.0,
                                                   "engine.aux.progress.Progress")

            if construction_animation_id > -1:
                # =================================================================================
                # Idle override
                # =================================================================================
                progress_raw_api_object.add_raw_parent("engine.aux.progress.specialization.AnimatedProgress")

                overrides = []
                override_ref = f"{game_entity_name}.Constructable.ConstructionProgress50.IdleOverride"
                override_raw_api_object = RawAPIObject(override_ref,
                                                       "IdleOverride",
                                                       dataset.nyan_api_objects)
                override_raw_api_object.add_raw_parent("engine.aux.animation_override.AnimationOverride")
                override_location = ForwardRef(line, progress_name)
                override_raw_api_object.set_location(override_location)

                idle_forward_ref = ForwardRef(line, f"{game_entity_name}.Idle")
                override_raw_api_object.add_raw_member("ability",
                                                       idle_forward_ref,
                                                       "engine.aux.animation_override.AnimationOverride")

                # Animation
                animations_set = []
                animation_forward_ref = AoCAbilitySubprocessor.create_animation(line,
                                                                                construction_animation_id,
                                                                                override_ref,
                                                                                "Idle",
                                                                                "idle_construct50_override_")

                animations_set.append(animation_forward_ref)
                override_raw_api_object.add_raw_member("animations",
                                                       animations_set,
                                                       "engine.aux.animation_override.AnimationOverride")

                override_raw_api_object.add_raw_member("priority",
                                                       1,
                                                       "engine.aux.animation_override.AnimationOverride")

                override_forward_ref = ForwardRef(line, override_ref)
                overrides.append(override_forward_ref)
                line.add_raw_api_object(override_raw_api_object)
                # =================================================================================
                progress_raw_api_object.add_raw_member("overrides",
                                                       overrides,
                                                       "engine.aux.progress.specialization.AnimatedProgress")

            # State change
            progress_raw_api_object.add_raw_parent("engine.aux.progress.specialization.StateChangeProgress")
            progress_raw_api_object.add_raw_member("state_change",
                                                   construct_state_forward_ref,
                                                   "engine.aux.progress.specialization.StateChangeProgress")
            # =====================================================================================
            progress_forward_refs.append(ForwardRef(line, progress_name))
            line.add_raw_api_object(progress_raw_api_object)
            # =====================================================================================
            progress_name = f"{game_entity_name}.Constructable.ConstructionProgress75"
            progress_raw_api_object = RawAPIObject(progress_name,
                                                   "ConstructionProgress75",
                                                   dataset.nyan_api_objects)
            progress_raw_api_object.add_raw_parent("engine.aux.progress.type.ConstructionProgress")
            progress_location = ForwardRef(line, ability_ref)
            progress_raw_api_object.set_location(progress_location)

            # Interval = (50.0, 75.0)
            progress_raw_api_object.add_raw_member("left_boundary",
                                                   50.0,
                                                   "engine.aux.progress.Progress")
            progress_raw_api_object.add_raw_member("right_boundary",
                                                   75.0,
                                                   "engine.aux.progress.Progress")

            if construction_animation_id > -1:
                # =================================================================================
                # Idle override
                # =================================================================================
                progress_raw_api_object.add_raw_parent("engine.aux.progress.specialization.AnimatedProgress")

                overrides = []
                override_ref = f"{game_entity_name}.Constructable.ConstructionProgress75.IdleOverride"
                override_raw_api_object = RawAPIObject(override_ref,
                                                       "IdleOverride",
                                                       dataset.nyan_api_objects)
                override_raw_api_object.add_raw_parent("engine.aux.animation_override.AnimationOverride")
                override_location = ForwardRef(line, progress_name)
                override_raw_api_object.set_location(override_location)

                idle_forward_ref = ForwardRef(line, f"{game_entity_name}.Idle")
                override_raw_api_object.add_raw_member("ability",
                                                       idle_forward_ref,
                                                       "engine.aux.animation_override.AnimationOverride")

                # Animation
                animations_set = []
                animation_forward_ref = AoCAbilitySubprocessor.create_animation(line,
                                                                                construction_animation_id,
                                                                                override_ref,
                                                                                "Idle",
                                                                                "idle_construct75_override_")

                animations_set.append(animation_forward_ref)
                override_raw_api_object.add_raw_member("animations",
                                                       animations_set,
                                                       "engine.aux.animation_override.AnimationOverride")

                override_raw_api_object.add_raw_member("priority",
                                                       1,
                                                       "engine.aux.animation_override.AnimationOverride")

                override_forward_ref = ForwardRef(line, override_ref)
                overrides.append(override_forward_ref)
                line.add_raw_api_object(override_raw_api_object)
                # =================================================================================
                progress_raw_api_object.add_raw_member("overrides",
                                                       overrides,
                                                       "engine.aux.progress.specialization.AnimatedProgress")

            # State change
            progress_raw_api_object.add_raw_parent("engine.aux.progress.specialization.StateChangeProgress")
            progress_raw_api_object.add_raw_member("state_change",
                                                   construct_state_forward_ref,
                                                   "engine.aux.progress.specialization.StateChangeProgress")
            # =====================================================================================
            progress_forward_refs.append(ForwardRef(line, progress_name))
            line.add_raw_api_object(progress_raw_api_object)
            # =====================================================================================
            progress_name = f"{game_entity_name}.Constructable.ConstructionProgress100"
            progress_raw_api_object = RawAPIObject(progress_name,
                                                   "ConstructionProgress100",
                                                   dataset.nyan_api_objects)
            progress_raw_api_object.add_raw_parent("engine.aux.progress.type.ConstructionProgress")
            progress_location = ForwardRef(line, ability_ref)
            progress_raw_api_object.set_location(progress_location)

            # Interval = (75.0, 100.0)
            progress_raw_api_object.add_raw_member("left_boundary",
                                                   75.0,
                                                   "engine.aux.progress.Progress")
            progress_raw_api_object.add_raw_member("right_boundary",
                                                   100.0,
                                                   "engine.aux.progress.Progress")

            if construction_animation_id > -1:
                # =================================================================================
                # Idle override
                # =================================================================================
                progress_raw_api_object.add_raw_parent("engine.aux.progress.specialization.AnimatedProgress")

                overrides = []
                override_ref = f"{game_entity_name}.Constructable.ConstructionProgress100.IdleOverride"
                override_raw_api_object = RawAPIObject(override_ref,
                                                       "IdleOverride",
                                                       dataset.nyan_api_objects)
                override_raw_api_object.add_raw_parent("engine.aux.animation_override.AnimationOverride")
                override_location = ForwardRef(line, progress_name)
                override_raw_api_object.set_location(override_location)

                idle_forward_ref = ForwardRef(line, f"{game_entity_name}.Idle")
                override_raw_api_object.add_raw_member("ability",
                                                       idle_forward_ref,
                                                       "engine.aux.animation_override.AnimationOverride")

                # Animation
                animations_set = []
                animation_forward_ref = AoCAbilitySubprocessor.create_animation(line,
                                                                                construction_animation_id,
                                                                                override_ref,
                                                                                "Idle",
                                                                                "idle_construct100_override_")

                animations_set.append(animation_forward_ref)
                override_raw_api_object.add_raw_member("animations",
                                                       animations_set,
                                                       "engine.aux.animation_override.AnimationOverride")

                override_raw_api_object.add_raw_member("priority",
                                                       1,
                                                       "engine.aux.animation_override.AnimationOverride")

                override_forward_ref = ForwardRef(line, override_ref)
                overrides.append(override_forward_ref)
                line.add_raw_api_object(override_raw_api_object)
                # =================================================================================
                progress_raw_api_object.add_raw_member("overrides",
                                                       overrides,
                                                       "engine.aux.progress.specialization.AnimatedProgress")

            # State change
            progress_raw_api_object.add_raw_parent("engine.aux.progress.specialization.StateChangeProgress")
            progress_raw_api_object.add_raw_member("state_change",
                                                   construct_state_forward_ref,
                                                   "engine.aux.progress.specialization.StateChangeProgress")
            # =====================================================================================
            progress_forward_refs.append(ForwardRef(line, progress_name))
            line.add_raw_api_object(progress_raw_api_object)
        # =====================================================================================
        ability_raw_api_object.add_raw_member("construction_progress",
                                              progress_forward_refs,
                                              "engine.ability.type.Constructable")

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def create_ability(line):
        """
        Adds the Create ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]
        ability_ref = f"{game_entity_name}.Create"
        ability_raw_api_object = RawAPIObject(ability_ref, "Create", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Create")
        ability_location = ForwardRef(line, game_entity_name)
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
            creatable_name = name_lookup_dict[creatable_id][0]

            raw_api_object_ref = f"{creatable_name}.CreatableGameEntity"
            creatable_forward_ref = ForwardRef(creatable,
                                               raw_api_object_ref)
            creatables_set.append(creatable_forward_ref)

        ability_raw_api_object.add_raw_member("creatables", creatables_set,
                                              "engine.ability.type.Create")
        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def death_ability(line):
        """
        Adds a PassiveTransformTo ability to a line that is used to make entities die.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        current_unit = line.get_head_unit()
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
        gset_lookup_dict = internal_name_lookups.get_graphic_set_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = f"{game_entity_name}.Death"
        ability_raw_api_object = RawAPIObject(ability_ref, "Death", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.PassiveTransformTo")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        ability_animation_id = current_unit["dying_graphic"].get_value()

        if ability_animation_id > -1:
            # Make the ability animated
            ability_raw_api_object.add_raw_parent("engine.ability.specialization.AnimatedAbility")

            animations_set = []
            animation_forward_ref = AoCAbilitySubprocessor.create_animation(line,
                                                                            ability_animation_id,
                                                                            ability_ref,
                                                                            "Death",
                                                                            "death_")
            animations_set.append(animation_forward_ref)
            ability_raw_api_object.add_raw_member("animations", animations_set,
                                                  "engine.ability.specialization.AnimatedAbility")

            # Create custom civ graphics
            handled_graphics_set_ids = set()
            for civ_group in dataset.civ_groups.values():
                civ = civ_group.civ
                civ_id = civ_group.get_id()

                # Only proceed if the civ stores the unit in the line
                if current_unit_id not in civ["units"].get_value().keys():
                    continue

                civ_animation_id = civ["units"][current_unit_id]["dying_graphic"].get_value()

                if civ_animation_id != ability_animation_id:
                    # Find the corresponding graphics set
                    graphics_set_id = -1
                    for set_id, items in gset_lookup_dict.items():
                        if civ_id in items[0]:
                            graphics_set_id = set_id
                            break

                    # Check if the object for the animation has been created before
                    obj_exists = graphics_set_id in handled_graphics_set_ids
                    if not obj_exists:
                        handled_graphics_set_ids.add(graphics_set_id)

                    obj_prefix = f"{gset_lookup_dict[graphics_set_id][1]}Death"
                    filename_prefix = f"death_{gset_lookup_dict[graphics_set_id][2]}_"
                    AoCAbilitySubprocessor.create_civ_animation(line,
                                                                civ_group,
                                                                civ_animation_id,
                                                                ability_ref,
                                                                obj_prefix,
                                                                filename_prefix,
                                                                obj_exists)

        # Death condition
        death_condition = [
            dataset.pregen_nyan_objects["aux.logic.literal.death.StandardHealthDeathLiteral"].get_nyan_object()
        ]
        ability_raw_api_object.add_raw_member("condition",
                                              death_condition,
                                              "engine.ability.type.PassiveTransformTo")

        # Transform time
        # Use the time of the dying graphics
        if ability_animation_id > -1:
            dying_animation = dataset.genie_graphics[ability_animation_id]
            death_time = dying_animation.get_animation_length()

        else:
            death_time = 0.0

        ability_raw_api_object.add_raw_member("transform_time",
                                              death_time,
                                              "engine.ability.type.PassiveTransformTo")

        # Target state
        # =====================================================================================
        target_state_name = f"{game_entity_name}.Death.DeadState"
        target_state_raw_api_object = RawAPIObject(target_state_name,
                                                   "DeadState",
                                                   dataset.nyan_api_objects)
        target_state_raw_api_object.add_raw_parent("engine.aux.state_machine.StateChanger")
        target_state_location = ForwardRef(line, ability_ref)
        target_state_raw_api_object.set_location(target_state_location)

        # Priority
        target_state_raw_api_object.add_raw_member("priority",
                                                   1000,
                                                   "engine.aux.state_machine.StateChanger")

        # Enabled abilities
        target_state_raw_api_object.add_raw_member("enable_abilities",
                                                   [],
                                                   "engine.aux.state_machine.StateChanger")

        # Disabled abilities
        disabled_forward_refs = []
        if isinstance(line, (GenieUnitLineGroup, GenieBuildingLineGroup)):
            disabled_forward_refs.append(ForwardRef(line,
                                                    f"{game_entity_name}.LineOfSight"))

        if isinstance(line, GenieBuildingLineGroup):
            disabled_forward_refs.append(ForwardRef(line,
                                                    "%s.AttributeChangeTracker"
                                                    % (game_entity_name)))

        if len(line.creates) > 0:
            disabled_forward_refs.append(ForwardRef(line,
                                                    f"{game_entity_name}.Create"))

            if isinstance(line, GenieBuildingLineGroup):
                disabled_forward_refs.append(ForwardRef(line,
                                                        "%s.ProductionQueue"
                                                        % (game_entity_name)))
        if len(line.researches) > 0:
            disabled_forward_refs.append(ForwardRef(line,
                                                    f"{game_entity_name}.Research"))

        if line.is_projectile_shooter():
            disabled_forward_refs.append(ForwardRef(line,
                                                    f"{game_entity_name}.Attack"))

        if line.is_garrison():
            disabled_forward_refs.append(ForwardRef(line,
                                                    f"{game_entity_name}.Storage"))
            disabled_forward_refs.append(ForwardRef(line,
                                                    "%s.RemoveStorage"
                                                    % (game_entity_name)))

            garrison_mode = line.get_garrison_mode()

            if garrison_mode == GenieGarrisonMode.NATURAL:
                disabled_forward_refs.append(ForwardRef(line,
                                                        "%s.SendBackToTask"
                                                        % (game_entity_name)))

            if garrison_mode in (GenieGarrisonMode.NATURAL, GenieGarrisonMode.SELF_PRODUCED):
                disabled_forward_refs.append(ForwardRef(line,
                                                        "%s.RallyPoint"
                                                        % (game_entity_name)))

        if line.is_harvestable():
            disabled_forward_refs.append(ForwardRef(line,
                                                    f"{game_entity_name}.Harvestable"))

        if isinstance(line, GenieBuildingLineGroup) and line.is_dropsite():
            disabled_forward_refs.append(ForwardRef(line,
                                                    f"{game_entity_name}.DropSite"))

        if isinstance(line, GenieBuildingLineGroup) and line.is_trade_post():
            disabled_forward_refs.append(ForwardRef(line,
                                                    f"{game_entity_name}.TradePost"))

        target_state_raw_api_object.add_raw_member("disable_abilities",
                                                   disabled_forward_refs,
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
        target_state_forward_ref = ForwardRef(line, target_state_name)
        ability_raw_api_object.add_raw_member("target_state",
                                              target_state_forward_ref,
                                              "engine.ability.type.PassiveTransformTo")

        # Transform progress
        # =====================================================================================
        progress_name = f"{game_entity_name}.Death.DeathProgress"
        progress_raw_api_object = RawAPIObject(progress_name,
                                               "DeathProgress",
                                               dataset.nyan_api_objects)
        progress_raw_api_object.add_raw_parent("engine.aux.progress.type.TransformProgress")
        progress_location = ForwardRef(line, ability_ref)
        progress_raw_api_object.set_location(progress_location)

        # Interval = (0.0, 100.0)
        progress_raw_api_object.add_raw_member("left_boundary",
                                               0.0,
                                               "engine.aux.progress.Progress")
        progress_raw_api_object.add_raw_member("right_boundary",
                                               100.0,
                                               "engine.aux.progress.Progress")

        progress_raw_api_object.add_raw_parent("engine.aux.progress.specialization.StateChangeProgress")
        progress_raw_api_object.add_raw_member("state_change",
                                               target_state_forward_ref,
                                               "engine.aux.progress.specialization.StateChangeProgress")

        line.add_raw_api_object(progress_raw_api_object)
        # =====================================================================================
        progress_forward_ref = ForwardRef(line, progress_name)
        ability_raw_api_object.add_raw_member("transform_progress",
                                              [progress_forward_ref],
                                              "engine.ability.type.PassiveTransformTo")

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def delete_ability(line):
        """
        Adds a PassiveTransformTo ability to a line that is used to make entities die.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        current_unit = line.get_head_unit()
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = f"{game_entity_name}.Delete"
        ability_raw_api_object = RawAPIObject(ability_ref, "Delete", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.ActiveTransformTo")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        ability_animation_id = current_unit["dying_graphic"].get_value()

        if ability_animation_id > -1:
            # Use the animation from Death ability
            ability_raw_api_object.add_raw_parent("engine.ability.specialization.AnimatedAbility")

            animations_set = []
            animation_ref = f"{game_entity_name}.Death.DeathAnimation"
            animation_forward_ref = ForwardRef(line, animation_ref)
            animations_set.append(animation_forward_ref)
            ability_raw_api_object.add_raw_member("animations", animations_set,
                                                  "engine.ability.specialization.AnimatedAbility")

        # Transform time
        # Use the time of the dying graphics
        if ability_animation_id > -1:
            dying_animation = dataset.genie_graphics[ability_animation_id]
            death_time = dying_animation.get_animation_length()

        else:
            death_time = 0.0

        ability_raw_api_object.add_raw_member("transform_time",
                                              death_time,
                                              "engine.ability.type.ActiveTransformTo")

        # Target state (reuse from Death)
        target_state_ref = f"{game_entity_name}.Death.DeadState"
        target_state_forward_ref = ForwardRef(line, target_state_ref)
        ability_raw_api_object.add_raw_member("target_state",
                                              target_state_forward_ref,
                                              "engine.ability.type.ActiveTransformTo")

        # Transform progress (reuse from Death)
        progress_ref = f"{game_entity_name}.Death.DeathProgress"
        progress_forward_ref = ForwardRef(line, progress_ref)
        ability_raw_api_object.add_raw_member("transform_progress",
                                              [progress_forward_ref],
                                              "engine.ability.type.ActiveTransformTo")

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def despawn_ability(line):
        """
        Adds the Despawn ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        current_unit = line.get_head_unit()
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        # Animation and time come from dead unit
        death_animation_id = current_unit["dying_graphic"].get_value()
        dead_unit_id = current_unit["dead_unit_id"].get_value()
        dead_unit = None
        if dead_unit_id > -1:
            dead_unit = dataset.genie_units[dead_unit_id]

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
        gset_lookup_dict = internal_name_lookups.get_graphic_set_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = f"{game_entity_name}.Despawn"
        ability_raw_api_object = RawAPIObject(ability_ref, "Despawn", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Despawn")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        ability_animation_id = -1
        if dead_unit:
            ability_animation_id = dead_unit["idle_graphic0"].get_value()

        if ability_animation_id > -1:
            # Make the ability animated
            ability_raw_api_object.add_raw_parent("engine.ability.specialization.AnimatedAbility")

            animations_set = []
            animation_forward_ref = AoCAbilitySubprocessor.create_animation(line,
                                                                            ability_animation_id,
                                                                            ability_ref,
                                                                            "Despawn",
                                                                            "despawn_")
            animations_set.append(animation_forward_ref)
            ability_raw_api_object.add_raw_member("animations",
                                                  animations_set,
                                                  "engine.ability.specialization.AnimatedAbility")

            # Create custom civ graphics
            handled_graphics_set_ids = set()
            for civ_group in dataset.civ_groups.values():
                civ = civ_group.civ
                civ_id = civ_group.get_id()

                # Only proceed if the civ stores the unit in the line
                if current_unit_id not in civ["units"].get_value().keys():
                    continue

                civ_unit = civ["units"][current_unit_id]
                civ_dead_unit_id = civ_unit["dead_unit_id"].get_value()
                civ_dead_unit = None
                if civ_dead_unit_id > -1:
                    civ_dead_unit = dataset.genie_units[civ_dead_unit_id]

                civ_animation_id = civ_dead_unit["idle_graphic0"].get_value()

                if civ_animation_id != ability_animation_id:
                    # Find the corresponding graphics set
                    graphics_set_id = -1
                    for set_id, items in gset_lookup_dict.items():
                        if civ_id in items[0]:
                            graphics_set_id = set_id
                            break

                    # Check if the object for the animation has been created before
                    obj_exists = graphics_set_id in handled_graphics_set_ids
                    if not obj_exists:
                        handled_graphics_set_ids.add(graphics_set_id)

                    obj_prefix = f"{gset_lookup_dict[graphics_set_id][1]}Despawn"
                    filename_prefix = f"despawn_{gset_lookup_dict[graphics_set_id][2]}_"
                    AoCAbilitySubprocessor.create_civ_animation(line,
                                                                civ_group,
                                                                civ_animation_id,
                                                                ability_ref,
                                                                obj_prefix,
                                                                filename_prefix,
                                                                obj_exists)

        # Activation condition
        # Uses the death condition of the units
        activation_condition = [
            dataset.pregen_nyan_objects["aux.logic.literal.death.StandardHealthDeathLiteral"].get_nyan_object()
        ]
        ability_raw_api_object.add_raw_member("activation_condition",
                                              activation_condition,
                                              "engine.ability.type.Despawn")

        # Despawn condition
        ability_raw_api_object.add_raw_member("despawn_condition",
                                              [],
                                              "engine.ability.type.Despawn")

        # Despawn time = corpse decay time (dead unit) or Death animation time (if no dead unit exist)
        despawn_time = 0
        if dead_unit:
            resource_storage = dead_unit["resource_storage"].get_value()
            for storage in resource_storage:
                resource_id = storage["type"].get_value()

                if resource_id == 12:
                    despawn_time = storage["amount"].get_value()

        elif death_animation_id > -1:
            despawn_time = dataset.genie_graphics[death_animation_id].get_animation_length()

        ability_raw_api_object.add_raw_member("despawn_time",
                                              despawn_time,
                                              "engine.ability.type.Despawn")

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def drop_resources_ability(line):
        """
        Adds the DropResources ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        if isinstance(line, GenieVillagerGroup):
            gatherers = line.variants[0].line

        else:
            gatherers = [line.line[0]]

        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
        gather_lookup_dict = internal_name_lookups.get_gather_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = f"{game_entity_name}.DropResources"
        ability_raw_api_object = RawAPIObject(ability_ref,
                                              "DropResources",
                                              dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.DropResources")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Resource containers
        containers = []
        for gatherer in gatherers:
            unit_commands = gatherer["unit_commands"].get_value()

            for command in unit_commands:
                # Find a gather ability. It doesn't matter which one because
                # they should all produce the same resource for one genie unit.
                type_id = command["type"].get_value()

                if type_id in (5, 110):
                    break

            gatherer_unit_id = gatherer.get_id()
            if gatherer_unit_id not in gather_lookup_dict.keys():
                # Skips hunting wolves
                continue

            container_ref = "%s.ResourceStorage.%sContainer" % (game_entity_name,
                                                                gather_lookup_dict[gatherer_unit_id][0])
            container_forward_ref = ForwardRef(line, container_ref)
            containers.append(container_forward_ref)

        ability_raw_api_object.add_raw_member("containers",
                                              containers,
                                              "engine.ability.type.DropResources")

        # Search range
        ability_raw_api_object.add_raw_member("search_range",
                                              MemberSpecialValue.NYAN_INF,
                                              "engine.ability.type.DropResources")

        # Allowed types
        allowed_types = [
            dataset.pregen_nyan_objects["aux.game_entity_type.types.DropSite"].get_nyan_object()
        ]
        ability_raw_api_object.add_raw_member("allowed_types",
                                              allowed_types,
                                              "engine.ability.type.DropResources")
        # Blacklisted enties
        ability_raw_api_object.add_raw_member("blacklisted_entities",
                                              [],
                                              "engine.ability.type.DropResources")

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def drop_site_ability(line):
        """
        Adds the DropSite ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
        gather_lookup_dict = internal_name_lookups.get_gather_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = f"{game_entity_name}.DropSite"
        ability_raw_api_object = RawAPIObject(ability_ref, "DropSite", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.DropSite")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Resource containers
        gatherer_ids = line.get_gatherer_ids()

        containers = []
        for gatherer_id in gatherer_ids:
            if gatherer_id not in gather_lookup_dict.keys():
                # Skips hunting wolves
                continue

            gatherer_line = dataset.unit_ref[gatherer_id]
            gatherer_head_unit_id = gatherer_line.get_head_unit_id()
            gatherer_name = name_lookup_dict[gatherer_head_unit_id][0]

            container_ref = "%s.ResourceStorage.%sContainer" % (gatherer_name,
                                                                gather_lookup_dict[gatherer_id][0])
            container_forward_ref = ForwardRef(gatherer_line, container_ref)
            containers.append(container_forward_ref)

        ability_raw_api_object.add_raw_member("accepts_from",
                                              containers,
                                              "engine.ability.type.DropSite")

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def enter_container_ability(line):
        """
        Adds the EnterContainer ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability. None if no valid containers were found.
        :rtype: ...dataformat.forward_ref.ForwardRef, None
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = f"{game_entity_name}.EnterContainer"
        ability_raw_api_object = RawAPIObject(ability_ref,
                                              "EnterContainer",
                                              dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.EnterContainer")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Containers
        containers = []
        entity_lookups = internal_name_lookups.get_entity_lookups(dataset.game_version)
        for garrison in line.garrison_locations:
            garrison_mode = garrison.get_garrison_mode()

            # Cannot enter production buildings or monk inventories
            if garrison_mode in (GenieGarrisonMode.SELF_PRODUCED, GenieGarrisonMode.MONK):
                continue

            garrison_name = entity_lookups[garrison.get_head_unit_id()][0]

            container_ref = f"{garrison_name}.Storage.{garrison_name}Container"
            container_forward_ref = ForwardRef(garrison, container_ref)
            containers.append(container_forward_ref)

        if not containers:
            return None

        ability_raw_api_object.add_raw_member("allowed_containers",
                                              containers,
                                              "engine.ability.type.EnterContainer")

        # Allowed types (all buildings/units)
        allowed_types = [
            dataset.pregen_nyan_objects["aux.game_entity_type.types.Unit"].get_nyan_object(),
            dataset.pregen_nyan_objects["aux.game_entity_type.types.Building"].get_nyan_object()
        ]

        ability_raw_api_object.add_raw_member("allowed_types",
                                              allowed_types,
                                              "engine.ability.type.EnterContainer")
        ability_raw_api_object.add_raw_member("blacklisted_entities",
                                              [],
                                              "engine.ability.type.EnterContainer")

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def exchange_resources_ability(line):
        """
        Adds the ExchangeResources ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        resource_names = ["Food", "Wood", "Stone"]

        abilities = []
        for resource_name in resource_names:
            ability_name = f"MarketExchange{resource_name}"
            ability_ref = f"{game_entity_name}.{ability_name}"
            ability_raw_api_object = RawAPIObject(ability_ref, ability_name, dataset.nyan_api_objects)
            ability_raw_api_object.add_raw_parent("engine.ability.type.ExchangeResources")
            ability_location = ForwardRef(line, game_entity_name)
            ability_raw_api_object.set_location(ability_location)

            # Resource that is exchanged (resource A)
            resource_a = dataset.pregen_nyan_objects[f"aux.resource.types.{resource_name}"].get_nyan_object()
            ability_raw_api_object.add_raw_member("resource_a",
                                                  resource_a,
                                                  "engine.ability.type.ExchangeResources")

            # Resource that is exchanged for (resource B)
            resource_b = dataset.pregen_nyan_objects["aux.resource.types.Gold"].get_nyan_object()
            ability_raw_api_object.add_raw_member("resource_b",
                                                  resource_b,
                                                  "engine.ability.type.ExchangeResources")

            # Exchange rate
            exchange_rate = dataset.pregen_nyan_objects[("aux.resource.market_trading.Market%sExchangeRate"
                                                         % (resource_name))].get_nyan_object()
            ability_raw_api_object.add_raw_member("exchange_rate",
                                                  exchange_rate,
                                                  "engine.ability.type.ExchangeResources")

            # Exchange modes
            exchange_modes = [
                dataset.pregen_nyan_objects["aux.resource.market_trading.MarketBuyExchangeMode"].get_nyan_object(),
                dataset.pregen_nyan_objects["aux.resource.market_trading.MarketSellExchangeMode"].get_nyan_object(),
            ]
            ability_raw_api_object.add_raw_member("exchange_modes",
                                                  exchange_modes,
                                                  "engine.ability.type.ExchangeResources")

            line.add_raw_api_object(ability_raw_api_object)
            ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())
            abilities.append(ability_forward_ref)

        return abilities

    @staticmethod
    def exit_container_ability(line):
        """
        Adds the ExitContainer ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability. None if no valid containers were found.
        :rtype: ...dataformat.forward_ref.ForwardRef, None
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = f"{game_entity_name}.ExitContainer"
        ability_raw_api_object = RawAPIObject(ability_ref,
                                              "ExitContainer",
                                              dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.ExitContainer")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Containers
        containers = []
        entity_lookups = internal_name_lookups.get_entity_lookups(dataset.game_version)
        for garrison in line.garrison_locations:
            garrison_mode = garrison.get_garrison_mode()

            # Cannot enter production buildings or monk inventories
            if garrison_mode == GenieGarrisonMode.MONK:
                continue

            garrison_name = entity_lookups[garrison.get_head_unit_id()][0]

            container_ref = f"{garrison_name}.Storage.{garrison_name}Container"
            container_forward_ref = ForwardRef(garrison, container_ref)
            containers.append(container_forward_ref)

        if not containers:
            return None

        ability_raw_api_object.add_raw_member("allowed_containers",
                                              containers,
                                              "engine.ability.type.ExitContainer")

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def game_entity_stance_ability(line):
        """
        Adds the GameEntityStance ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        current_unit = line.get_head_unit()
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = f"{game_entity_name}.GameEntityStance"
        ability_raw_api_object = RawAPIObject(ability_ref,
                                              "GameEntityStance",
                                              dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.GameEntityStance")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Stances
        search_range = current_unit["search_radius"].get_value()
        stance_names = ["Aggressive", "Defensive", "StandGround", "Passive"]

        # Attacking is prefered
        ability_preferences = []
        if line.is_projectile_shooter():
            ability_preferences.append(ForwardRef(line, f"{game_entity_name}.Attack"))

        elif line.is_melee() or line.is_ranged():
            if line.has_command(7):
                ability_preferences.append(ForwardRef(line, f"{game_entity_name}.Attack"))

            if line.has_command(105):
                ability_preferences.append(ForwardRef(line, f"{game_entity_name}.Heal"))

        # Units are prefered before buildings
        type_preferences = [
            dataset.pregen_nyan_objects["aux.game_entity_type.types.Unit"].get_nyan_object(),
            dataset.pregen_nyan_objects["aux.game_entity_type.types.Building"].get_nyan_object(),
        ]

        stances = []
        for stance_name in stance_names:
            stance_api_ref = f"engine.aux.game_entity_stance.type.{stance_name}"

            stance_ref = f"{game_entity_name}.GameEntityStance.{stance_name}"
            stance_raw_api_object = RawAPIObject(stance_ref, stance_name, dataset.nyan_api_objects)
            stance_raw_api_object.add_raw_parent(stance_api_ref)
            stance_location = ForwardRef(line, ability_ref)
            stance_raw_api_object.set_location(stance_location)

            # Search range
            stance_raw_api_object.add_raw_member("search_range",
                                                 search_range,
                                                 "engine.aux.game_entity_stance.GameEntityStance")

            # Ability preferences
            stance_raw_api_object.add_raw_member("ability_preference",
                                                 ability_preferences,
                                                 "engine.aux.game_entity_stance.GameEntityStance")

            # Type preferences
            stance_raw_api_object.add_raw_member("type_preference",
                                                 type_preferences,
                                                 "engine.aux.game_entity_stance.GameEntityStance")

            line.add_raw_api_object(stance_raw_api_object)
            stance_forward_ref = ForwardRef(line, stance_ref)
            stances.append(stance_forward_ref)

        ability_raw_api_object.add_raw_member("stances",
                                              stances,
                                              "engine.ability.type.GameEntityStance")

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def formation_ability(line):
        """
        Adds the Formation ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = f"{game_entity_name}.Formation"
        ability_raw_api_object = RawAPIObject(ability_ref, "Formation", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Formation")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Formation definitions
        if line.get_class_id() in (6,):
            subformation = dataset.pregen_nyan_objects["aux.formation.subformation.types.Infantry"].get_nyan_object()

        elif line.get_class_id() in (12, 47):
            subformation = dataset.pregen_nyan_objects["aux.formation.subformation.types.Cavalry"].get_nyan_object()

        elif line.get_class_id() in (0, 23, 36, 44, 55):
            subformation = dataset.pregen_nyan_objects["aux.formation.subformation.types.Ranged"].get_nyan_object()

        elif line.get_class_id() in (2, 13, 18, 20, 35, 43, 51, 59):
            subformation = dataset.pregen_nyan_objects["aux.formation.subformation.types.Siege"].get_nyan_object()

        else:
            subformation = dataset.pregen_nyan_objects["aux.formation.subformation.types.Support"].get_nyan_object()

        formation_names = ["Line", "Staggered", "Box", "Flank"]

        formation_defs = []
        for formation_name in formation_names:
            ge_formation_ref = f"{game_entity_name}.Formation.{formation_name}"
            ge_formation_raw_api_object = RawAPIObject(ge_formation_ref,
                                                       formation_name,
                                                       dataset.nyan_api_objects)
            ge_formation_raw_api_object.add_raw_parent("engine.aux.game_entity_formation.GameEntityFormation")
            ge_formation_location = ForwardRef(line, ability_ref)
            ge_formation_raw_api_object.set_location(ge_formation_location)

            # Formation
            formation_ref = f"aux.formation.types.{formation_name}"
            formation = dataset.pregen_nyan_objects[formation_ref].get_nyan_object()
            ge_formation_raw_api_object.add_raw_member("formation",
                                                       formation,
                                                       "engine.aux.game_entity_formation.GameEntityFormation")

            # Subformation
            ge_formation_raw_api_object.add_raw_member("subformation",
                                                       subformation,
                                                       "engine.aux.game_entity_formation.GameEntityFormation")

            line.add_raw_api_object(ge_formation_raw_api_object)
            ge_formation_forward_ref = ForwardRef(line, ge_formation_ref)
            formation_defs.append(ge_formation_forward_ref)

        ability_raw_api_object.add_raw_member("formations",
                                              formation_defs,
                                              "engine.ability.type.Formation")

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def foundation_ability(line, terrain_id=-1):
        """
        Adds the Foundation abilities to a line. Optionally chooses the specified
        terrain ID.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param terrain_id: Force this terrain ID as foundation
        :type terrain_id: int
        :returns: The forward references for the abilities.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        current_unit = line.get_head_unit()
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
        terrain_lookup_dict = internal_name_lookups.get_terrain_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = f"{game_entity_name}.Foundation"
        ability_raw_api_object = RawAPIObject(ability_ref, "Foundation", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Foundation")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Terrain
        if terrain_id == -1:
            terrain_id = current_unit["foundation_terrain_id"].get_value()

        terrain = dataset.terrain_groups[terrain_id]
        terrain_forward_ref = ForwardRef(terrain, terrain_lookup_dict[terrain_id][1])
        ability_raw_api_object.add_raw_member("foundation_terrain",
                                              terrain_forward_ref,
                                              "engine.ability.type.Foundation")

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def gather_ability(line):
        """
        Adds the Gather abilities to a line. Unlike the other methods, this
        creates multiple abilities.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward references for the abilities.
        :rtype: list
        """
        if isinstance(line, GenieVillagerGroup):
            gatherers = line.variants[0].line

        else:
            gatherers = [line.line[0]]

        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
        gather_lookup_dict = internal_name_lookups.get_gather_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        abilities = []
        for gatherer in gatherers:
            unit_commands = gatherer["unit_commands"].get_value()
            resource = None
            ability_animation_id = -1
            harvestable_class_ids = OrderedSet()
            harvestable_unit_ids = OrderedSet()

            for command in unit_commands:
                # Find a gather ability. It doesn't matter which one because
                # they should all produce the same resource for one genie unit.
                type_id = command["type"].get_value()

                if type_id not in (5, 110):
                    continue

                target_class_id = command["class_id"].get_value()
                if target_class_id > -1:
                    harvestable_class_ids.add(target_class_id)

                target_unit_id = command["unit_id"].get_value()
                if target_unit_id > -1:
                    harvestable_unit_ids.add(target_unit_id)

                resource_id = command["resource_out"].get_value()

                # If resource_out is not specified, the gatherer harvests resource_in
                if resource_id == -1:
                    resource_id = command["resource_in"].get_value()

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
                    ability_animation_id = command["work_sprite_id"].get_value()

                else:
                    ability_animation_id = command["proceed_sprite_id"].get_value()

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
            if gatherer_unit_id not in gather_lookup_dict.keys():
                # Skips hunting wolves
                continue

            ability_name = gather_lookup_dict[gatherer_unit_id][0]

            ability_ref = f"{game_entity_name}.{ability_name}"
            ability_raw_api_object = RawAPIObject(ability_ref, ability_name, dataset.nyan_api_objects)
            ability_raw_api_object.add_raw_parent("engine.ability.type.Gather")
            ability_location = ForwardRef(line, game_entity_name)
            ability_raw_api_object.set_location(ability_location)

            if ability_animation_id > -1:
                # Make the ability animated
                ability_raw_api_object.add_raw_parent("engine.ability.specialization.AnimatedAbility")

                animations_set = []
                animation_forward_ref = AoCAbilitySubprocessor.create_animation(line,
                                                                                ability_animation_id,
                                                                                ability_ref,
                                                                                ability_name,
                                                                                "%s_"
                                                                                % gather_lookup_dict[gatherer_unit_id][1])
                animations_set.append(animation_forward_ref)
                ability_raw_api_object.add_raw_member("animations", animations_set,
                                                      "engine.ability.specialization.AnimatedAbility")

            # Auto resume
            ability_raw_api_object.add_raw_member("auto_resume",
                                                  True,
                                                  "engine.ability.type.Gather")

            # search range
            ability_raw_api_object.add_raw_member("resume_search_range",
                                                  MemberSpecialValue.NYAN_INF,
                                                  "engine.ability.type.Gather")

            # Gather rate
            rate_name = f"{game_entity_name}.{ability_name}.GatherRate"
            rate_raw_api_object = RawAPIObject(rate_name, "GatherRate", dataset.nyan_api_objects)
            rate_raw_api_object.add_raw_parent("engine.aux.resource.ResourceRate")
            rate_location = ForwardRef(line, ability_ref)
            rate_raw_api_object.set_location(rate_location)

            rate_raw_api_object.add_raw_member("type", resource, "engine.aux.resource.ResourceRate")

            gather_rate = gatherer["work_rate"].get_value()
            rate_raw_api_object.add_raw_member("rate", gather_rate, "engine.aux.resource.ResourceRate")

            line.add_raw_api_object(rate_raw_api_object)

            rate_forward_ref = ForwardRef(line, rate_name)
            ability_raw_api_object.add_raw_member("gather_rate",
                                                  rate_forward_ref,
                                                  "engine.ability.type.Gather")

            # Resource container
            container_ref = "%s.ResourceStorage.%sContainer" % (game_entity_name,
                                                                gather_lookup_dict[gatherer_unit_id][0])
            container_forward_ref = ForwardRef(line, container_ref)
            ability_raw_api_object.add_raw_member("container",
                                                  container_forward_ref,
                                                  "engine.ability.type.Gather")

            # Targets (resource spots)
            entity_lookups = internal_name_lookups.get_entity_lookups(dataset.game_version)
            spot_forward_refs = []
            for group in harvestable_groups:
                group_id = group.get_head_unit_id()
                group_name = entity_lookups[group_id][0]

                spot_forward_ref = ForwardRef(group,
                                              "%s.Harvestable.%sResourceSpot"
                                              % (group_name, group_name))
                spot_forward_refs.append(spot_forward_ref)

            ability_raw_api_object.add_raw_member("targets",
                                                  spot_forward_refs,
                                                  "engine.ability.type.Gather")

            line.add_raw_api_object(ability_raw_api_object)

            ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())
            abilities.append(ability_forward_ref)

        return abilities

    @staticmethod
    def harvestable_ability(line):
        """
        Adds the Harvestable ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        current_unit = line.get_head_unit()
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = f"{game_entity_name}.Harvestable"
        ability_raw_api_object = RawAPIObject(ability_ref, "Harvestable", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Harvestable")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Resource spot
        resource_storage = current_unit["resource_storage"].get_value()

        for storage in resource_storage:
            resource_id = storage["type"].get_value()

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

            spot_name = f"{game_entity_name}.Harvestable.{game_entity_name}ResourceSpot"
            spot_raw_api_object = RawAPIObject(spot_name,
                                               f"{game_entity_name}ResourceSpot",
                                               dataset.nyan_api_objects)
            spot_raw_api_object.add_raw_parent("engine.aux.resource_spot.ResourceSpot")
            spot_location = ForwardRef(line, ability_ref)
            spot_raw_api_object.set_location(spot_location)

            # Type
            spot_raw_api_object.add_raw_member("resource",
                                               resource,
                                               "engine.aux.resource_spot.ResourceSpot")

            # Start amount (equals max amount)
            if line.get_id() == 50:
                # Farm food amount (hardcoded in civ)
                starting_amount = dataset.genie_civs[1]["resources"][36].get_value()

            elif line.get_id() == 199:
                # Fish trap food amount (hardcoded in civ)
                starting_amount = storage["amount"].get_value()
                starting_amount += dataset.genie_civs[1]["resources"][88].get_value()

            else:
                starting_amount = storage["amount"].get_value()

            spot_raw_api_object.add_raw_member("starting_amount",
                                               starting_amount,
                                               "engine.aux.resource_spot.ResourceSpot")

            # Max amount
            spot_raw_api_object.add_raw_member("max_amount",
                                               starting_amount,
                                               "engine.aux.resource_spot.ResourceSpot")

            # Decay rate
            decay_rate = current_unit["resource_decay"].get_value()
            spot_raw_api_object.add_raw_member("decay_rate",
                                               decay_rate,
                                               "engine.aux.resource_spot.ResourceSpot")

            spot_forward_ref = ForwardRef(line, spot_name)
            ability_raw_api_object.add_raw_member("resources",
                                                  spot_forward_ref,
                                                  "engine.ability.type.Harvestable")
            line.add_raw_api_object(spot_raw_api_object)

            # Only one resource spot per ability
            break

        # Harvest Progress (we don't use this for Aoe2)
        ability_raw_api_object.add_raw_member("harvest_progress",
                                              [],
                                              "engine.ability.type.Harvestable")

        # Restock Progress
        progress_forward_refs = []
        if line.get_class_id() == 49:
            # Farms
            # =====================================================================================
            progress_name = f"{game_entity_name}.Harvestable.RestockProgress33"
            progress_raw_api_object = RawAPIObject(progress_name,
                                                   "RestockProgress33",
                                                   dataset.nyan_api_objects)
            progress_raw_api_object.add_raw_parent("engine.aux.progress.type.RestockProgress")
            progress_location = ForwardRef(line, ability_ref)
            progress_raw_api_object.set_location(progress_location)

            # Interval = (0.0, 25.0)
            progress_raw_api_object.add_raw_member("left_boundary",
                                                   0.0,
                                                   "engine.aux.progress.Progress")
            progress_raw_api_object.add_raw_member("right_boundary",
                                                   33.0,
                                                   "engine.aux.progress.Progress")

            progress_raw_api_object.add_raw_parent("engine.aux.progress.specialization.TerrainOverlayProgress")

            # Terrain overlay
            terrain_ref = "FarmConstruction1"
            terrain_group = dataset.terrain_groups[29]
            terrain_forward_ref = ForwardRef(terrain_group, terrain_ref)
            progress_raw_api_object.add_raw_member("terrain_overlay",
                                                   terrain_forward_ref,
                                                   "engine.aux.progress.specialization.TerrainOverlayProgress")

            progress_raw_api_object.add_raw_parent("engine.aux.progress.specialization.StateChangeProgress")

            # State change
            init_state_ref = f"{game_entity_name}.Constructable.InitState"
            init_state_forward_ref = ForwardRef(line, init_state_ref)
            progress_raw_api_object.add_raw_member("state_change",
                                                   init_state_forward_ref,
                                                   "engine.aux.progress.specialization.StateChangeProgress")
            # =====================================================================================
            progress_forward_refs.append(ForwardRef(line, progress_name))
            line.add_raw_api_object(progress_raw_api_object)
            # =====================================================================================
            progress_name = f"{game_entity_name}.Harvestable.RestockProgress66"
            progress_raw_api_object = RawAPIObject(progress_name,
                                                   "RestockProgress66",
                                                   dataset.nyan_api_objects)
            progress_raw_api_object.add_raw_parent("engine.aux.progress.type.RestockProgress")
            progress_location = ForwardRef(line, ability_ref)
            progress_raw_api_object.set_location(progress_location)

            # Interval = (25.0, 50.0)
            progress_raw_api_object.add_raw_member("left_boundary",
                                                   33.0,
                                                   "engine.aux.progress.Progress")
            progress_raw_api_object.add_raw_member("right_boundary",
                                                   66.0,
                                                   "engine.aux.progress.Progress")

            progress_raw_api_object.add_raw_parent("engine.aux.progress.specialization.TerrainOverlayProgress")

            # Terrain overlay
            terrain_ref = "FarmConstruction2"
            terrain_group = dataset.terrain_groups[30]
            terrain_forward_ref = ForwardRef(terrain_group, terrain_ref)
            progress_raw_api_object.add_raw_member("terrain_overlay",
                                                   terrain_forward_ref,
                                                   "engine.aux.progress.specialization.TerrainOverlayProgress")

            progress_raw_api_object.add_raw_parent("engine.aux.progress.specialization.StateChangeProgress")

            # State change
            construct_state_ref = f"{game_entity_name}.Constructable.ConstructState"
            construct_state_forward_ref = ForwardRef(line, construct_state_ref)
            progress_raw_api_object.add_raw_member("state_change",
                                                   construct_state_forward_ref,
                                                   "engine.aux.progress.specialization.StateChangeProgress")
            # =====================================================================================
            progress_forward_refs.append(ForwardRef(line, progress_name))
            line.add_raw_api_object(progress_raw_api_object)
            # =====================================================================================
            progress_name = f"{game_entity_name}.Harvestable.RestockProgress100"
            progress_raw_api_object = RawAPIObject(progress_name,
                                                   "RestockProgress100",
                                                   dataset.nyan_api_objects)
            progress_raw_api_object.add_raw_parent("engine.aux.progress.type.RestockProgress")
            progress_location = ForwardRef(line, ability_ref)
            progress_raw_api_object.set_location(progress_location)

            progress_raw_api_object.add_raw_member("left_boundary",
                                                   66.0,
                                                   "engine.aux.progress.Progress")
            progress_raw_api_object.add_raw_member("right_boundary",
                                                   100.0,
                                                   "engine.aux.progress.Progress")

            progress_raw_api_object.add_raw_parent("engine.aux.progress.specialization.TerrainOverlayProgress")

            # Terrain overlay
            terrain_ref = "FarmConstruction3"
            terrain_group = dataset.terrain_groups[31]
            terrain_forward_ref = ForwardRef(terrain_group, terrain_ref)
            progress_raw_api_object.add_raw_member("terrain_overlay",
                                                   terrain_forward_ref,
                                                   "engine.aux.progress.specialization.TerrainOverlayProgress")

            progress_raw_api_object.add_raw_parent("engine.aux.progress.specialization.StateChangeProgress")

            # State change
            construct_state_ref = f"{game_entity_name}.Constructable.ConstructState"
            construct_state_forward_ref = ForwardRef(line, construct_state_ref)
            progress_raw_api_object.add_raw_member("state_change",
                                                   construct_state_forward_ref,
                                                   "engine.aux.progress.specialization.StateChangeProgress")
            # =======================================================================
            progress_forward_refs.append(ForwardRef(line, progress_name))
            line.add_raw_api_object(progress_raw_api_object)

        ability_raw_api_object.add_raw_member("restock_progress",
                                              progress_forward_refs,
                                              "engine.ability.type.Harvestable")

        # Gatherer limit (infinite in AoC except for farms)
        gatherer_limit = MemberSpecialValue.NYAN_INF
        if line.get_class_id() == 49:
            gatherer_limit = 1

        ability_raw_api_object.add_raw_member("gatherer_limit",
                                              gatherer_limit,
                                              "engine.ability.type.Harvestable")

        # Unit have to die before they are harvestable (except for farms)
        harvestable_by_default = current_unit["hit_points"].get_value() == 0
        if line.get_class_id() == 49:
            harvestable_by_default = True

        ability_raw_api_object.add_raw_member("harvestable_by_default",
                                              harvestable_by_default,
                                              "engine.ability.type.Harvestable")

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def herd_ability(line):
        """
        Adds the Herd ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = f"{game_entity_name}.Herd"
        ability_raw_api_object = RawAPIObject(ability_ref, "Herd", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Herd")
        ability_location = ForwardRef(line, game_entity_name)
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
        allowed_types = [
            dataset.pregen_nyan_objects["aux.game_entity_type.types.Herdable"].get_nyan_object()
        ]
        ability_raw_api_object.add_raw_member("allowed_types",
                                              allowed_types,
                                              "engine.ability.type.Herd")

        # Blacklisted entities
        ability_raw_api_object.add_raw_member("blacklisted_entities",
                                              [],
                                              "engine.ability.type.Herd")

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def herdable_ability(line):
        """
        Adds the Herdable ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = f"{game_entity_name}.Herdable"
        ability_raw_api_object = RawAPIObject(ability_ref,
                                              "Herdable",
                                              dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Herdable")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Mode
        mode = dataset.nyan_api_objects["engine.aux.herdable_mode.type.LongestTimeInRange"]
        ability_raw_api_object.add_raw_member("mode", mode, "engine.ability.type.Herdable")

        # Discover range
        ability_raw_api_object.add_raw_member("adjacent_discover_range",
                                              1.0,
                                              "engine.ability.type.Herdable")

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def hitbox_ability(line):
        """
        Adds the Hitbox ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        current_unit = line.get_head_unit()
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = f"{game_entity_name}.Hitbox"
        ability_raw_api_object = RawAPIObject(ability_ref, "Hitbox", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Hitbox")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Hitbox object
        hitbox_name = f"{game_entity_name}.Hitbox.{game_entity_name}Hitbox"
        hitbox_raw_api_object = RawAPIObject(hitbox_name,
                                             f"{game_entity_name}Hitbox",
                                             dataset.nyan_api_objects)
        hitbox_raw_api_object.add_raw_parent("engine.aux.hitbox.Hitbox")
        hitbox_location = ForwardRef(line, ability_ref)
        hitbox_raw_api_object.set_location(hitbox_location)

        radius_x = current_unit["radius_x"].get_value()
        radius_y = current_unit["radius_y"].get_value()
        radius_z = current_unit["radius_z"].get_value()

        hitbox_raw_api_object.add_raw_member("radius_x",
                                             radius_x,
                                             "engine.aux.hitbox.Hitbox")
        hitbox_raw_api_object.add_raw_member("radius_y",
                                             radius_y,
                                             "engine.aux.hitbox.Hitbox")
        hitbox_raw_api_object.add_raw_member("radius_z",
                                             radius_z,
                                             "engine.aux.hitbox.Hitbox")

        hitbox_forward_ref = ForwardRef(line, hitbox_name)
        ability_raw_api_object.add_raw_member("hitbox",
                                              hitbox_forward_ref,
                                              "engine.ability.type.Hitbox")

        line.add_raw_api_object(hitbox_raw_api_object)
        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def idle_ability(line):
        """
        Adds the Idle ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        current_unit = line.get_head_unit()
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
        gset_lookup_dict = internal_name_lookups.get_graphic_set_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = f"{game_entity_name}.Idle"
        ability_raw_api_object = RawAPIObject(ability_ref, "Idle", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Idle")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        ability_animation_id = current_unit["idle_graphic0"].get_value()

        if ability_animation_id > -1:
            # Make the ability animated
            ability_raw_api_object.add_raw_parent("engine.ability.specialization.AnimatedAbility")

            animations_set = []
            animation_forward_ref = AoCAbilitySubprocessor.create_animation(line,
                                                                            ability_animation_id,
                                                                            ability_ref,
                                                                            "Idle",
                                                                            "idle_")
            animations_set.append(animation_forward_ref)
            ability_raw_api_object.add_raw_member("animations",
                                                  animations_set,
                                                  "engine.ability.specialization.AnimatedAbility")

            # Create custom civ graphics
            handled_graphics_set_ids = set()
            for civ_group in dataset.civ_groups.values():
                civ = civ_group.civ
                civ_id = civ_group.get_id()

                # Only proceed if the civ stores the unit in the line
                if current_unit_id not in civ["units"].get_value().keys():
                    continue

                civ_animation_id = civ["units"][current_unit_id]["idle_graphic0"].get_value()

                if civ_animation_id != ability_animation_id:
                    # Find the corresponding graphics set
                    graphics_set_id = -1
                    for set_id, items in gset_lookup_dict.items():
                        if civ_id in items[0]:
                            graphics_set_id = set_id
                            break

                    # Check if the object for the animation has been created before
                    obj_exists = graphics_set_id in handled_graphics_set_ids
                    if not obj_exists:
                        handled_graphics_set_ids.add(graphics_set_id)

                    obj_prefix = f"{gset_lookup_dict[graphics_set_id][1]}Idle"
                    filename_prefix = f"idle_{gset_lookup_dict[graphics_set_id][2]}_"
                    AoCAbilitySubprocessor.create_civ_animation(line,
                                                                civ_group,
                                                                civ_animation_id,
                                                                ability_ref,
                                                                obj_prefix,
                                                                filename_prefix,
                                                                obj_exists)

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def live_ability(line):
        """
        Adds the Live ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        current_unit = line.get_head_unit()
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = f"{game_entity_name}.Live"
        ability_raw_api_object = RawAPIObject(ability_ref, "Live", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Live")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        attributes_set = []

        # Health
        # =======================================================================================
        health_ref = f"{game_entity_name}.Live.Health"
        health_raw_api_object = RawAPIObject(health_ref, "Health", dataset.nyan_api_objects)
        health_raw_api_object.add_raw_parent("engine.aux.attribute.AttributeSetting")
        health_location = ForwardRef(line, ability_ref)
        health_raw_api_object.set_location(health_location)

        attribute_value = dataset.pregen_nyan_objects["aux.attribute.types.Health"].get_nyan_object()
        health_raw_api_object.add_raw_member("attribute",
                                             attribute_value,
                                             "engine.aux.attribute.AttributeSetting")

        # Lowest HP can go
        health_raw_api_object.add_raw_member("min_value",
                                             0,
                                             "engine.aux.attribute.AttributeSetting")

        # Max HP and starting HP
        max_hp_value = current_unit["hit_points"].get_value()
        health_raw_api_object.add_raw_member("max_value",
                                             max_hp_value,
                                             "engine.aux.attribute.AttributeSetting")

        starting_value = max_hp_value
        if isinstance(line, GenieBuildingLineGroup):
            # Buildings spawn with 1 HP
            starting_value = 1

        health_raw_api_object.add_raw_member("starting_value",
                                             starting_value,
                                             "engine.aux.attribute.AttributeSetting")

        line.add_raw_api_object(health_raw_api_object)

        # =======================================================================================
        health_forward_ref = ForwardRef(line, health_raw_api_object.get_id())
        attributes_set.append(health_forward_ref)

        if current_unit_id == 125:
            # Faith (only monk)
            faith_ref = f"{game_entity_name}.Live.Faith"
            faith_raw_api_object = RawAPIObject(faith_ref, "Faith", dataset.nyan_api_objects)
            faith_raw_api_object.add_raw_parent("engine.aux.attribute.AttributeSetting")
            faith_location = ForwardRef(line, ability_ref)
            faith_raw_api_object.set_location(faith_location)

            attribute_value = dataset.pregen_nyan_objects["aux.attribute.types.Faith"].get_nyan_object()
            faith_raw_api_object.add_raw_member("attribute", attribute_value,
                                                "engine.aux.attribute.AttributeSetting")

            # Lowest faith can go
            faith_raw_api_object.add_raw_member("min_value",
                                                0,
                                                "engine.aux.attribute.AttributeSetting")

            # Max faith and starting faith
            faith_raw_api_object.add_raw_member("max_value",
                                                100,
                                                "engine.aux.attribute.AttributeSetting")
            faith_raw_api_object.add_raw_member("starting_value",
                                                100,
                                                "engine.aux.attribute.AttributeSetting")

            line.add_raw_api_object(faith_raw_api_object)

            faith_forward_ref = ForwardRef(line, faith_ref)
            attributes_set.append(faith_forward_ref)

        ability_raw_api_object.add_raw_member("attributes", attributes_set,
                                              "engine.ability.type.Live")

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def los_ability(line):
        """
        Adds the LineOfSight ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        current_unit = line.get_head_unit()
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = f"{game_entity_name}.LineOfSight"
        ability_raw_api_object = RawAPIObject(ability_ref, "LineOfSight", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.LineOfSight")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Line of sight
        line_of_sight = current_unit["line_of_sight"].get_value()
        ability_raw_api_object.add_raw_member("range", line_of_sight,
                                              "engine.ability.type.LineOfSight")

        # Diplomacy settings
        ability_raw_api_object.add_raw_parent("engine.ability.specialization.DiplomaticAbility")
        diplomatic_stances = [dataset.nyan_api_objects["engine.aux.diplomatic_stance.type.Self"]]
        ability_raw_api_object.add_raw_member("stances", diplomatic_stances,
                                              "engine.ability.specialization.DiplomaticAbility")

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def move_ability(line):
        """
        Adds the Move ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        current_unit = line.get_head_unit()
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
        gset_lookup_dict = internal_name_lookups.get_graphic_set_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = f"{game_entity_name}.Move"
        ability_raw_api_object = RawAPIObject(ability_ref, "Move", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Move")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Animation
        ability_animation_id = current_unit["move_graphics"].get_value()
        if ability_animation_id > -1:
            # Make the ability animated
            ability_raw_api_object.add_raw_parent("engine.ability.specialization.AnimatedAbility")

            animations_set = []

            animation_obj_prefix = "Move"
            animation_filename_prefix = "move_"

            animation_forward_ref = AoCAbilitySubprocessor.create_animation(line,
                                                                            ability_animation_id,
                                                                            ability_ref,
                                                                            animation_obj_prefix,
                                                                            animation_filename_prefix)
            animations_set.append(animation_forward_ref)
            ability_raw_api_object.add_raw_member("animations", animations_set,
                                                  "engine.ability.specialization.AnimatedAbility")

            # Create custom civ graphics
            handled_graphics_set_ids = set()
            for civ_group in dataset.civ_groups.values():
                civ = civ_group.civ
                civ_id = civ_group.get_id()

                # Only proceed if the civ stores the unit in the line
                if current_unit_id not in civ["units"].get_value().keys():
                    continue

                civ_animation_id = civ["units"][current_unit_id]["move_graphics"].get_value()

                if civ_animation_id != ability_animation_id:
                    # Find the corresponding graphics set
                    graphics_set_id = -1
                    for set_id, items in gset_lookup_dict.items():
                        if civ_id in items[0]:
                            graphics_set_id = set_id
                            break

                    # Check if the object for the animation has been created before
                    obj_exists = graphics_set_id in handled_graphics_set_ids
                    if not obj_exists:
                        handled_graphics_set_ids.add(graphics_set_id)

                    obj_prefix = f"{gset_lookup_dict[graphics_set_id][1]}Move"
                    filename_prefix = f"move_{gset_lookup_dict[graphics_set_id][2]}_"
                    AoCAbilitySubprocessor.create_civ_animation(line,
                                                                civ_group,
                                                                civ_animation_id,
                                                                ability_ref,
                                                                obj_prefix,
                                                                filename_prefix,
                                                                obj_exists)

        # Command Sound
        ability_comm_sound_id = current_unit["command_sound_id"].get_value()
        if ability_comm_sound_id > -1:
            # Make the ability animated
            ability_raw_api_object.add_raw_parent("engine.ability.specialization.CommandSoundAbility")

            sounds_set = []

            sound_obj_prefix = "Move"

            sound_forward_ref = AoCAbilitySubprocessor.create_sound(line,
                                                                    ability_comm_sound_id,
                                                                    ability_ref,
                                                                    sound_obj_prefix,
                                                                    "command_")
            sounds_set.append(sound_forward_ref)
            ability_raw_api_object.add_raw_member("sounds", sounds_set,
                                                  "engine.ability.specialization.CommandSoundAbility")

        # Speed
        speed = current_unit["speed"].get_value()
        ability_raw_api_object.add_raw_member("speed", speed, "engine.ability.type.Move")

        # Standard move modes
        move_modes = [
            dataset.nyan_api_objects["engine.aux.move_mode.type.AttackMove"],
            dataset.nyan_api_objects["engine.aux.move_mode.type.Normal"],
            dataset.nyan_api_objects["engine.aux.move_mode.type.Patrol"]
        ]

        # Follow
        ability_ref = f"{game_entity_name}.Move.Follow"
        follow_raw_api_object = RawAPIObject(ability_ref, "Follow", dataset.nyan_api_objects)
        follow_raw_api_object.add_raw_parent("engine.aux.move_mode.type.Follow")
        follow_location = ForwardRef(line, f"{game_entity_name}.Move")
        follow_raw_api_object.set_location(follow_location)

        follow_range = current_unit["line_of_sight"].get_value() - 1
        follow_raw_api_object.add_raw_member("range",
                                             follow_range,
                                             "engine.aux.move_mode.type.Follow")

        line.add_raw_api_object(follow_raw_api_object)
        follow_forward_ref = ForwardRef(line, follow_raw_api_object.get_id())
        move_modes.append(follow_forward_ref)

        ability_raw_api_object.add_raw_member("modes", move_modes, "engine.ability.type.Move")

        # Diplomacy settings
        ability_raw_api_object.add_raw_parent("engine.ability.specialization.DiplomaticAbility")
        diplomatic_stances = [dataset.nyan_api_objects["engine.aux.diplomatic_stance.type.Self"]]
        ability_raw_api_object.add_raw_member("stances",
                                              diplomatic_stances,
                                              "engine.ability.specialization.DiplomaticAbility")

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def move_projectile_ability(line, position=-1):
        """
        Adds the Move ability to a projectile of the specified line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        dataset = line.data

        if position == 0:
            current_unit_id = line.get_head_unit_id()
            projectile_id = line.get_head_unit()["attack_projectile_primary_unit_id"].get_value()
            current_unit = dataset.genie_units[projectile_id]

        elif position == 1:
            current_unit_id = line.get_head_unit_id()
            projectile_id = line.get_head_unit()["attack_projectile_secondary_unit_id"].get_value()
            current_unit = dataset.genie_units[projectile_id]

        else:
            raise Exception(f"Invalid projectile number: {position}")

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = f"Projectile{position}.Move"
        ability_raw_api_object = RawAPIObject(ability_ref, "Move", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Move")
        ability_location = ForwardRef(line,
                                      "%s.ShootProjectile.Projectile%s"
                                      % (game_entity_name, position))
        ability_raw_api_object.set_location(ability_location)

        # Animation
        ability_animation_id = current_unit["move_graphics"].get_value()
        if ability_animation_id > -1:
            # Make the ability animated
            ability_raw_api_object.add_raw_parent("engine.ability.specialization.AnimatedAbility")

            animations_set = []

            animation_obj_prefix = "ProjectileFly"
            animation_filename_prefix = "projectile_fly_"

            animation_forward_ref = AoCAbilitySubprocessor.create_animation(line,
                                                                            ability_animation_id,
                                                                            ability_ref,
                                                                            animation_obj_prefix,
                                                                            animation_filename_prefix)
            animations_set.append(animation_forward_ref)
            ability_raw_api_object.add_raw_member("animations", animations_set,
                                                  "engine.ability.specialization.AnimatedAbility")

        # Speed
        speed = current_unit["speed"].get_value()
        ability_raw_api_object.add_raw_member("speed", speed, "engine.ability.type.Move")

        # Move modes
        move_modes = [
            dataset.nyan_api_objects["engine.aux.move_mode.type.Normal"],
        ]
        ability_raw_api_object.add_raw_member("modes", move_modes, "engine.ability.type.Move")

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def named_ability(line):
        """
        Adds the Named ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        current_unit = line.get_head_unit()
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = f"{game_entity_name}.Named"
        ability_raw_api_object = RawAPIObject(ability_ref, "Named", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Named")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Name
        name_ref = f"{game_entity_name}.Named.{game_entity_name}Name"
        name_raw_api_object = RawAPIObject(name_ref,
                                           f"{game_entity_name}Name",
                                           dataset.nyan_api_objects)
        name_raw_api_object.add_raw_parent("engine.aux.translated.type.TranslatedString")
        name_location = ForwardRef(line, ability_ref)
        name_raw_api_object.set_location(name_location)

        name_string_id = current_unit["language_dll_name"].get_value()
        translations = AoCAbilitySubprocessor.create_language_strings(line,
                                                                      name_string_id,
                                                                      name_ref,
                                                                      "%sName"
                                                                      % (game_entity_name))
        name_raw_api_object.add_raw_member("translations",
                                           translations,
                                           "engine.aux.translated.type.TranslatedString")

        name_forward_ref = ForwardRef(line, name_ref)
        ability_raw_api_object.add_raw_member("name", name_forward_ref, "engine.ability.type.Named")
        line.add_raw_api_object(name_raw_api_object)

        # Description
        description_ref = f"{game_entity_name}.Named.{game_entity_name}Description"
        description_raw_api_object = RawAPIObject(description_ref,
                                                  f"{game_entity_name}Description",
                                                  dataset.nyan_api_objects)
        description_raw_api_object.add_raw_parent("engine.aux.translated.type.TranslatedMarkupFile")
        description_location = ForwardRef(line, ability_ref)
        description_raw_api_object.set_location(description_location)

        description_raw_api_object.add_raw_member("translations",
                                                  [],
                                                  "engine.aux.translated.type.TranslatedMarkupFile")

        description_forward_ref = ForwardRef(line, description_ref)
        ability_raw_api_object.add_raw_member("description",
                                              description_forward_ref,
                                              "engine.ability.type.Named")
        line.add_raw_api_object(description_raw_api_object)

        # Long description
        long_description_ref = f"{game_entity_name}.Named.{game_entity_name}LongDescription"
        long_description_raw_api_object = RawAPIObject(long_description_ref,
                                                       f"{game_entity_name}LongDescription",
                                                       dataset.nyan_api_objects)
        long_description_raw_api_object.add_raw_parent("engine.aux.translated.type.TranslatedMarkupFile")
        long_description_location = ForwardRef(line, ability_ref)
        long_description_raw_api_object.set_location(long_description_location)

        long_description_raw_api_object.add_raw_member("translations",
                                                       [],
                                                       "engine.aux.translated.type.TranslatedMarkupFile")

        long_description_forward_ref = ForwardRef(line, long_description_ref)
        ability_raw_api_object.add_raw_member("long_description",
                                              long_description_forward_ref,
                                              "engine.ability.type.Named")
        line.add_raw_api_object(long_description_raw_api_object)

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def overlay_terrain_ability(line):
        """
        Adds the OverlayTerrain to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward references for the abilities.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        current_unit = line.get_head_unit()
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
        terrain_lookup_dict = internal_name_lookups.get_terrain_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = f"{game_entity_name}.OverlayTerrain"
        ability_raw_api_object = RawAPIObject(ability_ref,
                                              "OverlayTerrain",
                                              dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.OverlayTerrain")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Terrain (Use foundation terrain)
        terrain_id = current_unit["foundation_terrain_id"].get_value()
        terrain = dataset.terrain_groups[terrain_id]
        terrain_forward_ref = ForwardRef(terrain, terrain_lookup_dict[terrain_id][1])
        ability_raw_api_object.add_raw_member("terrain_overlay",
                                              terrain_forward_ref,
                                              "engine.ability.type.OverlayTerrain")

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def passable_ability(line):
        """
        Adds the Passable ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = f"{game_entity_name}.Passable"
        ability_raw_api_object = RawAPIObject(ability_ref,
                                              "Passable",
                                              dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Passable")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Hitbox
        hitbox_ref = f"{game_entity_name}.Hitbox.{game_entity_name}Hitbox"
        hitbox_forward_ref = ForwardRef(line, hitbox_ref)
        ability_raw_api_object.add_raw_member("hitbox",
                                              hitbox_forward_ref,
                                              "engine.ability.type.Passable")

        # Passable mode
        # =====================================================================================
        mode_name = f"{game_entity_name}.Passable.PassableMode"
        mode_raw_api_object = RawAPIObject(mode_name, "PassableMode", dataset.nyan_api_objects)
        mode_parent = "engine.aux.passable_mode.type.Normal"
        if isinstance(line, GenieStackBuildingGroup):
            if line.is_gate():
                mode_parent = "engine.aux.passable_mode.type.Gate"

        mode_raw_api_object.add_raw_parent(mode_parent)
        mode_location = ForwardRef(line, ability_ref)
        mode_raw_api_object.set_location(mode_location)

        # Allowed types
        allowed_types = [
            dataset.pregen_nyan_objects["aux.game_entity_type.types.Unit"].get_nyan_object(),
            dataset.pregen_nyan_objects["aux.game_entity_type.types.Building"].get_nyan_object(),
            dataset.pregen_nyan_objects["aux.game_entity_type.types.Projectile"].get_nyan_object()
        ]
        mode_raw_api_object.add_raw_member("allowed_types",
                                           allowed_types,
                                           "engine.aux.passable_mode.PassableMode")

        # Blacklisted entities
        mode_raw_api_object.add_raw_member("blacklisted_entities",
                                           [],
                                           "engine.aux.passable_mode.PassableMode")

        line.add_raw_api_object(mode_raw_api_object)
        # =====================================================================================
        mode_forward_ref = ForwardRef(line, mode_name)
        ability_raw_api_object.add_raw_member("mode",
                                              mode_forward_ref,
                                              "engine.ability.type.Passable")

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def production_queue_ability(line):
        """
        Adds the ProductionQueue ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = f"{game_entity_name}.ProductionQueue"
        ability_raw_api_object = RawAPIObject(ability_ref,
                                              "ProductionQueue",
                                              dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.ProductionQueue")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Size
        size = 14

        ability_raw_api_object.add_raw_member("size",
                                              size,
                                              "engine.ability.type.ProductionQueue")

        # Production modes
        modes = []

        mode_name = f"{game_entity_name}.ProvideContingent.CreatablesMode"
        mode_raw_api_object = RawAPIObject(mode_name, "CreatablesMode", dataset.nyan_api_objects)
        mode_raw_api_object.add_raw_parent("engine.aux.production_mode.type.Creatables")
        mode_location = ForwardRef(line, ability_ref)
        mode_raw_api_object.set_location(mode_location)

        # AoE2 allows all creatables in production queue
        mode_raw_api_object.add_raw_member("exclude",
                                           [],
                                           "engine.aux.production_mode.type.Creatables")

        mode_forward_ref = ForwardRef(line, mode_name)
        modes.append(mode_forward_ref)

        ability_raw_api_object.add_raw_member("production_modes",
                                              modes,
                                              "engine.ability.type.ProductionQueue")

        line.add_raw_api_object(mode_raw_api_object)
        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def projectile_ability(line, position=0):
        """
        Adds a Projectile ability to projectiles in a line. Which projectile should
        be added is determined by the 'position' argument.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param position: When 0, gives the first projectile its ability. When 1, the second...
        :type position: int
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        current_unit = line.get_head_unit()
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        # First projectile is mandatory
        obj_ref = f"{game_entity_name}.ShootProjectile.Projectile{str(position)}"
        ability_ref = "%s.ShootProjectile.Projectile%s.Projectile"\
            % (game_entity_name, str(position))
        ability_raw_api_object = RawAPIObject(ability_ref,
                                              "Projectile",
                                              dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Projectile")
        ability_location = ForwardRef(line, obj_ref)
        ability_raw_api_object.set_location(ability_location)

        # Arc
        if position == 0:
            projectile_id = current_unit["attack_projectile_primary_unit_id"].get_value()

        elif position == 1:
            projectile_id = current_unit["attack_projectile_secondary_unit_id"].get_value()

        else:
            raise Exception("Invalid position")

        projectile = dataset.genie_units[projectile_id]
        arc = degrees(projectile["projectile_arc"].get_value())
        ability_raw_api_object.add_raw_member("arc",
                                              arc,
                                              "engine.ability.type.Projectile")

        # Accuracy
        accuracy_name = "%s.ShootProjectile.Projectile%s.Projectile.Accuracy"\
                        % (game_entity_name, str(position))
        accuracy_raw_api_object = RawAPIObject(accuracy_name,
                                               "Accuracy",
                                               dataset.nyan_api_objects)
        accuracy_raw_api_object.add_raw_parent("engine.aux.accuracy.Accuracy")
        accuracy_location = ForwardRef(line, ability_ref)
        accuracy_raw_api_object.set_location(accuracy_location)

        accuracy_value = current_unit["accuracy"].get_value()
        accuracy_raw_api_object.add_raw_member("accuracy",
                                               accuracy_value,
                                               "engine.aux.accuracy.Accuracy")

        accuracy_dispersion = current_unit["accuracy_dispersion"].get_value()
        accuracy_raw_api_object.add_raw_member("accuracy_dispersion",
                                               accuracy_dispersion,
                                               "engine.aux.accuracy.Accuracy")
        dropoff_type = dataset.nyan_api_objects["engine.aux.dropoff_type.type.InverseLinear"]
        accuracy_raw_api_object.add_raw_member("dispersion_dropoff",
                                               dropoff_type,
                                               "engine.aux.accuracy.Accuracy")

        allowed_types = [
            dataset.pregen_nyan_objects["aux.game_entity_type.types.Building"].get_nyan_object(),
            dataset.pregen_nyan_objects["aux.game_entity_type.types.Unit"].get_nyan_object()
        ]
        accuracy_raw_api_object.add_raw_member("target_types",
                                               allowed_types,
                                               "engine.aux.accuracy.Accuracy")
        accuracy_raw_api_object.add_raw_member("blacklisted_entities",
                                               [],
                                               "engine.aux.accuracy.Accuracy")

        line.add_raw_api_object(accuracy_raw_api_object)
        accuracy_forward_ref = ForwardRef(line, accuracy_name)
        ability_raw_api_object.add_raw_member("accuracy",
                                              [accuracy_forward_ref],
                                              "engine.ability.type.Projectile")

        # Target mode
        target_mode = dataset.nyan_api_objects["engine.aux.target_mode.type.CurrentPosition"]
        ability_raw_api_object.add_raw_member("target_mode",
                                              target_mode,
                                              "engine.ability.type.Projectile")

        # Ingore types; buildings are ignored unless targeted
        ignore_forward_refs = [
            dataset.pregen_nyan_objects["aux.game_entity_type.types.Building"].get_nyan_object()
        ]
        ability_raw_api_object.add_raw_member("ignored_types",
                                              ignore_forward_refs,
                                              "engine.ability.type.Projectile")
        ability_raw_api_object.add_raw_member("unignored_entities",
                                              [],
                                              "engine.ability.type.Projectile")

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def provide_contingent_ability(line):
        """
        Adds the ProvideContingent ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        current_unit = line.get_head_unit()
        if isinstance(line, GenieStackBuildingGroup):
            current_unit = line.get_stack_unit()

        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = f"{game_entity_name}.ProvideContingent"
        ability_raw_api_object = RawAPIObject(ability_ref,
                                              "ProvideContingent",
                                              dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.ProvideContingent")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Also stores the pop space
        resource_storage = current_unit["resource_storage"].get_value()

        contingents = []
        for storage in resource_storage:
            type_id = storage["type"].get_value()

            if type_id == 4:
                resource = dataset.pregen_nyan_objects["aux.resource.types.PopulationSpace"].get_nyan_object()
                resource_name = "PopSpace"

            else:
                continue

            amount = storage["amount"].get_value()

            contingent_amount_name = f"{game_entity_name}.ProvideContingent.{resource_name}"
            contingent_amount = RawAPIObject(contingent_amount_name, resource_name,
                                             dataset.nyan_api_objects)
            contingent_amount.add_raw_parent("engine.aux.resource.ResourceAmount")
            ability_forward_ref = ForwardRef(line, ability_ref)
            contingent_amount.set_location(ability_forward_ref)

            contingent_amount.add_raw_member("type",
                                             resource,
                                             "engine.aux.resource.ResourceAmount")
            contingent_amount.add_raw_member("amount",
                                             amount,
                                             "engine.aux.resource.ResourceAmount")

            line.add_raw_api_object(contingent_amount)
            contingent_amount_forward_ref = ForwardRef(line,
                                                       contingent_amount_name)
            contingents.append(contingent_amount_forward_ref)

        if not contingents:
            # Do not create the ability if its values are empty
            return None

        ability_raw_api_object.add_raw_member("amount",
                                              contingents,
                                              "engine.ability.type.ProvideContingent")
        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def rally_point_ability(line):
        """
        Adds the RallyPoint ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = f"{game_entity_name}.RallyPoint"
        ability_raw_api_object = RawAPIObject(ability_ref, "RallyPoint", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.RallyPoint")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def regenerate_attribute_ability(line):
        """
        Adds the RegenerateAttribute ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward references for the ability.
        :rtype: list
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        attribute = None
        attribute_name = ""
        if current_unit_id == 125:
            # Monk; regenerates Faith
            attribute = dataset.pregen_nyan_objects["aux.attribute.types.Faith"].get_nyan_object()
            attribute_name = "Faith"

        elif current_unit_id == 692:
            # Berserk: regenerates Health
            attribute = dataset.pregen_nyan_objects["aux.attribute.types.Health"].get_nyan_object()
            attribute_name = "Health"

        else:
            return []

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_name = f"Regenerate{attribute_name}"
        ability_ref = f"{game_entity_name}.{ability_name}"
        ability_raw_api_object = RawAPIObject(ability_ref, ability_name, dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.RegenerateAttribute")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Attribute rate
        # ===============================================================================
        rate_name = f"{attribute_name}Rate"
        rate_ref = f"{game_entity_name}.{ability_name}.{rate_name}"
        rate_raw_api_object = RawAPIObject(rate_ref, rate_name, dataset.nyan_api_objects)
        rate_raw_api_object.add_raw_parent("engine.aux.attribute.AttributeRate")
        rate_location = ForwardRef(line, ability_ref)
        rate_raw_api_object.set_location(rate_location)

        # Attribute
        rate_raw_api_object.add_raw_member("type",
                                           attribute,
                                           "engine.aux.attribute.AttributeRate")

        # Rate
        attribute_rate = 0
        if current_unit_id == 125:
            # stored in civ resources
            attribute_rate = dataset.genie_civs[0]["resources"][35].get_value()

        elif current_unit_id == 692:
            # stored in civ resources, but has to get converted to amount/second
            heal_timer = dataset.genie_civs[0]["resources"][96].get_value()
            attribute_rate = 1 / heal_timer

        rate_raw_api_object.add_raw_member("rate",
                                           attribute_rate,
                                           "engine.aux.attribute.AttributeRate")

        line.add_raw_api_object(rate_raw_api_object)
        # ===============================================================================
        rate_forward_ref = ForwardRef(line, rate_ref)
        ability_raw_api_object.add_raw_member("rate",
                                              rate_forward_ref,
                                              "engine.ability.type.RegenerateAttribute")

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return [ability_forward_ref]

    @staticmethod
    def regenerate_resource_spot_ability(line):
        """
        Adds the RegenerateResourceSpot ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        # Unused in AoC

    @staticmethod
    def remove_storage_ability(line):
        """
        Adds the RemoveStorage ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = f"{game_entity_name}.RemoveStorage"
        ability_raw_api_object = RawAPIObject(ability_ref,
                                              "RemoveStorage",
                                              dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.RemoveStorage")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Container
        container_ref = f"{game_entity_name}.Storage.{game_entity_name}Container"
        container_forward_ref = ForwardRef(line, container_ref)
        ability_raw_api_object.add_raw_member("container",
                                              container_forward_ref,
                                              "engine.ability.type.RemoveStorage")

        # Storage elements
        elements = []
        entity_lookups = internal_name_lookups.get_entity_lookups(dataset.game_version)
        for entity in line.garrison_entities:
            entity_ref = entity_lookups[entity.get_head_unit_id()][0]
            entity_forward_ref = ForwardRef(entity, entity_ref)
            elements.append(entity_forward_ref)

        ability_raw_api_object.add_raw_member("storage_elements",
                                              elements,
                                              "engine.ability.type.RemoveStorage")

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def restock_ability(line, restock_target_id):
        """
        Adds the Restock ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
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
            raise Exception(f"{restock_target} cannot be restocked: is not harvestable")

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
        restock_lookup_dict = internal_name_lookups.get_restock_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]
        ability_ref = f"{game_entity_name}.{restock_lookup_dict[restock_target_id][0]}"
        ability_raw_api_object = RawAPIObject(ability_ref,
                                              restock_lookup_dict[restock_target_id][0],
                                              dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Restock")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        ability_animation_id = -1

        if isinstance(line, GenieVillagerGroup) and restock_target_id == 50:
            # Search for the build graphic of farms
            restock_unit = line.get_units_with_command(101)[0]
            commands = restock_unit["unit_commands"].get_value()
            for command in commands:
                type_id = command["type"].get_value()

                if type_id == 101:
                    ability_animation_id = command["work_sprite_id"].get_value()

        if ability_animation_id > -1:
            # Make the ability animated
            ability_raw_api_object.add_raw_parent("engine.ability.specialization.AnimatedAbility")

            animations_set = []
            animation_forward_ref = AoCAbilitySubprocessor.create_animation(line,
                                                                            ability_animation_id,
                                                                            ability_ref,
                                                                            restock_lookup_dict[restock_target_id][0],
                                                                            "%s_"
                                                                            % restock_lookup_dict[restock_target_id][1])

            animations_set.append(animation_forward_ref)
            ability_raw_api_object.add_raw_member("animations", animations_set,
                                                  "engine.ability.specialization.AnimatedAbility")

        # Auto restock
        ability_raw_api_object.add_raw_member("auto_restock",
                                              True,  # always True since AoC
                                              "engine.ability.type.Restock")

        # Target
        restock_target_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
        restock_target_name = restock_target_lookup_dict[restock_target_id][0]
        spot_forward_ref = ForwardRef(restock_target,
                                      "%s.Harvestable.%sResourceSpot"
                                      % (restock_target_name,
                                         restock_target_name))
        ability_raw_api_object.add_raw_member("target",
                                              spot_forward_ref,
                                              "engine.ability.type.Restock")

        # restock time
        restock_time = restock_target.get_head_unit()["creation_time"].get_value()
        ability_raw_api_object.add_raw_member("restock_time",
                                              restock_time,
                                              "engine.ability.type.Restock")

        # Manual/Auto Cost
        # Link to the same Cost object as Create
        cost_forward_ref = ForwardRef(restock_target,
                                      "%s.CreatableGameEntity.%sCost"
                                      % (restock_target_name, restock_target_name))
        ability_raw_api_object.add_raw_member("manual_cost",
                                              cost_forward_ref,
                                              "engine.ability.type.Restock")
        ability_raw_api_object.add_raw_member("auto_cost",
                                              cost_forward_ref,
                                              "engine.ability.type.Restock")

        # Amount
        restock_amount = restock_target.get_head_unit()["resource_capacity"].get_value()
        if restock_target_id == 50:
            # Farm food amount (hardcoded in civ)
            restock_amount = dataset.genie_civs[1]["resources"][36].get_value()

        elif restock_target_id == 199:
            # Fish trap added food amount (hardcoded in civ)
            restock_amount += dataset.genie_civs[1]["resources"][88].get_value()

        ability_raw_api_object.add_raw_member("amount",
                                              restock_amount,
                                              "engine.ability.type.Restock")

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def research_ability(line):
        """
        Adds the Research ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
        tech_lookup_dict = internal_name_lookups.get_tech_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]
        ability_ref = f"{game_entity_name}.Research"
        ability_raw_api_object = RawAPIObject(ability_ref,
                                              "Research",
                                              dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Research")
        ability_location = ForwardRef(line, game_entity_name)
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
            researchable_name = tech_lookup_dict[researchable_id][0]

            raw_api_object_ref = f"{researchable_name}.ResearchableTech"
            researchable_forward_ref = ForwardRef(researchable,
                                                  raw_api_object_ref)
            researchables_set.append(researchable_forward_ref)

        ability_raw_api_object.add_raw_member("researchables", researchables_set,
                                              "engine.ability.type.Research")
        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def resistance_ability(line):
        """
        Adds the Resistance ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]
        ability_ref = f"{game_entity_name}.Resistance"
        ability_raw_api_object = RawAPIObject(ability_ref,
                                              "Resistance",
                                              dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Resistance")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Resistances
        resistances = []
        resistances.extend(AoCEffectSubprocessor.get_attack_resistances(line, ability_ref))
        if isinstance(line, (GenieUnitLineGroup, GenieBuildingLineGroup)):
            resistances.extend(AoCEffectSubprocessor.get_convert_resistances(line, ability_ref))

            if isinstance(line, GenieUnitLineGroup) and not line.is_repairable():
                resistances.extend(AoCEffectSubprocessor.get_heal_resistances(line, ability_ref))

            if isinstance(line, GenieBuildingLineGroup):
                resistances.extend(AoCEffectSubprocessor.get_construct_resistances(line, ability_ref))

            if line.is_repairable():
                resistances.extend(AoCEffectSubprocessor.get_repair_resistances(line, ability_ref))

        ability_raw_api_object.add_raw_member("resistances",
                                              resistances,
                                              "engine.ability.type.Resistance")

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def resource_storage_ability(line):
        """
        Adds the ResourceStorage ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        if isinstance(line, GenieVillagerGroup):
            gatherers = line.variants[0].line

        else:
            gatherers = [line.line[0]]

        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
        gather_lookup_dict = internal_name_lookups.get_gather_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = f"{game_entity_name}.ResourceStorage"
        ability_raw_api_object = RawAPIObject(ability_ref,
                                              "ResourceStorage",
                                              dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.ResourceStorage")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Create containers
        containers = []
        for gatherer in gatherers:
            unit_commands = gatherer["unit_commands"].get_value()
            resource = None

            used_command = None
            for command in unit_commands:
                # Find a gather ability. It doesn't matter which one because
                # they should all produce the same resource for one genie unit.
                type_id = command["type"].get_value()

                if type_id not in (5, 110):
                    continue

                resource_id = command["resource_out"].get_value()

                # If resource_out is not specified, the gatherer harvests resource_in
                if resource_id == -1:
                    resource_id = command["resource_in"].get_value()

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

                used_command = command

            if not used_command:
                # The unit uses no gathering command or we don't recognize it
                continue

            gatherer_unit_id = gatherer.get_id()
            if gatherer_unit_id not in gather_lookup_dict.keys():
                # Skips hunting wolves
                continue

            container_name = f"{gather_lookup_dict[gatherer_unit_id][0]}Container"

            container_ref = f"{ability_ref}.{container_name}"
            container_raw_api_object = RawAPIObject(container_ref,
                                                    container_name,
                                                    dataset.nyan_api_objects)
            container_raw_api_object.add_raw_parent("engine.aux.storage.ResourceContainer")
            container_location = ForwardRef(line, ability_ref)
            container_raw_api_object.set_location(container_location)

            # Resource
            container_raw_api_object.add_raw_member("resource",
                                                    resource,
                                                    "engine.aux.storage.ResourceContainer")

            # Carry capacity
            carry_capacity = gatherer["resource_capacity"].get_value()
            container_raw_api_object.add_raw_member("capacity",
                                                    carry_capacity,
                                                    "engine.aux.storage.ResourceContainer")

            # Carry progress
            carry_progress = []
            carry_move_animation_id = used_command["carry_sprite_id"].get_value()
            if carry_move_animation_id > -1:
                # ===========================================================================================
                progress_name = "%s.ResourceStorage.%sCarryProgress" % (game_entity_name,
                                                                        container_name)
                progress_raw_api_object = RawAPIObject(progress_name,
                                                       f"{container_name}CarryProgress",
                                                       dataset.nyan_api_objects)
                progress_raw_api_object.add_raw_parent("engine.aux.progress.type.CarryProgress")
                progress_location = ForwardRef(line, container_ref)
                progress_raw_api_object.set_location(progress_location)

                # Interval = (0.0, 100.0)
                progress_raw_api_object.add_raw_member("left_boundary",
                                                       0.0,
                                                       "engine.aux.progress.Progress")
                progress_raw_api_object.add_raw_member("right_boundary",
                                                       100.0,
                                                       "engine.aux.progress.Progress")

                progress_raw_api_object.add_raw_parent("engine.aux.progress.specialization.AnimatedProgress")

                overrides = []
                # ===========================================================================================
                # Move override
                # ===========================================================================================
                override_ref = f"{progress_name}.MoveOverride"
                override_raw_api_object = RawAPIObject(override_ref,
                                                       "MoveOverride",
                                                       dataset.nyan_api_objects)
                override_raw_api_object.add_raw_parent("engine.aux.animation_override.AnimationOverride")
                override_location = ForwardRef(line, progress_name)
                override_raw_api_object.set_location(override_location)

                idle_forward_ref = ForwardRef(line, f"{game_entity_name}.Move")
                override_raw_api_object.add_raw_member("ability",
                                                       idle_forward_ref,
                                                       "engine.aux.animation_override.AnimationOverride")

                # Animation
                animations_set = []
                animation_forward_ref = AoCAbilitySubprocessor.create_animation(line,
                                                                                carry_move_animation_id,
                                                                                override_ref,
                                                                                "Move",
                                                                                "move_carry_override_")

                animations_set.append(animation_forward_ref)
                override_raw_api_object.add_raw_member("animations",
                                                       animations_set,
                                                       "engine.aux.animation_override.AnimationOverride")

                override_raw_api_object.add_raw_member("priority",
                                                       1,
                                                       "engine.aux.animation_override.AnimationOverride")

                override_forward_ref = ForwardRef(line, override_ref)
                overrides.append(override_forward_ref)
                line.add_raw_api_object(override_raw_api_object)
                # ===========================================================================================
                progress_raw_api_object.add_raw_member("overrides",
                                                       overrides,
                                                       "engine.aux.progress.specialization.AnimatedProgress")

                line.add_raw_api_object(progress_raw_api_object)
                # ===========================================================================================
                progress_forward_ref = ForwardRef(line, progress_name)
                carry_progress.append(progress_forward_ref)

            container_raw_api_object.add_raw_member("carry_progress",
                                                    carry_progress,
                                                    "engine.aux.storage.ResourceContainer")

            line.add_raw_api_object(container_raw_api_object)

            container_forward_ref = ForwardRef(line, container_ref)
            containers.append(container_forward_ref)

        ability_raw_api_object.add_raw_member("containers",
                                              containers,
                                              "engine.ability.type.ResourceStorage")

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def selectable_ability(line):
        """
        Adds Selectable abilities to a line. Units will get two of these,
        one Rectangle box for the Self stance and one MatchToSprite box
        for other stances.

        :param line: Unit/Building line that gets the abilities.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the abilities.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        current_unit = line.get_head_unit()
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_refs = (f"{game_entity_name}.Selectable",)
        ability_names = ("Selectable",)

        if isinstance(line, GenieUnitLineGroup):
            ability_refs = (f"{game_entity_name}.SelectableOthers",
                            f"{game_entity_name}.SelectableSelf")
            ability_names = ("SelectableOthers",
                             "SelectableSelf")

        abilities = []

        # First box (MatchToSprite)
        ability_ref = ability_refs[0]
        ability_name = ability_names[0]

        ability_raw_api_object = RawAPIObject(ability_ref, ability_name, dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Selectable")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Selection box
        box_ref = dataset.nyan_api_objects["engine.aux.selection_box.type.MatchToSprite"]
        ability_raw_api_object.add_raw_member("selection_box",
                                              box_ref,
                                              "engine.ability.type.Selectable")

        # Diplomacy setting (for units)
        if isinstance(line, GenieUnitLineGroup):
            ability_raw_api_object.add_raw_parent("engine.ability.specialization.DiplomaticAbility")

            stances = [
                dataset.pregen_nyan_objects["aux.diplomatic_stance.types.Enemy"].get_nyan_object(),
                dataset.pregen_nyan_objects["aux.diplomatic_stance.types.Neutral"].get_nyan_object(),
                dataset.pregen_nyan_objects["aux.diplomatic_stance.types.Friendly"].get_nyan_object()
            ]
            ability_raw_api_object.add_raw_member("stances",
                                                  stances,
                                                  "engine.ability.specialization.DiplomaticAbility")

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        abilities.append(ability_forward_ref)

        if not isinstance(line, GenieUnitLineGroup):
            return abilities

        # Second box (Rectangle)
        ability_ref = ability_refs[1]
        ability_name = ability_names[1]

        ability_raw_api_object = RawAPIObject(ability_ref,
                                              ability_name,
                                              dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Selectable")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Command Sound
        ability_comm_sound_id = current_unit["selection_sound_id"].get_value()
        if ability_comm_sound_id > -1:
            # Make the ability animated
            ability_raw_api_object.add_raw_parent("engine.ability.specialization.CommandSoundAbility")

            sounds_set = []
            sound_forward_ref = AoCAbilitySubprocessor.create_sound(line,
                                                                    ability_comm_sound_id,
                                                                    ability_ref,
                                                                    ability_name,
                                                                    "select_")
            sounds_set.append(sound_forward_ref)
            ability_raw_api_object.add_raw_member("sounds",
                                                  sounds_set,
                                                  "engine.ability.specialization.CommandSoundAbility")

        # Selection box
        box_name = f"{game_entity_name}.SelectableSelf.Rectangle"
        box_raw_api_object = RawAPIObject(box_name, "Rectangle", dataset.nyan_api_objects)
        box_raw_api_object.add_raw_parent("engine.aux.selection_box.type.Rectangle")
        box_location = ForwardRef(line, ability_ref)
        box_raw_api_object.set_location(box_location)

        radius_x = current_unit["selection_shape_x"].get_value()
        box_raw_api_object.add_raw_member("radius_x",
                                          radius_x,
                                          "engine.aux.selection_box.type.Rectangle")

        radius_y = current_unit["selection_shape_y"].get_value()
        box_raw_api_object.add_raw_member("radius_y",
                                          radius_y,
                                          "engine.aux.selection_box.type.Rectangle")

        line.add_raw_api_object(box_raw_api_object)

        box_forward_ref = ForwardRef(line, box_name)
        ability_raw_api_object.add_raw_member("selection_box",
                                              box_forward_ref,
                                              "engine.ability.type.Selectable")

        # Diplomacy settings
        ability_raw_api_object.add_raw_parent("engine.ability.specialization.DiplomaticAbility")

        stances = [dataset.nyan_api_objects["engine.aux.diplomatic_stance.type.Self"]]
        ability_raw_api_object.add_raw_member("stances",
                                              stances,
                                              "engine.ability.specialization.DiplomaticAbility")

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        abilities.append(ability_forward_ref)

        return abilities

    @staticmethod
    def send_back_to_task_ability(line):
        """
        Adds the SendBackToTask ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]
        ability_ref = f"{game_entity_name}.SendBackToTask"
        ability_raw_api_object = RawAPIObject(ability_ref,
                                              "SendBackToTask",
                                              dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.SendBackToTask")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Only works on villagers
        allowed_types = [
            dataset.pregen_nyan_objects["aux.game_entity_type.types.Villager"].get_nyan_object()
        ]
        ability_raw_api_object.add_raw_member("allowed_types",
                                              allowed_types,
                                              "engine.ability.type.SendBackToTask")
        ability_raw_api_object.add_raw_member("blacklisted_entities",
                                              [],
                                              "engine.ability.type.SendBackToTask")

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def shoot_projectile_ability(line, command_id):
        """
        Adds the ShootProjectile ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        current_unit = line.get_head_unit()
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
        command_lookup_dict = internal_name_lookups.get_command_lookups(dataset.game_version)

        ability_name = command_lookup_dict[command_id][0]

        game_entity_name = name_lookup_dict[current_unit_id][0]
        ability_ref = f"{game_entity_name}.{ability_name}"
        ability_raw_api_object = RawAPIObject(ability_ref,
                                              ability_name,
                                              dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.ShootProjectile")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        ability_animation_id = current_unit["attack_sprite_id"].get_value()

        if ability_animation_id > -1:
            # Make the ability animated
            ability_raw_api_object.add_raw_parent("engine.ability.specialization.AnimatedAbility")

            animations_set = []
            animation_forward_ref = AoCAbilitySubprocessor.create_animation(line,
                                                                            ability_animation_id,
                                                                            ability_ref,
                                                                            ability_name,
                                                                            "%s_"
                                                                            % command_lookup_dict[command_id][1])
            animations_set.append(animation_forward_ref)
            ability_raw_api_object.add_raw_member("animations",
                                                  animations_set,
                                                  "engine.ability.specialization.AnimatedAbility")

        # Command Sound
        ability_comm_sound_id = current_unit["command_sound_id"].get_value()
        if ability_comm_sound_id > -1:
            # Make the ability animated
            ability_raw_api_object.add_raw_parent("engine.ability.specialization.CommandSoundAbility")

            sounds_set = []
            sound_forward_ref = AoCAbilitySubprocessor.create_sound(line,
                                                                    ability_comm_sound_id,
                                                                    ability_ref,
                                                                    ability_name,
                                                                    "command_")
            sounds_set.append(sound_forward_ref)
            ability_raw_api_object.add_raw_member("sounds",
                                                  sounds_set,
                                                  "engine.ability.specialization.CommandSoundAbility")

        # Projectile
        projectiles = []
        projectile_primary = current_unit["attack_projectile_primary_unit_id"].get_value()
        if projectile_primary > -1:
            projectiles.append(ForwardRef(line,
                                          f"{game_entity_name}.ShootProjectile.Projectile0"))

        projectile_secondary = current_unit["attack_projectile_secondary_unit_id"].get_value()
        if projectile_secondary > -1:
            projectiles.append(ForwardRef(line,
                                          f"{game_entity_name}.ShootProjectile.Projectile1"))

        ability_raw_api_object.add_raw_member("projectiles",
                                              projectiles,
                                              "engine.ability.type.ShootProjectile")

        # Projectile count
        min_projectiles = current_unit["attack_projectile_count"].get_value()
        max_projectiles = current_unit["attack_projectile_max_count"].get_value()

        if projectile_primary == -1:
            # Special case where only the second projectile is defined (town center)
            # The min/max projectile count is lowered by 1 in this case
            min_projectiles -= 1
            max_projectiles -= 1

        elif min_projectiles == 0 and max_projectiles == 0:
            # If there's a primary projectile defined, but these values are 0,
            # the game still fires a projectile on attack.
            min_projectiles += 1
            max_projectiles += 1

        if current_unit_id == 236:
            # Bombard Tower (gets treated like a tower for max projectiles)
            max_projectiles = 5

        ability_raw_api_object.add_raw_member("min_projectiles",
                                              min_projectiles,
                                              "engine.ability.type.ShootProjectile")
        ability_raw_api_object.add_raw_member("max_projectiles",
                                              max_projectiles,
                                              "engine.ability.type.ShootProjectile")

        # Range
        min_range = current_unit["weapon_range_min"].get_value()
        ability_raw_api_object.add_raw_member("min_range",
                                              min_range,
                                              "engine.ability.type.ShootProjectile")

        max_range = current_unit["weapon_range_max"].get_value()
        ability_raw_api_object.add_raw_member("max_range",
                                              max_range,
                                              "engine.ability.type.ShootProjectile")

        # Reload time and delay
        reload_time = current_unit["attack_speed"].get_value()
        ability_raw_api_object.add_raw_member("reload_time",
                                              reload_time,
                                              "engine.ability.type.ShootProjectile")

        if ability_animation_id > -1:
            animation = dataset.genie_graphics[ability_animation_id]
            frame_rate = animation.get_frame_rate()

        else:
            frame_rate = 0

        spawn_delay_frames = current_unit["frame_delay"].get_value()
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
        manual_aiming_allowed = line.get_head_unit_id() in (280, 331)

        ability_raw_api_object.add_raw_member("manual_aiming_allowed",
                                              manual_aiming_allowed,
                                              "engine.ability.type.ShootProjectile")

        # Spawning area
        spawning_area_offset_x = current_unit["weapon_offset"][0].get_value()
        spawning_area_offset_y = current_unit["weapon_offset"][1].get_value()
        spawning_area_offset_z = current_unit["weapon_offset"][2].get_value()

        ability_raw_api_object.add_raw_member("spawning_area_offset_x",
                                              spawning_area_offset_x,
                                              "engine.ability.type.ShootProjectile")
        ability_raw_api_object.add_raw_member("spawning_area_offset_y",
                                              spawning_area_offset_y,
                                              "engine.ability.type.ShootProjectile")
        ability_raw_api_object.add_raw_member("spawning_area_offset_z",
                                              spawning_area_offset_z,
                                              "engine.ability.type.ShootProjectile")

        spawning_area_width = current_unit["attack_projectile_spawning_area_width"].get_value()
        spawning_area_height = current_unit["attack_projectile_spawning_area_length"].get_value()
        spawning_area_randomness = current_unit["attack_projectile_spawning_area_randomness"].get_value()

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
        allowed_types = [
            dataset.pregen_nyan_objects["aux.game_entity_type.types.Building"].get_nyan_object(),
            dataset.pregen_nyan_objects["aux.game_entity_type.types.Unit"].get_nyan_object()
        ]
        ability_raw_api_object.add_raw_member("allowed_types",
                                              allowed_types,
                                              "engine.ability.type.ShootProjectile")
        ability_raw_api_object.add_raw_member("blacklisted_entities",
                                              [],
                                              "engine.ability.type.ShootProjectile")

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def stop_ability(line):
        """
        Adds the Stop ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = f"{game_entity_name}.Stop"
        ability_raw_api_object = RawAPIObject(ability_ref, "Stop", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Stop")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Diplomacy settings
        ability_raw_api_object.add_raw_parent("engine.ability.specialization.DiplomaticAbility")
        diplomatic_stances = [dataset.nyan_api_objects["engine.aux.diplomatic_stance.type.Self"]]
        ability_raw_api_object.add_raw_member("stances", diplomatic_stances,
                                              "engine.ability.specialization.DiplomaticAbility")

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def storage_ability(line):
        """
        Adds the Storage ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        current_unit = line.get_head_unit()
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = f"{game_entity_name}.Storage"
        ability_raw_api_object = RawAPIObject(ability_ref, "Storage", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Storage")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Container
        # ==============================================================================
        container_name = f"{game_entity_name}.Storage.{game_entity_name}Container"
        container_raw_api_object = RawAPIObject(container_name,
                                                f"{game_entity_name}Container",
                                                dataset.nyan_api_objects)
        container_raw_api_object.add_raw_parent("engine.aux.storage.Container")
        container_location = ForwardRef(line, ability_ref)
        container_raw_api_object.set_location(container_location)

        garrison_mode = line.get_garrison_mode()

        # Allowed types
        # TODO: Any should be fine for now, since Enter/Exit abilities limit the stored elements
        allowed_types = [dataset.nyan_api_objects["engine.aux.game_entity_type.type.Any"]]

        container_raw_api_object.add_raw_member("allowed_types",
                                                allowed_types,
                                                "engine.aux.storage.Container")

        # Blacklisted entities
        container_raw_api_object.add_raw_member("blacklisted_entities",
                                                [],
                                                "engine.aux.storage.Container")

        # Define storage elements
        storage_element_defs = []
        if garrison_mode is GenieGarrisonMode.UNIT_GARRISON:
            for storage_element in line.garrison_entities:
                storage_element_name = name_lookup_dict[storage_element.get_head_unit_id()][0]
                storage_def_ref = "%s.Storage.%sContainer.%sStorageDef" % (game_entity_name,
                                                                           game_entity_name,
                                                                           storage_element_name)
                storage_def_raw_api_object = RawAPIObject(storage_def_ref,
                                                          f"{storage_element_name}StorageDef",
                                                          dataset.nyan_api_objects)
                storage_def_raw_api_object.add_raw_parent("engine.aux.storage.StorageElementDefinition")
                storage_def_location = ForwardRef(line, container_name)
                storage_def_raw_api_object.set_location(storage_def_location)

                # Storage element
                storage_element_forward_ref = ForwardRef(storage_element, storage_element_name)
                storage_def_raw_api_object.add_raw_member("storage_element",
                                                          storage_element_forward_ref,
                                                          "engine.aux.storage.StorageElementDefinition")

                # Elements per slot
                storage_def_raw_api_object.add_raw_member("elements_per_slot",
                                                          1,
                                                          "engine.aux.storage.StorageElementDefinition")

                # Conflicts
                storage_def_raw_api_object.add_raw_member("conflicts",
                                                          [],
                                                          "engine.aux.storage.StorageElementDefinition")

                # TODO: State change (optional) -> speed boost

                storage_def_forward_ref = ForwardRef(storage_element, storage_element_name)
                storage_element_defs.append(storage_def_forward_ref)
                line.add_raw_api_object(storage_def_raw_api_object)

        container_raw_api_object.add_raw_member("storage_element_defs",
                                                storage_element_defs,
                                                "engine.aux.storage.Container")

        # Container slots
        slots = current_unit["garrison_capacity"].get_value()
        if garrison_mode is GenieGarrisonMode.MONK:
            slots = 1

        container_raw_api_object.add_raw_member("slots",
                                                slots,
                                                "engine.aux.storage.Container")

        # Carry progress
        carry_progress = []
        if garrison_mode is GenieGarrisonMode.MONK and isinstance(line, GenieMonkGroup):
            switch_unit = line.get_switch_unit()
            carry_idle_animation_id = switch_unit["idle_graphic0"].get_value()
            carry_move_animation_id = switch_unit["move_graphics"].get_value()

            progress_name = f"{game_entity_name}.Storage.CarryProgress"
            progress_raw_api_object = RawAPIObject(progress_name,
                                                   "CarryProgress",
                                                   dataset.nyan_api_objects)
            progress_raw_api_object.add_raw_parent("engine.aux.progress.type.CarryProgress")
            progress_location = ForwardRef(line, ability_ref)
            progress_raw_api_object.set_location(progress_location)

            # Interval = (0.0, 100.0)
            progress_raw_api_object.add_raw_member("left_boundary",
                                                   0.0,
                                                   "engine.aux.progress.Progress")
            progress_raw_api_object.add_raw_member("right_boundary",
                                                   100.0,
                                                   "engine.aux.progress.Progress")

            progress_raw_api_object.add_raw_parent("engine.aux.progress.specialization.AnimatedProgress")

            # =====================================================================================
            overrides = []
            # Idle override
            # ===========================================================================================
            override_ref = f"{game_entity_name}.Storage.CarryProgress.IdleOverride"
            override_raw_api_object = RawAPIObject(override_ref,
                                                   "IdleOverride",
                                                   dataset.nyan_api_objects)
            override_raw_api_object.add_raw_parent("engine.aux.animation_override.AnimationOverride")
            override_location = ForwardRef(line, progress_name)
            override_raw_api_object.set_location(override_location)

            idle_forward_ref = ForwardRef(line, f"{game_entity_name}.Idle")
            override_raw_api_object.add_raw_member("ability",
                                                   idle_forward_ref,
                                                   "engine.aux.animation_override.AnimationOverride")

            # Animation
            animations_set = []
            animation_forward_ref = AoCAbilitySubprocessor.create_animation(line,
                                                                            carry_idle_animation_id,
                                                                            override_ref,
                                                                            "Idle",
                                                                            "idle_carry_override_")

            animations_set.append(animation_forward_ref)
            override_raw_api_object.add_raw_member("animations",
                                                   animations_set,
                                                   "engine.aux.animation_override.AnimationOverride")

            override_raw_api_object.add_raw_member("priority",
                                                   1,
                                                   "engine.aux.animation_override.AnimationOverride")

            override_forward_ref = ForwardRef(line, override_ref)
            overrides.append(override_forward_ref)
            line.add_raw_api_object(override_raw_api_object)
            # ===========================================================================================
            # Move override
            # ===========================================================================================
            override_ref = f"{game_entity_name}.Storage.CarryProgress.MoveOverride"
            override_raw_api_object = RawAPIObject(override_ref,
                                                   "MoveOverride",
                                                   dataset.nyan_api_objects)
            override_raw_api_object.add_raw_parent("engine.aux.animation_override.AnimationOverride")
            override_location = ForwardRef(line, progress_name)
            override_raw_api_object.set_location(override_location)

            idle_forward_ref = ForwardRef(line, f"{game_entity_name}.Move")
            override_raw_api_object.add_raw_member("ability",
                                                   idle_forward_ref,
                                                   "engine.aux.animation_override.AnimationOverride")

            # Animation
            animations_set = []
            animation_forward_ref = AoCAbilitySubprocessor.create_animation(line,
                                                                            carry_move_animation_id,
                                                                            override_ref,
                                                                            "Move",
                                                                            "move_carry_override_")

            animations_set.append(animation_forward_ref)
            override_raw_api_object.add_raw_member("animations",
                                                   animations_set,
                                                   "engine.aux.animation_override.AnimationOverride")

            override_raw_api_object.add_raw_member("priority",
                                                   1,
                                                   "engine.aux.animation_override.AnimationOverride")

            override_forward_ref = ForwardRef(line, override_ref)
            overrides.append(override_forward_ref)
            line.add_raw_api_object(override_raw_api_object)
            # ===========================================================================================
            progress_raw_api_object.add_raw_member("overrides",
                                                   overrides,
                                                   "engine.aux.progress.specialization.AnimatedProgress")

            progress_raw_api_object.add_raw_parent("engine.aux.progress.specialization.StateChangeProgress")

            # State change
            # =====================================================================================
            carry_state_name = f"{progress_name}.CarryRelicState"
            carry_state_raw_api_object = RawAPIObject(carry_state_name,
                                                      "CarryRelicState",
                                                      dataset.nyan_api_objects)
            carry_state_raw_api_object.add_raw_parent("engine.aux.state_machine.StateChanger")
            carry_state_location = ForwardRef(line, progress_name)
            carry_state_raw_api_object.set_location(carry_state_location)

            # Priority
            carry_state_raw_api_object.add_raw_member("priority",
                                                      1,
                                                      "engine.aux.state_machine.StateChanger")

            # Enabled abilities
            carry_state_raw_api_object.add_raw_member("enable_abilities",
                                                      [],
                                                      "engine.aux.state_machine.StateChanger")

            # Disabled abilities
            disabled_forward_refs = []

            if line.has_command(104):
                disabled_forward_refs.append(ForwardRef(line,
                                                        f"{game_entity_name}.Convert"))

            if line.has_command(105):
                disabled_forward_refs.append(ForwardRef(line,
                                                        f"{game_entity_name}.Heal"))

            carry_state_raw_api_object.add_raw_member("disable_abilities",
                                                      disabled_forward_refs,
                                                      "engine.aux.state_machine.StateChanger")

            # Enabled modifiers
            carry_state_raw_api_object.add_raw_member("enable_modifiers",
                                                      [],
                                                      "engine.aux.state_machine.StateChanger")

            # Disabled modifiers
            carry_state_raw_api_object.add_raw_member("disable_modifiers",
                                                      [],
                                                      "engine.aux.state_machine.StateChanger")

            line.add_raw_api_object(carry_state_raw_api_object)
            # =====================================================================================
            init_state_forward_ref = ForwardRef(line, carry_state_name)
            progress_raw_api_object.add_raw_member("state_change",
                                                   init_state_forward_ref,
                                                   "engine.aux.progress.specialization.StateChangeProgress")
            # =====================================================================================
            line.add_raw_api_object(progress_raw_api_object)
            progress_forward_ref = ForwardRef(line, progress_name)
            carry_progress.append(progress_forward_ref)

        else:
            # Garrison graphics
            if current_unit.has_member("garrison_graphic"):
                garrison_animation_id = current_unit["garrison_graphic"].get_value()

            else:
                garrison_animation_id = -1

            if garrison_animation_id > -1:
                progress_name = f"{game_entity_name}.Storage.CarryProgress"
                progress_raw_api_object = RawAPIObject(progress_name,
                                                       "CarryProgress",
                                                       dataset.nyan_api_objects)
                progress_raw_api_object.add_raw_parent("engine.aux.progress.type.CarryProgress")
                progress_location = ForwardRef(line, ability_ref)
                progress_raw_api_object.set_location(progress_location)

                # Interval = (0.0, 100.0)
                progress_raw_api_object.add_raw_member("left_boundary",
                                                       0.0,
                                                       "engine.aux.progress.Progress")
                progress_raw_api_object.add_raw_member("right_boundary",
                                                       100.0,
                                                       "engine.aux.progress.Progress")

                progress_raw_api_object.add_raw_parent("engine.aux.progress.specialization.AnimatedProgress")
                # ===========================================================================================
                override_ref = f"{game_entity_name}.Storage.CarryProgress.IdleOverride"
                override_raw_api_object = RawAPIObject(override_ref,
                                                       "IdleOverride",
                                                       dataset.nyan_api_objects)
                override_raw_api_object.add_raw_parent("engine.aux.animation_override.AnimationOverride")
                override_location = ForwardRef(line, progress_name)
                override_raw_api_object.set_location(override_location)

                idle_forward_ref = ForwardRef(line, f"{game_entity_name}.Idle")
                override_raw_api_object.add_raw_member("ability",
                                                       idle_forward_ref,
                                                       "engine.aux.animation_override.AnimationOverride")

                # Animation
                animations_set = []
                animation_forward_ref = AoCAbilitySubprocessor.create_animation(line,
                                                                                garrison_animation_id,
                                                                                override_ref,
                                                                                "Idle",
                                                                                "idle_garrison_override_")

                animations_set.append(animation_forward_ref)
                override_raw_api_object.add_raw_member("animations",
                                                       animations_set,
                                                       "engine.aux.animation_override.AnimationOverride")

                override_raw_api_object.add_raw_member("priority",
                                                       1,
                                                       "engine.aux.animation_override.AnimationOverride")

                line.add_raw_api_object(override_raw_api_object)
                # ===========================================================================================
                override_forward_ref = ForwardRef(line, override_ref)
                progress_raw_api_object.add_raw_member("overrides",
                                                       [override_forward_ref],
                                                       "engine.aux.progress.specialization.AnimatedProgress")

                progress_forward_ref = ForwardRef(line, progress_name)
                carry_progress.append(progress_forward_ref)
                line.add_raw_api_object(progress_raw_api_object)

        container_raw_api_object.add_raw_member("carry_progress",
                                                carry_progress,
                                                "engine.aux.storage.Container")

        line.add_raw_api_object(container_raw_api_object)
        # ==============================================================================
        container_forward_ref = ForwardRef(line, container_name)
        ability_raw_api_object.add_raw_member("container",
                                              container_forward_ref,
                                              "engine.ability.type.Storage")

        # Empty condition
        if garrison_mode in (GenieGarrisonMode.UNIT_GARRISON, GenieGarrisonMode.MONK):
            # Empty before death
            condition = [dataset.pregen_nyan_objects["aux.logic.literal.death.StandardHealthDeathLiteral"].get_nyan_object()]

        elif garrison_mode in (GenieGarrisonMode.NATURAL, GenieGarrisonMode.SELF_PRODUCED):
            # Empty when HP < 20%
            condition = [dataset.pregen_nyan_objects["aux.logic.literal.garrison.BuildingDamageEmpty"].get_nyan_object()]

        else:
            # Never empty automatically (transport ships)
            condition = []

        ability_raw_api_object.add_raw_member("empty_condition",
                                              condition,
                                              "engine.ability.type.Storage")

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def terrain_requirement_ability(line):
        """
        Adds the TerrainRequirement to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward references for the abilities.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        current_unit = line.get_head_unit()
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
        terrain_type_lookup_dict = internal_name_lookups.get_terrain_type_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = f"{game_entity_name}.TerrainRequirement"
        ability_raw_api_object = RawAPIObject(ability_ref,
                                              "TerrainRequirement",
                                              dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.TerrainRequirement")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Allowed types
        allowed_types = []
        terrain_restriction = current_unit["terrain_restriction"].get_value()
        for terrain_type in terrain_type_lookup_dict.values():
            # Check if terrain type is covered by terrain restriction
            if terrain_restriction in terrain_type[1]:
                type_name = f"aux.terrain_type.types.{terrain_type[2]}"
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

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def trade_ability(line):
        """
        Adds the Trade ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        current_unit = line.get_head_unit()
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = f"{game_entity_name}.Trade"
        ability_raw_api_object = RawAPIObject(ability_ref, "Trade", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Trade")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Trade route (use the trade route o the market)
        trade_routes = []

        trade_post_id = -1
        unit_commands = current_unit["unit_commands"].get_value()
        for command in unit_commands:
            # Find the trade command and the trade post id
            type_id = command["type"].get_value()

            if type_id != 111:
                continue

            trade_post_id = command["unit_id"].get_value()
            if trade_post_id not in dataset.building_lines.keys():
                # Skips trade workshop
                continue

            trade_post_line = dataset.building_lines[trade_post_id]
            trade_post_name = name_lookup_dict[trade_post_id][0]

            trade_route_ref = f"{trade_post_name}.TradePost.AoE2{trade_post_name}TradeRoute"
            trade_route_forward_ref = ForwardRef(trade_post_line, trade_route_ref)
            trade_routes.append(trade_route_forward_ref)

        ability_raw_api_object.add_raw_member("trade_routes",
                                              trade_routes,
                                              "engine.ability.type.Trade")

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def trade_post_ability(line):
        """
        Adds the TradePost ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = f"{game_entity_name}.TradePost"
        ability_raw_api_object = RawAPIObject(ability_ref,
                                              "TradePost",
                                              dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.TradePost")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Trade route
        trade_routes = []
        # =====================================================================================
        trade_route_name = f"AoE2{game_entity_name}TradeRoute"
        trade_route_ref = f"{game_entity_name}.TradePost.{trade_route_name}"
        trade_route_raw_api_object = RawAPIObject(trade_route_ref,
                                                  trade_route_name,
                                                  dataset.nyan_api_objects)
        trade_route_raw_api_object.add_raw_parent("engine.aux.trade_route.type.AoE2TradeRoute")
        trade_route_location = ForwardRef(line, ability_ref)
        trade_route_raw_api_object.set_location(trade_route_location)

        # Trade resource
        resource = dataset.pregen_nyan_objects["aux.resource.types.Gold"].get_nyan_object()
        trade_route_raw_api_object.add_raw_member("trade_resource",
                                                  resource,
                                                  "engine.aux.trade_route.TradeRoute")

        # Start- and endpoints
        market_forward_ref = ForwardRef(line, game_entity_name)
        trade_route_raw_api_object.add_raw_member("start_trade_post",
                                                  market_forward_ref,
                                                  "engine.aux.trade_route.TradeRoute")
        trade_route_raw_api_object.add_raw_member("end_trade_post",
                                                  market_forward_ref,
                                                  "engine.aux.trade_route.TradeRoute")

        trade_route_forward_ref = ForwardRef(line, trade_route_ref)
        trade_routes.append(trade_route_forward_ref)

        line.add_raw_api_object(trade_route_raw_api_object)
        # =====================================================================================
        ability_raw_api_object.add_raw_member("trade_routes",
                                              trade_routes,
                                              "engine.ability.type.TradePost")

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def transfer_storage_ability(line):
        """
        Adds the TransferStorage ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef, None
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = f"{game_entity_name}.TransferStorage"
        ability_raw_api_object = RawAPIObject(ability_ref,
                                              "TransferStorage",
                                              dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.TransferStorage")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # storage element
        storage_entity = None
        garrisoned_forward_ref = None
        for garrisoned in line.garrison_entities:
            creatable_type = garrisoned.get_head_unit()["creatable_type"].get_value()

            if creatable_type == 4:
                storage_name = name_lookup_dict[garrisoned.get_id()][0]
                storage_entity = garrisoned
                garrisoned_forward_ref = ForwardRef(storage_entity, storage_name)

                break

        ability_raw_api_object.add_raw_member("storage_element",
                                              garrisoned_forward_ref,
                                              "engine.ability.type.TransferStorage")

        # Source container
        source_ref = f"{game_entity_name}.Storage.{game_entity_name}Container"
        source_forward_ref = ForwardRef(line, source_ref)
        ability_raw_api_object.add_raw_member("source_container",
                                              source_forward_ref,
                                              "engine.ability.type.TransferStorage")

        # Target container
        target = None
        unit_commands = line.get_switch_unit()["unit_commands"].get_value()
        for command in unit_commands:
            type_id = command["type"].get_value()

            # Deposit
            if type_id == 136:
                target_id = command["unit_id"].get_value()
                target = dataset.building_lines[target_id]

        target_name = name_lookup_dict[target.get_id()][0]
        target_ref = f"{target_name}.Storage.{target_name}Container"
        target_forward_ref = ForwardRef(target, target_ref)
        ability_raw_api_object.add_raw_member("target_container",
                                              target_forward_ref,
                                              "engine.ability.type.TransferStorage")

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def turn_ability(line):
        """
        Adds the Turn ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        current_unit = line.get_head_unit()
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = f"{game_entity_name}.Turn"
        ability_raw_api_object = RawAPIObject(ability_ref,
                                              "Turn",
                                              dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Turn")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Speed
        turn_speed_unmodified = current_unit["turn_speed"].get_value()

        # Default case: Instant turning
        turn_speed = MemberSpecialValue.NYAN_INF

        # Ships/Trebuchets turn slower
        if turn_speed_unmodified > 0:
            turn_yaw = current_unit["max_yaw_per_sec_moving"].get_value()
            turn_speed = degrees(turn_yaw)

        ability_raw_api_object.add_raw_member("turn_speed",
                                              turn_speed,
                                              "engine.ability.type.Turn")

        # Diplomacy settings
        ability_raw_api_object.add_raw_parent("engine.ability.specialization.DiplomaticAbility")
        diplomatic_stances = [dataset.nyan_api_objects["engine.aux.diplomatic_stance.type.Self"]]
        ability_raw_api_object.add_raw_member("stances", diplomatic_stances,
                                              "engine.ability.specialization.DiplomaticAbility")

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def use_contingent_ability(line):
        """
        Adds the UseContingent ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        current_unit = line.get_head_unit()
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = f"{game_entity_name}.UseContingent"
        ability_raw_api_object = RawAPIObject(ability_ref,
                                              "UseContingent",
                                              dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.UseContingent")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Also stores the pop space
        resource_storage = current_unit["resource_storage"].get_value()

        contingents = []
        for storage in resource_storage:
            type_id = storage["type"].get_value()

            if type_id == 11:
                resource = dataset.pregen_nyan_objects["aux.resource.types.PopulationSpace"].get_nyan_object()
                resource_name = "PopSpace"

            else:
                continue

            amount = storage["amount"].get_value()

            contingent_amount_name = f"{game_entity_name}.UseContingent.{resource_name}"
            contingent_amount = RawAPIObject(contingent_amount_name, resource_name,
                                             dataset.nyan_api_objects)
            contingent_amount.add_raw_parent("engine.aux.resource.ResourceAmount")
            ability_forward_ref = ForwardRef(line, ability_ref)
            contingent_amount.set_location(ability_forward_ref)

            contingent_amount.add_raw_member("type",
                                             resource,
                                             "engine.aux.resource.ResourceAmount")
            contingent_amount.add_raw_member("amount",
                                             amount,
                                             "engine.aux.resource.ResourceAmount")

            line.add_raw_api_object(contingent_amount)
            contingent_amount_forward_ref = ForwardRef(line,
                                                       contingent_amount_name)
            contingents.append(contingent_amount_forward_ref)

        if not contingents:
            # Do not create the ability if its values are empty
            return None

        ability_raw_api_object.add_raw_member("amount",
                                              contingents,
                                              "engine.ability.type.UseContingent")
        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        return ability_forward_ref

    @staticmethod
    def visibility_ability(line):
        """
        Adds the Visibility ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = f"{game_entity_name}.Visibility"
        ability_raw_api_object = RawAPIObject(ability_ref, "Visibility", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Visibility")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Units are not visible in fog...
        visible = False

        # ...Buidings and scenery is though
        if isinstance(line, (GenieBuildingLineGroup, GenieAmbientGroup)):
            visible = True

        ability_raw_api_object.add_raw_member("visible_in_fog", visible,
                                              "engine.ability.type.Visibility")

        # Diplomacy settings
        ability_raw_api_object.add_raw_parent("engine.ability.specialization.DiplomaticAbility")
        diplomatic_stances = [
            dataset.nyan_api_objects["engine.aux.diplomatic_stance.type.Self"],
            dataset.pregen_nyan_objects["aux.diplomatic_stance.types.Friendly"].get_nyan_object(),
            dataset.pregen_nyan_objects["aux.diplomatic_stance.types.Neutral"].get_nyan_object(),
            dataset.pregen_nyan_objects["aux.diplomatic_stance.types.Enemy"].get_nyan_object(),
            dataset.pregen_nyan_objects["aux.diplomatic_stance.types.Gaia"].get_nyan_object()
        ]
        ability_raw_api_object.add_raw_member("stances", diplomatic_stances,
                                              "engine.ability.specialization.DiplomaticAbility")

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        # Add another Visibility ability for buildings with construction progress = 0.0
        # It is not returned by this method, but referenced by the Constructable ability
        if isinstance(line, GenieBuildingLineGroup):
            ability_ref = f"{game_entity_name}.VisibilityConstruct0"
            ability_raw_api_object = RawAPIObject(ability_ref,
                                                  "VisibilityConstruct0",
                                                  dataset.nyan_api_objects)
            ability_raw_api_object.add_raw_parent("engine.ability.type.Visibility")
            ability_location = ForwardRef(line, game_entity_name)
            ability_raw_api_object.set_location(ability_location)

            # The construction site is not visible in fog
            visible = False

            ability_raw_api_object.add_raw_member("visible_in_fog", visible,
                                                  "engine.ability.type.Visibility")

            # Diplomacy settings
            ability_raw_api_object.add_raw_parent("engine.ability.specialization.DiplomaticAbility")
            # Only the player and friendly players can see the construction site
            diplomatic_stances = [
                dataset.nyan_api_objects["engine.aux.diplomatic_stance.type.Self"],
                dataset.pregen_nyan_objects["aux.diplomatic_stance.types.Friendly"].get_nyan_object()
            ]
            ability_raw_api_object.add_raw_member("stances", diplomatic_stances,
                                                  "engine.ability.specialization.DiplomaticAbility")

            line.add_raw_api_object(ability_raw_api_object)

        return ability_forward_ref

    @staticmethod
    def create_animation(line, animation_id, ability_ref, ability_name, filename_prefix):
        """
        Generates an animation for an ability.

        :param line: ConverterObjectGroup that the animation object is added to.
        :type line: ConverterObjectGroup
        :param animation_id: ID of the animation in the dataset.
        :type animation_id: int
        :param ability_ref: Reference of the ability object the animation is nested in.
        :type ability_ref: str
        :param ability_name: Name of the ability object.
        :type ability_name: str
        :param filename_prefix: Prefix for the animation PNG and sprite files.
        :type filename_prefix: str
        """
        dataset = line.data
        head_unit_id = line.get_head_unit_id()

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        animation_ref = f"{ability_ref}.{ability_name}Animation"
        animation_obj_name = f"{ability_name}Animation"
        animation_raw_api_object = RawAPIObject(animation_ref, animation_obj_name,
                                                dataset.nyan_api_objects)
        animation_raw_api_object.add_raw_parent("engine.aux.graphics.Animation")
        animation_location = ForwardRef(line, ability_ref)
        animation_raw_api_object.set_location(animation_location)

        if animation_id in dataset.combined_sprites.keys():
            ability_sprite = dataset.combined_sprites[animation_id]

        else:
            ability_sprite = CombinedSprite(animation_id,
                                            "%s%s" % (filename_prefix,
                                                      name_lookup_dict[head_unit_id][1]),
                                            dataset)
            dataset.combined_sprites.update({ability_sprite.get_id(): ability_sprite})

        ability_sprite.add_reference(animation_raw_api_object)

        animation_raw_api_object.add_raw_member("sprite", ability_sprite,
                                                "engine.aux.graphics.Animation")

        line.add_raw_api_object(animation_raw_api_object)

        animation_forward_ref = ForwardRef(line, animation_ref)

        return animation_forward_ref

    @staticmethod
    def create_civ_animation(line, civ_group, animation_id, ability_ref,
                             ability_name, filename_prefix, exists=False):
        """
        Generates an animation as a patch for a civ.

        :param line: ConverterObjectGroup that the animation object is added to.
        :type line: ConverterObjectGroup
        :param civ_group: ConverterObjectGroup that patches the animation object into the ability.
        :type civ_group: ConverterObjectGroup
        :param animation_id: ID of the animation in the dataset.
        :type animation_id: int
        :param ability_ref: Reference of the ability object the animation is nested in.
        :type ability_ref: str
        :param ability_name: Name of the ability object.
        :type ability_name: str
        :param filename_prefix: Prefix for the animation PNG and sprite files.
        :type filename_prefix: str
        :param exists: Tells the method if the animation object has already been created.
        :type exists: bool
        """
        dataset = civ_group.data
        head_unit_id = line.get_head_unit_id()
        civ_id = civ_group.get_id()

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
        civ_lookup_dict = internal_name_lookups.get_civ_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[head_unit_id][0]
        civ_name = civ_lookup_dict[civ_id][0]

        patch_target_ref = f"{ability_ref}"
        patch_target_forward_ref = ForwardRef(line, patch_target_ref)

        # Wrapper
        wrapper_name = f"{game_entity_name}{ability_name}AnimationWrapper"
        wrapper_ref = f"{civ_name}.{wrapper_name}"
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")
        wrapper_raw_api_object.set_location(ForwardRef(civ_group, civ_name))

        # Nyan patch
        nyan_patch_name = f"{game_entity_name}{ability_name}Animation"
        nyan_patch_ref = f"{civ_name}.{wrapper_name}.{nyan_patch_name}"
        nyan_patch_location = ForwardRef(civ_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

        if animation_id > -1:
            # If the animation object already exists, we do not need to create it again
            if exists:
                # Point to a previously created animation object
                animation_ref = f"{ability_ref}.{ability_name}Animation"
                animation_forward_ref = ForwardRef(line, animation_ref)

            else:
                # Create the animation object
                animation_forward_ref = AoCAbilitySubprocessor.create_animation(line,
                                                                                animation_id,
                                                                                ability_ref,
                                                                                ability_name,
                                                                                filename_prefix)

            # Patch animation into ability
            nyan_patch_raw_api_object.add_raw_patch_member(
                "animations",
                [animation_forward_ref],
                "engine.ability.specialization.AnimatedAbility",
                MemberOperator.ASSIGN
            )

        else:
            # No animation -> empty the set
            nyan_patch_raw_api_object.add_raw_patch_member(
                "animations",
                [],
                "engine.ability.specialization.AnimatedAbility",
                MemberOperator.ASSIGN
            )

        patch_forward_ref = ForwardRef(civ_group, nyan_patch_ref)
        wrapper_raw_api_object.add_raw_member("patch",
                                              patch_forward_ref,
                                              "engine.aux.patch.Patch")

        civ_group.add_raw_api_object(wrapper_raw_api_object)
        civ_group.add_raw_api_object(nyan_patch_raw_api_object)

        # Add patch to civ_setup
        civ_forward_ref = ForwardRef(civ_group, civ_name)
        wrapper_forward_ref = ForwardRef(civ_group, wrapper_ref)
        push_object = RawMemberPush(civ_forward_ref,
                                    "civ_setup",
                                    "engine.aux.civilization.Civilization",
                                    [wrapper_forward_ref])
        civ_group.add_raw_member_push(push_object)

    @staticmethod
    def create_sound(line, sound_id, ability_ref, ability_name, filename_prefix):
        """
        Generates a sound for an ability.
        """
        dataset = line.data

        sound_ref = f"{ability_ref}.{ability_name}Sound"
        sound_obj_name = f"{ability_name}Sound"
        sound_raw_api_object = RawAPIObject(sound_ref, sound_obj_name,
                                            dataset.nyan_api_objects)
        sound_raw_api_object.add_raw_parent("engine.aux.sound.Sound")
        sound_location = ForwardRef(line, ability_ref)
        sound_raw_api_object.set_location(sound_location)

        # Search for the sound if it exists
        sounds_set = []

        genie_sound = dataset.genie_sounds[sound_id]
        file_ids = genie_sound.get_sounds(civ_id=-1)

        for file_id in file_ids:
            if file_id in dataset.combined_sounds:
                sound = dataset.combined_sounds[file_id]

            else:
                sound = CombinedSound(sound_id,
                                      file_id,
                                      f"{filename_prefix}sound_{str(file_id)}",
                                      dataset)
                dataset.combined_sounds.update({file_id: sound})

            sound.add_reference(sound_raw_api_object)
            sounds_set.append(sound)

        sound_raw_api_object.add_raw_member("play_delay",
                                            0,
                                            "engine.aux.sound.Sound")
        sound_raw_api_object.add_raw_member("sounds",
                                            sounds_set,
                                            "engine.aux.sound.Sound")

        line.add_raw_api_object(sound_raw_api_object)

        sound_forward_ref = ForwardRef(line, sound_ref)

        return sound_forward_ref

    @staticmethod
    def create_language_strings(line, string_id, obj_ref, obj_name_prefix):
        """
        Generates a language string for an ability.
        """
        dataset = line.data
        string_resources = dataset.strings.get_tables()

        string_objs = []
        for language, strings in string_resources.items():
            if string_id in strings.keys():
                string_name = f"{obj_name_prefix}String"
                string_ref = f"{obj_ref}.{string_name}"
                string_raw_api_object = RawAPIObject(string_ref, string_name,
                                                     dataset.nyan_api_objects)
                string_raw_api_object.add_raw_parent("engine.aux.language.LanguageTextPair")
                string_location = ForwardRef(line, obj_ref)
                string_raw_api_object.set_location(string_location)

                # Language identifier
                lang_ref = f"aux.language.{language}"
                lang_forward_ref = dataset.pregen_nyan_objects[lang_ref].get_nyan_object()
                string_raw_api_object.add_raw_member("language",
                                                     lang_forward_ref,
                                                     "engine.aux.language.LanguageTextPair")

                # String
                string_raw_api_object.add_raw_member("string",
                                                     strings[string_id],
                                                     "engine.aux.language.LanguageTextPair")

                line.add_raw_api_object(string_raw_api_object)
                string_forward_ref = ForwardRef(line, string_ref)
                string_objs.append(string_forward_ref)

        return string_objs
