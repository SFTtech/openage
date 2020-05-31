# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Organize export data (nyan objects, media, scripts, etc.)
into modpacks.
"""


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
        pass

        # TODO: Implement
