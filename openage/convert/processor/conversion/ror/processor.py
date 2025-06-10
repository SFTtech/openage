# Copyright 2020-2025 the openage authors. See copying.md for legal info.
"""
Convert data from RoR to openage formats.
"""
from __future__ import annotations
import typing

from .....log import info
from ....entity_object.conversion.aoc.genie_object_container import GenieObjectContainer
from ....service.debug_info import debug_converter_objects, debug_converter_object_groups
from ....service.read.nyan_api_loader import load_api
from ..aoc.processor import AoCProcessor
from .media_subprocessor import RoRMediaSubprocessor
from .modpack_subprocessor import RoRModpackSubprocessor
from .nyan_subprocessor import RoRNyanSubprocessor
from .pregen_subprocessor import RoRPregenSubprocessor

from .main.extract.sound import extract_genie_sounds
from .main.extract.unit import extract_genie_units
from .main.groups.ambient_group import create_ambient_groups
from .main.groups.entity_line import create_entity_lines
from .main.groups.tech_group import create_tech_groups
from .main.groups.variant_group import create_variant_groups
from .main.link.garrison import link_garrison
from .main.link.repairable import link_repairables

if typing.TYPE_CHECKING:
    from argparse import Namespace
    from ....entity_object.conversion.stringresource import StringResource
    from ....entity_object.conversion.modpack import Modpack
    from ....value_object.read.value_members import ArrayMember
    from ....value_object.init.game_version import GameVersion


class RoRProcessor:
    """
    Main processor for converting data from RoR.
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

        # Create the custom openage formats (nyan, sprite, terrain)
        dataset = cls._processor(gamespec, dataset)
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

        extract_genie_units(gamespec, dataset)
        AoCProcessor.extract_genie_techs(gamespec, dataset)
        AoCProcessor.extract_genie_effect_bundles(gamespec, dataset)
        AoCProcessor.sanitize_effect_bundles(dataset)
        AoCProcessor.extract_genie_civs(gamespec, dataset)
        AoCProcessor.extract_genie_graphics(gamespec, dataset)
        extract_genie_sounds(gamespec, dataset)
        AoCProcessor.extract_genie_terrains(gamespec, dataset)
        AoCProcessor.extract_genie_restrictions(gamespec, dataset)

        return dataset

    @classmethod
    def _processor(cls, gamespec: ArrayMember, full_data_set: GenieObjectContainer) -> GenieObjectContainer:
        """
        Transfer structures used in Genie games to more openage-friendly
        Python objects.

        :param gamespec: Gamedata from empires.dat file.
        :type gamespec: class: ...dataformat.value_members.ArrayMember
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        """

        info("Creating API-like objects...")

        create_tech_groups(full_data_set)
        create_entity_lines(gamespec, full_data_set)
        create_ambient_groups(full_data_set)
        create_variant_groups(full_data_set)
        AoCProcessor.create_terrain_groups(full_data_set)
        AoCProcessor.create_civ_groups(full_data_set)

        info("Linking API-like objects...")

        AoCProcessor.link_creatables(full_data_set)
        AoCProcessor.link_researchables(full_data_set)
        AoCProcessor.link_gatherers_to_dropsites(full_data_set)
        link_garrison(full_data_set)
        AoCProcessor.link_trade_posts(full_data_set)
        link_repairables(full_data_set)

        info("Generating auxiliary objects...")

        RoRPregenSubprocessor.generate(full_data_set)

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

        RoRNyanSubprocessor.convert(full_data_set)

        info("Creating requests for media export...")

        RoRMediaSubprocessor.convert(full_data_set)

        return RoRModpackSubprocessor.get_modpacks(full_data_set)

    extract_genie_sounds = staticmethod(extract_genie_sounds)
    extract_genie_units = staticmethod(extract_genie_units)
    create_ambient_groups = staticmethod(create_ambient_groups)
    create_entity_lines = staticmethod(create_entity_lines)
    create_tech_groups = staticmethod(create_tech_groups)
    create_variant_groups = staticmethod(create_variant_groups)
    link_garrison = staticmethod(link_garrison)
    link_repairables = staticmethod(link_repairables)
