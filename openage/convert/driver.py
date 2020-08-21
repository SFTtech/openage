# Copyright 2015-2020 the openage authors. See copying.md for legal info.

"""
Receives cleaned-up srcdir and targetdir objects from .main, and drives the
actual conversion process.
"""

import os
from tempfile import gettempdir

from openage.convert.entity_object.language.stringresource import StringResource

from ..log import info, dbg
from .changelog import (ASSET_VERSION)
from .processor.modpack_exporter import ModpackExporter
from .service.language.languagetextfile import read_age2_hd_3x_stringresources,\
    read_de2_language_file
from .value_object.dataformat.game_version import GameEdition, GameExpansion
from .value_object.dataformat.media_types import MediaType
from .value_object.media.blendomatic import Blendomatic
from .value_object.media.colortable import ColorTable
from .value_object.media.datfile.empiresdat import load_gamespec


# REFA: function -> service
def get_string_resources(args):
    """ reads the (language) string resources """

    stringres = StringResource()

    srcdir = args.srcdir
    game_edition = args.game_version[0]

    language_files = game_edition.media_paths[MediaType.LANGUAGE]

    from .value_object.media.pefile import PEFile

    for language_file in language_files:
        if game_edition in (GameEdition.ROR, GameEdition.AOC, GameEdition.SWGB):
            # AoC/RoR use .DLL PE files for their string resources
            pefile = PEFile(srcdir[language_file].open('rb'))
            stringres.fill_from(pefile.resources().strings)

        elif game_edition is GameEdition.HDEDITION:
            read_age2_hd_3x_stringresources(stringres, srcdir)

        elif game_edition is GameEdition.AOE2DE:
            strings = read_de2_language_file(srcdir, language_file)
            stringres.fill_from(strings)

        else:
            raise Exception("No service found for parsing language files of version %s"
                            % game_edition.name)

        # TODO: Other game versions

    # TODO: transform and cleanup the read strings:
    #       convert formatting indicators from HTML to something sensible, etc

    return stringres


# REFA: function -> service
def get_blendomatic_data(args):
    """ reads blendomatic.dat """
    # in HD edition, blendomatic.dat has been renamed to
    # blendomatic_x1.dat; their new blendomatic.dat has a new, unsupported
    # format.
    game_edition = args.game_version[0]
    blendomatic_path = game_edition.media_paths[MediaType.BLEND][0]
    blendomatic_dat = args.srcdir[blendomatic_path].open('rb')

    return Blendomatic(blendomatic_dat)


# REFA: function -> service
def get_gamespec(srcdir, game_version, dont_pickle):
    """
    Reads empires.dat file.
    """
    if game_version[0] in (GameEdition.ROR, GameEdition.AOC, GameEdition.AOE2DE):
        filepath = srcdir.joinpath(game_version[0].media_paths[MediaType.DATFILE][0])

    elif game_version[0] is GameEdition.SWGB:
        if GameExpansion.SWGB_CC in game_version[1]:
            filepath = srcdir.joinpath(game_version[1][0].media_paths[MediaType.DATFILE][0])

        else:
            filepath = srcdir.joinpath(game_version[0].media_paths[MediaType.DATFILE][0])

    cache_file = os.path.join(gettempdir(), "{}.pickle".format(filepath.name))

    with filepath.open('rb') as empiresdat_file:
        gamespec = load_gamespec(empiresdat_file,
                                 game_version,
                                 cache_file,
                                 not dont_pickle)

    return gamespec


# REFA: function -> tool
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

    info("asset conversion complete; asset version: %s", ASSET_VERSION)


# REFA: function -> service
def get_palettes(srcdir, game_version, index=None):
    """
    Read and create the color palettes.
    """
    game_edition = game_version[0]

    palettes = {}

    if game_edition in (GameEdition.ROR, GameEdition.AOC, GameEdition.SWGB, GameEdition.HDEDITION):
        if index:
            palette_path = "%s/%s.bina" % (MediaType.PALETTES.value, str(index))
            palette_file = srcdir[palette_path]
            palette = ColorTable(palette_file.open("rb").read())
            palette_id = int(palette_file.stem)

            palettes[palette_id] = palette

        else:
            palette_dir = srcdir[MediaType.PALETTES.value]
            for palette_file in palette_dir.iterdir():
                # Only 505XX.bina files are usable palettes
                if palette_file.stem.startswith("505"):
                    palette = ColorTable(palette_file.open("rb").read())
                    palette_id = int(palette_file.stem)

                    palettes[palette_id] = palette

            if game_edition is GameEdition.HDEDITION:
                # TODO: HD edition has extra palettes in the dat folder
                pass

    elif game_edition in (GameEdition.AOE1DE, GameEdition.AOE2DE):
        # Parse palettes.conf file and save the ids/paths
        conf_filepath = "%s/palettes.conf" % (MediaType.PALETTES.value)
        conf_file = srcdir[conf_filepath].open('rb')
        palette_paths = {}

        for line in conf_file.read().decode('utf-8').split('\n'):
            line = line.strip()

            # skip comments and empty lines
            if not line or line.startswith('//'):
                continue

            palette_id, filepath = line.split(',')
            palette_id = int(palette_id)
            palette_paths[palette_id] = filepath

        if index:
            palette_path = "%s/%s" % (MediaType.PALETTES.value, palette_paths[index])
            palette = ColorTable(srcdir[palette_path].open("rb").read())

            palettes[index] = palette

        else:
            for palette_id, filepath in palette_paths.items():
                palette_path = "%s/%s" % (MediaType.PALETTES.value, filepath)
                palette_file = srcdir[palette_path]
                palette = ColorTable(palette_file.open("rb").read())

                palettes[palette_id] = palette

    return palettes


# REFA: function -> tool
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
    gamespec = get_gamespec(args.srcdir, args.game_version, args.flag("no_pickle_cache"))

    # Read strings
    string_resources = get_string_resources(args)

    # Existing graphic IDs/filenames
    existing_graphics = get_existing_graphics(args)

    # Convert
    modpacks = args.converter.convert(gamespec,
                                      args.game_version,
                                      string_resources,
                                      existing_graphics)

    for modpack in modpacks:
        ModpackExporter.export(modpack, args)

    if args.game_version[0] not in (GameEdition.ROR, GameEdition.AOE2DE):
        yield "blendomatic.dat"
        blend_data = get_blendomatic_data(args)
        blend_data.save(args.targetdir, "blendomatic")
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


# REFA: function -> service
def get_converter(game_version):
    """
    Returns the converter for the specified game version.
    """
    game_edition = game_version[0]
    game_expansions = game_version[1]

    if game_edition is GameEdition.ROR:
        from .processor.ror.processor import RoRProcessor
        return RoRProcessor

    if game_edition is GameEdition.AOC:
        from .processor.aoc.processor import AoCProcessor
        return AoCProcessor

    if game_edition is GameEdition.AOE2DE:
        from .processor.de2.processor import DE2Processor
        return DE2Processor

    if game_edition is GameEdition.SWGB:
        if GameExpansion.SWGB_CC in game_expansions:
            from .processor.swgbcc.processor import SWGBCCProcessor
            return SWGBCCProcessor

    raise Exception("no valid converter found for game edition %s"
                    % game_edition.edition_name)


# REFA: function -> service
def get_existing_graphics(args):
    """
    List the graphics files that exist in the graphics file paths.
    """
    filenames = []
    for filepath in args.srcdir[MediaType.GRAPHICS.value].iterdir():
        filenames.append(filepath.stem)

    return filenames
