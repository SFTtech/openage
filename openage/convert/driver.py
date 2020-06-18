# Copyright 2015-2020 the openage authors. See copying.md for legal info.
"""
Receives cleaned-up srcdir and targetdir objects from .main, and drives the
actual conversion process.
"""

import os
import re
from tempfile import gettempdir

from openage.convert.dataformat.media_types import MediaType
from openage.convert.dataformat.version_detect import GameEdition, GameExpansion
from openage.convert.langfile.hdlanguagefile import read_de2_language_file

from ..log import info, dbg
from .blendomatic import Blendomatic
from .changelog import (ASSET_VERSION)
from .colortable import ColorTable, PlayerColorTable
from .export.data_formatter import DataFormatter
from .gamedata.empiresdat import load_gamespec
from .hardcoded.termcolors import URXVTCOLS
from .hardcoded.terrain_tile_size import TILE_HALFSIZE
from .interface.cutter import InterfaceCutter
from .interface.rename import hud_rename
from .langfile.hdlanguagefile import read_age2_hd_3x_stringresources
from .langfile.stringresource import StringResource
from .opus import opusenc
from .processor.modpack_exporter import ModpackExporter
from .slp_converter_pool import SLPConverterPool


def get_string_resources(args):
    """ reads the (language) string resources """

    stringres = StringResource()

    srcdir = args.srcdir
    game_edition = args.game_version[0]

    language_files = game_edition.media_paths[MediaType.LANGUAGE]

    from .langfile.pefile import PEFile

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


def get_blendomatic_data(args):
    """ reads blendomatic.dat """
    # in HD edition, blendomatic.dat has been renamed to
    # blendomatic_x1.dat; their new blendomatic.dat has a new, unsupported
    # format.
    game_edition = args.game_version[0]
    blendomatic_path = game_edition.media_paths[MediaType.BLEND][0]
    blendomatic_dat = args.srcdir[blendomatic_path].open('rb')

    return Blendomatic(blendomatic_dat)


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


def convert_metadata(args):
    """
    Converts the metadata part.
    """
    if not args.flag("no_metadata"):
        info("converting metadata")
        data_formatter = DataFormatter()

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
    modpacks = args.converter.convert(gamespec, args.game_version, string_resources, existing_graphics)

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
    termcolortable = ColorTable(URXVTCOLS)
    # data_formatter.add_data(termcolortable.dump("termcolors"))

    yield "game specification files"
    # data_formatter.export(args.targetdir, ("csv",))

    if args.flag('gen_extra_files'):
        dbg("generating extra files for visualization")
        tgt = args.targetdir
        with tgt['info/colortable.pal.png'].open_w() as outfile:
            # palette.save_visualization(outfile)
            pass

        with tgt['info/playercolortable.pal.png'].open_w() as outfile:
            # player_palette.save_visualization(outfile)
            pass


def get_converter(game_version):
    """
    Returns the converter for the specified game version.
    """
    game_edition = game_version[0]
    game_expansions = game_version[1]

    if game_edition is GameEdition.ROR:
        from .processor.ror.processor import RoRProcessor
        return RoRProcessor

    elif game_edition is GameEdition.AOC:
        from .processor.aoc.processor import AoCProcessor
        return AoCProcessor

    elif game_edition is GameEdition.AOE2DE:
        from .processor.de2.processor import DE2Processor
        return DE2Processor

    elif game_edition is GameEdition.SWGB:
        if GameExpansion.SWGB_CC in game_expansions:
            from openage.convert.processor.swgbcc.processor import SWGBCCProcessor
            return SWGBCCProcessor

    raise Exception("no valid converter found for game edition %s"
                    % game_edition.edition_name)


def extract_mediafiles_names_map(srcdir):
    """
    Some *.bina files contain name assignments.
    They're in the form of e.g.:
    "background1_files     camdlg1  none  53171  -1"

    We use this mapping to rename the file.
    """

    matcher = re.compile(r"\w+_files\s+(\w+)\s+\w+\s+(\w+)")

    names_map = dict()

    for filepath in srcdir["interface"].iterdir():
        if filepath.suffix == '.bina':
            try:
                for line in filepath.open():
                    match = matcher.search(line)
                    if match:
                        groups = match.group(2, 1)
                        names_map[groups[0]] = groups[1].lower()
            except UnicodeDecodeError:
                # assume that the file is binary and ignore it
                continue

    return names_map


def slp_rename(filepath, names_map):
    """ Returns a human-readable name if it's in the map """
    try:
        # look up the slp obj_id (= file stem) in the rename map
        return filepath.parent[
            names_map[filepath.stem] + filepath.suffix
        ]

    except KeyError:
        return filepath


def get_existing_graphics(args):
    """
    List the graphics files that exist in the graphics file paths.
    """
    filenames = []
    for filepath in args.srcdir[MediaType.GRAPHICS.value].iterdir():
        filenames.append(filepath.stem)

    return filenames


