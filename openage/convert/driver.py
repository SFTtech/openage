# Copyright 2015-2015 the openage authors. See copying.md for legal info.

"""
Receives cleaned-up srcdir and targetdir objects from .main, and drives the
actual conversion process.
"""

import os
from subprocess import Popen, PIPE
from tempfile import gettempdir

from ..log import info, dbg

from .blendomatic import Blendomatic
from .changelog import ASSET_VERSION, ASSET_VERSION_FILENAME
from .colortable import ColorTable, PlayerColorTable
from .dataformat.data_formatter import DataFormatter
from .gamedata.empiresdat import load_gamespec
from .hardcoded.termcolors import URXVTCOLS
from .hardcoded.terrain_tile_size import TILE_HALFSIZE
from .slp_converter_pool import SLPConverterPool


def get_string_resources(srcdir):
    """ reads the (language) string resources """
    from .stringresource import StringResource
    stringres = StringResource()

    # AoK:TC uses .DLL PE files for its string resources,
    # HD uses plaintext files
    if srcdir["language.dll"].is_file():
        from .pefile import PEFile
        for name in ["language.dll", "language_x1.dll", "language_x1_p1.dll"]:
            pefile = PEFile(srcdir[name].open('rb'))
            stringres.fill_from(pefile.resources().strings)
    else:
        count = 0
        from .hdlanguagefile import read_hd_language_file
        
        bin_path = srcdir.joinpath("bin") 
        for lang in bin_path.list():
            if bin_path.joinpath(lang).is_file():
                continue
            
            try:
                langfilename = [b"bin", lang, lang + b"-language.txt"]
                with srcdir[langfilename].open('rb') as langfile:
                    stringres.fill_from(read_hd_language_file(langfile, lang))

                count += 1
            except FileNotFoundError:
                # that's fine, there are no language files for every language.
                pass

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
    yield from convert_metadata(args)

    if not args.flag('no_media'):
        yield from convert_media(args)

    # clean args (set by convert_metadata for convert_media)
    del args.palette

    args.targetdir[ASSET_VERSION_FILENAME].open('w').write(str(ASSET_VERSION))
    info("asset conversion complete; asset version: " + str(ASSET_VERSION))


def convert_metadata(args):
    """ Converts the metadata part """
    if not args.flag("no_metadata"):
        info("converting metadata")
        data_formatter = DataFormatter()

    # required for player palette and color lookup during SLP conversion.
    yield "palette"
    palette = ColorTable(args.srcdir["interface/50500.bin"].open("rb").read())
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
    stringres = get_string_resources(args.srcdir)
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
    for dirname in ['sounds', 'graphics', 'terrain']:
        for filepath in args.srcdir[dirname].iterdir():
            if filepath.suffix in ignored_suffixes:
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
        # do the CPU-intense part in a subprocess
        texture = args.slp_converter.convert(indata)

        # the hotspots of terrain textures must be fixed
        if filename.startswith('terrain/'):
            for entry in texture.image_metadata:
                entry["cx"] = TILE_HALFSIZE["x"]
                entry["cy"] = TILE_HALFSIZE["y"]

        # save atlas to targetdir
        texture.save(args.targetdir, filename, ("csv",))

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
