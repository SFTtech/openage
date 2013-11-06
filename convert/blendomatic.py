#!/usr/bin/env python3

import math
from struct import Struct, unpack_from
from util import NamedObject, dbg, file_open, file_get_path
from png import PNG

endianness = "< "

class Blendomatic:

	#struct blendomatic_header {
	# unsigned int nr_blending_modes;
	# unsigned int nr_tiles;
	#};
	blendomatic_header = Struct(endianness + "I I")


	def __init__(self, fname):
		self.fname = fname
		dbg("reading blendomatic data from %s" % fname, 1, push="blendomatic")

		fname = file_get_path(fname, write = False)
		f = file_open(fname, binary = True, write = False)

		buf = f.read(Blendomatic.blendomatic_header.size)
		self.header = Blendomatic.blendomatic_header.unpack_from(buf)

		blending_mode_count, tile_count = self.header

		dbg("%d blending modes, each %d tiles" % (blending_mode_count, tile_count), 2)

		self.blending_modes = []

		for i in range(blending_mode_count):

			blending_mode = Struct(endianness + "I %dB" % (tile_count))

			blending_mode_buf = f.read(blending_mode.size)

			bmode_header = blending_mode.unpack_from(blending_mode_buf)
			#should be 2353 -> number of pixels (single alpha byte values)
			tile_size = bmode_header[0]

			#tile_flags = bmode_header[1:]  #TODO

			dbg("tile in this blending mode %d has %d pixels" % (i, tile_size), 2)

			bmode_tiles = []

			#TODO
			#4*tile_size bytes -> 32 interleaved tiles => 1 bit masks
			dbg("reading 1bit masks -> %d bytes" % (tile_size * 4), 2)
			alpha_masks_buf = f.read(tile_size * 4)
			alpha_masks = unpack_from("%dB" % (tile_size * 4), alpha_masks_buf)


			#as we draw in isometric tile format, this is the row count
			row_count = int(math.sqrt(tile_size)) + 1  #49
			half_row_count = int(row_count/2)

			#draw mask tiles for this blending mode
			for j in range(tile_count):
				tile_buf = f.read(tile_size)

				dbg("tile has %d alpha value bytes" % (tile_size), 3)

				#get the alpha value pixels, interprete them in isometric tile format
				#    *
				#  *****
				#*********
				#  *****
				#    *    like this, only bigger..
				#
				# we end up drawing the rhombus with 49 rows,
				# and a dynamic space to the left of the data.

				read_so_far = 0
				max_width = 0
				tilerows = []

				for y in range(row_count):

					if y < half_row_count:
						#upper half of the tile
						#row i+1 has 4 more pixels than row i
						# +1 for the middle one
						read_values = 1 + (4 * y)
					else:
						#lower half of tile
						read_values = ((row_count*2) - 1) - (4 * (y - half_row_count))

					#how many empty pixels on the left
					#before the real data begins
					space_count = row_count - 1 - int(read_values/2)

					#insert as padding to the left (0 for fully transparent)
					space_left = (0,) * space_count


					if read_values > (tile_size - read_so_far):
						raise Exception("reading more bytes than tile has left")

					if read_values < 0:
						raise Exception("wanted to read a negative amount of bytes: %d" % read_values)

					#read the real pixel data here
					pixels = unpack_from("%dB" % read_values, tile_buf, read_so_far)
					pixels = space_left + pixels
					read_so_far = read_so_far + read_values

					if len(pixels) > max_width:
						max_width = len(pixels)

					#dbg("extracted %d values, %d so far, %d left" % (read_values, read_so_far, (tile_size-read_so_far)), 4)

					tilerows.append(pixels)

				tiledata = {
					"data": tilerows,
					"height": row_count,
					"width": max_width
				}
				bmode_tiles.append(tiledata)
				#print(str(tilerows))
				#break

			bmode_data = dict()
			bmode_data["pxcount"] = tile_size
			bmode_data["tiles"] = bmode_tiles
			bmode_data["alphamasks"] = alpha_masks

			self.blending_modes.append(bmode_data)

		dbg(pop = "blendomatic")


	def draw_frames(self):

		for idx, bmode in enumerate(self.blending_modes):

			for tidx, tile in enumerate(bmode["tiles"]):
				png = PNG(0, None, tile["data"])
				png.create(tile["width"], tile["height"], True)

				yield png, idx, tidx


	def __str__(self):
		return str(self.blending_modes)
