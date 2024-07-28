# Copyright 2021-2024 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-few-public-methods

"""
Convert data from AoE2:HD to openage formats.
"""
from __future__ import annotations
import typing


from .....log import info
from ....entity_object.conversion.aoc.genie_object_container import GenieObjectContainer
from ....service.debug_info import debug_converter_objects, \
    debug_converter_object_groups
from ....service.read.nyan_api_loader import load_api
from ..aoc.nyan_subprocessor import AoCNyanSubprocessor
from ..aoc.pregen_processor import AoCPregenSubprocessor
from ..aoc.processor import AoCProcessor
from .media_subprocessor import HDMediaSubprocessor
from .modpack_subprocessor import HDModpackSubprocessor

if typing.TYPE_CHECKING:
    from argparse import Namespace
    from openage.convert.entity_object.conversion.stringresource import StringResource
    from openage.convert.entity_object.conversion.modpack import Modpack
    from openage.convert.value_object.read.value_members import ArrayMember
    from openage.convert.value_object.init.game_version import GameVersion


class HDProcessor:
    """
    Main processor for converting data from HD Edition.
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

        # Create the custom openage formats (nyan, sprite, terrain, etc.)
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
        :type full_data_set: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        """

        info("Creating API-like objects...")

        AoCProcessor.create_unit_lines(full_data_set)
        AoCProcessor.create_extra_unit_lines(full_data_set)
        AoCProcessor.create_building_lines(full_data_set)
        AoCProcessor.create_villager_groups(full_data_set)
        AoCProcessor.create_ambient_groups(full_data_set)
        AoCProcessor.create_variant_groups(full_data_set)
        AoCProcessor.create_terrain_groups(full_data_set)
        AoCProcessor.create_tech_groups(full_data_set)
        AoCProcessor.create_civ_groups(full_data_set)

        info("Linking API-like objects...")

        AoCProcessor.link_building_upgrades(full_data_set)
        AoCProcessor.link_creatables(full_data_set)
        AoCProcessor.link_researchables(full_data_set)
        AoCProcessor.link_civ_uniques(full_data_set)
        AoCProcessor.link_gatherers_to_dropsites(full_data_set)
        AoCProcessor.link_garrison(full_data_set)
        AoCProcessor.link_trade_posts(full_data_set)
        AoCProcessor.link_repairables(full_data_set)

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

        HDMediaSubprocessor.convert(full_data_set)

        return HDModpackSubprocessor.get_modpacks(full_data_set)
