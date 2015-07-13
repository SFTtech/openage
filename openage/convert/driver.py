# Copyright 2015-2015 the openage authors. See copying.md for legal info.

"""
Receives cleaned-up srcdir and targetdir objects from .main, and drives the
actual conversion process.
"""

from tempfile import gettempdir
import os
from subprocess import Popen, PIPE

from ..config import VERSION as openage_version
from ..log import info, dbg

from .blendomatic import Blendomatic
from .colortable import ColorTable, PlayerColorTable
from .dataformat.data_formatter import DataFormatter
from .gamedata.empiresdat import load_gamespec
from .hardcoded.termcolors import URXVTCOLS
from .hardcoded.terrain_tile_size import TILE_HALFSIZE
from .slp import SLP
from .texture import Texture


def get_string_resources(srcdir):
    """ reads the (language) string resources """
    from .stringresource import StringResource
    stringres = StringResource()

    # AoK:TC uses .DLL PE files for its string resources,
    # HD uses plaintext files
    if srcdir.isfile("language.dll"):
        from .pefile import PEFile
        for name in ["language.dll", "language_x1.dll", "language_x1_p1.dll"]:
            pefile = PEFile(srcdir.open(name, 'rb'))
            stringres.fill_from(pefile.resources().strings)
    else:
        count = 0
        from .hdlanguagefile import read_hd_language_file
        for lang in srcdir.listdirs("bin"):
            try:
                langfilename = ["bin", lang, lang + "-language.txt"]
                with srcdir.open(langfilename, 'rb') as langfile:
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
        blendomatic_dat = srcdir.open("data/blendomatic_x1.dat", 'rb')
    except FileNotFoundError:
        blendomatic_dat = srcdir.open("data/blendomatic.dat", 'rb')

    return Blendomatic(blendomatic_dat)


def get_gamespec(srcdir):
    """ reads empires.dat """
    with srcdir.open("data/empires2_x1_p1.dat", "rb") as empiresdat_file:
        gamespec = load_gamespec(
            empiresdat_file,
            os.path.join(gettempdir(), "empires2_x1_p1.dat.pickle"))

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
    data_formatter = DataFormatter()

    # required for player palette and color lookup during SLP conversion.
    yield "palette"
    palette = ColorTable(args.srcdir.open("interface/50500.bin", "rb").read())
    # store for usage in convert_mediafile
    args.palette = palette

    yield "empires.dat"
    data_dump = get_gamespec(args.srcdir).dump("gamedata")
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

    yield "gamespec"
    data_formatter.export(args.targetdir, ("csv",))

    if args.flag('gen_extra_files'):
        dbg("generating extra files for visualization")
        tgt = args.targetdir
        with tgt.open('info/colortable.pal.png', 'wb') as outfile:
            palette.save_visualization(outfile)

        with tgt.open('info/playercolortable.pal.png', 'wb') as outfile:
            player_palette.save_visualization(outfile)

    dbg("collecting list of files to convert")
    files_to_convert = []
    if not args.flag('no_media'):
        for dirname in ['sounds', 'graphics', 'terrain']:
            for filename in args.srcdir.listfiles(dirname):
                files_to_convert.append(dirname + '/' + filename)

    yield len(files_to_convert)

    for filename in files_to_convert:
        yield filename
        convert_mediafile(filename, args)

    args.targetdir.open('converted_by', 'w').write(openage_version)
    info("asset conversion complete; converted by: " + openage_version)

    # clean args
    del args.palette


def convert_mediafile(filename, args):
    """
    Converts a single media file, according to the supplied arguments.

    May write multiple output files (e.g. in the case of textures: csv, png).

    Args shall contain srcdir, targetdir and palette.
    """
    with args.srcdir.open(filename, 'rb') as infile:
        indata = infile.read()

    if filename.endswith('.slp'):
        # TODO this is slooooow beyond imagination (TM).
        # optimize SLP conversion with Cython.

        # parse indata as SLP
        slp = SLP(indata)

        # rasterize to a PNG/CSV atlas
        # about 80% of the time seems to be spent in this method.
        texture = Texture(slp, args.palette)

        # the hotspots of terrain textures must be fixed:
        if filename.startswith('terrain.drs'):
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

        with args.targetdir.open(filename + ".opus", "wb") as outfile:
            outfile.write(outdata)

    else:
        # simply copy the file over.
        with args.targetdir.open(filename, "wb") as outfile:
            outfile.write(indata)
