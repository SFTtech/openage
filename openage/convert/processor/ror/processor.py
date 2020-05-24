# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Convert data from RoR to openage formats.
"""

from openage.convert.dataformat.aoc.genie_object_container import GenieObjectContainer
from openage.convert.dataformat.aoc.genie_unit import GenieUnitObject
from openage.convert.nyan.api_loader import load_api
from openage.convert.processor.aoc.processor import AoCProcessor

from ....log import info


class RoRProcessor:

    @classmethod
    def convert(cls, gamespec, game_version, string_resources, existing_graphics):
        """
        Input game speification and media here and get a set of
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
        data_set = cls._processor(data_set)

        # Create modpack definitions
        modpacks = cls._post_processor(data_set)

        return modpacks

    @classmethod
    def _pre_processor(cls, gamespec, game_version, string_resources, existing_graphics):
        """
        Store data from the reader in a conversion container.

        :param gamespec: Gamedata from empires.dat file.
        :type gamespec: class: ...dataformat.value_members.ArrayMember
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
        AoCProcessor._extract_genie_sounds(gamespec, dataset)
        AoCProcessor._extract_genie_terrains(gamespec, dataset)

        return dataset

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

        info("Creating API-like objects...")

        # cls._create_unit_lines(full_data_set)
        # cls._create_extra_unit_lines(full_data_set)
        # cls._create_building_lines(full_data_set)
        # cls._create_villager_groups(full_data_set)
        # cls._create_ambient_groups(full_data_set)
        # cls._create_variant_groups(full_data_set)
        # AoCProcessor._create_terrain_groups(full_data_set)
        # cls._create_tech_groups(full_data_set)
        # cls._create_node_tech_groups(full_data_set)
        # AoCProcessor._create_civ_groups(full_data_set)

        info("Linking API-like objects...")

        info("Generating auxiliary objects...")

        return full_data_set

    @classmethod
    def _post_processor(cls, full_data_set):

        info("Creating nyan objects...")

        info("Creating requests for media export...")

        return None

    @staticmethod
    def _extract_genie_units(gamespec, full_data_set):
        """
        Extract units from the game data.

        :param gamespec: Gamedata from empires.dat file.
        :type gamespec: class: ...dataformat.value_members.ArrayMember
        """
        # Units are stored in the civ container.
        # All civs point to the same units (?) except for Gaia which has more.
        # Gaia also seems to have the most units, so we only read from Gaia
        #
        # call hierarchy: wrapper[0]->civs[0]->units
        raw_units = gamespec.get_value()[0].get_value()["civs"].get_value()[0]\
            .get_value()["units"].get_value()

        for raw_unit in raw_units:
            unit_id = raw_unit.get_value()["id0"].get_value()
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
