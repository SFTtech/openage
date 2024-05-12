# Copyright 2020-2024 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-lines,too-many-branches,too-many-statements,too-many-locals
#
# TODO:
# pylint: disable=line-too-long
"""
Convert data from SWGB:CC to openage formats.
"""
from __future__ import annotations
import typing


from openage.convert.entity_object.conversion.aoc.genie_tech import BuildingUnlock
from .....log import info
from ....entity_object.conversion.aoc.genie_object_container import GenieObjectContainer
from ....entity_object.conversion.aoc.genie_tech import BuildingLineUpgrade, \
    AgeUpgrade, StatUpgrade, InitiatedTech, CivBonus
from ....entity_object.conversion.aoc.genie_unit import GenieUnitTaskGroup, \
    GenieVillagerGroup, GenieAmbientGroup, GenieVariantGroup, \
    GenieBuildingLineGroup, GenieGarrisonMode
from ....entity_object.conversion.swgbcc.genie_tech import SWGBUnitUnlock, \
    SWGBUnitLineUpgrade
from ....entity_object.conversion.swgbcc.genie_unit import SWGBUnitTransformGroup, \
    SWGBMonkGroup, SWGBUnitLineGroup, SWGBStackBuildingGroup
from ....service.debug_info import debug_converter_objects, \
    debug_converter_object_groups
from ....service.read.nyan_api_loader import load_api
from ....value_object.conversion.swgb.internal_nyan_names import MONK_GROUP_ASSOCS, \
    CIV_LINE_ASSOCS, AMBIENT_GROUP_LOOKUPS, VARIANT_GROUP_LOOKUPS, \
    CIV_TECH_ASSOCS
from ..aoc.media_subprocessor import AoCMediaSubprocessor
from ..aoc.processor import AoCProcessor
from .modpack_subprocessor import SWGBCCModpackSubprocessor
from .nyan_subprocessor import SWGBCCNyanSubprocessor
from .pregen_subprocessor import SWGBCCPregenSubprocessor

if typing.TYPE_CHECKING:
    from argparse import Namespace
    from openage.convert.entity_object.conversion.stringresource import StringResource
    from openage.convert.entity_object.conversion.modpack import Modpack
    from openage.convert.value_object.read.value_members import ArrayMember
    from openage.convert.value_object.init.game_version import GameVersion


