#!/usr/bin/env python3

import math
from png import PNG
from struct import Struct, unpack_from
from util import NamedObject, dbg, file_open, file_get_path, merge_frames, generate_meta_text, file_write
import os.path

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

			#as we draw in isometric tile format, this is the row count
			row_count = int(math.sqrt(tile_size)) + 1  #49


			#alpha_masks_raw is an array of bytes that will draw 32 images,
			#which are bit masks.
			#
			#one of these masks also has 2353 pixels (like terraintile or alpha mask)
			#the storage of the bit masks is 4*tilesize, here's why:
			#
			#4 * 8bit * 2353 pixels = 75296 bitpixels
			#==> 75296/(32 images) = 2353 bitpixel/image
			#
			#this means if we interprete the 75296 bitpixels as 32 images,
			#each of these images gets 2353 bit as data.
			#TODO: why 32 images? isn't that depending on tile_count?

			bitmask_buf_size = tile_size * 4
			dbg("reading 1bit masks -> %d bytes" % (bitmask_buf_size), 2)
			alpha_masks_buf = f.read(bitmask_buf_size)
			alpha_masks_raw = unpack_from("%dB" % (bitmask_buf_size), alpha_masks_buf)


			#list of alpha-mask tiles
			bmode_tiles = []

			dbg("reading 8bit masks for %d tiles -> %d bytes" % (tile_count, tile_size * tile_count), 2)

			#draw mask tiles for this blending mode
			for j in range(tile_count):
				tile_buf = f.read(tile_size)
				pixels = unpack_from("%dB" % tile_size, tile_buf)

				tile = self.get_tile_from_data(row_count, pixels)

				bmode_tiles.append(tile)


			bitvalues = []
			for i in alpha_masks_raw:
				for b_id in range(7, -1, -1):
					#bitmask from 0b00000001 to 0b10000000
					bit_mask = 2 ** b_id
					bit = i & bit_mask
					bitvalues.append(bit)

			#list of bit-mask tiles
			bmode_bittiles = []

			#TODO: again, is 32 really hardcoded?
			for i in range(32):
				data_begin =  i    * tile_size
				data_end   = (i+1) * tile_size
				pixels = bitvalues[ data_begin : data_end ]

				tile = self.get_tile_from_data(row_count, pixels)

				bmode_bittiles.append(tile)


			bmode_data = dict()
			bmode_data["pxcount"] = tile_size
			bmode_data["alphamasks"] = bmode_tiles
			bmode_data["bitmasks"] = bmode_bittiles

			self.blending_modes.append(bmode_data)

		dbg(pop = "blendomatic")


	#get the data pixels, interprete them in isometric tile format
	#    *
	#  *****
	#*********
	#  *****
	#    *    like this, only bigger..
	#
	# we end up drawing the rhombus with 49 rows,
	# and a dynamic space to the left of the data.
	def get_tile_from_data(self, row_count, data):
		half_row_count = int(row_count/2)
		tile_size = len(data)

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
				read_values = ((row_count * 2) - 1) - (4 * (y - half_row_count))

			#how many empty pixels on the left
			#before the real data begins
			space_count = row_count - 1 - int(read_values/2)

			#insert as padding to the left (0 for fully transparent)
			space_left = [-1] * space_count

			if read_values > (tile_size - read_so_far):
				raise Exception("reading more bytes than tile has left")

			if read_values < 0:
				raise Exception("wanted to read a negative amount of bytes: %d" % read_values)

			pdata_start = read_so_far
			pdata_end   = read_so_far + read_values

			pixels = list(data[ pdata_start : pdata_end ])
			pixels = space_left + pixels

			read_so_far = read_so_far + read_values

			if len(pixels) > max_width:
				max_width = len(pixels)

			#dbg("extracted %d values, %d so far, %d left" % (read_values, read_so_far, (tile_size-read_so_far)), 4)

			tilerows.append(pixels)

		if read_so_far != tile_size:
			raise Exception("got leftover bytes: %d" % (tile_size-read_so_far))

		tiledata = {
			"data": tilerows,
			"height": row_count,
			"width": max_width
		}
		return tiledata

	def draw_alpha_frames(self):
		for idx, bmode in enumerate(self.blending_modes):

			for tidx, tile in enumerate(bmode["alphamasks"]):
				png = PNG(0, None, tile["data"])
				png.create(tile["width"], tile["height"], True)

				yield png, idx, tidx

	def draw_alpha_frames_merged(self):
		for idx, bmode in enumerate(self.blending_modes):

			max_w = 0
			max_h = 0

			blendomatic_frames = []

			for tile in bmode["alphamasks"]:
				frame = PNG(0, None, tile["data"])
				tw = tile["width"]
				th = tile["height"]
				frame.create(tw, th, True)

				if tw > max_w:
					max_w = tw
				if th > max_h:
					max_h = th

				blendomatic_frames.append((frame, None, (0, 0)))

			atlas, atlas_meta, (width, height) = merge_frames(blendomatic_frames, max_w, max_h)
			meta_out = generate_meta_text(atlas_meta)

			yield idx, atlas, (width, height), meta_out


	def draw_bit_frames(self):
		for idx, bmode in enumerate(self.blending_modes):

			for tidx, tile in enumerate(bmode["bitmasks"]):
				png = PNG(0, None, tile["data"])
				png.create(tile["width"], tile["height"], True)

				yield png, idx, tidx

	def dump(self):
		ret = dict()

		ret["name_table_file"]  = "blending_modes"
		ret["name_struct_file"] = "blending_mode"
		ret["name_struct"]      = "blending_mode"
		ret["format"]           = {
			0: {"blend_mode": "int32_t"},
		}
		ret["data"] = list()

		for mode in range(len(self.blending_modes)):
			#dump terrains
			ret["data"].append({"blend_mode": mode})

		return [ ret ]

	def export(self, output_folder):
		for (modeidx, png, size, metadata) in self.draw_alpha_frames_merged():
			fname = os.path.join(output_folder, "mode%02d" % modeidx)
			filename = file_get_path(fname, write=True)
			dbg("saving blending mode%02d texture -> %s.png" % (modeidx, filename), 1)
			file_write(filename + ".png", png)
			file_write(filename + ".docx", metadata)
		dbg("blending modes exported successfully!", 1)

	def __str__(self):
		return str(self.blending_modes)
