#!/usr/bin/env python3
#
#handler routines for creating and converting data files

from util import dbg, set_write_dir, store_data_dump

def data_generate(args):

	#write mode is disabled by default, unless output is set
	if args.output != None:
		dbg("setting write dir to " + args.output, 1)
		set_write_dir(args.output)
		write_enabled = True
	else:
		write_enabled = False

	if write_enabled:

		import gamedata.empiresdat
		datfile_structs = gamedata.empiresdat.EmpiresDat.structs(["terrain"])
		store_data_dump(datfile_structs, ["struct", "cfile"])

		import blendomatic
		blendomatic_structs = blendomatic.Blendomatic.structs()
		store_data_dump(blendomatic_structs, ["struct", "cfile"])
	else:
		raise NotImplementedError("file listing")
