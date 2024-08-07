# Copyright 2021-2024 the openage authors. See copying.md for legal info.
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


class HDModpackSubprocessor:
    """
    Creates the modpacks containing the nyan files and media from the HD conversion.
    """

    @classmethod
    def get_modpacks(cls, full_data_set: GenieObjectContainer) -> list[Modpack]:
        """
        Return all modpacks that can be created from the gamedata.
        """
        hd_base = cls._get_aoe2_base(full_data_set)

        return [hd_base]

    @classmethod
    def _get_aoe2_base(cls, full_data_set: GenieObjectContainer) -> Modpack:
        """
        Create the aoe2_base modpack.
        """
        modpack = Modpack("hd_base")

        mod_def = modpack.get_info()

        targetmod_info = full_data_set.game_version.edition.target_modpacks["hd_base"]
        version = targetmod_info["version"]
        versionstr = targetmod_info["versionstr"]
        mod_def.set_info("hd_base", version, versionstr=versionstr, repo="openage")

        mod_def.add_include("data/**")

        AoCModpackSubprocessor.organize_nyan_objects(modpack, full_data_set)
        AoCModpackSubprocessor.organize_media_objects(modpack, full_data_set)

        return modpack
