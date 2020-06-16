# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Derives and adds abilities to lines. Subroutine of the
nyan subprocessor.

For SWGB we use the functions of the AoCAbilitySubprocessor, but additionally
create a diff for every civ line.
"""
from openage.convert.dataformat.aoc.forward_ref import ForwardRef
from openage.convert.dataformat.aoc.genie_unit import GenieVillagerGroup,\
    GenieStackBuildingGroup
from openage.convert.dataformat.converter_object import RawAPIObject
from openage.convert.processor.aoc.ability_subprocessor import AoCAbilitySubprocessor
from openage.convert.processor.aoc.effect_subprocessor import AoCEffectSubprocessor
from openage.convert.service import internal_name_lookups
from openage.nyan.nyan_structs import MemberSpecialValue
from openage.util.ordered_set import OrderedSet


class SWGBCCAbilitySubprocessor:

    @staticmethod
    def active_transform_to_ability(line):
        """
        Adds the ActiveTransformTo ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        ability_forward_ref = AoCAbilitySubprocessor.active_transform_to_ability(line)

        # TODO: Implement diffing of civ lines

        return ability_forward_ref

    @staticmethod
    def apply_continuous_effect_ability(line, command_id, ranged=False):
        """
        Adds the ApplyContinuousEffect ability to a line that is used to make entities die.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        ability_forward_ref = AoCAbilitySubprocessor.apply_continuous_effect_ability(line, command_id, ranged)

        # TODO: Implement diffing of civ lines

        return ability_forward_ref

    @staticmethod
    def apply_discrete_effect_ability(line, command_id, ranged=False, projectile=-1):
        """
        Adds the ApplyDiscreteEffect ability to a line that is used to make entities die.

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
            ability_ref = "%s.%s" % (game_entity_name, ability_name)
            ability_raw_api_object = RawAPIObject(ability_ref, ability_name, dataset.nyan_api_objects)
            ability_raw_api_object.add_raw_parent(ability_parent)
            ability_location = ForwardRef(line, game_entity_name)
            ability_raw_api_object.set_location(ability_location)

            ability_animation_id = current_unit["attack_sprite_id"].get_value()

        else:
            ability_ref = "%s.ShootProjectile.Projectile%s.%s" % (game_entity_name, str(projectile), ability_name)
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
            animation_forward_ref = AoCAbilitySubprocessor._create_animation(line,
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

            sound_forward_ref = AoCAbilitySubprocessor._create_sound(line,
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
            allowed_types = [dataset.pregen_nyan_objects["aux.game_entity_type.types.Unit"].get_nyan_object()]

        else:
            allowed_types = [dataset.pregen_nyan_objects["aux.game_entity_type.types.Unit"].get_nyan_object(),
                             dataset.pregen_nyan_objects["aux.game_entity_type.types.Building"].get_nyan_object()]

        ability_raw_api_object.add_raw_member("allowed_types",
                                              allowed_types,
                                              "engine.ability.type.ApplyDiscreteEffect")

        if command_id == 104:
            # Convert
            force_master_line = dataset.unit_lines[115]
            force_line = dataset.unit_lines[180]
            artillery_line = dataset.unit_lines[691]
            anti_air_line = dataset.unit_lines[702]
            pummel_line = dataset.unit_lines[713]

            blacklisted_entities = [ForwardRef(force_master_line, "ForceMaster"),
                                    ForwardRef(force_line, "ForceKnight"),
                                    ForwardRef(artillery_line, "Artillery"),
                                    ForwardRef(anti_air_line, "AntiAirMobile"),
                                    ForwardRef(pummel_line, "Pummel")]

        else:
            blacklisted_entities = []

        ability_raw_api_object.add_raw_member("blacklisted_entities",
                                              blacklisted_entities,
                                              "engine.ability.type.ApplyDiscreteEffect")

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        # TODO: Implement diffing of civ lines

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
        if isinstance(line, GenieStackBuildingGroup):
            current_unit = line.get_stack_unit()
            current_unit_id = line.get_stack_unit_id()

        else:
            current_unit = line.get_head_unit()
            current_unit_id = line.get_head_unit_id()

        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_ref = "%s.AttributeChangeTracker" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(ability_ref, "AttributeChangeTracker", dataset.nyan_api_objects)
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
                                                   "ChangeProgress%s" % (interval_right_bound),
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
                animation_forward_ref = AoCAbilitySubprocessor._create_animation(line,
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
    def constructable_ability(line):
        """
        Adds the Constructable ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        ability_forward_ref = AoCAbilitySubprocessor.constructable_ability(line)

        # TODO: Implement diffing of civ lines

        return ability_forward_ref

    @staticmethod
    def death_ability(line):
        """
        Adds the Death ability to a line that is used to make entities die.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        ability_forward_ref = AoCAbilitySubprocessor.death_ability(line)

        # TODO: Implement diffing of civ lines

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

        resource_names = ["Food", "Carbon", "Ore"]

        abilities = []
        for resource_name in resource_names:
            ability_name = "MarketExchange%s" % (resource_name)
            ability_ref = "%s.%s" % (game_entity_name, ability_name)
            ability_raw_api_object = RawAPIObject(ability_ref, ability_name, dataset.nyan_api_objects)
            ability_raw_api_object.add_raw_parent("engine.ability.type.ExchangeResources")
            ability_location = ForwardRef(line, game_entity_name)
            ability_raw_api_object.set_location(ability_location)

            # Resource that is exchanged (resource A)
            resource_a = dataset.pregen_nyan_objects["aux.resource.types.%s" % (resource_name)].get_nyan_object()
            ability_raw_api_object.add_raw_member("resource_a",
                                                  resource_a,
                                                  "engine.ability.type.ExchangeResources")

            # Resource that is exchanged for (resource B)
            resource_b = dataset.pregen_nyan_objects["aux.resource.types.Nova"].get_nyan_object()
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
                    resource = dataset.pregen_nyan_objects["aux.resource.types.Carbon"].get_nyan_object()

                elif resource_id == 2:
                    resource = dataset.pregen_nyan_objects["aux.resource.types.Ore"].get_nyan_object()

                elif resource_id == 3:
                    resource = dataset.pregen_nyan_objects["aux.resource.types.Nova"].get_nyan_object()

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

            ability_ref = "%s.%s" % (game_entity_name, ability_name)
            ability_raw_api_object = RawAPIObject(ability_ref, ability_name, dataset.nyan_api_objects)
            ability_raw_api_object.add_raw_parent("engine.ability.type.Gather")
            ability_location = ForwardRef(line, game_entity_name)
            ability_raw_api_object.set_location(ability_location)

            if ability_animation_id > -1:
                # Make the ability animated
                ability_raw_api_object.add_raw_parent("engine.ability.specialization.AnimatedAbility")

                animations_set = []
                animation_forward_ref = AoCAbilitySubprocessor._create_animation(line,
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
            rate_name = "%s.%s.GatherRate" % (game_entity_name, ability_name)
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

        ability_ref = "%s.Harvestable" % (game_entity_name)
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
                resource = dataset.pregen_nyan_objects["aux.resource.types.Carbon"].get_nyan_object()

            elif resource_id == 2:
                resource = dataset.pregen_nyan_objects["aux.resource.types.Ore"].get_nyan_object()

            elif resource_id == 3:
                resource = dataset.pregen_nyan_objects["aux.resource.types.Nova"].get_nyan_object()

            else:
                continue

            spot_name = "%s.Harvestable.%sResourceSpot" % (game_entity_name, game_entity_name)
            spot_raw_api_object = RawAPIObject(spot_name,
                                               "%sResourceSpot" % (game_entity_name),
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
                # Aqua harvester food amount (hardcoded in civ)
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

        # Harvest Progress (we don't use this for SWGB)
        ability_raw_api_object.add_raw_member("harvest_progress",
                                              [],
                                              "engine.ability.type.Harvestable")

        # Restock Progress
        progress_forward_refs = []
        if line.get_class_id() == 7:
            # Farms
            # =====================================================================================
            progress_name = "%s.Harvestable.RestockProgress33" % (game_entity_name)
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
            terrain_group = dataset.terrain_groups[31]
            terrain_forward_ref = ForwardRef(terrain_group, terrain_ref)
            progress_raw_api_object.add_raw_member("terrain_overlay",
                                                   terrain_forward_ref,
                                                   "engine.aux.progress.specialization.TerrainOverlayProgress")

            progress_raw_api_object.add_raw_parent("engine.aux.progress.specialization.StateChangeProgress")

            # State change
            init_state_ref = "%s.Constructable.InitState" % (game_entity_name)
            init_state_forward_ref = ForwardRef(line, init_state_ref)
            progress_raw_api_object.add_raw_member("state_change",
                                                   init_state_forward_ref,
                                                   "engine.aux.progress.specialization.StateChangeProgress")
            # =====================================================================================
            progress_forward_refs.append(ForwardRef(line, progress_name))
            line.add_raw_api_object(progress_raw_api_object)
            # =====================================================================================
            progress_name = "%s.Harvestable.RestockProgress66" % (game_entity_name)
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
            construct_state_ref = "%s.Constructable.ConstructState" % (game_entity_name)
            construct_state_forward_ref = ForwardRef(line, construct_state_ref)
            progress_raw_api_object.add_raw_member("state_change",
                                                   construct_state_forward_ref,
                                                   "engine.aux.progress.specialization.StateChangeProgress")
            # =====================================================================================
            progress_forward_refs.append(ForwardRef(line, progress_name))
            line.add_raw_api_object(progress_raw_api_object)
            # =====================================================================================
            progress_name = "%s.Harvestable.RestockProgress100" % (game_entity_name)
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
            terrain_group = dataset.terrain_groups[29]
            terrain_forward_ref = ForwardRef(terrain_group, terrain_ref)
            progress_raw_api_object.add_raw_member("terrain_overlay",
                                                   terrain_forward_ref,
                                                   "engine.aux.progress.specialization.TerrainOverlayProgress")

            progress_raw_api_object.add_raw_parent("engine.aux.progress.specialization.StateChangeProgress")

            # State change
            construct_state_ref = "%s.Constructable.ConstructState" % (game_entity_name)
            construct_state_forward_ref = ForwardRef(line, construct_state_ref)
            progress_raw_api_object.add_raw_member("state_change",
                                                   construct_state_forward_ref,
                                                   "engine.aux.progress.specialization.StateChangeProgress")
            #=======================================================================
            progress_forward_refs.append(ForwardRef(line, progress_name))
            line.add_raw_api_object(progress_raw_api_object)

        ability_raw_api_object.add_raw_member("restock_progress",
                                              progress_forward_refs,
                                              "engine.ability.type.Harvestable")

        # Gatherer limit (infinite in SWGB except for farms)
        gatherer_limit = MemberSpecialValue.NYAN_INF
        if line.get_class_id() == 7:
            gatherer_limit = 1

        ability_raw_api_object.add_raw_member("gatherer_limit",
                                              gatherer_limit,
                                              "engine.ability.type.Harvestable")

        # Unit have to die before they are harvestable (except for farms)
        harvestable_by_default = current_unit["hit_points"].get_value() == 0
        if line.get_class_id() == 7:
            harvestable_by_default = True

        ability_raw_api_object.add_raw_member("harvestable_by_default",
                                              harvestable_by_default,
                                              "engine.ability.type.Harvestable")

        line.add_raw_api_object(ability_raw_api_object)

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

        # TODO: Implement diffing of civ lines

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
        ability_forward_ref = AoCAbilitySubprocessor.hitbox_ability(line)

        # TODO: Implement diffing of civ lines

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
        ability_forward_ref = AoCAbilitySubprocessor.idle_ability(line)

        # TODO: Implement diffing of civ lines

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
        ability_forward_ref = AoCAbilitySubprocessor.live_ability(line)

        # TODO: Implement diffing of civ lines

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
        ability_forward_ref = AoCAbilitySubprocessor.los_ability(line)

        # TODO: Implement diffing of civ lines

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
        ability_forward_ref = AoCAbilitySubprocessor.move_ability(line)

        # TODO: Implement diffing of civ lines

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
        ability_forward_ref = AoCAbilitySubprocessor.named_ability(line)

        # TODO: Implement diffing of civ lines

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
        ability_forward_ref = AoCAbilitySubprocessor.provide_contingent_ability(line)

        # TODO: Implement diffing of civ lines

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
        if current_unit_id in (115, 180):
            # Monk; regenerates Faith
            attribute = dataset.pregen_nyan_objects["aux.attribute.types.Faith"].get_nyan_object()
            attribute_name = "Faith"

        elif current_unit_id == 8:
            # Berserk: regenerates Health
            attribute = dataset.pregen_nyan_objects["aux.attribute.types.Health"].get_nyan_object()
            attribute_name = "Health"

        else:
            return []

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        ability_name = "Regenerate%s" % (attribute_name)
        ability_ref = "%s.%s" % (game_entity_name, ability_name)
        ability_raw_api_object = RawAPIObject(ability_ref, ability_name, dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.RegenerateAttribute")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Attribute rate
        # ===============================================================================
        rate_name = "%sRate" % (attribute_name)
        rate_ref = "%s.%s.%s" % (game_entity_name, ability_name, rate_name)
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
        if current_unit_id in (115, 180):
            # stored in civ resources
            attribute_rate = dataset.genie_civs[0]["resources"][35].get_value()

        elif current_unit_id == 8:
            # stored in civ resources
            heal_timer = dataset.genie_civs[0]["resources"][96].get_value()
            attribute_rate = heal_timer

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

        ability_ref = "%s.ResourceStorage" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(ability_ref, "ResourceStorage",
                                              dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.ResourceStorage")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Create containers
        containers = []
        for gatherer in gatherers:
            unit_commands = gatherer["unit_commands"].get_value()
            resource = None

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
                    resource = dataset.pregen_nyan_objects["aux.resource.types.Carbon"].get_nyan_object()

                elif resource_id == 2:
                    resource = dataset.pregen_nyan_objects["aux.resource.types.Ore"].get_nyan_object()

                elif resource_id == 3:
                    resource = dataset.pregen_nyan_objects["aux.resource.types.Nova"].get_nyan_object()

                else:
                    continue

            gatherer_unit_id = gatherer.get_id()
            if gatherer_unit_id not in gather_lookup_dict.keys():
                # Skips hunting wolves
                continue

            container_name = "%sContainer" % (gather_lookup_dict[gatherer_unit_id][0])

            container_ref = "%s.%s" % (ability_ref, container_name)
            container_raw_api_object = RawAPIObject(container_ref, container_name, dataset.nyan_api_objects)
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
            carry_move_animation_id = command["carry_sprite_id"].get_value()
            if carry_move_animation_id > -1:
                # ===========================================================================================
                progress_name = "%s.ResourceStorage.%sCarryProgress" % (game_entity_name,
                                                                        container_name)
                progress_raw_api_object = RawAPIObject(progress_name,
                                                       "%sCarryProgress" % (container_name),
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
                override_ref = "%s.MoveOverride" % (progress_name)
                override_raw_api_object = RawAPIObject(override_ref,
                                                       "MoveOverride",
                                                       dataset.nyan_api_objects)
                override_raw_api_object.add_raw_parent("engine.aux.animation_override.AnimationOverride")
                override_location = ForwardRef(line, progress_name)
                override_raw_api_object.set_location(override_location)

                idle_forward_ref = ForwardRef(line, "%s.Move" % (game_entity_name))
                override_raw_api_object.add_raw_member("ability",
                                                       idle_forward_ref,
                                                       "engine.aux.animation_override.AnimationOverride")

                # Animation
                animations_set = []
                animation_forward_ref = AoCAbilitySubprocessor._create_animation(line,
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
    def restock_ability(line, restock_target_id):
        """
        Adds the Restock ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        ability_forward_ref = AoCAbilitySubprocessor.restock_ability(line)

        # TODO: Implement diffing of civ lines

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
        ability_forward_ref = AoCAbilitySubprocessor.selectable_ability(line)

        # TODO: Implement diffing of civ lines

        return ability_forward_ref

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
        ability_ref = "%s.SendBackToTask" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(ability_ref, "SendBackToTask", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.SendBackToTask")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Only works on villagers
        allowed_types = [dataset.pregen_nyan_objects["aux.game_entity_type.types.Worker"].get_nyan_object()]
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
        ability_forward_ref = AoCAbilitySubprocessor.shoot_projectile_ability(line, command_id)

        # TODO: Implement diffing of civ lines

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

        ability_ref = "%s.Trade" % (game_entity_name)
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
            if trade_post_id == 530:
                # Ignore Tattoine Spaceport
                continue

            trade_post_line = dataset.building_lines[trade_post_id]
            trade_post_name = name_lookup_dict[trade_post_id][0]

            trade_route_ref = "%s.TradePost.AoE2%sTradeRoute" % (trade_post_name, trade_post_name)
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

        ability_ref = "%s.TradePost" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(ability_ref, "TradePost", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.TradePost")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Trade route
        trade_routes = []
        # =====================================================================================
        trade_route_name = "AoE2%sTradeRoute" % (game_entity_name)
        trade_route_ref = "%s.TradePost.%s" % (game_entity_name, trade_route_name)
        trade_route_raw_api_object = RawAPIObject(trade_route_ref,
                                                  trade_route_name,
                                                  dataset.nyan_api_objects)
        trade_route_raw_api_object.add_raw_parent("engine.aux.trade_route.type.AoE2TradeRoute")
        trade_route_location = ForwardRef(line, ability_ref)
        trade_route_raw_api_object.set_location(trade_route_location)

        # Trade resource
        resource = dataset.pregen_nyan_objects["aux.resource.types.Nova"].get_nyan_object()
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
    def turn_ability(line):
        """
        Adds the Turn ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        ability_forward_ref = AoCAbilitySubprocessor.turn_ability(line)

        # TODO: Implement diffing of civ lines

        return ability_forward_ref
