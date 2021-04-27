# Copyright 2015-2021 the openage authors. See copying.md for legal info.

"""
Receives cleaned-up srcdir and targetdir objects from .main, and drives the
actual conversion process.
"""

from ...log import info, dbg
from ..processor.export.modpack_exporter import ModpackExporter
from ..service.debug_info import debug_gamedata_format
from ..service.debug_info import debug_string_resources,\
    debug_registered_graphics, debug_modpack
from ..service.init.changelog import (ASSET_VERSION)
from ..service.read.gamedata import get_gamespec
from ..service.read.palette import get_palettes
from ..service.read.register_media import get_existing_graphics
from ..service.read.string_resource import get_string_resources


def convert(args):
    """
    args must hold srcdir and targetdir (FS-like objects),
    plus any additional configuration options.

    Yields progress information in the form of:
        strings (filenames) that indicate the currently-converted object
        ints that predict the amount of objects remaining
    """
    # data conversion
    yield from convert_metadata(args)
    # with args.targetdir[GAMESPEC_VERSION_FILENAME].open('w') as fil:
    #     fil.write(EmpiresDat.get_hash(args.game_version))

    # clean args (set by convert_metadata for convert_media)
    del args.palettes

    info(f"asset conversion complete; asset version: {ASSET_VERSION}", )


def convert_metadata(args):
    """
    Converts the metadata part.
    """
    if not args.flag("no_metadata"):
        info("converting metadata")
        # data_formatter = DataFormatter()

    # required for player palette and color lookup during SLP conversion.
    yield "palette"
    palettes = get_palettes(args.srcdir, args.game_version)

    # store for use by convert_media
    args.palettes = palettes

    if args.flag("no_metadata"):
        return

    gamedata_path = args.targetdir.joinpath('gamedata')
    if gamedata_path.exists():
        gamedata_path.removerecursive()

    args.converter = get_converter(args.game_version)

    # Read .dat
    yield "empires.dat"
    debug_gamedata_format(args.debugdir, args.debug_info, args.game_version)
    gamespec = get_gamespec(args.srcdir, args.game_version, args.flag("no_pickle_cache"))

    # Blending mode count
    if args.game_version[0].game_id == "SWGB":
        args.blend_mode_count = gamespec[0]["blend_mode_count_swgb"].get_value()

    else:
        args.blend_mode_count = None

    # Read strings
    string_resources = get_string_resources(args)
    debug_string_resources(args.debugdir, args.debug_info, string_resources)

    # Existing graphic IDs/filenames
    existing_graphics = get_existing_graphics(args)
    debug_registered_graphics(args.debugdir, args.debug_info, existing_graphics)

    # Convert
    modpacks = args.converter.convert(gamespec,
                                      args,
                                      string_resources,
                                      existing_graphics)

    for modpack in modpacks:
        ModpackExporter.export(modpack, args)
        debug_modpack(args.debugdir, args.debug_info, modpack)

    yield "player color palette"
    # player_palette = PlayerColorTable(palette)
    # data_formatter.add_data(player_palette.dump("player_palette"))

    yield "terminal color palette"
    # termcolortable = ColorTable(URXVTCOLS)
    # data_formatter.add_data(termcolortable.dump("termcolors"))

    yield "game specification files"
    # data_formatter.export(args.targetdir, ("csv",))

    if args.flag('gen_extra_files'):
        dbg("generating extra files for visualization")
        # tgt = args.targetdir
        # with tgt['info/colortable.pal.png'].open_w() as outfile:
        #     palette.save_visualization(outfile)

        # with tgt['info/playercolortable.pal.png'].open_w() as outfile:
        #     player_palette.save_visualization(outfile)


def get_converter(game_version):
    """
    Returns the converter for the specified game version.
    """
    game_edition = game_version[0]
    game_expansions = game_version[1]

    if game_edition.game_id == "ROR":
        from ..processor.conversion.ror.processor import RoRProcessor
        return RoRProcessor

    if game_edition.game_id == "AOE1DE":
        from ..processor.conversion.de1.processor import DE1Processor
        return DE1Processor

    if game_edition.game_id in ("AOC", "HDEDITION"):
        from ..processor.conversion.aoc.processor import AoCProcessor
        return AoCProcessor

    if game_edition.game_id == "AOE2DE":
        from ..processor.conversion.de2.processor import DE2Processor
        return DE2Processor

    if game_edition.game_id == "SWGB":
        if "SWGB_CC" in [expansion.game_id for expansion in game_expansions]:
            from ..processor.conversion.swgbcc.processor import SWGBCCProcessor
            return SWGBCCProcessor

    raise Exception(f"no valid converter found for game edition {game_edition.edition_name}")
