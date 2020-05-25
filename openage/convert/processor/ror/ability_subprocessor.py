# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Derives and adds abilities to lines. REimplements only
abilities that are different from Aoc
"""
from openage.convert.dataformat.aoc.expected_pointer import ExpectedPointer
from openage.convert.dataformat.aoc.genie_unit import GenieBuildingLineGroup
from openage.convert.dataformat.converter_object import RawAPIObject
from openage.convert.processor.aoc.ability_subprocessor import AoCAbilitySubprocessor
from openage.convert.service import internal_name_lookups


class RoRAbilitySubprocessor:

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
