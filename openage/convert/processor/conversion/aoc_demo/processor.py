# Copyright 2023-2023 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-few-public-methods

"""
Convert data from the AoC Demo to openage formats.
"""
from __future__ import annotations
import typing

from .....log import info
from ....service.debug_info import debug_converter_objects, \
    debug_converter_object_groups
from ..aoc.processor import AoCProcessor
from ..aoc.nyan_subprocessor import AoCNyanSubprocessor
from ..aoc.media_subprocessor import AoCMediaSubprocessor
from .modpack_subprocessor import DemoModpackSubprocessor


if typing.TYPE_CHECKING:
    from argparse import Namespace
    from openage.convert.entity_object.conversion.stringresource import StringResource
    from openage.convert.entity_object.conversion.modpack import Modpack
    from openage.convert.value_object.read.value_members import ArrayMember
    from openage.convert.entity_object.conversion.aoc.genie_object_container \
        import GenieObjectContainer


class DemoProcessor:
    """
    Main processor for converting data from the AoC Demo.
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
        # pylint: disable=protected-access

        info("Starting conversion...")

        # Create a new container for the conversion process
        dataset = AoCProcessor._pre_processor(
            gamespec,
            args.game_version,
            string_resources,
            existing_graphics
        )
        debug_converter_objects(args.debugdir, args.debug_info, dataset)

        # Create the custom openae formats (nyan, sprite, terrain)
        dataset = AoCProcessor._processor(dataset)
        debug_converter_object_groups(args.debugdir, args.debug_info, dataset)

        # Create modpack definitions
        modpacks = cls._post_processor(dataset)

        return modpacks

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

        return DemoModpackSubprocessor.get_modpacks(full_data_set)
