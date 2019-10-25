# Copyright 2019-2019 the openage authors. See copying.md for legal info.

"""
Convert data and/or media from AoC to openage formats.
"""
from ...dataformat.aoc.genie_object_container import GenieObjectContainer
from ...dataformat.aoc.genie_unit import GenieUnitObject
from ...dataformat.aoc.genie_tech import GenieTechObject
from ...dataformat.aoc.genie_effect import GenieEffectObject,\
    GenieEffectBundle
from ...dataformat.aoc.genie_civ import GenieCivilizationObject
from ...dataformat.aoc.genie_connection import GenieAgeConnection,\
    GenieBuildingConnection, GenieUnitConnection, GenieTechConnection
from ...dataformat.aoc.genie_graphic import GenieGraphic
from ...dataformat.aoc.genie_sound import GenieSound

from ...nyan.api_loader import load_api
from ...dataformat.aoc.genie_terrain import GenieTerrainObject
from ...dataformat.aoc.genie_unit import GenieUnitLineGroup,\
    GenieUnitTransformGroup, GenieMonkGroup, GenieVillagerGroup


class AoĆProcessor:

    # The interface to the API
    nyan_api_objects = load_api()

    @classmethod
    def convert(cls, gamespec, media):
        """
        Input game speification and media here and get a set of
        modpacks back.

        :param gamespec: Gamedata from empires.dat read in by the
                         reader functions.
        :type gamespec: class: ...dataformat.value_members.ArrayMember
        :param media: Pointers to media files/directories and related metadata.
        :returns: A list of modpacks.
        :rtype: list
        """

        # Create a new container for the conversion process
        data_set = cls._pre_processor(gamespec, media)

        # Create the custom openae formats (nyan, sprite, terrain)
        data_set = cls._processor(data_set)

        # Create modpack definitions
        modpacks = cls._post_processor(data_set)

        return modpacks

    @classmethod
    def _pre_processor(cls, gamespec, media):
        """
        Store data from the reader in a conversion container.

        :param gamespec: Gamedata from empires.dat file.
        :type gamespec: class: ...dataformat.value_members.ArrayMember
        """
        data_set = GenieObjectContainer()

        cls._extract_genie_units(gamespec, data_set)
        cls._extract_genie_techs(gamespec, data_set)
        cls._extract_genie_effect_bundles(gamespec, data_set)
        cls._extract_genie_civs(gamespec, data_set)
        cls._extract_age_connections(gamespec, data_set)
        cls._extract_building_connections(gamespec, data_set)
        cls._extract_unit_connections(gamespec, data_set)
        cls._extract_tech_connections(gamespec, data_set)
        cls._extract_genie_graphics(gamespec, data_set)
        cls._extract_genie_sounds(gamespec, data_set)
        cls._extract_genie_terrains(gamespec, data_set)

        return data_set

    @classmethod
    def _processor(cls, full_data_set):
        """
        1. Transfer structures used in Genie games to more openage-friendly
           Python objects.
        2. Convert these objects to nyan.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        """

        cls._create_unit_lines(full_data_set)

    @classmethod
    def _post_processor(cls, full_data_set):
        pass

    @staticmethod
    def _extract_genie_units(gamespec, full_data_set):
        """
        Extract units from the game data.

        :param gamespec: Gamedata from empires.dat file.
        :type gamespec: class: ...dataformat.value_members.ArrayMember
        """
        # Units are stored in the civ container.
        # All civs point to the same units (?) except for Gaia.
        # Gaia also seems to have the most units, so we only read from Gaia
        #
        # call hierarchy: wrapper[0]->civs[0]->units
        raw_units = gamespec.get_value()[0].get_value()["civs"].get_value()[0]\
            .get_value()["units"].get_value()

        # Unit headers store the things units can do
        raw_unit_headers = gamespec.get_value()[0].get_value()["unit_headers"].get_value()

        for raw_unit in raw_units:
            unit_id = raw_unit.get_value()["id0"].get_value()
            unit_members = raw_unit.get_value()

            # Created objects are added automatically to the data set.
            obj = GenieUnitObject(unit_id, full_data_set, members=unit_members)

            # Commands
            unit_commands = raw_unit_headers[unit_id].get_value()["unit_commands"]
            obj.add_member(unit_commands)

    @staticmethod
    def _extract_genie_techs(gamespec, full_data_set):
        """
        Extract techs from the game data.

        :param gamespec: Gamedata from empires.dat file.
        :type gamespec: class: ...dataformat.value_members.ArrayMember
        """
        # Techs are stored as "researches".
        #
        # call hierarchy: wrapper[0]->researches
        raw_techs = gamespec.get_value()[0].get_value()["researches"].get_value()

        index = 0
        for raw_tech in raw_techs:
            tech_id = index
            tech_members = raw_tech.get_value()

            # Created objects are added automatically to the data set.
            GenieTechObject(tech_id, full_data_set, members=tech_members)

            index += 1

    @staticmethod
    def _extract_genie_effect_bundles(gamespec, full_data_set):
        """
        Extract effects and effect bundles from the game data.

        :param gamespec: Gamedata from empires.dat file.
        :type gamespec: class: ...dataformat.value_members.ArrayMember
        """
        # call hierarchy: wrapper[0]->effect_bundles
        raw_effect_bundles = gamespec.get_value()[0].get_value()["effect_bundles"].get_value()

        index_bundle = 0
        for raw_effect_bundle in raw_effect_bundles:
            bundle_id = index_bundle

            # call hierarchy: effect_bundle->effects
            raw_effects = raw_effect_bundle.get_value()["effects"].get_value()

            effects = {}

            index_effect = 0
            for raw_effect in raw_effects:
                effect_id = index_effect
                effect_members = raw_effect.get_value()

                effect = GenieEffectObject(effect_id, bundle_id, full_data_set, members=effect_members)

                effects.update({effect_id: effect})

                index_effect += 1

            # Pass everything to the bundle
            effect_bundle_members = raw_effect_bundle.get_value()
            effect_bundle_members.pop("effects")                    # Removed because we store them as separate objects

            # Created objects are added automatically to the data set.
            GenieEffectBundle(bundle_id, effects, full_data_set, members=effect_bundle_members)

            index_bundle += 1

    @staticmethod
    def _extract_genie_civs(gamespec, full_data_set):
        """
        Extract civs (without units) from the game data.

        :param gamespec: Gamedata from empires.dat file.
        :type gamespec: class: ...dataformat.value_members.ArrayMember
        """
        # call hierarchy: wrapper[0]->civs
        raw_civs = gamespec.get_value()[0].get_value()["civs"].get_value()

        index = 0
        for raw_civ in raw_civs:
            civ_id = index

            civ_members = raw_civ.get_value()
            civ_members.pop("units")            # Removed because we store them as separate objects

            # Created objects are added automatically to the data set.
            GenieCivilizationObject(civ_id, full_data_set, members=civ_members)

            index += 1

    @staticmethod
    def _extract_age_connections(gamespec, full_data_set):
        """
        Extract age connections from the game data.

        :param gamespec: Gamedata from empires.dat file.
        :type gamespec: class: ...dataformat.value_members.ArrayMember
        """
        # call hierarchy: wrapper[0]->age_connections
        raw_connections = gamespec.get_value()[0].get_value()["age_connections"].get_value()

        for raw_connection in raw_connections:
            age_id = raw_connection.get_value()["id"].get_value()
            connection_members = raw_connection.get_value()

            # Created objects are added automatically to the data set.
            GenieAgeConnection(age_id, full_data_set, members=connection_members)

    @staticmethod
    def _extract_building_connections(gamespec, full_data_set):
        """
        Extract building connections from the game data.

        :param gamespec: Gamedata from empires.dat file.
        :type gamespec: class: ...dataformat.value_members.ArrayMember
        """
        # call hierarchy: wrapper[0]->building_connections
        raw_connections = gamespec.get_value()[0].get_value()["building_connections"].get_value()

        for raw_connection in raw_connections:
            building_id = raw_connection.get_value()["id"].get_value()
            connection_members = raw_connection.get_value()

            # Created objects are added automatically to the data set.
            GenieBuildingConnection(building_id, full_data_set, members=connection_members)

    @staticmethod
    def _extract_unit_connections(gamespec, full_data_set):
        """
        Extract unit connections from the game data.

        :param gamespec: Gamedata from empires.dat file.
        :type gamespec: class: ...dataformat.value_members.ArrayMember
        """
        # call hierarchy: wrapper[0]->unit_connections
        raw_connections = gamespec.get_value()[0].get_value()["unit_connections"].get_value()

        for raw_connection in raw_connections:
            unit_id = raw_connection.get_value()["id"].get_value()
            connection_members = raw_connection.get_value()

            # Created objects are added automatically to the data set.
            GenieUnitConnection(unit_id, full_data_set, members=connection_members)

    @staticmethod
    def _extract_tech_connections(gamespec, full_data_set):
        """
        Extract tech connections from the game data.

        :param gamespec: Gamedata from empires.dat file.
        :type gamespec: class: ...dataformat.value_members.ArrayMember
        """
        # call hierarchy: wrapper[0]->tech_connections
        raw_connections = gamespec.get_value()[0].get_value()["tech_connections"].get_value()

        for raw_connection in raw_connections:
            tech_id = raw_connection.get_value()["id"].get_value()
            connection_members = raw_connection.get_value()

            # Created objects are added automatically to the data set.
            GenieTechConnection(tech_id, full_data_set, members=connection_members)

    @staticmethod
    def _extract_genie_graphics(gamespec, full_data_set):
        """
        Extract graphic definitions from the game data.

        :param gamespec: Gamedata from empires.dat file.
        :type gamespec: class: ...dataformat.value_members.ArrayMember
        """
        # call hierarchy: wrapper[0]->graphics
        raw_graphics = gamespec.get_value()[0].get_value()["graphics"].get_value()

        for raw_graphic in raw_graphics:
            graphic_id = raw_graphic.get_value()["graphic_id"].get_value()
            graphic_members = raw_graphic.get_value()

            # Created objects are added automatically to the data set.
            GenieGraphic(graphic_id, full_data_set, members=graphic_members)

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

            # Created objects are added automatically to the data set.
            GenieSound(sound_id, full_data_set, members=sound_members)

    @staticmethod
    def _extract_genie_terrains(gamespec, full_data_set):
        """
        Extract terrains from the game data.

        :param gamespec: Gamedata from empires.dat file.
        :type gamespec: class: ...dataformat.value_members.ArrayMember
        """
        # call hierarchy: wrapper[0]->terrains
        raw_terrains = gamespec.get_value()[0].get_value()["terrains"].get_value()

        index = 0
        for raw_terrain in raw_terrains:
            terrain_index = index
            terrain_members = raw_terrain.get_value()

            # Created objects are added automatically to the data set.
            GenieTerrainObject(terrain_index, full_data_set, members=terrain_members)

            index += 1

    @staticmethod
    def _create_unit_lines(full_data_set):
        """
        Sort units into lines, based on information in the unit connections.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        """

        unit_connections = full_data_set.unit_connections

        for _, connection in unit_connections.items():
            unit_id = connection.get_member("id").get_value()
            unit = full_data_set.genie_units[unit_id]
            line_id = connection.get_member("vertical_line").get_value()

            # Check if a line object already exists for this id
            # if not, create it
            if line_id in full_data_set.unit_lines.keys():
                unit_line = full_data_set.unit_lines[line_id]

            else:
                # Check for special cases first
                if unit.has_member("transform_id") and unit.get_member("transform_id").get_value() > -1:
                    # Trebuchet
                    unit_line = GenieUnitTransformGroup(line_id, unit_id, full_data_set)

                elif line_id == 65:
                    # Monks
                    # Switch to mobk with relic is hardcoded :(
                    unit_line = GenieMonkGroup(line_id, unit_id, 286, full_data_set)

                elif unit.has_member("task_group") and unit.get_member("task_group").get_value() > 0:
                    # Villager
                    # done somewhere else because they are special^TM
                    continue

                else:
                    # Normal units
                    unit_line = GenieUnitLineGroup(line_id, full_data_set)

            if connection.get_member("line_mode").get_value() == 2:
                # The unit is the first in line
                unit_line.add_unit(unit)
            else:
                # The unit comes after another one
                # Search other_connections for the previous unit in line
                connected_types = connection.get_member("other_connections").get_value()
                connected_index = -1
                for index in range(len(connected_types)):
                    connected_type = connected_types[index].get_value()["other_connection"].get_value()
                    if connected_type == 2:
                        # 2 == Unit
                        connected_index = index
                        break

                else:
                    raise Exception("Unit %s is not first in line, but no previous unit can"
                                    " be found in other_connections" % (unit_id))

                # Find the id of the connected unit
                connected_ids = connection.get_member("other_connected_ids").get_value()
                previous_unit_id = connected_ids[connected_index].get_value()
                previous_unit = full_data_set.genie_units[previous_unit_id]

                unit_line.add_unit(unit, after=previous_unit)

    @staticmethod
    def _create_building_lines(full_data_set):
        pass
