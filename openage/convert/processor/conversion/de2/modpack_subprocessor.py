# Copyright 2020-2021 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-few-public-methods

"""
Organize export data (nyan objects, media, scripts, etc.)
into modpacks.
"""
from ....entity_object.conversion.modpack import Modpack
from ..aoc.modpack_subprocessor import AoCModpackSubprocessor


class DE2ModpackSubprocessor:
    """
    Creates the modpacks containing the nyan files and media from the DE2 conversion.
    """

    @classmethod
    def get_modpacks(cls, gamedata):
        """
        Return all modpacks that can be created from the gamedata.
        """
        de2_base = cls._get_aoe2_base(gamedata)

        return [de2_base]

    @classmethod
    def _get_aoe2_base(cls, gamedata):
        """
        Create the aoe2-base modpack.
        """
        modpack = Modpack("de2_base")

        mod_def = modpack.get_info()

        mod_def.set_info("aoe2_base", "1.0c", repo="openage")

        mod_def.add_include("data/*")

        AoCModpackSubprocessor.organize_nyan_objects(modpack, gamedata)
        AoCModpackSubprocessor.organize_media_objects(modpack, gamedata)

        return modpack
