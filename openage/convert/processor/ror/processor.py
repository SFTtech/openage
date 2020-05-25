# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Convert data from RoR to openage formats.
"""

from openage.convert.dataformat.aoc.genie_object_container import GenieObjectContainer
from openage.convert.dataformat.aoc.genie_tech import InitiatedTech
from openage.convert.dataformat.aoc.genie_unit import GenieUnitObject
from openage.convert.dataformat.ror.genie_sound import RoRSound
from openage.convert.dataformat.ror.genie_tech import RoRStatUpgrade,\
    RoRBuildingLineUpgrade, RoRUnitLineUpgrade, RoRBuildingUnlock, RoRUnitUnlock,\
    RoRAgeUpgrade
from openage.convert.dataformat.ror.genie_unit import RoRUnitTaskGroup,\
    RoRUnitLineGroup, RoRBuildingLineGroup, RoRVillagerGroup, RoRAmbientGroup
from openage.convert.dataformat.ror.internal_nyan_names import AMBIENT_GROUP_LOOKUPS
from openage.convert.nyan.api_loader import load_api
from openage.convert.processor.aoc.media_subprocessor import AoCMediaSubprocessor
from openage.convert.processor.aoc.processor import AoCProcessor
from openage.convert.processor.ror.nyan_subprocessor import RoRNyanSubprocessor
from openage.convert.processor.ror.pregen_subprocessor import RoRPregenSubprocessor

from ....log import info


class RoRProcessor:

    @classmethod
    def convert(cls, gamespec, game_version, string_resources, existing_graphics):
        """
        Input game specification and media here and get a set of
        modpacks back.

        :param gamespec: Gamedata from empires.dat read in by the
                         reader functions.
        :type gamespec: class: ...dataformat.value_members.ArrayMember
        :returns: A list of modpacks.
        :rtype: list
        """

        info("Starting conversion...")

        # Create a new container for the conversion process
        data_set = cls._pre_processor(gamespec, game_version, string_resources, existing_graphics)

        # Create the custom openae formats (nyan, sprite, terrain)
        data_set = cls._processor(gamespec, data_set)

        # Create modpack definitions
        modpacks = cls._post_processor(data_set)

        return modpacks

    @classmethod
    def _pre_processor(cls, gamespec, game_version, string_resources, existing_graphics):
        """
        Store data from the reader in a conversion container.

        :param gamespec: Gamedata from empires.dat file.
        :type gamespec: class: ...dataformat.value_members.ArrayMember
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        """
        dataset = GenieObjectContainer()

        dataset.game_version = game_version
        dataset.nyan_api_objects = load_api()
        dataset.strings = string_resources
        dataset.existing_graphics = existing_graphics

        info("Extracting Genie data...")

        cls._extract_genie_units(gamespec, dataset)
        AoCProcessor._extract_genie_techs(gamespec, dataset)
        AoCProcessor._extract_genie_effect_bundles(gamespec, dataset)
        AoCProcessor._sanitize_effect_bundles(dataset)
        AoCProcessor._extract_genie_civs(gamespec, dataset)
        AoCProcessor._extract_genie_graphics(gamespec, dataset)
        cls._extract_genie_sounds(gamespec, dataset)
        AoCProcessor._extract_genie_terrains(gamespec, dataset)

        return dataset

    @classmethod
    def _processor(cls, gamespec, full_data_set):
        """
        1. Transfer structures used in Genie games to more openage-friendly
           Python objects.
        2. Convert these objects to nyan.

        :param gamespec: Gamedata from empires.dat file.
        :type gamespec: class: ...dataformat.value_members.ArrayMember
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        """

        info("Creating API-like objects...")

        cls._create_tech_groups(full_data_set)
        cls._create_entity_lines(gamespec, full_data_set)
        cls._create_ambient_groups(full_data_set)
        AoCProcessor._create_terrain_groups(full_data_set)
        AoCProcessor._create_civ_groups(full_data_set)

        info("Linking API-like objects...")

        AoCProcessor._link_creatables(full_data_set)
        AoCProcessor._link_researchables(full_data_set)
        AoCProcessor._link_gatherers_to_dropsites(full_data_set)
        cls._link_garrison(full_data_set)
        AoCProcessor._link_trade_posts(full_data_set)

        info("Generating auxiliary objects...")

        RoRPregenSubprocessor.generate(full_data_set)

        return full_data_set

    @classmethod
    def _post_processor(cls, full_data_set):

        info("Creating nyan objects...")

        RoRNyanSubprocessor.convert(full_data_set)

        info("Creating requests for media export...")

        AoCMediaSubprocessor.convert(full_data_set)

        return None

    @staticmethod
    def _extract_genie_units(gamespec, full_data_set):
        """
        Extract units from the game data.

        :param gamespec: Gamedata from empires.dat file.
        :type gamespec: class: ...dataformat.value_members.ArrayMember
        """
        # Units are stored in the civ container.
        # In RoR the normal civs are not subsets of the Gaia civ, so we search units from
        # Gaia and one player civ (egyptiians).
        #
        # call hierarchy: wrapper[0]->civs[0]->units
        raw_units = []

        # Gaia units
        raw_units.extend(gamespec[0]["civs"][0]["units"].get_value())

        # Egyptians
        raw_units.extend(gamespec[0]["civs"][1]["units"].get_value())

        for raw_unit in raw_units:
            unit_id = raw_unit["id0"].get_value()

            if unit_id in full_data_set.genie_units.keys():
                continue

            unit_members = raw_unit.get_value()

            # Turn attack and armor into containers to make diffing work
            if "attacks" in unit_members.keys():
                attacks_member = unit_members.pop("attacks")
                attacks_member = attacks_member.get_container("type_id")
                armors_member = unit_members.pop("armors")
                armors_member = armors_member.get_container("type_id")

                unit_members.update({"attacks": attacks_member})
                unit_members.update({"armors": armors_member})

            unit = GenieUnitObject(unit_id, full_data_set, members=unit_members)
            full_data_set.genie_units.update({unit.get_id(): unit})

        # Sort the dict to make debugging easier :)
        full_data_set.genie_units = dict(sorted(full_data_set.genie_units.items()))

    @staticmethod
    def _extract_genie_sounds(gamespec, full_data_set):
        """
        Extract sound definitions from the game data.

        :param gamespec: Gamedata from empires.dat file.
        :type gamespec: class: ...dataformat.value_members.ArrayMember
        """
        # call hierarchy: wrapper[0]->sounds
        raw_sounds = gamespec.get_value()[0].get_value()["sounds"].get_value()

        for raw_sound in raw_sounds:
            sound_id = raw_sound.get_value()["sound_id"].get_value()
            sound_members = raw_sound.get_value()

            sound = RoRSound(sound_id, full_data_set, members=sound_members)
            full_data_set.genie_sounds.update({sound.get_id(): sound})

    @staticmethod
    def _create_entity_lines(gamespec, full_data_set):
        """
        Sort units/buildings into lines, based on information from techs and civs.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        """
        # Search a player civ (egyptians) for the starting units
        player_civ_units = gamespec[0]["civs"][1]["units"].get_value()
        task_group_ids = set()
        villager_unit_ids = set()

        for raw_unit in player_civ_units.values():
            unit_id = raw_unit["id0"].get_value()
            enabled = raw_unit["enabled"].get_value()
            entity = full_data_set.genie_units[unit_id]

            if not enabled:
                # Unlocked by tech
                continue

            unit_type = entity["unit_type"].get_value()

            if unit_type == 70:
                if entity.has_member("task_group") and\
                        entity.get_member("task_group").get_value() > 0:
                    task_group_id = entity["task_group"].get_value()
                    villager_unit_ids.add(unit_id)

                    if task_group_id in task_group_ids:
                        task_group = full_data_set.task_groups[task_group_id]
                        task_group.add_unit(entity)

                    else:
                        if task_group_id == 1:
                            line_id = RoRUnitTaskGroup.male_line_id

                        task_group = RoRUnitTaskGroup(line_id, task_group_id, -1, full_data_set)
                        task_group.add_unit(entity)
                        task_group_ids.add(task_group_id)
                        full_data_set.task_groups.update({task_group_id: task_group})

                else:
                    unit_line = RoRUnitLineGroup(unit_id, -1, full_data_set)
                    unit_line.add_unit(entity)
                    full_data_set.unit_lines.update({unit_line.get_id(): unit_line})
                    full_data_set.unit_ref.update({unit_id: unit_line})

            elif unit_type == 80:
                building_line = RoRBuildingLineGroup(unit_id, -1, full_data_set)
                building_line.add_unit(entity)
                full_data_set.building_lines.update({building_line.get_id(): building_line})
                full_data_set.unit_ref.update({unit_id: building_line})

        # Create the villager task group
        villager = RoRVillagerGroup(118, task_group_ids, full_data_set)
        full_data_set.unit_lines.update({villager.get_id(): villager})
        full_data_set.villager_groups.update({villager.get_id(): villager})
        for unit_id in villager_unit_ids:
            full_data_set.unit_ref.update({unit_id: villager})

        # Other units unlocks through techs
        unit_unlocks = full_data_set.unit_unlocks
        for unit_unlock in unit_unlocks.values():
            line_id = unit_unlock.get_line_id()
            unit = full_data_set.genie_units[line_id]

            unit_line = RoRUnitLineGroup(line_id, unit_unlock.get_id(), full_data_set)
            unit_line.add_unit(unit)
            full_data_set.unit_lines.update({unit_line.get_id(): unit_line})
            full_data_set.unit_ref.update({line_id: unit_line})

            # Check if the tech unlocks other lines
            # TODO: Make this cleaner
            unlock_effects = unit_unlock.get_effects(effect_type=2)
            for unlock_effect in unlock_effects:
                line_id = unlock_effect["attr_a"].get_value()
                unit = full_data_set.genie_units[line_id]

                if line_id not in full_data_set.unit_lines.keys():
                    unit_line = RoRUnitLineGroup(line_id, unit_unlock.get_id(), full_data_set)
                    unit_line.add_unit(unit)
                    full_data_set.unit_lines.update({unit_line.get_id(): unit_line})
                    full_data_set.unit_ref.update({line_id: unit_line})

        # Upgraded units in a line
        unit_upgrades = full_data_set.unit_upgrades
        for unit_upgrade in unit_upgrades.values():
            line_id = unit_upgrade.get_line_id()
            target_id = unit_upgrade.get_upgrade_target_id()
            unit = full_data_set.genie_units[target_id]

            # Find the previous unit in the line
            required_techs = unit_upgrade.tech.get_member("required_techs").get_value()
            for required_tech in required_techs:
                required_tech_id = required_tech.get_value()
                if required_tech_id in full_data_set.unit_unlocks.keys():
                    source_id = full_data_set.unit_unlocks[required_tech_id].get_line_id()
                    break

                elif required_tech_id in full_data_set.unit_upgrades.keys():
                    source_id = full_data_set.unit_upgrades[required_tech_id].get_upgrade_target_id()
                    break

            unit_line = full_data_set.unit_lines[line_id]
            unit_line.add_unit(unit, after=source_id)
            full_data_set.unit_ref.update({target_id: unit_line})

        # Other buildings unlocks through techs
        building_unlocks = full_data_set.building_unlocks
        for building_unlock in building_unlocks.values():
            line_id = building_unlock.get_line_id()
            building = full_data_set.genie_units[line_id]

            building_line = RoRBuildingLineGroup(line_id, building_unlock.get_id(), full_data_set)
            building_line.add_unit(building)
            full_data_set.building_lines.update({building_line.get_id(): building_line})
            full_data_set.unit_ref.update({line_id: building_line})

        # Upgraded buildings through techs
        building_upgrades = full_data_set.building_upgrades
        for building_upgrade in building_upgrades.values():
            line_id = building_upgrade.get_line_id()
            target_id = building_upgrade.get_upgrade_target_id()
            unit = full_data_set.genie_units[target_id]

            # Find the previous unit in the line
            required_techs = building_upgrade.tech.get_member("required_techs").get_value()
            for required_tech in required_techs:
                required_tech_id = required_tech.get_value()
                if required_tech_id in full_data_set.building_unlocks.keys():
                    source_id = full_data_set.building_unlocks[required_tech_id].get_line_id()
                    break

                elif required_tech_id in full_data_set.building_upgrades.keys():
                    source_id = full_data_set.building_upgrades[required_tech_id].get_upgrade_target_id()
                    break

            building_line = full_data_set.building_lines[line_id]
            building_line.add_unit(unit, after=source_id)
            full_data_set.unit_ref.update({target_id: building_line})

        # Upgraded units/buildings through age ups
        age_ups = full_data_set.age_upgrades
        for age_up in age_ups.values():
            effects = age_up.get_effects(effect_type=3)
            for effect in effects:
                source_id = effect["attr_a"].get_value()
                target_id = effect["attr_b"].get_value()
                unit = full_data_set.genie_units[target_id]

                if source_id in full_data_set.building_lines.keys():
                    building_line = full_data_set.building_lines[source_id]
                    building_line.add_unit(unit, after=source_id)
                    full_data_set.unit_ref.update({target_id: building_line})

                elif source_id in full_data_set.unit_lines.keys():
                    unit_line = full_data_set.unit_lines[source_id]
                    unit_line.add_unit(unit, after=source_id)
                    full_data_set.unit_ref.update({target_id: unit_line})

    @staticmethod
    def _create_ambient_groups(full_data_set):
        """
        Create ambient groups, mostly for resources and scenery.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        """
        ambient_ids = AMBIENT_GROUP_LOOKUPS.keys()
        genie_units = full_data_set.genie_units

        for ambient_id in ambient_ids:
            ambient_group = RoRAmbientGroup(ambient_id, full_data_set)
            ambient_group.add_unit(genie_units[ambient_id])
            full_data_set.ambient_groups.update({ambient_group.get_id(): ambient_group})
            full_data_set.unit_ref.update({ambient_id: ambient_group})

    @staticmethod
    def _create_tech_groups(full_data_set):
        """
        Create techs from tech connections and unit upgrades/unlocks
        from unit connections.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        """
        genie_techs = full_data_set.genie_techs

        for tech_id, tech in genie_techs.items():
            tech_type = tech["tech_type"].get_value()

            # Test if a tech exist and skip it if it doesn't
            required_techs = tech["required_techs"].get_value()
            if all(required_tech.get_value() == 0 for required_tech in required_techs):
                # If all required techs are tech ID 0, the tech doesnt exist
                continue

            effect_bundle_id = tech["tech_effect_id"].get_value()

            if effect_bundle_id == -1:
                continue

            effect_bundle = full_data_set.genie_effect_bundles[effect_bundle_id]

            # Ignore techs without effects
            if len(effect_bundle.get_effects()) == 0:
                continue

            # Town Center techs (only age ups)
            if tech_type == 12:
                # Age ID is set as resource value
                setr_effects = effect_bundle.get_effects(effect_type=1)
                for effect in setr_effects:
                    resource_id = effect["attr_a"].get_value()

                    if resource_id == 6:
                        age_id = int(effect["attr_d"].get_value())
                        break

                age_up = RoRAgeUpgrade(tech_id, age_id, full_data_set)
                full_data_set.tech_groups.update({age_up.get_id(): age_up})
                full_data_set.age_upgrades.update({age_up.get_id(): age_up})

            else:
                effects = effect_bundle.get_effects()
                for effect in effects:
                    # Enabling techs
                    if effect.get_type() == 2:
                        unit_id = effect["attr_a"].get_value()
                        unit = full_data_set.genie_units[unit_id]
                        unit_type = unit["unit_type"].get_value()

                        if unit_type == 70:
                            unit_unlock = RoRUnitUnlock(tech_id, unit_id, full_data_set)
                            full_data_set.tech_groups.update({unit_unlock.get_id(): unit_unlock})
                            full_data_set.unit_unlocks.update({unit_unlock.get_id(): unit_unlock})
                            break

                        elif unit_type == 80:
                            building_unlock = RoRBuildingUnlock(tech_id, unit_id, full_data_set)
                            full_data_set.tech_groups.update({building_unlock.get_id(): building_unlock})
                            full_data_set.building_unlocks.update({building_unlock.get_id(): building_unlock})
                            break

                    # Upgrades
                    elif effect.get_type() == 3:
                        source_unit_id = effect["attr_a"].get_value()
                        target_unit_id = effect["attr_b"].get_value()
                        unit = full_data_set.genie_units[source_unit_id]
                        unit_type = unit["unit_type"].get_value()

                        if unit_type == 70:
                            unit_upgrade = RoRUnitLineUpgrade(tech_id, source_unit_id, target_unit_id, full_data_set)
                            full_data_set.tech_groups.update({unit_upgrade.get_id(): unit_upgrade})
                            full_data_set.unit_upgrades.update({unit_upgrade.get_id(): unit_upgrade})
                            break

                        elif unit_type == 80:
                            building_upgrade = RoRBuildingLineUpgrade(tech_id, source_unit_id, target_unit_id, full_data_set)
                            full_data_set.tech_groups.update({building_upgrade.get_id(): building_upgrade})
                            full_data_set.building_upgrades.update({building_upgrade.get_id(): building_upgrade})
                            break

                else:
                    # Anything else must be a stat upgrade
                    stat_up = RoRStatUpgrade(tech_id, full_data_set)
                    full_data_set.tech_groups.update({stat_up.get_id(): stat_up})
                    full_data_set.stat_upgrades.update({stat_up.get_id(): stat_up})

        # Initiated techs are stored with buildings
        genie_units = full_data_set.genie_units

        for genie_unit in genie_units.values():
            if not genie_unit.has_member("research_id"):
                continue

            building_id = genie_unit.get_member("id0").get_value()
            initiated_tech_id = genie_unit.get_member("research_id").get_value()

            if initiated_tech_id == -1:
                continue

            if building_id not in full_data_set.building_lines.keys():
                # Skips upgraded buildings (which initiate the same techs)
                continue

            initiated_tech = InitiatedTech(initiated_tech_id, building_id, full_data_set)
            full_data_set.tech_groups.update({initiated_tech.get_id(): initiated_tech})
            full_data_set.initiated_techs.update({initiated_tech.get_id(): initiated_tech})

    @staticmethod
    def _link_garrison(full_data_set):
        """
        Link a garrison unit to the lines that are stored and vice versa. This is done
        to provide quick access during conversion.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        """
        unit_lines = full_data_set.unit_lines

        garrison_class_assignments = {}

        for unit_line in unit_lines.values():
            head_unit = unit_line.get_head_unit()

            unit_commands = head_unit["unit_commands"].get_value()
            for command in unit_commands:
                command_type = command["type"].get_value()

                if not command_type == 3:
                    continue

                class_id = command["class_id"].get_value()

                if class_id in garrison_class_assignments.keys():
                    garrison_class_assignments[class_id].append(unit_line)

                else:
                    garrison_class_assignments[class_id] = [unit_line]

                break

        for garrison in unit_lines.values():
            class_id = garrison.get_class_id()

            if class_id in garrison_class_assignments.keys():
                for line in garrison_class_assignments[class_id]:
                    garrison.garrison_entities.append(line)
                    line.garrison_locations.append(garrison)
