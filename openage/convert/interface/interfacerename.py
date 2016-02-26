# Copyright 2016-2016 the openage authors. See copying.md for legal info.

""" Renaming interface assets and splitting into direvtories """

from .interfacehardcoded import ingame_hud_background_index


def interface_rename(filename):
    """
    Returns a human-usable name according to the original and hardcoded metadata.
    """
    try:
        return "interface/hud" + str(ingame_hud_background_index(filename)).zfill(4) + ".slp"
    except ValueError:
        return filename
