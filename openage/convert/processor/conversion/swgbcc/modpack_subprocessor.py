# Copyright 2020-2024 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-few-public-methods

"""
Organize export data (nyan objects, media, scripts, etc.)
into modpacks.
"""
from __future__ import annotations
import typing

from ....entity_object.conversion.modpack import Modpack
from ..aoc.modpack_subprocessor import AoCModpackSubprocessor

if typing.TYPE_CHECKING:
    from openage.convert.entity_object.conversion.aoc.genie_object_container\
        import GenieObjectContainer


class SWGBCCModpackSubprocessor:
    """
    Creates the modpacks containing the nyan files and media from the SWGB conversion.
    """

    @classmethod
    def get_modpacks(cls, full_data_set: GenieObjectContainer) -> list[Modpack]:
        """
        Return all modpacks that can be created from the gamedata.
        """
        swgb_base = cls._get_swgb_base(full_data_set)

        return [swgb_base]

    @classmethod
    def _get_swgb_base(cls, full_data_set: GenieObjectContainer) -> Modpack:
        """
        Create the swgb-base modpack.
        """
        modpack = Modpack("swgb_base")

        mod_def = modpack.get_info()

        targetmod_info = full_data_set.game_version.edition.target_modpacks["swgb_base"]
        version = targetmod_info["version"]
        versionstr = targetmod_info["versionstr"]
        mod_def.set_info("swgb_base", version, versionstr=versionstr, repo="openage")

        mod_def.add_include("data/**")

        AoCModpackSubprocessor.organize_nyan_objects(modpack, full_data_set)
        AoCModpackSubprocessor.organize_media_objects(modpack, full_data_set)

        return modpack