def convert_media(args):
    """ Converts the media part """

    # set of (drsname, suffix) to ignore
    # if dirname is None, it matches to any name
    ignored = get_filter(args)

    files_to_convert = []
    for dirname in ['sounds', 'graphics', 'terrain',
                    'interface', 'gamedata']:

        for filepath in args.srcdir[dirname].iterdir():
            skip_file = False

            # check if the path should be ignored
            for folders, ext in ignored:
                if ext == filepath.suffix and\
                   (not folders or dirname in folders):
                    skip_file = True
                    break

            # skip unwanted ids ("just debugging things(tm)")
            if getattr(args, "obj_id", None) and\
               int(filepath.stem) != args.obj_id:
                skip_file = True

            if skip_file or filepath.is_dir():
                continue

            # by default, keep the "dirname" the same.
            # we may want to rename though.
            output_dir = None

            # do the dir "renaming"
            if dirname == "gamedata" and filepath.suffix == ".slp":
                output_dir = "graphics"
            elif dirname in {"gamedata", "interface"} and filepath.suffix == ".wav":
                output_dir = "sounds"

            files_to_convert.append((filepath, output_dir))

    yield len(files_to_convert)

    if not files_to_convert:
        return

    info("converting media")

    # there is obj_id->name mapping information in some bina files
    named_mediafiles_map = extract_mediafiles_names_map(args.srcdir)

    jobs = getattr(args, "jobs", None)
    with SLPConverterPool(args.palette, jobs) as pool:

        from ..util.threading import concurrent_chain
        yield from concurrent_chain(
            (convert_mediafile(
                fpath,
                dirname,
                named_mediafiles_map,
                pool,
                args
            ) for (fpath, dirname) in files_to_convert),
            jobs
        )


def get_filter(args):
    """
    Return a set containing tuples (DIRNAMES, SUFFIX), where DIRNAMES
    is a set of strings. Files in directory D with D in DIRNAMES and with
    suffix SUFFIX shall not be converted.
    If DIRNAMES is None, it matches all directories.
    """
    ignored = set()
    if args.flag("no_sounds"):
        ignored.add((None, '.wav'))
    if args.flag("no_graphics"):
        ignored.add((frozenset({'graphics', 'terrain', 'gamedata'}), '.slp'))
    if args.flag("no_interface"):
        ignored.add((frozenset({'interface'}), '.slp'))
        ignored.add((frozenset({'interface'}), '.bina'))
    if args.flag("no_scripts"):
        ignored.add((frozenset({'gamedata'}), '.bina'))

    return ignored


def change_dir(path_parts, dirname):
    """
    If requested, rename the containing directory
    This assumes the path ends with dirname/filename.ext,
    so that dirname can be replaced.
    This is used for placing e.g. interface/lol.wav in sounds/lol.wav
    """

    # this assumes the directory name is the second last part
    # if we decide for other directory hierarchies,
    # this assumption will be wrong!
    if dirname:
        new_parts = list(path_parts)
        new_parts[-2] = dirname.encode()
        return new_parts

    return path_parts


def convert_slp(filepath, dirname, names_map, converter_pool, args):
    """
    Convert a slp image and save it to the target dir.
    This also writes the accompanying metadata file.
    """

    with filepath.open_r() as infile:
        indata = infile.read()

    # some user interface textures must be cut using hardcoded values
    if filepath.parent.name == 'interface':
        # the stem is the file obj_id
        cutter = InterfaceCutter(int(filepath.stem))
    else:
        cutter = None

    # do the CPU-intense part a worker process
    texture = converter_pool.convert(indata, cutter)

    # the hotspots of terrain textures must be fixed
    # it has to be in the west corner of a tile.
    if filepath.parent.name == 'terrain':
        for entry in texture.image_metadata:
            entry["cx"] = 0
            entry["cy"] = TILE_HALFSIZE["y"]

    # replace .slp by .png and rename the file
    # by some lookups (that map obj_id -> human readable)
    tex_filepath = hud_rename(slp_rename(
        filepath,
        names_map
    )).with_suffix(".slp.png")

    # pretty hacky: use the source path-parts as output filename,
    # additionally change the output dir name if requested
    out_filename = b'/'.join(change_dir(tex_filepath.parts, dirname)).decode()

    # save atlas to targetdir
    texture.save(args.targetdir, out_filename, ("csv",))


def convert_wav(filepath, dirname, args):
    """
    Convert a wav audio file to an opus file
    """

    with filepath.open_r() as infile:
        indata = infile.read()

    outdata = opusenc.encode(indata)
    if isinstance(outdata, (str, int)):
        raise Exception("opusenc failed: {}".format(outdata))

    # rename the directory
    out_parts = change_dir(filepath.parts, dirname)

    # save the converted sound data
    with args.targetdir[out_parts].with_suffix('.opus').open_w() as outfile:
        outfile.write(outdata)


def convert_mediafile(filepath, dirname, names_map, converter_pool, args):
    """
    Converts a single media file, according to the supplied arguments.
    Designed to be run in a thread via concurrent_chain.

    May write multiple output files (e.g. in the case of textures: csv, png).

    Args shall contain srcdir, targetdir.
    """

    # progress message
    filename = b'/'.join(filepath.parts).decode()
    yield filename

    if filepath.suffix == '.slp':
        convert_slp(filepath, dirname, names_map, converter_pool, args)

    elif filepath.suffix == '.wav':
        convert_wav(filepath, dirname, args)

    else:
        # simply copy the file over.
        with filepath.open_r() as infile:
            with args.targetdir[filename].open_w() as outfile:
                outfile.write(infile.read())
