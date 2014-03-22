#!/usr/bin/env python3
#
#media files conversion stuff

from colortable import ColorTable, PlayerColorTable
import dataformat
from drs import DRS
from os import remove
import os.path
from string import Template
import subprocess
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
		if self.drsname != drsname and not self.drsname == None:
			return False
		if self.fno != fno and not self.fno == None:
			return False
		if self.fext != fext and not self.fext == None:
			return False

		return True


def media_convert(args):
	#assume to extract all files when nothing specified.
	if args.extract == []:
		args.extract.append('*:*.*')

	extraction_rules = [ ExtractionRule(e) for e in args.extract ]

	#set path in utility class
	dbg("setting age2 input directory to " + args.srcdir, 1)
	set_read_dir(args.srcdir)

	#write mode is disabled by default, unless destdir is set
	if args.output != None:
		dbg("setting write dir to " + args.output, 1)
		set_write_dir(args.output)
		write_enabled = True
	else:
		write_enabled = False

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

	palette_id = 50500
	palette = ColorTable(drsfiles["interface"].get_file_data('bin', palette_id), palette_id)

	#metadata dumping output format, more to come?
	storeas = ["csv"]

	if write_enabled:
		from slp import SLP

		player_palette = PlayerColorTable(palette)

		import blendomatic
		blend_data = blendomatic.Blendomatic("Data/blendomatic.dat")
		blend_data.save(os.path.join(asset_folder, "blendomatic.dat/"), storeas)

		#create the dump for the dat file
		import gamedata.empiresdat
		datfile = gamedata.empiresdat.EmpiresDat("Data/empires2_x1_p1.dat")

		#modify the read contents of datfile
		import fix_data
		datfile = fix_data.fix_data(datfile)

		#dump metadata information
		meta_dump = list()
		meta_dump += datfile.dump(args.sections)
		meta_dump += blend_data.metadata()
		meta_dump += player_palette.metadata()

		#create metadata content from the collected dumps
		metadata = dataformat.merge_data_dump(dataformat.metadata_format(meta_dump, storeas))

		#save the meta files
		util.file_write_multi(metadata, file_prefix=asset_folder)

		if args.extrafiles:
			datfile.raw_dump('raw/empires2x1p1.raw')
			palette.save_visualization('info/colortable.pal.png')

	file_list = dict()
	media_files_extracted = 0

	for drsname, drsfile in drsfiles.items():
		for file_extension, file_id in drsfile.files:
			if not any((er.matches(drsname, file_id, file_extension) for er in extraction_rules)):
				continue

			#append this file to the list result
			if args.list_files:
				fid = int(file_id)
				if fid not in file_list:
					file_list[fid] = list()

				file_list[fid] += [(drsfile.fname, file_extension)]
				continue

			#generate output filename where data will be stored in
			if write_enabled:
				fname = os.path.join(asset_folder, drsfile.fname, "%d.%s" % (file_id, file_extension))
				dbg("Extracting to " + fname + "...", 2)
				file_data = drsfile.get_file_data(file_extension, file_id)
			else:
				continue

			if file_extension == 'slp':
				s = SLP(file_data)
				out_file_tmp = "%s: %d.%s" % (drsname, file_id, file_extension)

				dbg(out_file_tmp + " -> " + fname + " -> generating atlas", 1)

				#create exportable texture from the slp
				texture = s.get_texture(palette)

				#save the image and the corresponding metadata file
				texture.save(fname, storeas)

			elif file_extension == 'wav':
				file_write(fname, file_data)

				if not args.no_opus:
					#opusenc invokation (TODO: ffmpeg?)
					opus_convert_call = ['opusenc', fname, fbase + '.opus']
					dbg("converting... : " + fname + " to opus.", 1)

					oc = subprocess.Popen(opus_convert_call, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
					oc_out, oc_err = oc.communicate()

					if ifdbg(2):
						oc_out = oc_out.decode("utf-8")
						oc_err = oc_err.decode("utf-8")

						dbg(oc_out + "\n" + oc_err, 2)

					#remove original wave file
					remove(fname)


			else:
				#this type is unknown or does not require conversion
				file_write(fname, file_data)

			media_files_extracted += 1

	if write_enabled:
		dbg("media files extracted: %d" % (media_files_extracted), 0)

	if args.list_files:
		for idx, f in file_list.items():
			ret = "%d = [ " % idx
			for file_name, file_extension in f:
				ret += "%s/%d.%s, " % (file_name, idx, file_extension)
			ret += "]"
			print(ret)
