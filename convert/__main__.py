#!/usr/bin/python3
import argparse
from colortable import ColorTable
from drs import DRS
from extractionrule import ExtractionRule
from os import system, remove
from os.path import join
from slp import SLP, PNG
from util import file_write, dbg, ifdbg, set_dir, set_verbosity, file_get_path

def main():

	p = argparse.ArgumentParser()
	p.add_argument("-v", "--verbose", help = "Turn on verbose log messages", action='count', default=0)
	p.add_argument("-l", "--info", help = "Show information about the resources", action='store_true')
	p.add_argument("-o", "--destdir", help = "The openage root directory", default='/dev/null')
	p.add_argument("-m", "--merge", help = "Merge frames of slps onto a single texture atlas", action='store_true')

	p.add_argument("srcdir", help = "The Age of Empires II root directory")
	p.add_argument("extract", metavar = "resource", nargs = "*", help = "A specific extraction rule, such as graphics:*.slp, terrain:15008.slp or *:*.wav. If no rules are specified, *:*.* is assumed")

	args = p.parse_args()

	#assume to extract all files when nothing specified.
	if args.extract == []:
		args.extract.append('*:*.*')

	args.extractionrules = [ ExtractionRule(e) for e in args.extract ]

	#write mode is disabled by default, unless destdir is set
	write_enabled = False

	merge_images = args.merge

	#set path in utility class
	print("setting age2 input directory to " + args.srcdir)
	set_dir(args.srcdir, is_writedir=False)

	if args.destdir != '/dev/null':
		print("setting write dir to " + args.destdir)
		set_dir(args.destdir, is_writedir=True)
		write_enabled = True

	if args.info == True:
		print("information mode")
		return

	#set verbose value in util
	set_verbosity(args.verbose)

	drsfiles = {
		"graphics":  DRS("Data/graphics.drs"),
		"interface": DRS("Data/interfac.drs"),
		"gamedata":  DRS("Data/gamedata.drs"),
		"sounds":    DRS("Data/sounds.drs"),
		"terrain":   DRS("Data/terrain.drs")
	}

	if write_enabled:
		palette = ColorTable(drsfiles["interface"].get_file_data('bin', 50500))
		file_write(file_get_path('info/colortable.pal.png', write=True), palette.gen_image())
		file_write(file_get_path('processed/player_color_palette.pal', write=True), palette.gen_player_color_palette())

	files_extracted = 0

	for drsname, drsfile in drsfiles.items():
		for file_extension, file_id in drsfile.files:
			if not any((er.matches(drsname, file_id, file_extension) for er in args.extractionrules)):
				continue

			if write_enabled:
				fbase = file_get_path('raw/' + drsfile.fname + '/' + str(file_id), write=True)
				fname = fbase + '.' + file_extension

				dbg("Extracting to " + fname + "...", 2)

			file_data = drsfile.get_file_data(file_extension, file_id)

			if file_extension == 'slp':
				s = SLP(file_data)

				if write_enabled:

					if merge_images:
						png, (width, height), metadata = s.draw_frames_merged(palette)
						file_write(fname + ".png", png)
						file_write(fname + '.docx', metadata)
						dbg(drsname + ": " + str(file_id) + "." + file_extension + " -> saving packed atlas", 1)

					else:
						for idx, (png, metadata) in enumerate(s.draw_frames(palette)):
							filename = fname + '.' + str(idx)
							file_write(filename + '.png', png.image)
							file_write(filename + '.docx', metadata)

							dbg(drsname + ": " + str(file_id) + "." + file_extension + " -> extracting frame %3d...\r" % (idx), 1, end="")
						dbg("", 1)

			elif file_extension == 'wav':

				if write_enabled:

					file_write(fname, file_data)

					use_opus = True

					if use_opus:
					#opusenc invokation (TODO: ffmpeg?)
						opus_convert_call = 'opusenc "' + fname + '" "' + fbase + '.opus"'
						print("converting... : " + opus_convert_call)
						system(opus_convert_call)

						#remove original wave file
						remove(fname)


			else:
				#this type is unknown or does not require conversion

				if write_enabled:
					file_write(fname, file_data)

			files_extracted += 1

	if write_enabled:
		dbg(str(files_extracted) + " files extracted", 0)

if __name__ == "__main__":
	main()
