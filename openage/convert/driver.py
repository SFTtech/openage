# Copyright 2015-2015 the openage authors. See copying.md for legal info.

""" Driver for all of the asset conversion. """

# TODO pylint: disable=R,no-member

import os
import subprocess
from tempfile import gettempdir

from ..log import dbg, info
from ..config import VERSION as openage_version
from ..util.fslike import CaseIgnoringReadOnlyDirectory, FSLikeObjectWrapper

from .blendomatic import Blendomatic
from .colortable import ColorTable, PlayerColorTable
from .dataformat.data_formatter import DataFormatter
from .gamedata.empiresdat import load_gamespec
from .hardcoded.termcolors import URXVTCOLS
from .hardcoded.terrain_tile_size import TILE_HALFSIZE
from .slp import SLP
from .texture import Texture


class DirectoryCreator(FSLikeObjectWrapper):
    """
    Wrapper around a filesystem-like object that automatically creates
    directories when attempting to create a file.
    """
    def _open_impl(self, pathcomponents, mode):
        if 'w' in mode or 'x' in mode or '+' in mode:
            self.mkdirs(pathcomponents[:-1])

        return self.wrapped.open(pathcomponents, mode)


def mount_drs_archives(srcdir):
    """
    Returns a Union where srcdir is mounted at /, and all the DRS files
    are mounted in subfolders.
    """
    from ..util.fslike import Union
    from .drs import DRS

    result = Union()
    result.mount(srcdir, '.')

    def mount_drs(filename, target, ignore_nonexistant=False):
        """ Mounts the DRS file from srcdir's filename at result's target. """
        if ignore_nonexistant and not srcdir.isfile(filename):
            return

        result.mount(DRS(srcdir.open(filename, 'rb')), target)

    mount_drs("data/graphics.drs", "graphics")
    mount_drs("data/interfac.drs", "interface")

    mount_drs("data/sounds.drs", "sounds")
    mount_drs("data/sounds_x1.drs", "sounds")

    # In the HD edition, gamedata.drs has been merged into gamedata_x1.drs
    mount_drs("data/gamedata.drs", "gamedata", ignore_nonexistant=True)
    mount_drs("data/gamedata_x1.drs", "gamedata")
    mount_drs("data/gamedata_x1_p1.drs", "gamedata")

    mount_drs("data/terrain.drs", "terrain")

    return result


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
    gamespec.empiresdat[0] = fix_data(gamespec.empiresdat[0])

    return gamespec


