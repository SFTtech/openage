#!/usr/bin/python3
import argparse
from colortable import ColorTable
from drs import DRS
from extractionrule import ExtractionRule
from os import remove
from os.path import join
import pprint
from slp import SLP, PNG
import subprocess
import util
from util import file_write, dbg, ifdbg, set_write_dir, set_read_dir, set_verbosity, file_get_path


p = argparse.ArgumentParser()
p.add_argument("-v", "--verbose", help = "Turn on verbose log messages", action='count', default=0)
p.add_argument("-l", "--listfiles", help = "List files in the DRS archives matching 'resource', or all", action='store_true')
p.add_argument("--dumpfilelist", help = "Return python representation of all found files, useful for existence testing.", action='store_true')

p.add_argument("-e", "--extrafiles", help = "Extract extra files that are not needed, but useful (mainly visualizations).", action='store_true')
p.add_argument("-d", "--development", help = "Execute extracting routines being in development.", action='store_true')
p.add_argument("-o", "--destdir", help = "The openage root directory", default='/dev/null')
p.add_argument("-s", "--nomerge", help = "Don't merge frames of slps onto a texture atlas, create single files instead", action='store_true')

p.add_argument("srcdir", help = "The Age of Empires II root directory")
p.add_argument("extract", metavar = "resource", nargs = "*", help = "A specific extraction rule, such as graphics:*.slp, terrain:15008.slp or *:*.wav. If no rules are specified, *:*.* is assumed")


def main(args):

	#set verbose value in util
	set_verbosity(args.verbose)

	#assume to extract all files when nothing specified.
	if args.extract == []:
		args.extract.append('*:*.*')

	extraction_rules = [ ExtractionRule(e) for e in args.extract ]

	merge_images = not args.nomerge
	exec_dev = args.development

	#set path in utility class
	dbg("setting age2 input directory to " + args.srcdir, 1)
	set_read_dir(args.srcdir)

	#write mode is disabled by default, unless destdir is set
	if args.destdir != '/dev/null' and not args.listfiles and not args.dumpfilelist:
		dbg("setting write dir to " + args.destdir, 1)
		set_write_dir(args.destdir)
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

	if exec_dev:
		if write_enabled:
			print("running script dev functions...")

			convert_datfile()

			print("finished dev functions")
			return
		else:
			raise Exception("development mode requires write access")

	if write_enabled:
		file_write(file_get_path('processed/player_color_palette.pal', write=True), palette.gen_player_color_palette())

		import blendomatic
		blend_data = blendomatic.Blendomatic("Data/blendomatic.dat")

		for (modeidx, png, size, metadata) in blend_data.draw_alpha_frames_merged():
			fname = 'processed/blendomatic.dat/mode%02d' % (modeidx)
			filename = file_get_path(fname, write=True)
			file_write(filename + ".png", png)
			file_write(filename + ".docx", metadata)
			dbg("blending mode%02d -> saved packed atlas" % (modeidx), 1)

		convert_datfile()

		if args.extrafiles:
			file_write(file_get_path('info/colortable.pal.png', write=True), palette.gen_image())


	file_list = dict()
	files_extracted = 0

	for drsname, drsfile in drsfiles.items():
		for file_extension, file_id in drsfile.files:
			if not any((er.matches(drsname, file_id, file_extension) for er in extraction_rules)):
				continue

			if args.listfiles or args.dumpfilelist:
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

	if args.listfiles or args.dumpfilelist:
		#file_list = sorted(file_list)
		if not args.dumpfilelist:
			for idx, f in file_list.items():
				ret = "%d = [ " % idx
				for file_name, file_extension in f:
					ret += "%s/%d.%s, " % (file_name, idx, file_extension)
				ret += "]"
				print(ret)
		else:
			ret = "#!/usr/bin/python\n\n#auto generated age2tc file list\n\n"
			import pprint
			ret += "avail_files = "
			ret += pprint.pformat(file_list)
			print(ret)



def convert_datfile():
	import gamedata.empiresdat

	datfile = gamedata.empiresdat.EmpiresDat("Data/empires2_x1_p1.dat")

	#import code
	#console = code.InteractiveConsole(locals())
	#console.interact("'datfile' is the data file object.")

	dbg("dumping data stuff:")

	#key: output mode, value: folder where to place files
	output_advices = {
		"csv":    "processed",
		"struct": "structs",
	}

	data_dump = datfile.dump(["terrain"])

	for mode, folder in output_advices.items():
		formatted_data = util.format_data(mode, data_dump)

		for output_name, output_data in formatted_data.items():
			filename = file_get_path("%s/%s" % (folder, output_name), write=True)
			file_write(filename, output_data)

	return
	#old, crappy, hardcoded, legacy meta storage
	#it will be pwnd very soon. i promise.

	tmeta = "#terrain specification\n"
	tmeta += "#idx=terrain_id, slp_id, sound_id, blend_mode, blend_priority, angle_count, frame_count, terrain_dimensions0, terrain_dimensions1, terrain_replacement_id, name0, name1\n"

	tmeta += "n=%d\n" % len(datfile.terrain.data.terrains)

	i = 0
	blending_modes = set()
	for tk in datfile.terrain.data.terrains:
		if tk.slp_id < 0:
			continue

		blending_modes.add(tk.blend_mode)

		wanted = ["terrain_id", "slp_id", "sound_id", "blend_mode", "blend_priority", "angle_count", "frame_count", "terrain_dimensions0", "terrain_dimensions1", "terrain_replacement_id", "name0", "name1"]

		line = [getattr(tk, w) for w in wanted]

		#as blending mode 0==1 and 7==8, and ice is 5 for sure,
		#we subtract one from the ids, and can map -1 to 0, as mode (0-1) == (1-1)
		#TODO: this can't be correct...
		line[3] -= 1
		if line[3] < 0:
			line[3] = 0

		line = map(str, line)
		tmeta += ("%d=" % i) + ",".join(line) + "\n"
		i += 1

	file_write(filename, tmeta)


	filename = file_get_path("processed/blending_meta.docx", write=True)

	bmeta = "#blending mode specification\n"
	bmeta += "#yeah, i know that this content is totally stupid, but that's how the data can be injected later\n"
	bmeta += "#idx=mode_id\n"

	bmeta += "n=%d\n" % len(blending_modes)

	i = 0
	for m in blending_modes:
		bmeta += "%d=%d\n" % (i, m)
		i += 1

	file_write(filename, bmeta)




if __name__ == "__main__":
	args = p.parse_args()

	main(args)
