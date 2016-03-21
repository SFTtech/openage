# Copyright 2015-2016 the openage authors. See copying.md for legal info.
# pylint: disable=R0912
"""
Receives cleaned-up srcdir and targetdir objects from .main, and drives the
actual conversion process.
"""

import os
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
from .slp_converter_pool import SLPConverterPool
from .interface.interfacecutter import InterfaceCutter
from .interface.interfacerename import interface_rename


def get_string_resources(args):
    """ reads the (language) string resources """
    from .stringresource import StringResource
    stringres = StringResource()

    srcdir = args.srcdir
    count = 0

    # AoK:TC uses .DLL PE files for its string resources,
    # HD uses plaintext files
    if GameVersion.age2_fe in args.game_versions:
        from .hdlanguagefile import read_hd_language_file

        for lang in srcdir["resources"].list():
            try:
                if lang == b'_common':
                    continue
                langfilename = ["resources", lang.decode(),
                                "strings", "key-value",
                                "key-value-strings-utf8.txt"]

                with srcdir[langfilename].open('rb') as langfile:
                    stringres.fill_from(read_hd_language_file(langfile, lang))

                count += 1
            except FileNotFoundError:
                # that's fine, there are no language files for every language.
                pass
    elif GameVersion.age2_hd_3x in args.game_versions:
        from .hdlanguagefile import read_hd_language_file

        # HD Edition 3.x and below store language .txt files
        # in the Bin/ folder.
        # Specific language strings are in Bin/$LANG/*.txt.
        for lang in srcdir["bin"].list():
            dirname = ["bin", lang.decode()]

            # There are some .txt files immediately in bin/, but they don't
            # seem to contain anything useful. (Everything is overridden by
            # files in Bin/$LANG/.)
            if not srcdir[dirname].is_dir():
                continue

            # Sometimes we can have language DLLs in Bin/$LANG/
            # e.g. HD Edition 2.0
            # We do _not_ want to treat these as text files
            # so first check explicitly

            if srcdir["bin", lang.decode(), "language.dll"].is_file():
                from .pefile import PEFile
                for name in ["language.dll",
                             "language_x1.dll",
                             "language_x1_p1.dll"]:
                    pefile = PEFile(
                        srcdir["bin", lang.decode(), name].open('rb'))
                    stringres.fill_from(pefile.resources().strings)
                    count += 1

            else:
                for basename in srcdir[dirname].list():
                    langfilename = ["bin", lang.decode(), basename]
                    with srcdir[langfilename].open('rb') as langfile:
                        # No utf-8 :(
                        stringres.fill_from(
                            read_hd_language_file(
                                langfile, lang, enc='iso-8859-1'))
                    count += 1

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

    with blendomatic_dat:
        return Blendomatic(blendomatic_dat)


def get_gamespec(srcdir, dont_pickle):
    """ reads empires.dat """

    cache_file = os.path.join(gettempdir(), "empires2_x1_p1.dat.pickle")

    with srcdir["data/empires2_x1_p1.dat"].open('rb') as empiresdat_file:
        gamespec = load_gamespec(empiresdat_file, cache_file, not dont_pickle)

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
    if GameVersion.age2_fe in args.game_versions:
        palette_path = "interface/50500.bina"
    else:
        palette_path = "interface/50500.bin"

    palette = ColorTable(args.srcdir[palette_path].open("rb").read())
    # store for use by convert_media
    args.palette = palette

    if args.flag("no_metadata"):
        return

    yield "empires.dat"
    gamespec = get_gamespec(args.srcdir, args.flag("no_pickle_cache"))
    data_dump = gamespec.dump("gamedata")
    data_formatter.add_data(data_dump[0], prefix="gamedata/")

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


def convert_media(args):
    """ Converts the media part """
    ignored_suffixes = set()
    if args.flag("no_sounds"):
        ignored_suffixes.add('.wav')
    if args.flag("no_graphics"):
        ignored_suffixes.add('.slp')

    files_to_convert = []
    for dirname in ['sounds', 'graphics', 'terrain', 'interface']:
        for filepath in args.srcdir[dirname].iterdir():
            if filepath.suffix in ignored_suffixes:
                continue
            elif filepath.is_dir():
                continue

            files_to_convert.append(filepath)

    yield len(files_to_convert)

    if not files_to_convert:
        return

    info("converting media")

    jobs = getattr(args, "jobs", None)
    with SLPConverterPool(args.palette, jobs) as slp_converter:
        args.slp_converter = slp_converter

        from ..util.threading import concurrent_chain
        yield from concurrent_chain(
            (convert_mediafile(fpath, args) for fpath in files_to_convert),
            jobs)

    # clean args
    del args.slp_converter


def convert_mediafile(filepath, args):
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
        cutter = InterfaceCutter(filename) if filename.startswith('interface/') else None

        # do the CPU-intense part in a subprocess
        texture = args.slp_converter.convert(indata, cutter)

        # the hotspots of terrain textures must be fixed
        if filename.startswith('terrain/'):
            for entry in texture.image_metadata:
                entry["cx"] = TILE_HALFSIZE["x"]
                entry["cy"] = TILE_HALFSIZE["y"]

        # save atlas to targetdir
        texture.save(args.targetdir, interface_rename(filename), ("csv",))

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
