#!/usr/bin/env python3
#
#media files conversion stuff

from colortable import ColorTable, PlayerColorTable
from collections import defaultdict
import dataformat
from drs import DRS
import filelist
import hardcoded.termcolors
import os
import os.path
import pickle
from string import Template
import subprocess
from texture import Texture
import util
from util import file_write, dbg, ifdbg, set_write_dir, set_read_dir, set_verbosity, file_get_path


asset_folder = "assets/"

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
    set_read_dir(args.srcdir)

    drsfiles = {
        "graphics":  DRS("Data/graphics.drs"),
        "interface": DRS("Data/interfac.drs"),
        "sounds0":   DRS("Data/sounds.drs"),
        "sounds1":   DRS("Data/sounds_x1.drs"),
        "gamedata0": DRS("Data/gamedata.drs"),
        "gamedata1": DRS("Data/gamedata_x1.drs"),
        "gamedata2": DRS("Data/gamedata_x1_p1.drs"),
        "terrain":   DRS("Data/terrain.drs")
    }

    #this is the ingame color palette file id, 256 color lookup for all graphics pixels
    palette_id = 50500
    palette = ColorTable(drsfiles["interface"].get_file_data('bin', palette_id))

    #metadata dumping output format, more to come?
    output_formats = ("csv",)

    termcolortable = ColorTable(hardcoded.termcolors.urxvtcoltable)
    #write mode is disabled by default, unless destdir is set

    #saving files is disabled by default
    write_enabled = False

    if args.output:
        from slp import SLP

        write_enabled = True

        dbg("setting write dir to " + args.output, 1)
        set_write_dir(args.output)

        player_palette = PlayerColorTable(palette)

        if args.extrafiles:
            palette.save_visualization('info/colortable.pal.png')
            player_palette.save_visualization('info/playercolortable.pal.png')

        import blendomatic
        blend_data = blendomatic.Blendomatic("Data/blendomatic.dat")
        blend_data.save(os.path.join(asset_folder, "blendomatic.dat/"), output_formats)

        from pefile import PEFile
        from stringresource import StringResource
        stringres = StringResource()
        stringres.fill_from(PEFile("language.dll"))
        stringres.fill_from(PEFile("language_x1.dll"))
        stringres.fill_from(PEFile("language_x1_p1.dll"))
        #TODO: transform and cleanup the read strings... (strip html, insert formatchars, ...)

        #create the dump for the dat file
        import gamedata.empiresdat

        dat_cache_file = "/tmp/empires2_x1_p1.dat.pickle"
        datfile_name = "empires2_x1_p1.dat"

        #try to use cached version
        parse_empiresdat = False
        try:
            with open(dat_cache_file, "rb") as f:
                gamedata = pickle.load(f)
        except FileNotFoundError as err:
            parse_empiresdat = True

        if parse_empiresdat:
            datfile = gamedata.empiresdat.EmpiresDatGzip("Data/%s" % datfile_name)
            gamedata = gamedata.empiresdat.EmpiresDatWrapper()

            if args.extrafiles:
                datfile.raw_dump('raw/empires2x1p1.raw')

            dbg("reading main data file %s..." % (datfile_name), lvl=1)
            gamedata.read(datfile.content, 0)

            #store the datfile serialization for caching
            with open(dat_cache_file, "wb") as f:
                pickle.dump(gamedata, f)

        #modify the read contents of datfile
        dbg("repairing some values in main data file %s..." % (datfile_name), lvl=1)
        import fix_data
        gamedata.empiresdat[0] = fix_data.fix_data(gamedata.empiresdat[0])

        #dbg("transforming main data file %s..." % (datfile_name), lvl=1)
        #TODO: data transformation nao! (merge stuff, etcetc)

        data_formatter = dataformat.DataFormatter()

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
        util.file_write_multi(output_data, file_prefix=asset_folder)

    file_list = defaultdict(lambda: list())
    media_files_extracted = 0

    sound_list = filelist.SoundList()

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

                dbg("Extracting to " + fname + "...", 2)
                file_data = drsfile.get_file_data(file_extension, file_id)
            else:
                continue

            if file_extension == 'slp':
                s = SLP(file_data)
                out_file_tmp = "%s: %d.%s" % (drsname, file_id, file_extension)

                dbg(out_file_tmp + " -> " + fname + " -> generating atlas", 1)

                #create exportable texture from the slp
                texture = Texture(s, palette)

                #save the image and the corresponding metadata file
                texture.save(fname, output_formats)

            elif file_extension == 'wav':
                sound_filename = fname

                wav_output_file = file_get_path(fname, write=True)
                file_write(wav_output_file, file_data)

                if not args.no_opus:
                    file_extension   = "opus"
                    sound_filename   = "%s.%s" % (fbase, file_extension)
                    opus_output_file = file_get_path(sound_filename, write=True)

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

                sound_list.add_sound(file_id, sound_filename, file_extension)

            else:
                #format does not require conversion, store it as plain blob
                file_write(fname, file_data)

            media_files_extracted += 1

    if write_enabled:
        sound_formatter = dataformat.DataFormatter()
        sound_formatter.add_data(sound_list.dump())
        util.file_write_multi(sound_formatter.export(output_formats), file_prefix=asset_folder)

        dbg("media files extracted: %d" % (media_files_extracted), 0)

    #was a file listing requested?
    if args.list_files:
        for idx, f in file_list.items():
            print("%d = [ %s ]" % (idx, ", ".join(
                "%s/%d.%s" % (file_name, idx, file_extension) for file_name, file_extension in f)))
