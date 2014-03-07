#!/usr/bin/env python3
#
#media files conversion stuff

from colortable import ColorTable
from drs import DRS
from os import remove
from os.path import join
import pprint
from slp import SLP, PNG
from string import Template
import subprocess
import util
from util import file_write, dbg, ifdbg, set_write_dir, set_read_dir, set_verbosity, file_get_path, transform_dump, merge_data_dump


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

	palette = ColorTable(drsfiles["interface"].get_file_data('bin', 50500))

	if write_enabled:
		palette_file_name = file_get_path('processed/player_color_palette.pal', write=True)
		file_write(palette_file_name, palette.gen_player_color_palette())

		import blendomatic
		blend_data = blendomatic.Blendomatic("Data/blendomatic.dat")
		blend_data.export("processed/blendomatic.dat/")

		#create the dump for the dat file
		import gamedata.empiresdat
		datfile = gamedata.empiresdat.EmpiresDat()
		datfile.fill("Data/empires2_x1_p1.dat")

		storeas = ["csv"]

		raw_dump = list()
		raw_dump += datfile.dump(["terrain"])
		raw_dump += blend_data.dump()

		output_content = merge_data_dump(transform_dump(raw_dump, storeas))

		for file_name, file_data in output_content.items():
			dbg("writing %s.." % file_name, 1)
			file_name = file_get_path(file_name, write=True)
			file_write(file_name, file_data)


		if args.extrafiles:
			file_write(file_get_path('info/colortable.pal.png', write=True), palette.gen_image())


	file_list = dict()
	media_files_extracted = 0

	for drsname, drsfile in drsfiles.items():
		for file_extension, file_id in drsfile.files:
			if not any((er.matches(drsname, file_id, file_extension) for er in extraction_rules)):
				continue

			if args.list_files:
				fid = int(file_id)
				if fid not in file_list:
					file_list[fid] = list()

				file_list[fid] += [(drsfile.fname, file_extension)]
				continue

			if write_enabled:
				fbase = file_get_path('processed/' + drsfile.fname + '/' + str(file_id), write=True)
				fname = fbase + '.' + file_extension

				dbg("Extracting to " + fname + "...", 2)

				file_data = drsfile.get_file_data(file_extension, file_id)

			if file_extension == 'slp':

				if write_enabled:
					s = SLP(file_data)
					out_file_tmp = drsname + ": " + str(file_id) + "." + file_extension

					if args.no_merge:
						#create each frame as a separate file
						for idx, (png, metadata) in enumerate(s.draw_frames(palette)):
							filename = fname + '.' + str(idx)
							dbg(out_file_tmp + " -> extracting frame %3d...\r" % (idx), 1, end="")
							file_write(filename + '.png', png.image)
							file_write(filename + '.docx', metadata)

						dbg(out_file_tmp + " -> saved single frame(s)", 1)

					else:
						#create a packed texture atlas
						png, (width, height), metadata = s.draw_frames_merged(palette)
						dbg(out_file_tmp + " -> " + fname + " -> saving atlas", 1)
						file_write(fname + ".png", png)
						file_write(fname + '.docx', metadata)

			elif file_extension == 'wav':

				if write_enabled:
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

				if write_enabled:
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
