# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Derives and adds abilities to lines. REimplements only
abilities that are different from Aoc
"""
from openage.convert.dataformat.aoc.expected_pointer import ExpectedPointer
from openage.convert.dataformat.aoc.genie_unit import GenieBuildingLineGroup,\
    GenieVillagerGroup
from openage.convert.dataformat.converter_object import RawAPIObject
from openage.convert.processor.aoc.ability_subprocessor import AoCAbilitySubprocessor
from openage.convert.processor.aoc.effect_subprocessor import AoCEffectSubprocessor
from openage.convert.service import internal_name_lookups


class RoRAbilitySubprocessor:

    @staticmethod
    def apply_discrete_effect_ability(line, command_id, ranged=False, projectile=-1):
        """
        Adds the ApplyDiscreteEffect ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
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
            ability_ref = "%s.%s" % (game_entity_name, ability_name)
            ability_raw_api_object = RawAPIObject(ability_ref, ability_name, dataset.nyan_api_objects)
            ability_raw_api_object.add_raw_parent(ability_parent)
            ability_location = ExpectedPointer(line, game_entity_name)
            ability_raw_api_object.set_location(ability_location)

            if command_id == 104:
                # Get animation from commands proceed sprite
                unit_commands = current_unit.get_member("unit_commands").get_value()
                for command in unit_commands:
                    type_id = command.get_value()["type"].get_value()

                    if type_id != command_id:
                        continue

                    ability_animation_id = command["proceed_sprite_id"].get_value()
                    break

                else:
                    ability_animation_id = -1

            else:
                ability_animation_id = current_unit.get_member("attack_sprite_id").get_value()

        else:
            ability_ref = "%s.ShootProjectile.Projectile%s.%s" % (game_entity_name, str(projectile), ability_name)
            ability_raw_api_object = RawAPIObject(ability_ref, ability_name, dataset.nyan_api_objects)
            ability_raw_api_object.add_raw_parent(ability_parent)
            ability_location = ExpectedPointer(line,
                                               "%s.ShootProjectile.Projectile%s"
                                               % (game_entity_name, str(projectile)))
            ability_raw_api_object.set_location(ability_location)

            ability_animation_id = -1

        if ability_animation_id > -1:
            # Make the ability animated
            ability_raw_api_object.add_raw_parent("engine.ability.specialization.AnimatedAbility")

            animations_set = []
            animation_expected_pointer = AoCAbilitySubprocessor._create_animation(line,
                                                                                  ability_animation_id,
                                                                                  ability_ref,
                                                                                  ability_name,
                                                                                  "%s_"
                                                                                  % command_lookup_dict[command_id][1])
            animations_set.append(animation_expected_pointer)
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
                    for graphics_set_id, items in gset_lookup_dict.items():
                        if civ_id in items[0]:
                            break

                    # Check if the object for the animation has been created before
                    obj_exists = graphics_set_id in handled_graphics_set_ids
                    if not obj_exists:
                        handled_graphics_set_ids.add(graphics_set_id)

                    obj_prefix = "%s%s" % (gset_lookup_dict[graphics_set_id][1], ability_name)
                    filename_prefix = "%s_%s_" % (command_lookup_dict[command_id][1],
                                                  gset_lookup_dict[graphics_set_id][2],)
                    AoCAbilitySubprocessor._create_civ_animation(line,
                                                                 civ_group,
                                                                 civ_animation_id,
                                                                 ability_ref,
                                                                 obj_prefix,
                                                                 filename_prefix,
                                                                 obj_exists)

        # Command Sound
        if projectile == -1:
            ability_comm_sound_id = current_unit.get_member("command_sound_id").get_value()

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

            sound_expected_pointer = AoCAbilitySubprocessor._create_sound(line,
                                                                          ability_comm_sound_id,
                                                                          ability_ref,
                                                                          sound_obj_prefix,
                                                                          "command_")
            sounds_set.append(sound_expected_pointer)
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
            allowed_types = [dataset.pregen_nyan_objects["aux.game_entity_type.types.Unit"].get_nyan_object()]

        else:
            allowed_types = [dataset.pregen_nyan_objects["aux.game_entity_type.types.Unit"].get_nyan_object(),
                             dataset.pregen_nyan_objects["aux.game_entity_type.types.Building"].get_nyan_object()]

        ability_raw_api_object.add_raw_member("allowed_types",
                                              allowed_types,
                                              "engine.ability.type.ApplyDiscreteEffect")

        if command_id == 104:
            # Convert
            priest_line = dataset.unit_lines[125]

            blacklisted_entities = [ExpectedPointer(priest_line, "Priest")]

        else:
            blacklisted_entities = []

        ability_raw_api_object.add_raw_member("blacklisted_entities",
                                              blacklisted_entities,
                                              "engine.ability.type.ApplyDiscreteEffect")

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

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = "%s.ProductionQueue" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(ability_ref, "ProductionQueue", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.ProductionQueue")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Size
        size = 22

        ability_raw_api_object.add_raw_member("size", size, "engine.ability.type.ProductionQueue")

        # Production modes
        modes = []

        mode_name = "%s.ProvideContingent.CreatablesMode" % (game_entity_name)
        mode_raw_api_object = RawAPIObject(mode_name, "CreatablesMode", dataset.nyan_api_objects)
        mode_raw_api_object.add_raw_parent("engine.aux.production_mode.type.Creatables")
        mode_location = ExpectedPointer(line, ability_ref)
        mode_raw_api_object.set_location(mode_location)

        # RoR allows all creatables in production queue
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

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
        command_lookup_dict = internal_name_lookups.get_command_lookups(dataset.game_version)

        ability_name = command_lookup_dict[command_id][0]

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
            animation_expected_pointer = AoCAbilitySubprocessor._create_animation(line,
                                                                                  ability_animation_id,
                                                                                  ability_ref,
                                                                                  ability_name,
                                                                                  "%s_"
                                                                                  % command_lookup_dict[command_id][1])
            animations_set.append(animation_expected_pointer)
            ability_raw_api_object.add_raw_member("animations", animations_set,
                                                  "engine.ability.specialization.AnimatedAbility")

        # Command Sound
        ability_comm_sound_id = current_unit.get_member("command_sound_id").get_value()
        if ability_comm_sound_id > -1:
            # Make the ability animated
            ability_raw_api_object.add_raw_parent("engine.ability.specialization.CommandSoundAbility")

            sounds_set = []
            sound_expected_pointer = AoCAbilitySubprocessor._create_sound(line,
                                                                          ability_comm_sound_id,
                                                                          ability_ref,
                                                                          ability_name,
                                                                          "command_")
            sounds_set.append(sound_expected_pointer)
            ability_raw_api_object.add_raw_member("sounds", sounds_set,
                                                  "engine.ability.specialization.CommandSoundAbility")

        # Projectile
        projectiles = []
        projectile_primary = current_unit.get_member("attack_projectile_primary_unit_id").get_value()
        if projectile_primary > -1:
            projectiles.append(ExpectedPointer(line,
                                               "%s.ShootProjectile.Projectile0" % (game_entity_name)))

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

        # Manual aiming (does not exist in RoR)
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
        allowed_types = [dataset.pregen_nyan_objects["aux.game_entity_type.types.Building"].get_nyan_object(),
                         dataset.pregen_nyan_objects["aux.game_entity_type.types.Unit"].get_nyan_object()]
        ability_raw_api_object.add_raw_member("allowed_types",
                                              allowed_types,
                                              "engine.ability.type.ShootProjectile")
        ability_raw_api_object.add_raw_member("blacklisted_entities",
                                              [],
                                              "engine.ability.type.ShootProjectile")

        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer
