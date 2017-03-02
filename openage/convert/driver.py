# Copyright 2015-2017 the openage authors. See copying.md for legal info.
"""
Receives cleaned-up srcdir and targetdir objects from .main, and drives the
actual conversion process.
"""

import os
import re
from subprocess import Popen, PIPE
from tempfile import gettempdir

from ..log import info, dbg
from .game_versions import GameVersion
from .blendomatic import Blendomatic
from .changelog import (ASSET_VERSION, ASSET_VERSION_FILENAME,
                        GAMESPEC_VERSION_FILENAME)
from .colortable import ColorTable, PlayerColorTable
from .dataformat.data_formatter import DataFormatter
from .gamedata.empiresdat import load_gamespec, EmpiresDat
from .hardcoded.termcolors import URXVTCOLS
from .hardcoded.terrain_tile_size import TILE_HALFSIZE
from .hdlanguagefile import (read_age2_hd_fe_stringresources,
                             read_age2_hd_3x_stringresources)
from .interface.interfacecutter import InterfaceCutter
from .interface.interfacerename import interface_rename
from .slp_converter_pool import SLPConverterPool
from .stringresource import StringResource


def get_string_resources(args):
    """ reads the (language) string resources """

    stringres = StringResource()

    srcdir = args.srcdir
    count = 0

    # AoK:TC uses .DLL PE files for its string resources,
    # HD uses plaintext files
    if GameVersion.age2_hd_fe in args.game_versions:
        count += read_age2_hd_fe_stringresources(stringres, srcdir["resources"])

    elif GameVersion.age2_hd_3x in args.game_versions:
        count += read_age2_hd_3x_stringresources(stringres, srcdir)

    elif srcdir["language.dll"].is_file():
        from .pefile import PEFile
        for name in ["language.dll", "language_x1.dll", "language_x1_p1.dll"]:
            pefile = PEFile(srcdir[name].open('rb'))
            stringres.fill_from(pefile.resources().strings)
            count += 1

    if not count:
        raise FileNotFoundError("could not find any language files")

    # TODO transform and cleanup the read strings:
    #      convert formatting indicators from HTML to something sensible, etc

    return stringres


def get_blendomatic_data(srcdir):
    """ reads blendomatic.dat """
    # in HD edition, blendomatic.dat has been renamed to
    # blendomatic_x1.dat; their new blendomatic.dat has a new, unsupported
    # format.
    try:
        blendomatic_dat = srcdir["data/blendomatic_x1.dat"].open('rb')
    except FileNotFoundError:
        blendomatic_dat = srcdir["data/blendomatic.dat"].open('rb')

    return Blendomatic(blendomatic_dat)


def get_gamespec(srcdir, game_versions, dont_pickle):
    """ reads empires.dat and fixes it """

    if GameVersion.age2_hd_ak in game_versions:
        filename = "empires2_x2_p1.dat"
    else:
        filename = "empires2_x1_p1.dat"

    cache_file = os.path.join(gettempdir(), "{}.pickle".format(filename))

    with srcdir["data", filename].open('rb') as empiresdat_file:
        gamespec = load_gamespec(empiresdat_file,
                                 game_versions,
                                 cache_file,
                                 not dont_pickle)

    # modify the read contents of datfile
    from .fix_data import fix_data
    # pylint: disable=no-member
    gamespec.empiresdat[0] = fix_data(gamespec.empiresdat[0])

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
    with args.targetdir[GAMESPEC_VERSION_FILENAME].open('w') as fil:
        fil.write(EmpiresDat.get_hash())

    # media conversion
    if not args.flag('no_media'):
        yield from convert_media(args)

        with args.targetdir[ASSET_VERSION_FILENAME].open('w') as fil:
            fil.write(str(ASSET_VERSION))

    # clean args (set by convert_metadata for convert_media)
    del args.palette

    info("asset conversion complete; asset version: " + str(ASSET_VERSION))


def convert_metadata(args):
    """ Converts the metadata part """
    if not args.flag("no_metadata"):
        info("converting metadata")
        data_formatter = DataFormatter()

    # required for player palette and color lookup during SLP conversion.
    yield "palette"

    # `.bin` files are renamed `.bina` in HD version 4
    if GameVersion.age2_hd_fe in args.game_versions:
        palette_path = "interface/50500.bina"
    else:
        palette_path = "interface/50500.bin"

    palette = ColorTable(args.srcdir[palette_path].open("rb").read())
    # store for use by convert_media
    args.palette = palette

    if args.flag("no_metadata"):
        return

    gamedata_path = args.targetdir.joinpath('gamedata')
    if gamedata_path.exists():
        gamedata_path.removerecursive()

    yield "empires.dat"
    gamespec = get_gamespec(args.srcdir, args.game_versions, args.flag("no_pickle_cache"))
    data_dump = gamespec.dump("gamedata")
    data_formatter.add_data(data_dump[0], prefix="gamedata/", single_output="gamedata")

    yield "blendomatic.dat"
    blend_data = get_blendomatic_data(args.srcdir)
    blend_data.save(args.targetdir, "blendomatic", ("csv",))
    data_formatter.add_data(blend_data.dump("blending_modes"))

    yield "player color palette"
    player_palette = PlayerColorTable(palette)
    data_formatter.add_data(player_palette.dump("player_palette"))

    yield "terminal color palette"
    termcolortable = ColorTable(URXVTCOLS)
    data_formatter.add_data(termcolortable.dump("termcolors"))

    yield "string resources"
    stringres = get_string_resources(args)
    data_formatter.add_data(stringres.dump("string_resources"))

    yield "writing gamespec csv files"
    data_formatter.export(args.targetdir, ("csv",))

    if args.flag('gen_extra_files'):
        dbg("generating extra files for visualization")
        tgt = args.targetdir
        with tgt['info/colortable.pal.png'].open_w() as outfile:
            palette.save_visualization(outfile)

        with tgt['info/playercolortable.pal.png'].open_w() as outfile:
            player_palette.save_visualization(outfile)


def extract_mediafiles_names_map(files_to_convert, args):
    """ Gets names from the *.bin files, make them lowercase """
    if GameVersion.age2_hd_fe in args.game_versions:
        suffix = '.bina'
    else:
        suffix = '.bin'

    names_map = dict()

    for filepath in files_to_convert:
        filename = b'/'.join(filepath.parts).decode()
        if filename.endswith(suffix):
            try:
                for line in filepath.open():
                    match = re.match(r"\w+_files\s+(\w+)\s+\w+\s+(\w+)", line)
                    if match:
                        groups = match.group(2, 1)
                        names_map[groups[0]] = groups[1].lower()
            except UnicodeDecodeError:
                # assume that the file is binary and ignore it
                continue

    return names_map


def slp_rename(filename, names_map):
    """ Returns a human-readable name if it's in the map """
    try:
        dirname_basename = re.match(r"^(.*/)(\d+)\.slp$", filename).group(1, 2)
        return dirname_basename[0] + names_map[dirname_basename[1]] + ".slp"
    except KeyError:
        return filename


def convert_media(args):
    """ Converts the media part """

    # set of (drsname, suffix) to ignore
    # if drsname is None, it matches to any naoe
    ignored = set()
    if args.flag("no_sounds"):
        ignored.add((None, '.wav'))
    if args.flag("no_graphics"):
        ignored.update([('graphics', '.slp'), ('terrain', '.slp')])
    if args.flag("no_interface"):
        ignored.add(('interface', '.slp'))

    files_to_convert = []
    for dirname in ['sounds', 'graphics', 'terrain', 'interface']:
        for filepath in args.srcdir[dirname].iterdir():
            if (None, filepath.suffix) in ignored:
                continue
            elif (filepath.parts[0].decode(), filepath.suffix) in ignored:
                # TODO:    ^^ parts[0] is most likely wrong.
                continue
            elif filepath.is_dir():
                continue

            files_to_convert.append(filepath)

    yield len(files_to_convert)

    if not files_to_convert:
        return

    info("converting media")

    named_mediafiles_map = extract_mediafiles_names_map(files_to_convert, args)

    jobs = getattr(args, "jobs", None)
    with SLPConverterPool(args.palette, jobs) as slp_converter:
        args.slp_converter = slp_converter

        from ..util.threading import concurrent_chain
        yield from concurrent_chain(
            (convert_mediafile(fpath,
                               named_mediafiles_map,
                               args) for fpath in files_to_convert), jobs)

    # clean args
    del args.slp_converter


def convert_mediafile(filepath, names_map, args):
    """
    Converts a single media file, according to the supplied arguments.
    Designed to be run in a thread via concurrent_chain.

    May write multiple output files (e.g. in the case of textures: csv, png).

    Args shall contain srcdir, targetdir, and slp_converter.
    """
    # progress message
    filename = b'/'.join(filepath.parts).decode()
    yield filename

    with filepath.open_r() as infile:
        indata = infile.read()

    if filename.endswith('.slp'):
        # some user interface textures must be cut using hardcoded values
        if filename.startswith('interface/'):
            cutter = InterfaceCutter(filename)
        else:
            cutter = None

        # do the CPU-intense part in a subprocess
        texture = args.slp_converter.convert(indata, cutter)

        # the hotspots of terrain textures must be fixed
        if filename.startswith('terrain/'):
            for entry in texture.image_metadata:
                entry["cx"] = TILE_HALFSIZE["x"]
                entry["cy"] = TILE_HALFSIZE["y"]

        # save atlas to targetdir
        texture.save(args.targetdir,
                     interface_rename(slp_rename(filename, names_map)),
                     ("csv",))

    elif filename.endswith('.wav'):
        # convert the WAV file to an opus file
        # TODO use libav or something to avoid this utility dependency
        invocation = ('opusenc', '--quiet', '-', '-')
        opusenc = Popen(invocation, stdin=PIPE, stdout=PIPE)
        outdata = opusenc.communicate(input=indata)[0]
        if opusenc.returncode != 0:
            raise Exception("opusenc failed")

        with args.targetdir[filename].with_suffix('.opus').open_w() as outfile:
            outfile.write(outdata)

    else:
        # simply copy the file over.
        with args.targetdir[filename].open_w() as outfile:
            outfile.write(indata)
