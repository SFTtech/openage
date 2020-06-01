# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Convert data from SWGB:CC to openage formats.
"""

from openage.convert.dataformat.aoc.genie_object_container import GenieObjectContainer
from openage.convert.nyan.api_loader import load_api
from openage.convert.processor.aoc.media_subprocessor import AoCMediaSubprocessor
from openage.convert.processor.aoc.processor import AoCProcessor
from openage.convert.processor.swgbcc.modpack_subprocessor import SWGBCCModpackSubprocessor
from openage.convert.processor.swgbcc.nyan_subprocessor import SWGBCCNyanSubprocessor

from ....log import info


class SWGBCCProcessor:

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

        AoCProcessor._extract_genie_units(gamespec, dataset)
        AoCProcessor._extract_genie_techs(gamespec, dataset)
        AoCProcessor._extract_genie_effect_bundles(gamespec, dataset)
        AoCProcessor._sanitize_effect_bundles(dataset)
        AoCProcessor._extract_genie_civs(gamespec, dataset)
        AoCProcessor._extract_age_connections(gamespec, dataset)
        AoCProcessor._extract_building_connections(gamespec, dataset)
        AoCProcessor._extract_unit_connections(gamespec, dataset)
        AoCProcessor._extract_tech_connections(gamespec, dataset)
        AoCProcessor._extract_genie_graphics(gamespec, dataset)
        AoCProcessor._extract_genie_sounds(gamespec, dataset)
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

        # cls._create_unit_lines(full_data_set)
        # cls._create_extra_unit_lines(full_data_set)
        AoCProcessor._create_building_lines(full_data_set)
        # cls._create_villager_groups(full_data_set)
        # cls._create_ambient_groups(full_data_set)
        # cls._create_variant_groups(full_data_set)
        AoCProcessor._create_terrain_groups(full_data_set)
        AoCProcessor._create_tech_groups(full_data_set)
        # cls._create_civ_groups(full_data_set)

        info("Linking API-like objects...")

        # AoCProcessor._link_building_upgrades(full_data_set)
        AoCProcessor._link_creatables(full_data_set)
        AoCProcessor._link_researchables(full_data_set)
        # cls._link_civ_uniques(full_data_set)
        AoCProcessor._link_gatherers_to_dropsites(full_data_set)
        # cls._link_garrison(full_data_set)
        # cls._link_trade_posts(full_data_set)

        info("Generating auxiliary objects...")

        # AoCPregenSubprocessor.generate(full_data_set)

        return full_data_set

    @classmethod
    def _post_processor(cls, full_data_set):

        info("Creating nyan objects...")

        SWGBCCNyanSubprocessor.convert(full_data_set)

        info("Creating requests for media export...")

        AoCMediaSubprocessor.convert(full_data_set)

        return SWGBCCModpackSubprocessor.get_modpacks(full_data_set)

    @staticmethod
    def _create_unit_lines(full_data_set):
        """
        Sort units into lines, based on information in the unit connections.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        """
        pass
