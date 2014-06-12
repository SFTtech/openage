#!/usr/bin/env python3
#
#handler routines for creating and converting data files

import blendomatic
import colortable
import dataformat
import filelist
from fnmatch import fnmatch
import gamedata.empiresdat
import stringresource
import texture
from util import dbg, set_write_dir, file_write, file_get_path


def data_generate(args):

    list_enabled = False
    write_enabled = False

    #write mode is disabled by default, unless output is set
    if args.output != None:
        dbg("setting write dir to " + args.output, 1)
        set_write_dir(args.output)
        write_enabled = True
    else:
        if args.list_files:
            set_write_dir("")
            list_enabled = True

    #generate files in these formats
    formats = ("struct", "structimpl")

    data_formatter = dataformat.DataFormatter()

    struct_data = list()
    struct_data += dataformat.MultisubtypeMember.MultisubtypeBaseFile.structs()
    struct_data += gamedata.empiresdat.EmpiresDat.structs(args.sections)
    struct_data += blendomatic.Blendomatic.structs()
    struct_data += colortable.ColorTable.structs()
    struct_data += texture.Texture.structs()
    struct_data += filelist.SoundList.structs()
    struct_data += stringresource.StringResource.structs()

    data_formatter.add_data(struct_data)
    output_data = data_formatter.export(formats)

    output_filenames = list()
    written_file_count = 0

    for file_name, file_data in output_data.items():
        output_filenames.append(file_name)

        #only generate requested files
        if write_enabled and (args.filename == "*" or any(fnmatch(file_name, file_pattern) for file_pattern in args.filename)):
            dbg("writing %s.." % file_name, 1)
            file_name = file_get_path(file_name, write=True)
            file_write(file_name, file_data)
            written_file_count += 1

    if list_enabled:
        #print the list separated by ';' for the cmake rule generation
        print(";".join(output_filenames), end="")

    else:
        if written_file_count == 0:
            dbg("no source files generated!", 0)
