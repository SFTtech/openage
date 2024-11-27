# Copyright 2019-2024 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-lines,too-many-branches,too-many-statements
# pylint: disable=too-many-locals,too-many-public-methods
"""
Convert data from AoC to openage formats.
"""
from __future__ import annotations
import typing


from .....log import info
from ....entity_object.conversion.aoc.genie_civ import GenieCivilizationGroup
from ....entity_object.conversion.aoc.genie_civ import GenieCivilizationObject
from ....entity_object.conversion.aoc.genie_connection import GenieAgeConnection, \
    GenieBuildingConnection, GenieUnitConnection, GenieTechConnection
from ....entity_object.conversion.aoc.genie_effect import GenieEffectObject, \
    GenieEffectBundle
from ....entity_object.conversion.aoc.genie_graphic import GenieGraphic
from ....entity_object.conversion.aoc.genie_object_container import GenieObjectContainer
from ....entity_object.conversion.aoc.genie_sound import GenieSound
from ....entity_object.conversion.aoc.genie_tech import AgeUpgrade, \
    UnitUnlock, UnitLineUpgrade, CivBonus
from ....entity_object.conversion.aoc.genie_tech import BuildingLineUpgrade
from ....entity_object.conversion.aoc.genie_tech import GenieTechObject
from ....entity_object.conversion.aoc.genie_tech import StatUpgrade, InitiatedTech, \
    BuildingUnlock
from ....entity_object.conversion.aoc.genie_terrain import GenieTerrainGroup, \
    GenieTerrainObject, GenieTerrainRestriction
from ....entity_object.conversion.aoc.genie_unit import GenieAmbientGroup, \
    GenieGarrisonMode
from ....entity_object.conversion.aoc.genie_unit import GenieStackBuildingGroup, \
    GenieBuildingLineGroup
from ....entity_object.conversion.aoc.genie_unit import GenieUnitLineGroup, \
    GenieUnitTransformGroup, GenieMonkGroup
from ....entity_object.conversion.aoc.genie_unit import GenieUnitObject
from ....entity_object.conversion.aoc.genie_unit import GenieUnitTaskGroup, \
    GenieVillagerGroup
from ....entity_object.conversion.aoc.genie_unit import GenieVariantGroup
from ....service.debug_info import debug_converter_objects, \
    debug_converter_object_groups
from ....service.read.nyan_api_loader import load_api
from ....value_object.conversion.aoc.internal_nyan_names import AMBIENT_GROUP_LOOKUPS, \
    VARIANT_GROUP_LOOKUPS
from .media_subprocessor import AoCMediaSubprocessor
from .modpack_subprocessor import AoCModpackSubprocessor
from .nyan_subprocessor import AoCNyanSubprocessor
from .pregen_processor import AoCPregenSubprocessor

if typing.TYPE_CHECKING:
    from argparse import Namespace
    from openage.convert.entity_object.conversion.stringresource import StringResource
    from openage.convert.entity_object.conversion.modpack import Modpack
    from openage.convert.value_object.read.value_members import ArrayMember
    from openage.convert.value_object.init.game_version import GameVersion


