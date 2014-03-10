import math
from PIL import Image, ImageDraw

import util
from util import dbg

class ColorTable:
	name_struct        = "palette_color"
	name_struct_file   = "color"
	struct_description = "indexed color storage."

	data_format = {
		0: {"idx": "int32_t"},
		1: {"r":   "uint8_t"},
		2: {"g":   "uint8_t"},
		3: {"b":   "uint8_t"},
		4: {"a":   "uint8_t"},
	}

	def __init__(self):
		pass

	def fill(self, data, file_id):
		self.file_id = file_id
		self.data_table_name = "palette_%d" % self.file_id

		#split all lines of the input data
		lines = data.decode('ascii').split('\r\n') #windows windows windows baby

		self.header = lines[0]
		self.version = lines[1]

		# check for palette header
		if self.header != "JASC-PAL":
			raise Exception("No palette header 'JASC-PAL' found, instead: %r" % self.header)
		if self.version != "0100":
			raise Exception("palette version mispatch, got %s" % self.version)

		self.entry_count = int(lines[2])

		self.palette = []

		#data entries are line 3 to n
		for i in range(3, self.entry_count + 3):
			#one entry looks like "13 37 42", where 13 is the red value, 37 green and 42 blue.
			self.palette.append(tuple(map(int, lines[i].split(' '))))

	def __getitem__(self, index):
		return self.palette[index]

	def __len__(self):
		return len(self.palette)

	def __repr__(self):
		return "color palette: %d entries." % self.entry_count

	def __str__(self):
		return repr(self) + "\n" + str(self.palette)

	def gen_image(self, draw_text = True, squaresize = 100):
		#writes this color table (palette) to a png image.
		imgside_length = math.ceil(math.sqrt(self.entry_count))
		imgsize = imgside_length * squaresize

		dbg("generating palette image with size %dx%d" % (imgsize, imgsize))

		palette_image = Image.new('RGBA', (imgsize, imgsize), (255, 255, 255, 0))
		draw = ImageDraw.ImageDraw(palette_image)

		text_padlength = len(str(self.entry_count)) #dirty, i know.
		text_format = "%0" + str(text_padlength) + "d"

		drawn = 0

		#squaresize 1 means draw single pixels
		if squaresize == 1:
			for y in range(imgside_length):
					for x in range(imgside_length):
						if drawn < self.entry_count:
							r,g,b = self.palette[drawn]
							draw.point((x, y), fill=(r, g, b, 255))
							drawn = drawn + 1

		#draw nice squares with given side length
		elif squaresize > 1:
			for y in range(imgside_length):
					for x in range(imgside_length):
						if drawn < self.entry_count:
							sx = x * squaresize - 1
							sy = y * squaresize - 1
							ex = sx + squaresize - 1
							ey = sy + squaresize
							r,g,b = self.palette[drawn]
							vertices = [(sx, sy), (ex, sy), (ex, ey), (sx, ey)] #(begin top-left, go clockwise)
							draw.polygon(vertices, fill=(r, g, b, 255))

							if draw_text and squaresize > 40:
								#draw the color id

								ctext = text_format % drawn #insert current color id into string
								tcolor = (255-r, 255-b, 255-g, 255)

								#draw the text  #TODO: use customsized font
								draw.text((sx+3, sy+1),ctext,fill=tcolor,font=None)

							drawn = drawn + 1


		else:
			raise Exception("fak u, no negative values for the squaresize pls.")

		return palette_image

	def dump(self):
		ret = dict()

		ret.update(util.gather_format(self))
		ret["name_table_file"] = self.data_table_name
		ret["data"] = list()

		#dump all color entries
		for idx, entry in enumerate(self.palette):
			color_entry = {
				"idx": idx,
				"r":   entry[0],
				"g":   entry[1],
				"b":   entry[2],
				"a":   255,
			}
			ret["data"].append(color_entry)

		return [ ret ]

	def structs():
		ret = dict()
		ret.update(util.gather_format(ColorTable))
		return [ ret ]



class PlayerColorTable(ColorTable):
	"""
	this class represents stock player color values.

	each player has 8 subcolors, where 0 is the darkest and 7 is the lightest
	"""

	def __init__(self, base_table):
		super().__init__()

		if not isinstance(base_table, ColorTable):
			raise Exception("can only create a player color table from a regular color table.")

		self.file_id = base_table.file_id
		self.data_table_name = "player_palette_%d" % self.file_id
		self.header  = base_table.header
		self.version = base_table.version
		self.palette = list()
		#now, strip the base table entries to the player colors.

		#entry id = ((playernum-1) * 8) + subcolor

		players = range(1, 9)
		psubcolors = range(8)

		for i in players:
			for subcol in psubcolors:
				r, g, b = base_table[16 * i + subcol]
				self.palette.append((r, g, b))
