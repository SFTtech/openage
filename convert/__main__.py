#!/usr/bin/python3
from util import file_write, dbg, args
from drs import DRS
from slp import SLP, PNG
from colortable import ColorTable
import argparse

def main():

	p = argparse.ArgumentParser()
	p.add_argument("srcdir", help = "The Age of Empires II root directory")
	p.add_argument("destdir", help = "The openage root directory")
	p.add_argument("extract", nargs = "*", help = "A specific extraction rule, such as graphics:*.slp, or *:*.wav. If no rules are specified, *:*.* is assumed")
	p.add_argument("-v", "--verbose", help = "Turn on verbose log messages", action='count', default=0)

	args = p.parse_args()

	#assume to extract all files when nothing specified.
	if args.extract == []:
		args.extract.append('*:*.*')

	args.extractionrules = [ ExtractionRule(e) for e in args.extract ]


	drsfiles = {
		"graphics": DRS("Data/graphics.drs"),
		"interface": DRS("Data/interfac.drs"),
		"gamedata": DRS("Data/gamedata.drs"),
		"sounds": DRS("Data/sounds.drs"),
		"terrain": DRS("Data/terrain.drs")
	}

	palette = ColorTable(drsfiles["interface"].get_file_data('bin', 50500))
	file_write('info/colortable.pal.png', palette.gen_image())
	file_write('processed/player_color_palette.pal', palette.gen_player_color_palette())

	files_extracted = 0

	for drsname, drsfile in drsfiles.items():
		for file_extension, file_id in drsfile.files:
			if not any((er.matches(drsname, file_id, file_extension) for er in args.extractionrules)):
				continue

			fname = 'raw/' + drsfile.fname + '/' + str(file_id) + '.' + file_extension
			dbg("Extracting " + fname, 2)

			file_data = drsfile.get_file_data(file_extension, file_id)

			if file_extension == 'slp':
				s = SLP(file_data)


				#TODO: intelligent merging of all the frames to one texture plane
				for idx, (png, (width, height), (hotspot_x, hotspot_y)) in enumerate(s.draw_frames(palette)):
					filename = fname + '.' + str(idx)
					file_write(filename + '.png', png.image)

					#x,y of lower left origin
					#width and height of the subtexture
					tx = 0
					ty = 0
					tw = width
					th = height

					#metadata writing
					meta_out = "#texture meta information: subtexid=x,y,w,h,hotspot_x,hotspot_y\n"
					meta_out = meta_out + "n=1\n"
					meta_out = meta_out + "%d=" % idx
					meta_out = meta_out + "%d,%d,%d,%d," % (tx, ty, tw, th)
					meta_out = meta_out + "%d,%d\n" % (hotspot_x, hotspot_y)

					file_write(filename + '.docx', meta_out)
			else:
				#this type is unknown or does not require conversion
				file_write(fname, file_data)

			files_extracted += 1

	dbg(str(files_extracted) + " files extracted", 0)

if __name__ == "__main__":
	main()