class AoCProcessor:
    """
    Main processor for converting data from AoC.
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
        Input game speification and media here and get a set of
        modpacks back.

        :param gamespec: Gamedata from empires.dat read in by the
                         reader functions.
        :type gamespec: ...dataformat.value_members.ArrayMember
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
        :type gamespec: ...dataformat.value_members.ArrayMember
        """
        dataset = GenieObjectContainer()

        dataset.game_version = game_version
        dataset.nyan_api_objects = load_api()
        dataset.strings = string_resources
        dataset.existing_graphics = existing_graphics

        info("Extracting Genie data...")

        cls.extract_genie_units(gamespec, dataset)
        cls.extract_genie_techs(gamespec, dataset)
        cls.extract_genie_effect_bundles(gamespec, dataset)
        cls.sanitize_effect_bundles(dataset)
        cls.extract_genie_civs(gamespec, dataset)
        cls.extract_age_connections(gamespec, dataset)
        cls.extract_building_connections(gamespec, dataset)
        cls.extract_unit_connections(gamespec, dataset)
        cls.extract_tech_connections(gamespec, dataset)
        cls.extract_genie_graphics(gamespec, dataset)
        cls.extract_genie_sounds(gamespec, dataset)
        cls.extract_genie_terrains(gamespec, dataset)
        cls.extract_genie_restrictions(gamespec, dataset)

        return dataset

    @classmethod
    def _processor(cls, full_data_set: GenieObjectContainer) -> GenieObjectContainer:
        """
        Transfer structures used in Genie games to more openage-friendly
        Python objects.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        """

        info("Creating API-like objects...")

        cls.create_unit_lines(full_data_set)
        cls.create_extra_unit_lines(full_data_set)
        cls.create_building_lines(full_data_set)
        cls.create_villager_groups(full_data_set)
        cls.create_ambient_groups(full_data_set)
        cls.create_variant_groups(full_data_set)
        cls.create_terrain_groups(full_data_set)
        cls.create_tech_groups(full_data_set)
        cls.create_civ_groups(full_data_set)

        info("Linking API-like objects...")

        cls.link_building_upgrades(full_data_set)
        cls.link_creatables(full_data_set)
        cls.link_researchables(full_data_set)
        cls.link_civ_uniques(full_data_set)
        cls.link_gatherers_to_dropsites(full_data_set)
        cls.link_garrison(full_data_set)
        cls.link_trade_posts(full_data_set)
        cls.link_repairables(full_data_set)

        info("Generating auxiliary objects...")

        AoCPregenSubprocessor.generate(full_data_set)

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

        AoCNyanSubprocessor.convert(full_data_set)

        info("Creating requests for media export...")

        AoCMediaSubprocessor.convert(full_data_set)

        return AoCModpackSubprocessor.get_modpacks(full_data_set)

    @staticmethod
    def extract_genie_units(gamespec: ArrayMember, full_data_set: GenieObjectContainer) -> None:
        """
        Extract units from the game data.

        :param gamespec: Gamedata from empires.dat file.
        :type gamespec: ...dataformat.value_members.ArrayMember
        """
        # Units are stored in the civ container.
        # All civs point to the same units (?) except for Gaia which has more.
        # Gaia also seems to have the most units, so we only read from Gaia
        #
        # call hierarchy: wrapper[0]->civs[0]->units
        raw_units = gamespec[0]["civs"][0]["units"].value

        # Unit headers store the things units can do
        raw_unit_headers = gamespec[0]["unit_headers"].value

        for raw_unit in raw_units:
            unit_id = raw_unit["id0"].value
            unit_members = raw_unit.value

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

            # Commands
            unit_commands = raw_unit_headers[unit_id]["unit_commands"]
            unit.add_member(unit_commands)

    @staticmethod
    def extract_genie_techs(gamespec: ArrayMember, full_data_set: GenieObjectContainer) -> None:
        """
        Extract techs from the game data.

        :param gamespec: Gamedata from empires.dat file.
        :type gamespec: ...dataformat.value_members.ArrayMember
        """
        # Techs are stored as "researches".
        #
        # call hierarchy: wrapper[0]->researches
        raw_techs = gamespec[0]["researches"].value

        index = 0
        for raw_tech in raw_techs:
            tech_id = index
            tech_members = raw_tech.value

            tech = GenieTechObject(tech_id, full_data_set, members=tech_members)
            full_data_set.genie_techs.update({tech.get_id(): tech})

            index += 1

    @staticmethod
    def extract_genie_effect_bundles(
        gamespec: ArrayMember,
        full_data_set: GenieObjectContainer
    ) -> None:
        """
        Extract effects and effect bundles from the game data.

        :param gamespec: Gamedata from empires.dat file.
        :type gamespec: ...dataformat.value_members.ArrayMember
        """
        # call hierarchy: wrapper[0]->effect_bundles
        raw_effect_bundles = gamespec[0]["effect_bundles"].value

        index_bundle = 0
        for raw_effect_bundle in raw_effect_bundles:
            bundle_id = index_bundle

            # call hierarchy: effect_bundle->effects
            raw_effects = raw_effect_bundle["effects"].value

            effects = {}

            index_effect = 0
            for raw_effect in raw_effects:
                effect_id = index_effect
                effect_members = raw_effect.value

                effect = GenieEffectObject(effect_id, bundle_id, full_data_set,
                                           members=effect_members)

                effects.update({effect_id: effect})

                index_effect += 1

            # Pass everything to the bundle
            effect_bundle_members = raw_effect_bundle.value
            # Remove effects we store them as separate objects
            effect_bundle_members.pop("effects")

            bundle = GenieEffectBundle(bundle_id, effects, full_data_set,
                                       members=effect_bundle_members)
            full_data_set.genie_effect_bundles.update({bundle.get_id(): bundle})

            index_bundle += 1

    @staticmethod
    def extract_genie_civs(
        gamespec: ArrayMember,
        full_data_set: GenieObjectContainer
    ) -> None:
        """
        Extract civs from the game data.

        :param gamespec: Gamedata from empires.dat file.
        :type gamespec: class: ...dataformat.value_members.ArrayMember
        """
        # call hierarchy: wrapper[0]->civs
        raw_civs = gamespec[0]["civs"].value

        index = 0
        for raw_civ in raw_civs:
            civ_id = index

            civ_members = raw_civ.value
            units_member = civ_members.pop("units")
            units_member = units_member.get_container("id0")

            civ_members.update({"units": units_member})

            civ = GenieCivilizationObject(civ_id, full_data_set, members=civ_members)
            full_data_set.genie_civs.update({civ.get_id(): civ})

            index += 1

    @staticmethod
    def extract_age_connections(gamespec: ArrayMember, full_data_set: GenieObjectContainer) -> None:
        """
        Extract age connections from the game data.

        :param gamespec: Gamedata from empires.dat file.
        :type gamespec: class: ...dataformat.value_members.ArrayMember
        """
        # call hierarchy: wrapper[0]->age_connections
        raw_connections = gamespec[0]["age_connections"].value

        for raw_connection in raw_connections:
            age_id = raw_connection["id"].value
            connection_members = raw_connection.value

            connection = GenieAgeConnection(age_id, full_data_set, members=connection_members)
            full_data_set.age_connections.update({connection.get_id(): connection})

    @staticmethod
    def extract_building_connections(
        gamespec: ArrayMember,
        full_data_set: GenieObjectContainer
    ) -> None:
        """
        Extract building connections from the game data.

        :param gamespec: Gamedata from empires.dat file.
        :type gamespec: class: ...dataformat.value_members.ArrayMember
        """
        # call hierarchy: wrapper[0]->building_connections
        raw_connections = gamespec[0]["building_connections"].value

        for raw_connection in raw_connections:
            building_id = raw_connection["id"].value
            connection_members = raw_connection.value

            connection = GenieBuildingConnection(building_id, full_data_set,
                                                 members=connection_members)
            full_data_set.building_connections.update({connection.get_id(): connection})

    @staticmethod
    def extract_unit_connections(
        gamespec: ArrayMember,
        full_data_set: GenieObjectContainer
    ) -> None:
        """
        Extract unit connections from the game data.

        :param gamespec: Gamedata from empires.dat file.
        :type gamespec: class: ...dataformat.value_members.ArrayMember
        """
        # call hierarchy: wrapper[0]->unit_connections
        raw_connections = gamespec[0]["unit_connections"].value

        for raw_connection in raw_connections:
            unit_id = raw_connection["id"].value
            connection_members = raw_connection.value

            connection = GenieUnitConnection(unit_id, full_data_set, members=connection_members)
            full_data_set.unit_connections.update({connection.get_id(): connection})

    @staticmethod
    def extract_tech_connections(
        gamespec: ArrayMember,
        full_data_set: GenieObjectContainer
    ) -> None:
        """
        Extract tech connections from the game data.

        :param gamespec: Gamedata from empires.dat file.
        :type gamespec: class: ...dataformat.value_members.ArrayMember
        """
        # call hierarchy: wrapper[0]->tech_connections
        raw_connections = gamespec[0]["tech_connections"].value

        for raw_connection in raw_connections:
            tech_id = raw_connection["id"].value
            connection_members = raw_connection.value

            connection = GenieTechConnection(tech_id, full_data_set, members=connection_members)
            full_data_set.tech_connections.update({connection.get_id(): connection})

    @staticmethod
    def extract_genie_graphics(gamespec: ArrayMember, full_data_set: GenieObjectContainer) -> None:
        """
        Extract graphic definitions from the game data.

        :param gamespec: Gamedata from empires.dat file.
        :type gamespec: class: ...dataformat.value_members.ArrayMember
        """
        # call hierarchy: wrapper[0]->graphics
        raw_graphics = gamespec[0]["graphics"].value

        for raw_graphic in raw_graphics:
            # Can be ignored if there is no filename associated
            filename = raw_graphic["filename"].value
            if not filename:
                continue

            graphic_id = raw_graphic["graphic_id"].value
            graphic_members = raw_graphic.value

            graphic = GenieGraphic(graphic_id, full_data_set, members=graphic_members)
            slp_id = raw_graphic["slp_id"].value
            if str(slp_id) not in full_data_set.existing_graphics:
                graphic.exists = False

            full_data_set.genie_graphics.update({graphic.get_id(): graphic})

        # Detect subgraphics
        for genie_graphic in full_data_set.genie_graphics.values():
            genie_graphic.detect_subgraphics()

    @staticmethod
    def extract_genie_sounds(gamespec: ArrayMember, full_data_set: GenieObjectContainer) -> None:
        """
        Extract sound definitions from the game data.

        :param gamespec: Gamedata from empires.dat file.
        :type gamespec: class: ...dataformat.value_members.ArrayMember
        """
        # call hierarchy: wrapper[0]->sounds
        raw_sounds = gamespec[0]["sounds"].value

        for raw_sound in raw_sounds:
            sound_id = raw_sound["sound_id"].value
            sound_members = raw_sound.value

            sound = GenieSound(sound_id, full_data_set, members=sound_members)
            full_data_set.genie_sounds.update({sound.get_id(): sound})

    @staticmethod
    def extract_genie_terrains(gamespec: ArrayMember, full_data_set: GenieObjectContainer) -> None:
        """
        Extract terrains from the game data.

        :param gamespec: Gamedata from empires.dat file.
        :type gamespec: class: ...dataformat.value_members.ArrayMember
        """
        # call hierarchy: wrapper[0]->terrains
        raw_terrains = gamespec[0]["terrains"].value

        for index, raw_terrain in enumerate(raw_terrains):
            terrain_index = index
            terrain_members = raw_terrain.value

            terrain = GenieTerrainObject(terrain_index, full_data_set, members=terrain_members)
            full_data_set.genie_terrains.update({terrain.get_id(): terrain})

    @staticmethod
    def extract_genie_restrictions(
        gamespec: ArrayMember,
        full_data_set: GenieObjectContainer
    ) -> None:
        """
        Extract terrain restrictions from the game data.

        :param gamespec: Gamedata from empires.dat file.
        :type gamespec: class: ...dataformat.value_members.ArrayMember
        """
        # call hierarchy: wrapper[0]->terrains
        raw_restrictions = gamespec[0]["terrain_restrictions"].value

        for index, raw_restriction in enumerate(raw_restrictions):
            restriction_index = index
            restriction_members = raw_restriction.value

            restriction = GenieTerrainRestriction(restriction_index,
                                                  full_data_set,
                                                  members=restriction_members)
            full_data_set.genie_terrain_restrictions.update({restriction.get_id(): restriction})

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

        # First only handle the line heads (firstunits in a line)
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
                # Trebuchet
                unit_line = GenieUnitTransformGroup(unit_id, unit_id, full_data_set)
                full_data_set.transform_groups.update({unit_line.get_id(): unit_line})

            elif unit_id == 125:
                # Monks
                # Switch to monk with relic is hardcoded :(
                unit_line = GenieMonkGroup(unit_id, unit_id, 286, full_data_set)
                full_data_set.monk_groups.update({unit_line.get_id(): unit_line})

            elif unit.has_member("task_group")\
                    and unit["task_group"].value > 0:
                # Villager
                # done somewhere else because they are special^TM
                continue

            else:
                # Normal units
                unit_line = GenieUnitLineGroup(unit_id, full_data_set)

            unit_line.add_unit(unit)
            full_data_set.unit_lines.update({unit_line.get_id(): unit_line})
            full_data_set.unit_ref.update({unit_id: unit_line})

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
                raise RuntimeError(f"Unit {unit_id} is not first in line, but no previous "
                                   "unit can be found in other_connections")

            connected_ids = connection["other_connected_ids"].value
            previous_unit_id = connected_ids[connected_index].value

            # Search for the first unit ID in the line recursively
            previous_id = previous_unit_id
            previous_connection = unit_connections[previous_unit_id]
            while previous_connection["line_mode"] != 2:
                if previous_id in full_data_set.unit_ref.keys():
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

            unit_line = full_data_set.unit_ref[previous_id]
            unit_line.add_unit(unit, after=previous_unit_id)
            full_data_set.unit_ref.update({unit_id: unit_line})

    @staticmethod
    def create_extra_unit_lines(full_data_set: GenieObjectContainer) -> None:
        """
        Create additional units that are not in the unit connections.

        :param full_data_set: GenieObjectContainer instance that
                                contains all relevant data for the conversion
                                process.
        :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        """
        extra_units = (48, 65, 594, 833)  # Wildlife

        for unit_id in extra_units:
            unit_line = GenieUnitLineGroup(unit_id, full_data_set)
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

        for connection in building_connections.values():
            building_id = connection["id"].value
            building = full_data_set.genie_units[building_id]
            previous_building_id = None
            stack_building = False

            # Buildings have no actual lines, so we use
            # their unit ID as the line ID.
            line_id = building_id

            # Check if we have to create a GenieStackBuildingGroup
            if building.has_member("stack_unit_id") and \
                    building["stack_unit_id"].value > -1:
                stack_building = True

            if building.has_member("head_unit_id") and \
                    building["head_unit_id"].value > -1:
                # we don't care about head units because we process
                # them with their stack unit
                continue

            # Check if the building is part of an existing line.
            # To do this, we look for connected techs and
            # check if any tech has an upgrade effect.
            connected_types = connection["other_connections"].value
            connected_tech_indices = []
            for index, _ in enumerate(connected_types):
                connected_type = connected_types[index]["other_connection"].value
                if connected_type == 3:
                    # 3 == Tech
                    connected_tech_indices.append(index)

            connected_ids = connection["other_connected_ids"].value

            for index in connected_tech_indices:
                connected_tech_id = connected_ids[index].value
                connected_tech = full_data_set.genie_techs[connected_tech_id]
                effect_bundle_id = connected_tech["tech_effect_id"].value
                effect_bundle = full_data_set.genie_effect_bundles[effect_bundle_id]

                upgrade_effects = effect_bundle.get_effects(effect_type=3)

                if len(upgrade_effects) == 0:
                    continue

                # Search upgrade effects for the line_id
                for upgrade in upgrade_effects:
                    upgrade_source = upgrade["attr_a"].value
                    upgrade_target = upgrade["attr_b"].value

                    # Check if the upgrade target is correct
                    if upgrade_target == building_id:
                        # Line id is the source building id
                        line_id = upgrade_source
                        break

                else:
                    # If no upgrade was found, then search remaining techs
                    continue

                # Find the previous building
                for c_index, _ in enumerate(connected_types):
                    connected_type = connected_types[c_index]["other_connection"].value
                    if connected_type == 1:
                        # 1 == Building
                        connected_index = c_index
                        break

                else:
                    raise RuntimeError(f"Building {building_id} is not first in line, but no "
                                       "previous building could be found in other_connections")

                previous_building_id = connected_ids[connected_index].value
                break

            if line_id == building_id:
                # First building in line
                if stack_building:
                    stack_unit_id = building["stack_unit_id"].value
                    building_line = GenieStackBuildingGroup(stack_unit_id, line_id, full_data_set)

                else:
                    building_line = GenieBuildingLineGroup(line_id, full_data_set)

                full_data_set.building_lines.update({building_line.get_id(): building_line})
                building_line.add_unit(building, after=previous_building_id)
                full_data_set.unit_ref.update({building_id: building_line})

            else:
                # It's an upgraded building
                building_line = full_data_set.building_lines[line_id]
                building_line.add_unit(building, after=previous_building_id)
                full_data_set.unit_ref.update({building_id: building_line})

    @staticmethod
    def sanitize_effect_bundles(full_data_set: GenieObjectContainer) -> None:
        """
        Remove garbage data from effect bundles.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        """
        effect_bundles = full_data_set.genie_effect_bundles

        for bundle in effect_bundles.values():
            sanitized_effects = {}

            effects = bundle.get_effects()

            index = 0
            for effect in effects:
                effect_type = effect["type_id"].value
                if effect_type < 0:
                    # Effect has no type
                    continue

                if effect_type == 3:
                    if effect["attr_b"].value < 0:
                        # Upgrade to invalid unit
                        continue

                if effect_type == 102:
                    if effect["attr_d"].value < 0:
                        # Tech disable effect with no tech id specified
                        continue

                sanitized_effects.update({index: effect})
                index += 1

            bundle.effects = sanitized_effects
            bundle.sanitized = True

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

        # In tech connection are age ups, building unlocks/upgrades and stat upgrades
        for connection in tech_connections.values():
            connected_buildings = connection["buildings"].value
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

            if len(connected_buildings) > 0:
                # Building unlock or upgrade
                unlock_effects = tech_effects.get_effects(effect_type=2)
                upgrade_effects = tech_effects.get_effects(effect_type=2)
                if len(unlock_effects) > 0:
                    unlock = unlock_effects[0]
                    unlock_id = unlock["attr_a"].value

                    building_unlock = BuildingUnlock(tech_id, unlock_id, full_data_set)
                    full_data_set.tech_groups.update(
                        {building_unlock.get_id(): building_unlock}
                    )
                    full_data_set.building_unlocks.update(
                        {building_unlock.get_id(): building_unlock}
                    )
                    continue

                if len(upgrade_effects) > 0:
                    upgrade = upgrade_effects[0]
                    line_id = upgrade["attr_a"].value
                    upgrade_id = upgrade["attr_b"].value

                    building_upgrade = BuildingLineUpgrade(
                        tech_id,
                        line_id,
                        upgrade_id,
                        full_data_set
                    )
                    full_data_set.tech_groups.update(
                        {building_upgrade.get_id(): building_upgrade}
                    )
                    full_data_set.building_upgrades.update(
                        {building_upgrade.get_id(): building_upgrade}
                    )
                    continue

            # Create a stat upgrade for other techs
            stat_up = StatUpgrade(tech_id, full_data_set)
            full_data_set.tech_groups.update({stat_up.get_id(): stat_up})
            full_data_set.stat_upgrades.update({stat_up.get_id(): stat_up})

        # Unit upgrades and unlocks are stored in unit connections
        unit_connections = full_data_set.unit_connections
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
                # Unit is first in line, there should be an unlock tech id
                # This is usually the enabling tech id
                unlock_tech_id = enabling_research_id
                if unlock_tech_id == -1:
                    # Battle elephant is a curious exception wher it's the required tech id
                    unlock_tech_id = required_research_id

                unit_unlock = UnitUnlock(unlock_tech_id, line_id, full_data_set)
                full_data_set.tech_groups.update({unit_unlock.get_id(): unit_unlock})
                full_data_set.unit_unlocks.update({unit_unlock.get_id(): unit_unlock})

            elif line_mode == 3:
                # Units further down the line receive line upgrades
                unit_upgrade = UnitLineUpgrade(required_research_id, line_id,
                                               unit_id, full_data_set)
                full_data_set.tech_groups.update({unit_upgrade.get_id(): unit_upgrade})
                full_data_set.unit_upgrades.update({unit_upgrade.get_id(): unit_upgrade})

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
    def create_civ_groups(full_data_set: GenieObjectContainer) -> None:
        """
        Create civilization groups from civ objects.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        """
        civ_objects = full_data_set.genie_civs

        for index in range(len(civ_objects)):
            civ_id = index

            civ_group = GenieCivilizationGroup(civ_id, full_data_set)
            full_data_set.civ_groups.update({civ_group.get_id(): civ_group})

            index += 1

    @staticmethod
    def create_villager_groups(full_data_set: GenieObjectContainer) -> None:
        """
        Create task groups and assign the relevant male and female group to a
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
                    line_id = GenieUnitTaskGroup.male_line_id

                elif task_group_id == 2:
                    line_id = GenieUnitTaskGroup.female_line_id

                task_group = GenieUnitTaskGroup(line_id, task_group_id, full_data_set)
                task_group.add_unit(unit)
                full_data_set.task_groups.update({task_group_id: task_group})

            task_group_ids.add(task_group_id)
            unit_ids.add(unit["id0"].value)

        # Create the villager task group
        villager = GenieVillagerGroup(118, task_group_ids, full_data_set)
        full_data_set.unit_lines.update({villager.get_id(): villager})
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
    def create_terrain_groups(full_data_set: GenieObjectContainer) -> None:
        """
        Create terrain groups.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        """
        terrains = full_data_set.genie_terrains.values()

        for terrain in terrains:
            slp_id = terrain["slp_id"].value
            replacement_id = terrain["terrain_replacement_id"].value

            if slp_id == -1 and replacement_id == -1:
                # No graphics and no graphics replacement means this terrain is unused
                continue

            enabled = terrain["enabled"].value

            if enabled:
                terrain_group = GenieTerrainGroup(terrain.get_id(), full_data_set)
                full_data_set.terrain_groups.update({terrain.get_id(): terrain_group})

    @staticmethod
    def link_building_upgrades(full_data_set: GenieObjectContainer) -> None:
        """
        Find building upgrades in the AgeUp techs and append them to the building lines.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        """
        age_ups = full_data_set.age_upgrades

        # Order of age ups should be correct
        for age_up in age_ups.values():
            for effect in age_up.effects.get_effects():
                type_id = effect.get_type()

                if type_id != 3:
                    continue

                upgrade_source_id = effect["attr_a"].value
                upgrade_target_id = effect["attr_b"].value

                if upgrade_source_id not in full_data_set.building_lines.keys():
                    continue

                upgraded_line = full_data_set.building_lines[upgrade_source_id]
                upgrade_target = full_data_set.genie_units[upgrade_target_id]

                upgraded_line.add_unit(upgrade_target)
                full_data_set.unit_ref.update({upgrade_target_id: upgraded_line})

    @staticmethod
    def link_creatables(full_data_set: GenieObjectContainer) -> None:
        """
        Link creatable units and buildings to their creating entity. This is done
        to provide quick access during conversion.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        """
        # Link units to buildings
        unit_lines = full_data_set.unit_lines

        for unit_line in unit_lines.values():
            if unit_line.is_creatable():
                train_location_id = unit_line.get_train_location_id()
                full_data_set.building_lines[train_location_id].add_creatable(unit_line)

        # Link buildings to villagers and fishing ships
        building_lines = full_data_set.building_lines

        for building_line in building_lines.values():
            if building_line.is_creatable():
                train_location_id = building_line.get_train_location_id()

                if train_location_id in full_data_set.villager_groups.keys():
                    full_data_set.villager_groups[train_location_id].add_creatable(building_line)

                else:
                    # try normal units
                    full_data_set.unit_lines[train_location_id].add_creatable(building_line)

    @staticmethod
    def link_researchables(full_data_set: GenieObjectContainer) -> None:
        """
        Link techs to their buildings. This is done
        to provide quick access during conversion.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        """
        tech_groups = full_data_set.tech_groups

        for tech in tech_groups.values():
            if tech.is_researchable():
                research_location_id = tech.get_research_location_id()
                full_data_set.building_lines[research_location_id].add_researchable(tech)

    @staticmethod
    def link_civ_uniques(full_data_set: GenieObjectContainer) -> None:
        """
        Link civ bonus techs, unique units and unique techs to their civs.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        """
        for bonus in full_data_set.civ_boni.values():
            civ_id = bonus.get_civilization()
            full_data_set.civ_groups[civ_id].add_civ_bonus(bonus)

        for unit_line in full_data_set.unit_lines.values():
            if unit_line.is_unique():
                head_unit_id = unit_line.get_head_unit_id()
                head_unit_connection = full_data_set.unit_connections[head_unit_id]
                enabling_research_id = head_unit_connection["enabling_research"].value
                enabling_research = full_data_set.genie_techs[enabling_research_id]
                enabling_civ_id = enabling_research["civilization_id"].value

                full_data_set.civ_groups[enabling_civ_id].add_unique_entity(unit_line)

        for building_line in full_data_set.building_lines.values():
            if building_line.is_unique():
                head_unit_id = building_line.get_head_unit_id()
                head_building_connection = full_data_set.building_connections[head_unit_id]
                enabling_research_id = head_building_connection["enabling_research"].value
                enabling_research = full_data_set.genie_techs[enabling_research_id]
                enabling_civ_id = enabling_research["civilization_id"].value

                full_data_set.civ_groups[enabling_civ_id].add_unique_entity(building_line)

        for tech_group in full_data_set.tech_groups.values():
            if tech_group.is_unique() and tech_group.is_researchable():
                civ_id = tech_group.get_civilization()
                full_data_set.civ_groups[civ_id].add_unique_tech(tech_group)

    @staticmethod
    def link_gatherers_to_dropsites(full_data_set: GenieObjectContainer) -> None:
        """
        Link gatherers to the buildings they drop resources off. This is done
        to provide quick access during conversion.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        """
        villager_groups = full_data_set.villager_groups

        for villager in villager_groups.values():
            for unit in villager.variants[0].line:
                drop_site_members = unit["drop_sites"].value
                unit_id = unit["id0"].value

                for drop_site_member in drop_site_members:
                    drop_site_id = drop_site_member.value

                    if drop_site_id > -1:
                        drop_site = full_data_set.building_lines[drop_site_id]
                        drop_site.add_gatherer_id(unit_id)

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

                        if class_id == 3:
                            # Towers because Ensemble didn't like consistent rules
                            garrison_classes.append(52)

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

                # Monk inventories
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
    def link_trade_posts(full_data_set: GenieObjectContainer) -> None:
        """
        Link a trade post building to the lines that it trades with.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        """
        unit_lines = full_data_set.unit_lines.values()

        for unit_line in unit_lines:
            if unit_line.has_command(111):
                head_unit = unit_line.get_head_unit()
                unit_commands = head_unit["unit_commands"].value
                trade_post_id = -1
                for command in unit_commands:
                    # Find the trade command and the trade post id
                    type_id = command["type"].value

                    if type_id != 111:
                        continue

                    trade_post_id = command["unit_id"].value

                    # Notify buiding
                    if trade_post_id in full_data_set.building_lines.keys():
                        full_data_set.building_lines[trade_post_id].add_trading_line(unit_line)

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
                    repair_classes.append(3)
                    repair_classes.append(13)
                    repair_classes.append(52)
                    repair_classes.append(54)
                    repair_classes.append(55)

                else:
                    repair_classes.append(class_id)

        for repair_line in repair_lines.values():
            if repair_line.get_class_id() in repair_classes:
                repair_line.repairable = True
