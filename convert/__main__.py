#!/usr/bin/python3
from util import file_write, dbg, args
from drs import DRS
from slp import SLP, PNG
from colortable import ColorTable

def main():
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
				for idx, (image, (width, height), (hotspot_x, hotspot_y)) in enumerate(s.draw_frames(palette)):
					file_write(fname + '.' + str(idx) + '.png', image)
					file_write(fname + '.' + str(idx) + '.txt', "#hotspot x y\n" + str(hotspot_x) + " " + str(hotspot_y) + "\n")
			else:
				#this type is unknown or does not require conversion
				file_write(fname, file_data)

			files_extracted += 1

	dbg(str(files_extracted) + " files extracted", 0)

if __name__ == "__main__":
	main()
