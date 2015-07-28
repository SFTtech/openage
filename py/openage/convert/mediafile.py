# Copyright 2013-2015 the openage authors. See copying.md for legal info.

# media files conversion stuff

from collections import defaultdict
from openage.log import dbg, ifdbg
from tempfile import gettempdir
import os
import os.path
import pickle
import subprocess

from .. import util
from .colortable import ColorTable, PlayerColorTable
from .dataformat.data_formatter import DataFormatter
from .drs import DRS
from .hardcoded import termcolors, terrain_tile_size
from .util import file_write_multi
from .texture import Texture


# path of a file where the python structure gets serialized to.
# speeds up massively as the binary doesn't need to be reparsed.
dat_cache_file = os.path.join(gettempdir(), "empires2_x1_p1.dat.pickle")


class ExtractionRule:
    """
    rule for matching media file names
    """

    def __init__(self, rulestr):
        drsname, fname = rulestr.split(':')
        fnostr, fext = fname.split('.')

        if drsname == '*':
            drsname = None

        if fnostr == '*':
            fno = None
        else:
            fno = int(fnostr)

        if fext == '*':
            fext = None

        self.drsname = drsname
        self.fno = fno
        self.fext = fext

    def matches(self, drsname, fno, fext):
        if self.drsname and self.drsname != drsname:
            return False

        if self.fno and self.fno != fno:
            return False

        if self.fext and self.fext != fext:
            return False

        return True


