#!/usr/bin/env python3
#
#version detection hardcoded shit

from version_detection import GameVersion

base = GameVersion(
    name = "base",
    drs_files = {
        "Data/gamedata.drs",
        "Data/graphics.drs",
        "Data/interfac.drs",
        "Data/sounds.drs",
        "Data/terrain.drs",
    },
    dat_files = {
        "Data/empires2.dat",
        "Data/blendomatic.dat",
    },
    langdll_files = {"language.dll"},
    prereqs = set()
)

x1 = GameVersion(
    name = "x1",
    drs_files = {
        "Data/gamedata_x1.drs",
        "Data/sounds_x1.drs",
    },
    dat_files = {"Data/empires2_x1.dat"},
    langdll_files = {"language_x1.dll"},
    prereqs = {base}
)

x1_p1 = GameVersion(
    name = "x1_p1",
    drs_files = {"Data/gamedata_x1_p1.drs"},
    dat_files = {"Data/empires2_x1_p1.dat"},
    langdll_files = {"language_x1_p1.dll"},
    prereqs = {x1}
)

#best version first
known_versions = (x1_p1, x1, base)
