#!/usr/bin/python3
import argparse
from colortable import ColorTable
from drs import DRS
from extractionrule import ExtractionRule
from os import remove
from os.path import join
from slp import SLP, PNG
import subprocess
from util import file_write, dbg, ifdbg, set_dir, set_verbosity, file_get_path

def main():

	p = argparse.ArgumentParser()
	p.add_argument("-v", "--verbose", help = "Turn on verbose log messages", action='count', default=0)
	p.add_argument("-l", "--listfiles", help = "List files in the DRS archives matching 'resource', or all", action='store_true')
	p.add_argument("-e", "--extrafiles", help = "Extract extra files that are not needed, but useful (mainly visualizations).", action='store_true')
	p.add_argument("-d", "--development", help = "Execute extracting routines being in development.", action='store_true')
	p.add_argument("-o", "--destdir", help = "The openage root directory", default='/dev/null')
	p.add_argument("-s", "--nomerge", help = "Don't merge frames of slps onto a texture atlas, create single files instead", action='store_true')

	p.add_argument("srcdir", help = "The Age of Empires II root directory")
	p.add_argument("extract", metavar = "resource", nargs = "*", help = "A specific extraction rule, such as graphics:*.slp, terrain:15008.slp or *:*.wav. If no rules are specified, *:*.* is assumed")

	args = p.parse_args()

	#assume to extract all files when nothing specified.
	if args.extract == []:
		args.extract.append('*:*.*')

	args.extractionrules = [ ExtractionRule(e) for e in args.extract ]

	merge_images = not args.nomerge
	exec_dev = args.development

	#set path in utility class
	print("setting age2 input directory to " + args.srcdir)
	set_dir(args.srcdir, is_writedir=False)

	#write mode is disabled by default, unless destdir is set
	if args.destdir != '/dev/null' and not args.listfiles:
		print("setting write dir to " + args.destdir)
		set_dir(args.destdir, is_writedir=True)
		write_enabled = True
	else:
		write_enabled = False


	#set verbose value in util
	set_verbosity(args.verbose)

	drsfiles = {
		"graphics":  DRS("Data/graphics.drs"),
		"interface": DRS("Data/interfac.drs"),
		"gamedata":  DRS("Data/gamedata.drs"),
		"sounds":    DRS("Data/sounds.drs"),
		"terrain":   DRS("Data/terrain.drs")
	}

	palette = ColorTable(drsfiles["interface"].get_file_data('bin', 50500))

	if write_enabled:

		if exec_dev:
			import gamedata.empiresdat
			gamedata.empiresdat.test("Data/empires2_x1_p1.dat")
			return


		file_write(file_get_path('processed/player_color_palette.pal', write=True), palette.gen_player_color_palette())

		import blendomatic
		blend_data = blendomatic.Blendomatic("Data/blendomatic.dat")

		for (modeidx, png, size, metadata) in blend_data.draw_alpha_frames_merged():
			fname = 'alphamask/mode%02d' % (modeidx)
			filename = file_get_path(fname, write=True)
			file_write(filename + ".png", png)
			file_write(filename + ".docx", metadata)
			dbg("blending mode%02d -> saved packed atlas" % (modeidx), 1)

		if args.extrafiles:
			file_write(file_get_path('info/colortable.pal.png', write=True), palette.gen_image())
	else:
		if not exec_dev:
			raise Exception("development mode requires write access")


	files_extracted = 0

	for drsname, drsfile in drsfiles.items():
		for file_extension, file_id in drsfile.files:
			if not any((er.matches(drsname, file_id, file_extension) for er in args.extractionrules)):
				continue

			if args.listfiles:
				print("%s:%s.%s" % (drsfile.fname, file_id, file_extension))
				continue

			if write_enabled:
				fbase = file_get_path('raw/' + drsfile.fname + '/' + str(file_id), write=True)
				fname = fbase + '.' + file_extension

				dbg("Extracting to " + fname + "...", 2)

			file_data = drsfile.get_file_data(file_extension, file_id)

			if file_extension == 'slp':
				s = SLP(file_data)

				if write_enabled:

					out_file_tmp = drsname + ": " + str(file_id) + "." + file_extension

					if merge_images:
						png, (width, height), metadata = s.draw_frames_merged(palette)
						file_write(fname + ".png", png)
						file_write(fname + '.docx', metadata)
						dbg(out_file_tmp + " -> saved packed atlas", 1)

					else:
						for idx, (png, metadata) in enumerate(s.draw_frames(palette)):
							filename = fname + '.' + str(idx)
							file_write(filename + '.png', png.image)
							file_write(filename + '.docx', metadata)

							dbg(out_file_tmp + " -> extracting frame %3d...\r" % (idx), 1, end="")
						dbg(out_file_tmp + " -> saved single frame(s)", 1)

			elif file_extension == 'wav':

				if write_enabled:

					file_write(fname, file_data)

					use_opus = True

					if use_opus:
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

			files_extracted += 1

	if write_enabled:
		dbg(str(files_extracted) + " files extracted", 0)

if __name__ == "__main__":
	main()
