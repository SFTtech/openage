import math
from PIL import Image, ImageDraw

from util import dbg

class ColorTable():
	def __init__(self, data):
		lines = data.decode('ascii').split('\r\n') #windows windows windows baby

		self.header = lines[0]
		self.version = lines[1]

		# check for palette header
		if self.header != "JASC-PAL":
			raise Exception("No palette header 'JASC-PAL' found, instead: %r" % lines[0])
		if self.version != "0100":
			raise Exception("palette version mispatch, got %s" % lines[1])

		self.num_entries = int(lines[2])

		self.palette = []

		for i in range(self.num_entries):
			#one entry looks like "13 37 42", where 13 is the red value, 37 green and 42 blue.
			self.palette.append(tuple(map(int, lines[i + 3].split(' '))))

	def __getitem__(self, index):
		return self.palette[index]

	def __repr__(self):
		return "color palette: %d entries." % self.num_entries

	def __str__(self):
		return repr(self) + "\n" + str(self.palette)

	def gen_player_color_palette(self):
		#exports the player colors of the game palette
		#each player has 8 subcolors, where 0 is the darkest and 7 is the lightest
		result = "#aoe player color palette\n"
		result += "#entry id = ((playernum-1) * 8) + subcolor\n"
		i = 0

		players = range(1, 9);
		psubcolors = range(8);

		numpcolors = len(players) * len(psubcolors)
		result += "n=%d\n" % numpcolors

		for i in players:
			for subcol in psubcolors:
				r, g, b = self[16 * i + subcol] #player i subcolor
				cid = (i - 1) * len(players) + subcol  #each entry has this index
				result += "%d=%d,%d,%d,255\n" % (cid, r, g, b)

		return result

	def gen_image(self, draw_text = True, squaresize = 100):
		#writes this color table (palette) to a png image.
		imgside_length = math.ceil(math.sqrt(self.num_entries))
		imgsize = imgside_length * squaresize

		dbg("generating palette image with size %dx%d" % (imgsize, imgsize))

		palette_image = Image.new('RGBA', (imgsize, imgsize), (255, 255, 255, 0))
		draw = ImageDraw.ImageDraw(palette_image)

		text_padlength = len(str(self.num_entries)) #dirty, i know.
		text_format = "%0" + str(text_padlength) + "d"

		drawn = 0

		if squaresize == 1:
			for y in range(imgside_length):
					for x in range(imgside_length):
						if drawn < self.num_entries:
							r,g,b = self.palette[drawn]
							draw.point((x, y), fill=(r, g, b, 255))
							drawn = drawn + 1

		elif squaresize > 1:
			for y in range(imgside_length):
					for x in range(imgside_length):
						if drawn < self.num_entries:
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

