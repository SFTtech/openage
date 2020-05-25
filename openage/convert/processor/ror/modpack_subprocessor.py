# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Organize export data (nyan objects, media, scripts, etc.)
into modpacks.
"""
from openage.convert.dataformat.modpack import Modpack
from openage.convert.processor.aoc.modpack_subprocessor import AoCModpackSubprocessor


class RoRModpackSubprocessor:

    @classmethod
    def get_modpacks(cls, gamedata):

        aoe2_base = cls._get_aoe1_base(gamedata)

        return [aoe2_base]

    @classmethod
    def _get_aoe1_base(cls, gamedata):
        """
        Create the aoe1-base modpack.
        """
        modpack = Modpack("aoe1-base")

        mod_def = modpack.get_info()

        mod_def.set_version("1.0B")
        mod_def.set_uid(1000)

        mod_def.add_assets_to_load("data/*")

        AoCModpackSubprocessor._organize_nyan_objects(modpack, gamedata)
        AoCModpackSubprocessor._organize_media_objects(modpack, gamedata)

        return modpack
