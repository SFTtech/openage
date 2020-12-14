# Copyright 2020-2020 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-branches,too-many-statements,too-many-locals
#
# TODO:
# pylint: disable=line-too-long

"""
Derives and adds abilities to lines. Reimplements only
abilities that are different from AoC.
"""
from math import degrees

from ....entity_object.conversion.aoc.genie_unit import GenieBuildingLineGroup,\
    GenieVillagerGroup, GenieUnitLineGroup
from ....entity_object.conversion.converter_object import RawAPIObject
from ....service.conversion import internal_name_lookups
from ....value_object.conversion.forward_ref import ForwardRef
from ..aoc.ability_subprocessor import AoCAbilitySubprocessor
from ..aoc.effect_subprocessor import AoCEffectSubprocessor


class RoRAbilitySubprocessor:
    """
    Creates raw API objects for abilities in RoR.
    """

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

            if command_id == 104:
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

            else:
                ability_animation_id = current_unit["attack_sprite_id"].get_value()

        else:
            ability_ref = "%s.ShootProjectile.Projectile%s.%s" % (game_entity_name,
                                                                  str(projectile),
                                                                  ability_name)
            ability_raw_api_object = RawAPIObject(ability_ref,
                                                  ability_name,
                                                  dataset.nyan_api_objects)
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
        effects = []
        if command_id == 7:
            # Attack
            if projectile != 1:
                effects = AoCEffectSubprocessor.get_attack_effects(line, ability_ref)

            else:
                effects = AoCEffectSubprocessor.get_attack_effects(line, ability_ref, projectile=1)

        elif command_id == 104:
            # TODO: Convert
            # effects = AoCEffectSubprocessor.get_convert_effects(line, ability_ref)
            pass

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
            apply_graphic = dataset.genie_graphics[ability_animation_id]
            frame_rate = apply_graphic.get_frame_rate()
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
                    continue

        else:
            blacklisted_entities = []

        ability_raw_api_object.add_raw_member("blacklisted_entities",
                                              blacklisted_entities,
                                              "engine.ability.type.ApplyDiscreteEffect")

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
        stance_names = ["Aggressive", "StandGround"]

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
        size = 22

        ability_raw_api_object.add_raw_member("size", size, "engine.ability.type.ProductionQueue")

        # Production modes
        modes = []

        mode_name = f"{game_entity_name}.ProvideContingent.CreatablesMode"
        mode_raw_api_object = RawAPIObject(mode_name, "CreatablesMode", dataset.nyan_api_objects)
        mode_raw_api_object.add_raw_parent("engine.aux.production_mode.type.Creatables")
        mode_location = ForwardRef(line, ability_ref)
        mode_raw_api_object.set_location(mode_location)

        # RoR allows all creatables in production queue
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
        ability_ref = "%s.ShootProjectile.Projectile%s.Projectile" % (game_entity_name,
                                                                      str(position))
        ability_raw_api_object = RawAPIObject(ability_ref,
                                              "Projectile",
                                              dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Projectile")
        ability_location = ForwardRef(line, obj_ref)
        ability_raw_api_object.set_location(ability_location)

        # Arc
        if position == 0:
            projectile_id = current_unit["attack_projectile_primary_unit_id"].get_value()

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
        accuracy_raw_api_object = RawAPIObject(accuracy_name, "Accuracy", dataset.nyan_api_objects)
        accuracy_raw_api_object.add_raw_parent("engine.aux.accuracy.Accuracy")
        accuracy_location = ForwardRef(line, ability_ref)
        accuracy_raw_api_object.set_location(accuracy_location)

        accuracy_value = current_unit["accuracy"].get_value()
        accuracy_raw_api_object.add_raw_member("accuracy",
                                               accuracy_value,
                                               "engine.aux.accuracy.Accuracy")

        accuracy_dispersion = 0
        accuracy_raw_api_object.add_raw_member("accuracy_dispersion",
                                               accuracy_dispersion,
                                               "engine.aux.accuracy.Accuracy")
        dropoff_type = dataset.nyan_api_objects["engine.aux.dropoff_type.type.NoDropoff"]
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
        ability_raw_api_object = RawAPIObject(ability_ref, "Resistance", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Resistance")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Resistances
        resistances = []
        resistances.extend(AoCEffectSubprocessor.get_attack_resistances(line,
                                                                        ability_ref))
        if isinstance(line, (GenieUnitLineGroup, GenieBuildingLineGroup)):
            # TODO: Conversion resistance
            # resistances.extend(RoREffectSubprocessor.get_convert_resistances(line,
            #                                                                  ability_ref))

            if isinstance(line, GenieUnitLineGroup) and not line.is_repairable():
                resistances.extend(AoCEffectSubprocessor.get_heal_resistances(line,
                                                                              ability_ref))

            if isinstance(line, GenieBuildingLineGroup):
                resistances.extend(AoCEffectSubprocessor.get_construct_resistances(line,
                                                                                   ability_ref))

            if line.is_repairable():
                resistances.extend(AoCEffectSubprocessor.get_repair_resistances(line,
                                                                                ability_ref))

        ability_raw_api_object.add_raw_member("resistances",
                                              resistances,
                                              "engine.ability.type.Resistance")

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
        ability_raw_api_object = RawAPIObject(ability_ref, ability_name, dataset.nyan_api_objects)
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
            ability_raw_api_object.add_raw_member("animations", animations_set,
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

        ability_raw_api_object.add_raw_member("projectiles",
                                              projectiles,
                                              "engine.ability.type.ShootProjectile")

        # Projectile count (does not exist in RoR)
        min_projectiles = 1
        max_projectiles = 1

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

        # Projectile delay (unused because RoR has no multiple projectiles)
        ability_raw_api_object.add_raw_member("projectile_delay",
                                              0.0,
                                              "engine.ability.type.ShootProjectile")

        # Turning
        if isinstance(line, GenieBuildingLineGroup):
            require_turning = False

        else:
            require_turning = True

        ability_raw_api_object.add_raw_member("require_turning",
                                              require_turning,
                                              "engine.ability.type.ShootProjectile")

        # Manual aiming
        manual_aiming_allowed = line.get_head_unit_id() in (35, 250)
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

        # Spawn Area (does not exist in RoR)
        spawning_area_width = 0
        spawning_area_height = 0
        spawning_area_randomness = 0

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
