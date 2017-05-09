# Copyright 2014-2015 the openage authors. See copying.md for legal info.

from ..version_detection import GameVersion, FU

base = GameVersion(
    name = "base",
    interfac = FU("Data/interfac.drs"),
    drs_files = FU(
        "Data/gamedata.drs",
        "Data/graphics.drs",
        "Data/sounds.drs",
        "Data/terrain.drs",
    ),
    blendomatic = FU("Data/blendomatic.dat"),
    dat_files = FU("Data/empires2.dat", override=True),
    langdll_files = FU("language.dll"),
)

x1 = GameVersion(
    name = "x1",
    drs_files = FU(
        "Data/gamedata_x1.drs",
        "Data/sounds_x1.drs",
    ),
    dat_files = FU("Data/empires2_x1.dat", override=True),
    langdll_files = FU("language_x1.dll"),
    prereqs = {base}
)

x1_p1 = GameVersion(
    name = "x1_p1",
    drs_files = FU("Data/gamedata_x1_p1.drs"),
    dat_files = FU("Data/empires2_x1_p1.dat", override=True),
    langdll_files = FU("language_x1_p1.dll"),
    prereqs = {x1}
)

# best version first
known_versions = (x1_p1, x1, base)
