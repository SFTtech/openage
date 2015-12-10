import enum


@enum.unique
class GameVersion(enum.Enum):
    """
    An installed version of the original game. Multiple may coexist (e.g. AoK
    and TC). We should usually distinguish versions only as far as we're
    concerned (i.e. assets changed).
    """
    age2_aok = "Age of Empires 2: The Age of Kings"
    age2_tc = "Age of Empires 2: The Conquerors"
    age2_tc_10c = "Age of Empires 2: The Conquerors, Patch 1.0c"
    # HD edition version 4.0
    age2_fe = "Forgotten Empires"


def get_game_versions(srcdir):
    """
    Determine what versions of the game are installed in srcdir. Yield
    GameVersion values.
    """
    fixed_filenames = {
        GameVersion.age2_aok: [
            'empires2.exe', 'data/empires2.dat',
        ],
        GameVersion.age2_tc: [
            'age2_x1/age2_x1.exe', 'data/empires2_x1.dat',
        ],
        GameVersion.age2_tc_10c: [
            'age2_x1/age2_x1.exe', 'data/empires2_x1_p1.dat',
        ],
        GameVersion.age2_fe: [
            'AoK HD.exe', 'resources/_common/dat/empires2_x1_p1.dat',
        ],
    }

    for version, paths in fixed_filenames.items():
        if all(srcdir.joinpath(path).is_file() for path in paths):
            yield version


SUPPORTED_GAME_VERSIONS = {
    GameVersion.age2_aok,
    GameVersion.age2_tc,
    GameVersion.age2_tc_10c,
    GameVersion.age2_fe,
}