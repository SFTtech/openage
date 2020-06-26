# Copyright 2020-2020 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-few-public-methods

"""
Organize export data (nyan objects, media, scripts, etc.)
into modpacks.
"""
from ...dataformat.modpack import Modpack
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
        modpack = Modpack("de2-base")

        mod_def = modpack.get_info()

        mod_def.set_version("TODO")
        mod_def.set_uid(2000)

        mod_def.add_assets_to_load("data/*")

        AoCModpackSubprocessor.organize_nyan_objects(modpack, gamedata)
        AoCModpackSubprocessor.organize_media_objects(modpack, gamedata)

        return modpack
