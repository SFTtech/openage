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

	storeas = ["struct", "cfile"]

	struct_data = list()
	struct_data += gamedata.empiresdat.EmpiresDat.structs(args.sections)
	struct_data += blendomatic.Blendomatic.structs()
	struct_data += colortable.ColorTable.structs()
	struct_data += texture.Texture.structs()
	struct_data += filelist.SoundList.structs()
	struct_data += stringresource.StringResource.structs()

	output_data = dataformat.metadata_format(struct_data, storeas)
	output_data = dataformat.merge_data_dump(output_data)

	output_filenames = list()
	written_file_count = 0

	for file_name, file_data in output_data.items():
		output_filenames.append(file_name)

		if write_enabled:
			#only generate requested files
			write_file = False

			if args.filename == "*":
				write_file = True

			if any((fnmatch(file_name, file_pattern) for file_pattern in args.filename)):
				write_file = True

			if write_file:
				#write dat shit
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
