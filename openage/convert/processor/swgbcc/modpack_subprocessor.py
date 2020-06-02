# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Organize export data (nyan objects, media, scripts, etc.)
into modpacks.
"""
from openage.convert.dataformat.modpack import Modpack
from openage.convert.processor.aoc.modpack_subprocessor import AoCModpackSubprocessor


class SWGBCCModpackSubprocessor:

    @classmethod
    def get_modpacks(cls, gamedata):

        swgb_base = cls._get_swgb_base(gamedata)

        return [swgb_base]

    @classmethod
    def _get_swgb_base(cls, gamedata):
        """
        Create the swgb-base modpack.
        """
        modpack = Modpack("swgb-base")

        mod_def = modpack.get_info()

        mod_def.set_version("GOG")
        mod_def.set_uid(5000)

        mod_def.add_assets_to_load("data/*")

        AoCModpackSubprocessor._organize_nyan_objects(modpack, gamedata)
        AoCModpackSubprocessor._organize_media_objects(modpack, gamedata)

        return modpack