class SWGBCCProcessor:
    """
    Main processor for converting data from SWGB.
    """

    @classmethod
    def convert(
        cls,
        gamespec: ArrayMember,
        args: Namespace,
        string_resources: StringResource,
        existing_graphics: list[str]
    ) -> list[Modpack]:
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
        dataset = cls._pre_processor(
            gamespec,
            args.game_version,
            string_resources,
            existing_graphics
        )
        debug_converter_objects(args.debugdir, args.debug_info, dataset)

        # Create the custom openae formats (nyan, sprite, terrain)
        dataset = cls._processor(dataset)
        debug_converter_object_groups(args.debugdir, args.debug_info, dataset)

        # Create modpack definitions
        modpacks = cls._post_processor(dataset)

        return modpacks

    @classmethod
    def _pre_processor(
        cls,
        gamespec: ArrayMember,
        game_version: GameVersion,
        string_resources: StringResource,
        existing_graphics: list[str]
    ) -> GenieObjectContainer:
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

        AoCProcessor.extract_genie_units(gamespec, dataset)
        AoCProcessor.extract_genie_techs(gamespec, dataset)
        AoCProcessor.extract_genie_effect_bundles(gamespec, dataset)
        AoCProcessor.sanitize_effect_bundles(dataset)
        AoCProcessor.extract_genie_civs(gamespec, dataset)
        AoCProcessor.extract_age_connections(gamespec, dataset)
        AoCProcessor.extract_building_connections(gamespec, dataset)
        AoCProcessor.extract_unit_connections(gamespec, dataset)
        AoCProcessor.extract_tech_connections(gamespec, dataset)
        AoCProcessor.extract_genie_graphics(gamespec, dataset)
        AoCProcessor.extract_genie_sounds(gamespec, dataset)
        AoCProcessor.extract_genie_terrains(gamespec, dataset)
        AoCProcessor.extract_genie_restrictions(gamespec, dataset)

        return dataset

    @classmethod
    def _processor(cls, full_data_set: GenieObjectContainer) -> GenieObjectContainer:
        """
        Transfer structures used in Genie games to more openage-friendly
        Python objects.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        """

        info("Creating API-like objects...")

        cls.create_unit_lines(full_data_set)
        cls.create_extra_unit_lines(full_data_set)
        cls.create_building_lines(full_data_set)
        cls.create_villager_groups(full_data_set)
        cls.create_ambient_groups(full_data_set)
        cls.create_variant_groups(full_data_set)
        AoCProcessor.create_terrain_groups(full_data_set)
        cls.create_tech_groups(full_data_set)
        AoCProcessor.create_civ_groups(full_data_set)

        info("Linking API-like objects...")

        AoCProcessor.link_building_upgrades(full_data_set)
        AoCProcessor.link_creatables(full_data_set)
        AoCProcessor.link_researchables(full_data_set)
        AoCProcessor.link_civ_uniques(full_data_set)
        AoCProcessor.link_gatherers_to_dropsites(full_data_set)
        cls.link_garrison(full_data_set)
        AoCProcessor.link_trade_posts(full_data_set)
        cls.link_repairables(full_data_set)

        info("Generating auxiliary objects...")

        SWGBCCPregenSubprocessor.generate(full_data_set)

        return full_data_set

    @classmethod
    def _post_processor(cls, full_data_set: GenieObjectContainer) -> list[Modpack]:
        """
        Convert API-like Python objects to nyan.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        """

        info("Creating nyan objects...")

        SWGBCCNyanSubprocessor.convert(full_data_set)

        info("Creating requests for media export...")

        AoCMediaSubprocessor.convert(full_data_set)

        return SWGBCCModpackSubprocessor.get_modpacks(full_data_set)

    @staticmethod
    def create_unit_lines(full_data_set: GenieObjectContainer) -> None:
        """
        Sort units into lines, based on information in the unit connections.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        """
        unit_connections = full_data_set.unit_connections
        unit_lines = {}
        unit_ref = {}

        # First only handle the line heads (first units in a line)
        for connection in unit_connections.values():
            unit_id = connection["id"].value
            unit = full_data_set.genie_units[unit_id]
            line_mode = connection["line_mode"].value

            if line_mode != 2:
                # It's an upgrade. Skip and handle later
                continue

            # Check for special cases first
            if unit.has_member("transform_unit_id")\
                    and unit["transform_unit_id"].value > -1:
                # Cannon
                # SWGB stores the deployed cannon in the connections, but we
                # want the undeployed cannon
                transform_id = unit["transform_unit_id"].value
                unit_line = SWGBUnitTransformGroup(transform_id, transform_id, full_data_set)

            elif unit_id in MONK_GROUP_ASSOCS:
                # Jedi/Sith
                # Switch to monk with relic is hardcoded :(
                # for every civ (WTF LucasArts)
                switch_unit_id = MONK_GROUP_ASSOCS[unit_id]
                unit_line = SWGBMonkGroup(unit_id, unit_id, switch_unit_id, full_data_set)

            elif unit.has_member("task_group")\
                    and unit["task_group"].value > 0:
                # Villager
                # done somewhere else because they are special^TM
                continue

            else:
                # Normal units
                unit_line = SWGBUnitLineGroup(unit_id, full_data_set)

            unit_line.add_unit(unit)
            unit_lines.update({unit_line.get_id(): unit_line})
            unit_ref.update({unit_id: unit_line})

        # Second, handle all upgraded units
        for connection in unit_connections.values():
            unit_id = connection["id"].value
            unit = full_data_set.genie_units[unit_id]
            line_mode = connection["line_mode"].value

            if line_mode != 3:
                # This unit is not an upgrade and was handled in the last for-loop
                continue

            # Search other_connections for the previous unit in line
            connected_types = connection["other_connections"].value
            for index, _ in enumerate(connected_types):
                connected_type = connected_types[index]["other_connection"].value
                if connected_type == 2:
                    # 2 == Unit
                    connected_index = index
                    break

            else:
                raise ValueError(f"Unit {unit_id} is not first in line, but no previous "
                                 "unit can be found in other_connections")

            connected_ids = connection["other_connected_ids"].value
            previous_unit_id = connected_ids[connected_index].value

            # Search for the first unit ID in the line recursively
            previous_id = previous_unit_id
            previous_connection = unit_connections[previous_unit_id]
            while previous_connection["line_mode"] != 2:
                if previous_id in unit_ref:
                    # Short-circuit here, if we the previous unit was already handled
                    break

                connected_types = previous_connection["other_connections"].value
                connected_index = -1
                for index, _ in enumerate(connected_types):
                    connected_type = connected_types[index]["other_connection"].value
                    if connected_type == 2:
                        # 2 == Unit
                        connected_index = index
                        break

                connected_ids = previous_connection["other_connected_ids"].value
                previous_id = connected_ids[connected_index].value
                previous_connection = unit_connections[previous_id]

            unit_line = unit_ref[previous_id]
            unit_line.add_unit(unit, after=previous_unit_id)

        # Search for civ lines and attach them to their main line
        final_unit_lines = {}
        final_unit_lines.update(unit_lines)
        for line in unit_lines.values():
            if line.get_head_unit_id() not in CIV_LINE_ASSOCS:
                for main_head_unit_id, civ_head_unit_ids in CIV_LINE_ASSOCS.items():
                    if line.get_head_unit_id() in civ_head_unit_ids:
                        # The line is an alternative civ line and should be stored
                        # with the main line only.
                        main_line = unit_lines[main_head_unit_id]
                        main_line.add_civ_line(line)

                        # Remove the line from the main reference dict, so that
                        # it doesn't get converted to a game entity
                        final_unit_lines.pop(line.get_id())

                        # Store a reference to the main line in the unit ID refs
                        for unit in line.line:
                            full_data_set.unit_ref[unit.get_id()] = main_line

                        break

                else:
                    # Store a reference to the line in the unit ID refs
                    for unit in line.line:
                        full_data_set.unit_ref[unit.get_id()] = line

            else:
                # Store a reference to the line in the unit ID refs
                for unit in line.line:
                    full_data_set.unit_ref[unit.get_id()] = line

        # Store the remaining lines in the main reference dict
        full_data_set.unit_lines.update(final_unit_lines)

    @staticmethod
    def create_extra_unit_lines(full_data_set: GenieObjectContainer) -> None:
        """
        Create additional units that are not in the unit connections.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        """
        # Wildlife
        extra_units = (48, 594, 822, 833, 1203, 1249, 1363, 1364,
                       1365, 1366, 1367, 1469, 1471, 1473, 1475)

        for unit_id in extra_units:
            unit_line = SWGBUnitLineGroup(unit_id, full_data_set)
            unit_line.add_unit(full_data_set.genie_units[unit_id])
            full_data_set.unit_lines.update({unit_line.get_id(): unit_line})
            full_data_set.unit_ref.update({unit_id: unit_line})

    @staticmethod
    def create_building_lines(full_data_set: GenieObjectContainer) -> None:
        """
        Establish building lines, based on information in the building connections.
        Because of how Genie building lines work, this will only find the first
        building in the line. Subsequent buildings in the line have to be determined
        by effects in AgeUpTechs.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        """
        building_connections = full_data_set.building_connections
        genie_techs = full_data_set.genie_techs

        # Unlocked = first in line
        unlocked_by_tech = set()

        # Upgraded = later in line
        upgraded_by_tech = {}

        # Search all techs for building upgrades. This is necessary because they are
        # not stored in tech connections in SWGB
        for tech_id, tech in genie_techs.items():
            tech_effect_id = tech["tech_effect_id"].value
            if tech_effect_id < 0:
                continue

            tech_effects = full_data_set.genie_effect_bundles[tech_effect_id].get_effects()

            # Search for upgrade or unlock effects
            age_up = False
            for effect in tech_effects:
                effect_type = effect["type_id"].value
                unit_id_a = effect["attr_a"].value
                unit_id_b = effect["attr_b"].value

                if effect_type == 1 and effect["attr_a"].value == 6:
                    # if this is an age up tech, we do not need to create any additional
                    # unlock techs
                    age_up = True

                if effect_type == 2 and unit_id_a in building_connections.keys():
                    # Unlocks
                    unlocked_by_tech.add(unit_id_a)

                    if not age_up:
                        # Add an unlock tech group to the data set
                        building_unlock = BuildingUnlock(tech_id, unit_id_a, full_data_set)
                        full_data_set.building_unlocks.update(
                            {building_unlock.get_id(): building_unlock})

                elif effect_type == 2 and unit_id_a in full_data_set.genie_units.keys():
                    # Check if this is a stacked unit (gate or command center)
                    # for these units, we needs the stack_unit_id
                    building = full_data_set.genie_units[unit_id_a]

                    if building.has_member("stack_unit_id") and \
                            building["stack_unit_id"].value > -1:
                        unit_id_a = building["stack_unit_id"].value
                        unlocked_by_tech.add(unit_id_a)

                        if not age_up:
                            building_unlock = BuildingUnlock(tech_id, unit_id_a, full_data_set)
                            full_data_set.building_unlocks.update(
                                {building_unlock.get_id(): building_unlock})

                if effect_type == 3 and unit_id_b in building_connections.keys():
                    # Upgrades
                    upgraded_by_tech[unit_id_b] = tech_id

        # First only handle the line heads (first buildings in a line)
        for connection in building_connections.values():
            building_id = connection["id"].value

            if building_id not in unlocked_by_tech:
                continue

            building = full_data_set.genie_units[building_id]

            # Check if we have to create a GenieStackBuildingGroup
            if building.has_member("stack_unit_id") and \
                    building["stack_unit_id"].value > -1:
                # we don't care about head units because we process
                # them with their stack unit
                continue

            if building.has_member("head_unit_id") and \
                    building["head_unit_id"].value > -1:
                head_unit_id = building["head_unit_id"].value
                building_line = SWGBStackBuildingGroup(building_id, head_unit_id, full_data_set)

            else:
                building_line = GenieBuildingLineGroup(building_id, full_data_set)

            building_line.add_unit(building)
            full_data_set.building_lines.update({building_line.get_id(): building_line})
            full_data_set.unit_ref.update({building_id: building_line})

        # Second, handle all upgraded buildings
        for connection in building_connections.values():
            building_id = connection["id"].value

            if building_id not in upgraded_by_tech:
                continue

            building = full_data_set.genie_units[building_id]

            # Search other_connections for the previous unit in line
            connected_types = connection["other_connections"].value
            for index, _ in enumerate(connected_types):
                connected_type = connected_types[index]["other_connection"].value
                if connected_type == 1:
                    # 1 == Building
                    connected_index = index
                    break

            else:
                raise ValueError(f"Building {building_id} is not first in line, but no previous "
                                 "building can be found in other_connections")

            connected_ids = connection["other_connected_ids"].value
            previous_unit_id = connected_ids[connected_index].value

            # Search for the first unit ID in the line recursively
            previous_id = previous_unit_id
            previous_connection = building_connections[previous_unit_id]
            while previous_connection["line_mode"] != 2:
                if previous_id in full_data_set.unit_ref.keys():
                    # Short-circuit here, if we the previous unit was already handled
                    break

                connected_types = previous_connection["other_connections"].value
                connected_index = -1
                for index, _ in enumerate(connected_types):
                    connected_type = connected_types[index]["other_connection"].value
                    if connected_type == 1:
                        # 1 == Building
                        connected_index = index
                        break

                connected_ids = previous_connection["other_connected_ids"].value
                previous_id = connected_ids[connected_index].value
                previous_connection = building_connections[previous_id]

            building_line = full_data_set.unit_ref[previous_id]
            building_line.add_unit(building, after=previous_unit_id)
            full_data_set.unit_ref.update({building_id: building_line})

            # Also add the building upgrade tech here
            building_upgrade = BuildingLineUpgrade(
                upgraded_by_tech[building_id],
                building_line.get_id(),
                building_id,
                full_data_set
            )
            full_data_set.tech_groups.update({building_upgrade.get_id(): building_upgrade})
            full_data_set.building_upgrades.update({building_upgrade.get_id(): building_upgrade})

    @staticmethod
    def create_villager_groups(full_data_set: GenieObjectContainer) -> None:
        """
        Create task groups and assign the relevant worker group to a
        villager group.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        """
        units = full_data_set.genie_units
        task_group_ids = set()
        unit_ids = set()

        # Find task groups in the dataset
        for unit in units.values():
            if unit.has_member("task_group"):
                task_group_id = unit["task_group"].value

            else:
                task_group_id = 0

            if task_group_id == 0:
                # no task group
                continue

            if task_group_id in task_group_ids:
                task_group = full_data_set.task_groups[task_group_id]
                task_group.add_unit(unit)

            else:
                if task_group_id == 1:
                    # SWGB uses the same IDs as AoC
                    line_id = GenieUnitTaskGroup.male_line_id

                elif task_group_id == 2:
                    # No differences to task group 1; probably unused
                    continue

                task_group = GenieUnitTaskGroup(line_id, task_group_id, full_data_set)
                task_group.add_unit(unit)
                full_data_set.task_groups.update({task_group_id: task_group})

            task_group_ids.add(task_group_id)
            unit_ids.add(unit["id0"].value)

        # Create the villager task group
        villager = GenieVillagerGroup(118, task_group_ids, full_data_set)
        full_data_set.unit_lines.update({villager.get_id(): villager})
        # TODO: Find the line id elsewhere
        full_data_set.unit_lines_vertical_ref.update({36: villager})
        full_data_set.villager_groups.update({villager.get_id(): villager})
        for unit_id in unit_ids:
            full_data_set.unit_ref.update({unit_id: villager})

    @staticmethod
    def create_ambient_groups(full_data_set: GenieObjectContainer) -> None:
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
            ambient_group = GenieAmbientGroup(ambient_id, full_data_set)
            ambient_group.add_unit(genie_units[ambient_id])
            full_data_set.ambient_groups.update({ambient_group.get_id(): ambient_group})
            full_data_set.unit_ref.update({ambient_id: ambient_group})

    @staticmethod
    def create_variant_groups(full_data_set: GenieObjectContainer) -> None:
        """
        Create variant groups.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        """
        variants = VARIANT_GROUP_LOOKUPS

        for group_id, variant in variants.items():
            variant_group = GenieVariantGroup(group_id, full_data_set)
            full_data_set.variant_groups.update({variant_group.get_id(): variant_group})

            for variant_id in variant[2]:
                variant_group.add_unit(full_data_set.genie_units[variant_id])
                full_data_set.unit_ref.update({variant_id: variant_group})

    @staticmethod
    def create_tech_groups(full_data_set: GenieObjectContainer) -> None:
        """
        Create techs from tech connections and unit upgrades/unlocks
        from unit connections.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        """
        tech_connections = full_data_set.tech_connections

        for connection in tech_connections.values():
            tech_id = connection["id"].value
            tech = full_data_set.genie_techs[tech_id]

            effect_id = tech["tech_effect_id"].value
            if effect_id < 0:
                continue

            tech_effects = full_data_set.genie_effect_bundles[effect_id]

            # Check if the tech is an age upgrade
            tech_found = False
            resource_effects = tech_effects.get_effects(effect_type=1)
            for effect in resource_effects:
                # Resource ID 6: Current Age
                if effect["attr_a"].value != 6:
                    continue

                age_id = effect["attr_b"].value
                age_up = AgeUpgrade(tech_id, age_id, full_data_set)
                full_data_set.tech_groups.update({age_up.get_id(): age_up})
                full_data_set.age_upgrades.update({age_up.get_id(): age_up})
                tech_found = True
                break

            if tech_found:
                continue

            # Building unlocks/upgrades are not in SWGB tech connections

            # Create a stat upgrade for other techs
            stat_up = StatUpgrade(tech_id, full_data_set)
            full_data_set.tech_groups.update({stat_up.get_id(): stat_up})
            full_data_set.stat_upgrades.update({stat_up.get_id(): stat_up})

        # Unit upgrades and unlocks are stored in unit connections
        unit_connections = full_data_set.unit_connections
        unit_unlocks = {}
        unit_upgrades = {}
        for connection in unit_connections.values():
            unit_id = connection["id"].value
            required_research_id = connection["required_research"].value
            enabling_research_id = connection["enabling_research"].value
            line_mode = connection["line_mode"].value
            line_id = full_data_set.unit_ref[unit_id].get_id()

            if required_research_id == -1 and enabling_research_id == -1:
                # Unit is unlocked from the start
                continue

            if line_mode == 2:
                # Unit is first in line, there should be an unlock tech
                unit_unlock = SWGBUnitUnlock(enabling_research_id, line_id, full_data_set)
                unit_unlocks.update({unit_id: unit_unlock})

            elif line_mode == 3:
                # Units further down the line receive line upgrades
                unit_upgrade = SWGBUnitLineUpgrade(required_research_id, line_id,
                                                   unit_id, full_data_set)
                unit_upgrades.update({required_research_id: unit_upgrade})

        # Unit unlocks for civ lines
        final_unit_unlocks = {}
        for unit_unlock in unit_unlocks.values():
            line = unit_unlock.get_unlocked_line()
            if line.get_head_unit_id() not in CIV_LINE_ASSOCS:
                for main_head_unit_id, civ_head_unit_ids in CIV_LINE_ASSOCS.items():
                    if line.get_head_unit_id() in civ_head_unit_ids:
                        if isinstance(line, SWGBUnitTransformGroup):
                            main_head_unit_id = line.get_transform_unit_id()

                        # The line is an alternative civ line so the unlock
                        # is stored with the main unlock
                        main_unlock = unit_unlocks[main_head_unit_id]
                        main_unlock.add_civ_unlock(unit_unlock)
                        break

                else:
                    # The unlock is for a line without alternative civ lines
                    final_unit_unlocks.update({unit_unlock.get_id(): unit_unlock})

            else:
                # The unlock is for a main line
                final_unit_unlocks.update({unit_unlock.get_id(): unit_unlock})

        full_data_set.tech_groups.update(final_unit_unlocks)
        full_data_set.unit_unlocks.update(final_unit_unlocks)

        # Unit upgrades for civ lines
        final_unit_upgrades = {}
        for unit_upgrade in unit_upgrades.values():
            tech_id = unit_upgrade.tech.get_id()
            if tech_id not in CIV_TECH_ASSOCS:
                for main_tech_id, civ_tech_ids in CIV_TECH_ASSOCS.items():
                    if tech_id in civ_tech_ids:
                        # The tech is upgrade for an alternative civ so the upgrade
                        # is stored with the main upgrade
                        main_upgrade = unit_upgrades[main_tech_id]
                        main_upgrade.add_civ_upgrade(unit_upgrade)
                        break

                else:
                    # The upgrade is for a line without alternative civ lines
                    final_unit_upgrades.update({unit_upgrade.get_id(): unit_upgrade})

            else:
                # The upgrade is for a main line
                final_unit_upgrades.update({unit_upgrade.get_id(): unit_upgrade})

        full_data_set.tech_groups.update(final_unit_upgrades)
        full_data_set.unit_upgrades.update(final_unit_upgrades)

        # Initiated techs are stored with buildings
        genie_units = full_data_set.genie_units
        for genie_unit in genie_units.values():
            if not genie_unit.has_member("research_id"):
                continue

            building_id = genie_unit["id0"].value
            initiated_tech_id = genie_unit["research_id"].value

            if initiated_tech_id == -1:
                continue

            if building_id not in full_data_set.building_lines.keys():
                # Skips upgraded buildings (which initiate the same techs)
                continue

            initiated_tech = InitiatedTech(initiated_tech_id, building_id, full_data_set)
            full_data_set.tech_groups.update({initiated_tech.get_id(): initiated_tech})
            full_data_set.initiated_techs.update({initiated_tech.get_id(): initiated_tech})

        # Civ boni have to be aquired from techs
        # Civ boni = ONLY passive boni (not unit unlocks, unit upgrades or team bonus)
        genie_techs = full_data_set.genie_techs
        for index, _ in enumerate(genie_techs):
            tech_id = index

            # Civ ID must be positive and non-zero
            civ_id = genie_techs[index]["civilization_id"].value
            if civ_id <= 0:
                continue

            # Passive boni are not researched anywhere
            research_location_id = genie_techs[index]["research_location_id"].value
            if research_location_id > 0:
                continue

            # Passive boni are not available in full tech mode
            full_tech_mode = genie_techs[index]["full_tech_mode"].value
            if full_tech_mode:
                continue

            civ_bonus = CivBonus(tech_id, civ_id, full_data_set)
            full_data_set.tech_groups.update({civ_bonus.get_id(): civ_bonus})
            full_data_set.civ_boni.update({civ_bonus.get_id(): civ_bonus})

    @staticmethod
    def link_garrison(full_data_set: GenieObjectContainer) -> None:
        """
        Link a garrison unit to the lines that are stored and vice versa. This is done
        to provide quick access during conversion.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        """
        garrisoned_lines = {}
        garrisoned_lines.update(full_data_set.unit_lines)
        garrisoned_lines.update(full_data_set.ambient_groups)

        garrison_lines = {}
        garrison_lines.update(full_data_set.unit_lines)
        garrison_lines.update(full_data_set.building_lines)

        # Search through all units and look at their garrison commands
        for unit_line in garrisoned_lines.values():
            garrison_classes = []
            garrison_units = []

            if unit_line.has_command(3):
                unit_commands = unit_line.get_head_unit()["unit_commands"].value
                for command in unit_commands:
                    type_id = command["type"].value

                    if type_id != 3:
                        continue

                    class_id = command["class_id"].value
                    if class_id > -1:
                        garrison_classes.append(class_id)

                        if class_id == 18:
                            # Towers because LucasArts ALSO didn't like consistent rules
                            garrison_classes.append(10)

                    unit_id = command["unit_id"].value
                    if unit_id > -1:
                        garrison_units.append(unit_id)

            for garrison_line in garrison_lines.values():
                if not garrison_line.is_garrison():
                    continue

                # Natural garrison
                garrison_mode = garrison_line.get_garrison_mode()
                if garrison_mode == GenieGarrisonMode.NATURAL:
                    if unit_line.get_head_unit().has_member("creatable_type"):
                        creatable_type = unit_line.get_head_unit()["creatable_type"].value

                    else:
                        creatable_type = 0

                    if garrison_line.get_head_unit().has_member("garrison_type"):
                        garrison_type = garrison_line.get_head_unit()["garrison_type"].value

                    else:
                        garrison_type = 0

                    if creatable_type == 1 and not garrison_type & 0x01:
                        continue

                    if creatable_type == 2 and not garrison_type & 0x02:
                        continue

                    if creatable_type == 3 and not garrison_type & 0x04:
                        continue

                    if creatable_type == 6 and not garrison_type & 0x08:
                        continue

                    if (creatable_type == 0 and unit_line.get_class_id() == 1) and not\
                            garrison_type & 0x10:
                        # Bantha/Nerf
                        continue

                    if garrison_line.get_class_id() in garrison_classes:
                        unit_line.garrison_locations.append(garrison_line)
                        garrison_line.garrison_entities.append(unit_line)
                        continue

                    if garrison_line.get_head_unit_id() in garrison_units:
                        unit_line.garrison_locations.append(garrison_line)
                        garrison_line.garrison_entities.append(unit_line)
                        continue

                # Transports/ unit garrisons (no conditions)
                elif garrison_mode in (GenieGarrisonMode.TRANSPORT,
                                       GenieGarrisonMode.UNIT_GARRISON):
                    if garrison_line.get_class_id() in garrison_classes:
                        unit_line.garrison_locations.append(garrison_line)
                        garrison_line.garrison_entities.append(unit_line)

                # Self produced units (these cannot be determined from commands)
                elif garrison_mode == GenieGarrisonMode.SELF_PRODUCED:
                    if unit_line in garrison_line.creates:
                        unit_line.garrison_locations.append(garrison_line)
                        garrison_line.garrison_entities.append(unit_line)

                # Jedi/Sith inventories
                elif garrison_mode == GenieGarrisonMode.MONK:
                    # Search for a pickup command
                    unit_commands = garrison_line.get_head_unit()["unit_commands"].value
                    for command in unit_commands:
                        type_id = command["type"].value

                        if type_id != 132:
                            continue

                        unit_id = command["unit_id"].value
                        if unit_id == unit_line.get_head_unit_id():
                            unit_line.garrison_locations.append(garrison_line)
                            garrison_line.garrison_entities.append(unit_line)

    @staticmethod
    def link_repairables(full_data_set: GenieObjectContainer) -> None:
        """
        Set units/buildings as repairable

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        """
        villager_groups = full_data_set.villager_groups

        repair_lines = {}
        repair_lines.update(full_data_set.unit_lines)
        repair_lines.update(full_data_set.building_lines)

        repair_classes = []
        for villager in villager_groups.values():
            repair_unit = villager.get_units_with_command(106)[0]
            unit_commands = repair_unit["unit_commands"].value
            for command in unit_commands:
                type_id = command["type"].value

                if type_id != 106:
                    continue

                class_id = command["class_id"].value
                if class_id == -1:
                    # Buildings/Siege
                    repair_classes.append(10)
                    repair_classes.append(18)
                    repair_classes.append(32)
                    repair_classes.append(33)
                    repair_classes.append(34)
                    repair_classes.append(35)
                    repair_classes.append(36)
                    repair_classes.append(53)

                else:
                    repair_classes.append(class_id)

        for repair_line in repair_lines.values():
            if repair_line.get_class_id() in repair_classes:
                repair_line.repairable = True
