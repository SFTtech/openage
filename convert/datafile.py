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
		datfile = gamedata.empiresdat.EmpiresDat(None)

		store_data_dump(datfile.dump(["terrain"]), ["struct", "cfile"])
		store_data_dump(blend_data.dump(), ["struct", "cfile"])
	else:
		raise NotImplementedError("file listing")
