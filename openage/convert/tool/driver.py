# Copyright 2015-2020 the openage authors. See copying.md for legal info.

"""
Receives cleaned-up srcdir and targetdir objects from .main, and drives the
actual conversion process.
"""

from openage.convert.service.debug_info import debug_init,\
    debug_string_resources, debug_registered_graphics

from ...log import info, dbg
from ..processor.export.modpack_exporter import ModpackExporter
from ..service.debug_info import debug_gamedata_format
from ..service.init.changelog import (ASSET_VERSION)
from ..service.read.gamedata import get_gamespec
from ..service.read.palette import get_palettes
from ..service.read.register_media import get_existing_graphics
from ..service.read.string_resource import get_string_resources
from ..value_object.read.media.blendomatic import Blendomatic
from ..value_object.read.media_types import MediaType


def get_blendomatic_data(args, blend_mode_count=None):
    """
    reads blendomatic.dat

    TODO: make this a MediaExportRequest.
    """
    # in HD edition, blendomatic.dat has been renamed to
    # blendomatic_x1.dat; their new blendomatic.dat has a new, unsupported
    # format.
    game_edition = args.game_version[0]
    blendomatic_path = game_edition.media_paths[MediaType.BLEND][0]
    blendomatic_dat = args.srcdir[blendomatic_path].open('rb')

    return Blendomatic(blendomatic_dat, blend_mode_count)


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
    debug_init(args.debugdir, args, args.debug_log)

    # Read .dat
    yield "empires.dat"
    debug_gamedata_format(args.debugdir, args.game_version, args.debug_log)
    gamespec = get_gamespec(args.srcdir, args.game_version, args.flag("no_pickle_cache"))

    # Read strings
    string_resources = get_string_resources(args)
    debug_string_resources(args.debugdir, string_resources, args.debug_log)

    # Existing graphic IDs/filenames
    existing_graphics = get_existing_graphics(args)
    debug_registered_graphics(args.debugdir, existing_graphics, args.debug_log)

    # Convert
    modpacks = args.converter.convert(gamespec,
                                      args.game_version,
                                      string_resources,
                                      existing_graphics)

    for modpack in modpacks:
        ModpackExporter.export(modpack, args)

    if args.game_version[0].game_id not in ("ROR", "AOE2DE"):
        yield "blendomatic.dat"

        if args.game_version[0].game_id == "SWGB":
            blend_mode_count = gamespec[0]["blend_mode_count_swgb"].get_value()
            blend_data = get_blendomatic_data(args, blend_mode_count)

        else:
            blend_data = get_blendomatic_data(args)

        blend_data.save(args.targetdir, "blendomatic", args.compression_level)
        # data_formatter.add_data(blend_data.dump("blending_modes"))

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

    if game_edition.game_id == "AOC":
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
