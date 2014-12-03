# Copyright 2013-2014 the openage authors. See copying.md for legal info.

# media files conversion stuff

from collections import defaultdict
from openage.log import dbg, ifdbg, set_verbosity
from tempfile import gettempdir
import os
import os.path
import pickle
import subprocess

from . import filelist
from . import util
from .colortable import ColorTable, PlayerColorTable
from .dataformat.data_formatter import DataFormatter
from .drs import DRS
from .hardcoded import termcolors
from .texture import Texture


asset_folder = "" #TODO: optimize out
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
    #assume to extract all files when nothing specified.
    if not args.extract:
        args.extract.append('*:*.*')

    extraction_rules = [ ExtractionRule(e) for e in args.extract ]

    #set path in utility class
    dbg("setting age2 input directory to " + args.srcdir, 1)
    util.set_read_dir(args.srcdir)

    drsfiles = {
        "graphics":  DRS("Data/graphics.drs"),
        "interface": DRS("Data/interfac.drs"),
        "sounds0":   DRS("Data/sounds.drs"),
        "sounds1":   DRS("Data/sounds_x1.drs"),
        "gamedata1": DRS("Data/gamedata_x1.drs"),
        "gamedata2": DRS("Data/gamedata_x1_p1.drs"),
        "terrain":   DRS("Data/terrain.drs")
    }

    #gamedata.drs does not exist in HD edition, but its contents are
    #in gamedata_x1.drs instead, so we can ignore this file if it doesn't exist
    if os.path.isfile(util.file_get_path("Data/gamedata.drs")):
        drsfiles["gamedata0"] = DRS("Data/gamedata.drs")

    #this is the ingame color palette file id, 256 color lookup for all graphics pixels
    palette_id = 50500
    palette = ColorTable(drsfiles["interface"].get_file_data('bin', palette_id))

    #metadata dumping output format, more to come?
    output_formats = ("csv",)

    termcolortable = ColorTable(termcolors.urxvtcoltable)
    #write mode is disabled by default, unless destdir is set

    #saving files is disabled by default
    write_enabled = False

    if args.output:
        from .slp import SLP

        write_enabled = True

        dbg("setting write dir to " + args.output, 1)
        util.set_write_dir(args.output)

        player_palette = PlayerColorTable(palette)

        if args.extrafiles:
            palette.save_visualization('info/colortable.pal.png')
            player_palette.save_visualization('info/playercolortable.pal.png')

        from . import blendomatic

        #HD Edition has a blendomatic_x1.dat in addition to its new blendomatic.dat
        #blendomatic_x1.dat is the same file as AoK:TC's blendomatic.dat, and TC does not have
        #blendomatic.dat, so we try _x1 first and fall back to the AoK:TC way if it does not exist
        blend_file = "Data/blendomatic_x1.dat"
        if not os.path.isfile(util.file_get_path(blend_file)):
            blend_file = "Data/blendomatic.dat"
        blend_data = blendomatic.Blendomatic(blend_file)
        blend_data.save(os.path.join(asset_folder, "blendomatic.dat/"), output_formats)

        from .stringresource import StringResource
        stringres = StringResource()

        #AoK:TC uses .DLL files for its string resources,
        #HD uses plaintext files
        if os.path.isfile(util.file_get_path("language.dll")):
            from .pefile import PEFile
            stringres.fill_from(PEFile("language.dll"))
            stringres.fill_from(PEFile("language_x1.dll"))
            stringres.fill_from(PEFile("language_x1_p1.dll"))
            #stringres.fill_from(PEFile("Games/Forgotten Empires/Data/language_x1_p1.dll"))
        else:
            from .hdlanguagefile import HDLanguageFile
            for lang in os.listdir(util.file_get_path("Bin")):
                langfile = "Bin/%s/%s-language.txt" % (lang, lang)

                #there is some "base language" files in HD that we don't need
                #and only the dir for the language that's currently in use contains a language file
                if os.path.isdir(util.file_get_path("Bin/%s" % (lang))) and os.path.isfile(util.file_get_path(langfile)):
                    stringres.fill_from(HDLanguageFile(langfile, lang))

        #TODO: transform and cleanup the read strings... (strip html, insert formatchars, ...)

        #create the dump for the dat file
        from .gamedata import empiresdat
        datfile_name = "empires2_x1_p1.dat"

        #try to use cached version?
        parse_empiresdat = False
        if args.use_dat_cache:
            dbg("trying to use cache file %s..." % (dat_cache_file), lvl=1)
            try:
                with open(dat_cache_file, "rb") as f:
                    gamedata = pickle.load(f)
                    dbg("could successfully load cached gamedata!", lvl=1)

            except FileNotFoundError as err:
                parse_empiresdat = True

        if not args.use_dat_cache or parse_empiresdat:
            datfile = empiresdat.EmpiresDatGzip("Data/%s" % datfile_name)
            gamedata = empiresdat.EmpiresDatWrapper()

            if args.extrafiles:
                datfile.raw_dump('raw/empires2x1p1.raw')

            dbg("reading main data file %s..." % (datfile_name), lvl=1)
            gamedata.read(datfile.content, 0)

            #store the datfile serialization for caching
            with open(dat_cache_file, "wb") as f:
                pickle.dump(gamedata, f)

        #modify the read contents of datfile
        dbg("repairing some values in main data file %s..." % (datfile_name), lvl=1)
        from . import fix_data
        gamedata.empiresdat[0] = fix_data.fix_data(gamedata.empiresdat[0])

        #dbg("transforming main data file %s..." % (datfile_name), lvl=1)
        #TODO: data transformation nao! (merge stuff, etcetc)

        dbg("formatting output data...", lvl=1)
        data_formatter = DataFormatter()

        #dump metadata information
        data_dump = list()
        data_dump += blend_data.dump("blending_modes")
        data_dump += player_palette.dump("player_palette_%d" % palette_id)
        data_dump += termcolortable.dump("termcolors")
        data_dump += stringres.dump("string_resources")
        data_formatter.add_data(data_dump)

        #dump gamedata datfile data
        gamedata_dump = gamedata.dump("gamedata")
        data_formatter.add_data(gamedata_dump[0], prefix="gamedata/")

        output_data = data_formatter.export(output_formats)

        #save the meta files
        dbg("saving output data files...", lvl=1)
        util.file_write_multi(output_data, file_prefix=asset_folder)

    file_list = defaultdict(lambda: list())
    media_files_extracted = 0

    #iterate over all available files in the drs, check whether they should be extracted
    for drsname, drsfile in drsfiles.items():
        for file_extension, file_id in drsfile.files:
            if not any(er.matches(drsname, file_id, file_extension) for er in extraction_rules):
                continue

            #append this file to the list result
            if args.list_files:
                file_list[file_id].append((drsfile.fname, file_extension))
                continue

            #generate output filename where data will be stored in
            if write_enabled:
                fbase = os.path.join(asset_folder, drsfile.fname, str(file_id))
                fname = "%s.%s" % (fbase, file_extension)

                dbg("Extracting to %s..." % (fname), 2)
                file_data = drsfile.get_file_data(file_extension, file_id)
            else:
                continue

            if file_extension == 'slp':
                s = SLP(file_data)
                out_file_tmp = "%s: %d.%s" % (drsname, file_id, file_extension)

                dbg("%s -> %s -> generating atlas" % (out_file_tmp, fname), 1)

                #create exportable texture from the slp
                texture = Texture(s, palette)

                # the hotspots of terrain textures have to be fixed:
                if drsname == "terrain":
                    for entry in texture.image_metadata:
                        entry["cx"] = 48
                        entry["cy"] = 24

                #save the image and the corresponding metadata file
                texture.save(fname, output_formats)

            elif file_extension == 'wav':
                sound_filename = fname

                wav_output_file = util.file_get_path(fname, write=True)
                util.file_write(wav_output_file, file_data)

                if not args.no_opus:
                    file_extension   = "opus"
                    sound_filename   = "%s.%s" % (fbase, file_extension)
                    opus_output_file = util.file_get_path(sound_filename, write=True)

                    #opusenc invokation (TODO: ffmpeg?)
                    opus_convert_call = ['opusenc', wav_output_file, opus_output_file]
                    dbg("opus convert: %s -> %s ..." % (fname, sound_filename), 1)

                    oc = subprocess.Popen(opus_convert_call, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                    oc_out, oc_err = oc.communicate()

                    if ifdbg(2):
                        oc_out = oc_out.decode("utf-8")
                        oc_err = oc_err.decode("utf-8")

                        dbg(oc_out + "\n" + oc_err, 2)

                    #remove original wave file
                    os.remove(wav_output_file)

            else:
                #format does not require conversion, store it as plain blob
                output_file = util.file_get_path(fname, write=True)
                util.file_write(output_file, file_data)

            media_files_extracted += 1

    if write_enabled:
        dbg("media files extracted: %d" % (media_files_extracted), 0)

    #was a file listing requested?
    if args.list_files:
        for idx, f in file_list.items():
            print("%d = [ %s ]" % (idx, ", ".join(
                "%s/%d.%s" % (file_name, idx, file_extension) for file_name, file_extension in f)))