def media_convert(args):
    """
    Perform asset conversion.

    Requires original assets and stores them in usable and free formats.

    The data is extracted from AoE's DRS files. See `doc/media/drs-files.md`
    for more information.

    """

    # assume to extract all files when nothing specified.
    if not args.extract:
        args.extract.append('*:*.*')

    extraction_rules = [ExtractionRule(e) for e in args.extract]

    dbg("age2 input directory: %s" % (args.srcdir,), 1)

    # soon to be replaced by a sane version detection
    drsmap = {
        "graphics":  "graphics.drs",
        "interface": "interfac.drs",
        "sounds0":   "sounds.drs",
        "sounds1":   "sounds_x1.drs",
        "gamedata0": "gamedata.drs",
        "gamedata1": "gamedata_x1.drs",
        "gamedata2": "gamedata_x1_p1.drs",
        "terrain":   "terrain.drs",
    }

    drsfiles = {
        k: util.ifilepath(args.srcdir, os.path.join("data", v), True)
        for k, v in drsmap.items()
    }

    # gamedata.drs does not exist in HD edition,
    # but its contents are in gamedata_x1.drs instead,
    # so we can ignore this file if it doesn't exist
    drsfiles = {
        k: DRS(p, drsmap[k])
        for k, p in drsfiles.items()
        if p and os.path.getsize(p) > 0
    }

    # this is the ingame color palette file id,
    # 256 color lookup for all graphics pixels
    palette_id = 50500
    palette_data = drsfiles["interface"].get_file_data('bin', palette_id)
    palette = ColorTable(palette_data)

    # metadata dumping output format, more to come?
    output_formats = ("csv",)

    termcolortable = ColorTable(termcolors.urxvtcoltable)

    # saving files is disabled by default
    write_enabled = False

    if args.output:
        dbg("storing files to %s" % args.output, 1)
        write_enabled = True

        player_palette = PlayerColorTable(palette)

        if args.extrafiles:
            palette.save_visualization('info/colortable.pal.png')
            player_palette.save_visualization('info/playercolortable.pal.png')

        from . import blendomatic

        # HD Edition has a blendomatic_x1.dat in addition to its new
        # blendomatic.dat blendomatic_x1.dat is the same file as AoK:TC's
        # blendomatic.dat, and HD does not have blendomatic.dat, so we try
        # _x1 first and fall back to the AoK:TC way if it does not exist
        blend_file = util.ifilepath(args.srcdir,
                                    "data/blendomatic_x1.dat", True)
        if not blend_file:
            blend_file = util.ifilepath(args.srcdir, "data/blendomatic.dat")

        blend_data = blendomatic.Blendomatic(blend_file)
        blend_data.save(os.path.join(args.output, "blendomatic.dat/"),
                        output_formats)

        from .stringresource import StringResource
        stringres = StringResource()

        # AoK:TC uses .DLL files for its string resources,
        # HD uses plaintext files
        lang_dll = util.ifilepath(args.srcdir, "language.dll", True)
        if lang_dll:
            from .pefile import PEFile
            for l in ["language.dll", "language_x1.dll", "language_x1_p1.dll"]:
                lpath = util.ifilepath(args.srcdir, l)
                stringres.fill_from(PEFile(lpath))

        else:
            from .hdlanguagefile import HDLanguageFile
            bindir = util.ifilepath(args.srcdir, "bin")
            for lang in os.listdir(bindir):
                langfile = "%s/%s/%s-language.txt" % (bindir, lang, lang)

                # there are some "base language" files in HD that we don't
                # need and only the dir for the language that's currently in
                # use contains a language file
                if os.path.isfile(langfile):
                    stringres.fill_from(HDLanguageFile(langfile, lang))

        # TODO: transform and cleanup the read strings...
        # (strip html, insert formatchars/identifiers, ...)

        # create the dump for the dat file
        from .gamedata import empiresdat

        # try to use cached version?
        parse_empiresdat = False
        if args.use_dat_cache:
            dbg("trying to use cache file %s..." % (dat_cache_file), lvl=1)
            try:
                with open(dat_cache_file, "rb") as f:
                    gamedata = pickle.load(f)
                    dbg("could successfully load cached gamedata!", lvl=1)

            except FileNotFoundError:
                parse_empiresdat = True

        if not args.use_dat_cache or parse_empiresdat:
            datfile_name = util.ifilepath(args.srcdir,
                                          os.path.join("data",
                                                       "empires2_x1_p1.dat"))
            datfile = empiresdat.EmpiresDatGzip(datfile_name)
            gamedata = empiresdat.EmpiresDatWrapper()

            if args.extrafiles:
                datfile.raw_dump('raw/empires2x1p1.raw')

            dbg("reading main data file %s..." % (datfile_name), lvl=1)
            gamedata.read(datfile.content, 0)

            # store the datfile serialization for caching
            with open(dat_cache_file, "wb") as f:
                pickle.dump(gamedata, f)

        # modify the read contents of datfile
        dbg("repairing some values in main data file %s..." % (datfile_name),
            lvl=1)
        from . import fix_data
        gamedata.empiresdat[0] = fix_data.fix_data(gamedata.empiresdat[0])

        # dbg("transforming main data file %s..." % (datfile_name), lvl=1)
        # TODO: data transformation nao! (merge stuff, etcetc)

        dbg("formatting output data...", lvl=1)
        data_formatter = DataFormatter()

        # dump metadata information
        data_dump = list()
        data_dump += blend_data.dump("blending_modes")
        data_dump += palette.dump("palette_%d" % palette_id)
        data_dump += player_palette.dump("player_palette_%d" % palette_id)
        data_dump += termcolortable.dump("termcolors")
        data_dump += stringres.dump("string_resources")
        data_formatter.add_data(data_dump)

        # dump gamedata datfile data
        gamedata_dump = gamedata.dump("gamedata")
        data_formatter.add_data(gamedata_dump[0], prefix="gamedata/")

        output_data = data_formatter.export(output_formats)

        # save the meta files
        dbg("saving output data files...", lvl=1)
        file_write_multi(output_data, args.output)

    file_list = defaultdict(lambda: list())
    media_files_extracted = 0

    # iterate over all available files in the drs, check whether they should
    # be extracted
    for drsname, drsfile in drsfiles.items():
        for file_extension, file_id in drsfile.files:
            if not any(er.matches(drsname, file_id, file_extension)
                       for er in extraction_rules):
                continue

            # append this file to the list result
            if args.list_files:
                file_list[file_id].append((drsfile.fname, file_extension))
                continue

            # generate output filename where data will be stored in
            if write_enabled:
                fbase = os.path.join(args.output, "Data",
                                     drsfile.name, str(file_id))
                fname = "%s.%s" % (fbase, file_extension)

                # create output folder
                util.mkdirs(os.path.split(fbase)[0])

                dbg("Extracting to %s..." % (fname), 2)
                file_data = drsfile.get_file_data(file_extension, file_id)
            else:
                continue

            # create an image file
            if file_extension == 'slp':
                from .slp import SLP
                s = SLP(file_data)

                dbg("%s: %d.%s -> %s -> generating atlas" % (
                    drsname, file_id, file_extension, fname), 1)

                # create exportable texture from the slp
                texture = Texture(s, palette)

                # the hotspots of terrain textures have to be fixed:
                if drsname == "terrain":
                    for entry in texture.image_metadata:
                        entry["cx"] = terrain_tile_size.tile_halfsize["x"]
                        entry["cy"] = terrain_tile_size.tile_halfsize["y"]

                # save the image and the corresponding metadata file
                texture.save(fname, output_formats)

            # create a sound file
            elif file_extension == 'wav':
                sound_filename = fname

                dbg("%s: %d.%s -> %s -> storing wav file" % (
                    drsname, file_id, file_extension, fname), 1)

                with open(fname, "wb") as f:
                    f.write(file_data)

                if not args.no_opus:
                    file_extension = "opus"
                    sound_filename = "%s.%s" % (fbase, file_extension)

                    # opusenc invokation (TODO: ffmpeg? some python-lib?)
                    opus_convert_call = ('opusenc',
                                         fname,
                                         sound_filename)
                    dbg("opus convert: %s -> %s ..." % (fname,
                                                        sound_filename), 1)

                    # TODO: when the output is big enough, this deadlocks.
                    oc = subprocess.Popen(opus_convert_call,
                                          stdout=subprocess.PIPE,
                                          stderr=subprocess.PIPE)
                    oc_out, oc_err = oc.communicate()

                    if ifdbg(2):
                        oc_out = oc_out.decode("utf-8")
                        oc_err = oc_err.decode("utf-8")

                        dbg(oc_out + "\n" + oc_err, 2)

                    # remove extracted original wave file
                    os.remove(fname)

            else:
                # format does not require conversion, store it as plain blob
                with open(fname, "wb") as f:
                    f.write(file_data)

            media_files_extracted += 1

    if write_enabled:
        dbg("media files extracted: %d" % (media_files_extracted), 0)

    # was a file listing requested?
    if args.list_files:
        for idx, f in file_list.items():
            print("%d = [ %s ]" % (idx, ", ".join(
                "%s/%d.%s" % ((file_name, idx, file_extension)
                              for file_name, file_extension in f))))
