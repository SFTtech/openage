# Copyright 2019-2025 the openage authors. See copying.md for legal info.

"""
Convert data from AoC to openage formats.
"""
from __future__ import annotations
import typing

from .....log import info
from ....entity_object.conversion.aoc.genie_object_container import GenieObjectContainer
from ....service.debug_info import debug_converter_objects, \
    debug_converter_object_groups
from ....service.read.nyan_api_loader import load_api
from .media_subprocessor import AoCMediaSubprocessor
from .modpack_subprocessor import AoCModpackSubprocessor
from .nyan_subprocessor import AoCNyanSubprocessor
from .pregen_processor import AoCPregenSubprocessor

from .main.extract.civ import extract_genie_civs
from .main.extract.connection import extract_age_connections, extract_building_connections, \
    extract_unit_connections, extract_tech_connections
from .main.extract.effect_bundle import extract_genie_effect_bundles, sanitize_effect_bundles
from .main.extract.graphics import extract_genie_graphics
from .main.extract.sound import extract_genie_sounds
from .main.extract.tech import extract_genie_techs
from .main.extract.terrain import extract_genie_terrains, extract_genie_restrictions
from .main.extract.unit import extract_genie_units

from .main.groups.ambient_group import create_ambient_groups
from .main.groups.civ_group import create_civ_groups
from .main.groups.building_line import create_building_lines
from .main.groups.tech_group import create_tech_groups
from .main.groups.terrain_group import create_terrain_groups
from .main.groups.unit_line import create_unit_lines, create_extra_unit_lines
from .main.groups.variant_group import create_variant_groups
from .main.groups.villager_group import create_villager_groups

from .main.link.building_upgrade import link_building_upgrades
from .main.link.creatable import link_creatables
from .main.link.civ_unique import link_civ_uniques
from .main.link.gather import link_gatherers_to_dropsites
from .main.link.garrison import link_garrison
from .main.link.repairable import link_repairables
from .main.link.researchable import link_researchables
from .main.link.trade_post import link_trade_posts

if typing.TYPE_CHECKING:
    from argparse import Namespace
    from ....entity_object.conversion.stringresource import StringResource
    from ....entity_object.conversion.modpack import Modpack
    from ....value_object.read.value_members import ArrayMember
    from ....value_object.init.game_version import GameVersion


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

        extract_genie_units(gamespec, dataset)
        extract_genie_techs(gamespec, dataset)
        extract_genie_effect_bundles(gamespec, dataset)
        sanitize_effect_bundles(dataset)
        extract_genie_civs(gamespec, dataset)
        extract_age_connections(gamespec, dataset)
        extract_building_connections(gamespec, dataset)
        extract_unit_connections(gamespec, dataset)
        extract_tech_connections(gamespec, dataset)
        extract_genie_graphics(gamespec, dataset)
        extract_genie_sounds(gamespec, dataset)
        extract_genie_terrains(gamespec, dataset)
        extract_genie_restrictions(gamespec, dataset)

        return dataset

    extract_genie_units = staticmethod(extract_genie_units)
    extract_genie_techs = staticmethod(extract_genie_techs)
    extract_genie_effect_bundles = staticmethod(extract_genie_effect_bundles)
    sanitize_effect_bundles = staticmethod(sanitize_effect_bundles)
    extract_genie_civs = staticmethod(extract_genie_civs)
    extract_age_connections = staticmethod(extract_age_connections)
    extract_building_connections = staticmethod(extract_building_connections)
    extract_unit_connections = staticmethod(extract_unit_connections)
    extract_tech_connections = staticmethod(extract_tech_connections)
    extract_genie_graphics = staticmethod(extract_genie_graphics)
    extract_genie_sounds = staticmethod(extract_genie_sounds)
    extract_genie_terrains = staticmethod(extract_genie_terrains)
    extract_genie_restrictions = staticmethod(extract_genie_restrictions)

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

        create_unit_lines(full_data_set)
        create_extra_unit_lines(full_data_set)
        create_building_lines(full_data_set)
        create_villager_groups(full_data_set)
        create_ambient_groups(full_data_set)
        create_variant_groups(full_data_set)
        create_terrain_groups(full_data_set)
        create_tech_groups(full_data_set)
        create_civ_groups(full_data_set)

        info("Linking API-like objects...")

        link_building_upgrades(full_data_set)
        link_creatables(full_data_set)
        link_researchables(full_data_set)
        link_civ_uniques(full_data_set)
        link_gatherers_to_dropsites(full_data_set)
        link_garrison(full_data_set)
        link_trade_posts(full_data_set)
        link_repairables(full_data_set)

        info("Generating auxiliary objects...")

        AoCPregenSubprocessor.generate(full_data_set)

        return full_data_set

    create_unit_lines = staticmethod(create_unit_lines)
    create_extra_unit_lines = staticmethod(create_extra_unit_lines)
    create_building_lines = staticmethod(create_building_lines)
    create_tech_groups = staticmethod(create_tech_groups)
    create_civ_groups = staticmethod(create_civ_groups)
    create_villager_groups = staticmethod(create_villager_groups)
    create_ambient_groups = staticmethod(create_ambient_groups)
    create_variant_groups = staticmethod(create_variant_groups)
    create_terrain_groups = staticmethod(create_terrain_groups)

    link_building_upgrades = staticmethod(link_building_upgrades)
    link_creatables = staticmethod(link_creatables)
    link_civ_uniques = staticmethod(link_civ_uniques)
    link_researchables = staticmethod(link_researchables)
    link_gatherers_to_dropsites = staticmethod(link_gatherers_to_dropsites)
    link_garrison = staticmethod(link_garrison)
    link_trade_posts = staticmethod(link_trade_posts)
    link_repairables = staticmethod(link_repairables)

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
