#!/usr/bin/env python3

from struct import Struct, unpack_from
from util import NamedObject, dbg, file_open, file_get_path


endianness = "< "

class Blendomatic:

	#struct blendomatic_header {
	# unsigned int nr_blending_modes;
	# unsigned int nr_tiles;
	#};
	blendomatic_header = Struct(endianness + "I I")

	#struct blending_mode {
	# unsigned int tile_size;
	# unsigned char tile_flags[31];
	#};
	blending_mode = Struct(endianness + "I 31B")

	def __init__(self, fname):
		self.fname = fname
		dbg("reading blendomatic data from %s" % fname, 1, push="blendomatic")

		fname = file_get_path(fname, write = False)
		f = file_open(fname, binary = True, write = False)

		buf = f.read(Blendomatic.blendomatic_header.size)
		self.header = Blendomatic.blendomatic_header.unpack_from(buf)

		blending_mode_count, tile_count = self.header

		dbg("%d blending modes, each %d tiles" % (blending_mode_count, tile_count))

		self.blending_modes = []

		for i in range(blending_mode_count):
			blending_mode_buf = f.read(Blendomatic.blending_mode.size)

			#size should be 2353 -> 48*49 pixel
			bmode_header = Blendomatic.blending_mode.unpack_from(blending_mode_buf)
			tile_size = bmode_header[0]

			#tile_flags = bmode_header[1:]  #unknown

			dbg("tile in this blending mode %d has %d pixels" % (i, tile_size), 2)

			bmode_tiles = []

			#4*tile_size bytes -> 32 interleaved tiles => 1 bit masks
			alpha_masks_buf = f.read(tile_size * 4)
			alpha_masks = unpack_from("%dB" % (tile_size * 4), alpha_masks_buf)

			for j in range(tile_count):
				tile_buf = f.read(tile_size)

				#get the alpha value pixels, interprete them in isometric tile format
				#  *
				# ***
				#*****
				# ***
				#  *   like this, only bigger.. 97x49 drawpane, 48x49 actually drawn
				pixels = unpack_from("%dB" % tile_size, tile_buf)
				bmode_tiles.append(pixels)

			bmode_data = dict()
			bmode_data["pxcount"] = tile_size
			bmode_data["tiles"]  = bmode_tiles
			bmode_data["alphamasks"] = alpha_masks

			self.blending_modes.append(bmode_data)

		dbg(pop = "blendomatic")


	def __str__(self):
		return str(self.blending_modes)