def convert_assets(srcdir, targetdir, gen_extra_files=False):
    """
    Perform asset conversion.

    Requires original assets and stores them in usable and free formats.

    The data is extracted from AoE's DRS files. See `doc/media/drs-files.md`
    for more information.

    sourcedir and targetdir are filesystem-like objects.

    If gen_extra_files is True, some more files, mostly for debugging purposes,
    are created.
    """
    srcdir = mount_drs_archives(srcdir)
    targetdir = DirectoryCreator(targetdir)
    data_formatter = DataFormatter()

    # required for player palette and color lookup during SLP conversion.
    info("reading color palette")
    palette = ColorTable(srcdir.open("interface/50500.bin", "rb").read())

    info("reading empires.dat")
    data_dump = get_gamespec(srcdir).dump("gamedata")
    data_formatter.add_data(data_dump[0], prefix="gamedata/")

    info("reading blendomatic.dat")
    blend_data = get_blendomatic_data(srcdir)
    blend_data.save(targetdir, "blendomatic/", ("csv",))
    data_formatter.add_data(blend_data.dump("blending_modes"))

    dbg("creating player color palette")
    player_palette = PlayerColorTable(palette)
    data_formatter.add_data(player_palette.dump("player_palette"))

    dbg("creating terminal color palette")
    termcolortable = ColorTable(URXVTCOLS)
    data_formatter.add_data(termcolortable.dump("termcolors"))

    info("reading string resources")
    stringres = get_string_resources(srcdir)
    data_formatter.add_data(stringres.dump("string_resources"))

    info("exporting all metadata")
    data_formatter.export(targetdir, ("csv",))

    if gen_extra_files:
        dbg("generating extra files for visualization")
        palette.save_visualization(
            targetdir.open('info/colortable.pal.png', 'wb'))
        player_palette.save_visualization(
            targetdir.open('info/playercolortable.pal.png', 'wb'))

    dbg("collecting list of files to convert")
    files_to_convert = []
    for dirname in ['sounds', 'graphics', 'terrain']:
        for filename in srcdir.listfiles(dirname):
            files_to_convert.append(dirname + '/' + filename)

    info("converting a total of %d files" % len(files_to_convert))
    for progress, filename in enumerate(files_to_convert):
        infile = srcdir.open(filename, 'rb')

        info("[%d/%d] %s" % (progress, len(files_to_convert), filename))

        if filename.endswith('.slp'):
            # convert the SLP file to a PNG/CSV atlas
            texture = Texture(SLP(infile.read()), palette)

            # the hotspots of terrain textures must be fixed:
            if filename.startswith('terrain.drs'):
                for entry in texture.image_metadata:
                    entry["cx"] = TILE_HALFSIZE["x"]
                    entry["cy"] = TILE_HALFSIZE["y"]

            # save the image and the corresponding metadata file
            texture.save(targetdir, filename, ("csv",))

        elif filename.endswith('.wav'):
            # convert the WAV file to an opus file
            # TODO use libopusfile directly

            tmpwavname = os.path.join(gettempdir(), "tmpsound.wav")
            tmpopusname = os.path.join(gettempdir(), "tmpsound.opus")

            with open(tmpwavname, 'wb') as tmpfile:
                tmpfile.write(infile.read())

            invocation = ('opusenc', '--quiet', tmpwavname, tmpopusname)
            if subprocess.call(invocation) != 0:
                raise Exception("opusenc failed")

            with targetdir.open(filename + ".opus", "wb") as outfile:
                outfile.write(open(tmpopusname, "rb").read())

            os.remove(tmpwavname)
            os.remove(tmpopusname)

        else:
            # simply copy the file over.
            with targetdir.open(filename, "wb") as outfile:
                outfile.write(infile.read())

    targetdir.open('converted_by', 'w').write(openage_version)

    info("conversion complete; converted by: " + openage_version)


def acquire_conversion_source_data():
    """
    Acquires source data for the asset conversion.

    Returns a file system-like object that holds all the required files.
    """
    # ask the for conversion source
    print("media conversion is required.")

    if 'AGE2DIR' in os.environ:
        sourcedir = os.environ['AGE2DIR']
        print("using AGE2DIR: " + sourcedir)
    else:
        print("please provide your AoE II installation dir:")

        try:
            sourcedir = input("> ")
        except KeyboardInterrupt:
            print("")
            exit(0)
        except EOFError:
            print("")
            exit(0)

    return CaseIgnoringReadOnlyDirectory(sourcedir)


def ensure_assets(force_reconvert=False):
    """
    Makes sure that all assets have been converted, and are up to date.
    """
    from ..util.fslike import Directory
    targetdir = Directory("assets/converted")

    try:
        converted_by = targetdir.open("converted_by").read().strip()

        if converted_by:
            # assets have already been converted; no conversion needed.
            # TODO check whether conversion has been done by a recent-enough
            #      version of openage.
            if not force_reconvert:
                return True

    except FileNotFoundError:
        # assets do not exist yet.
        pass

    # acquire conversion source data
    srcdir = acquire_conversion_source_data()

    testfile = 'data/empires2_x1_p1.dat'
    if not srcdir.isfile(testfile):
        print("file not found: " + testfile)
        return False

    convert_assets(srcdir, targetdir)

    return True
