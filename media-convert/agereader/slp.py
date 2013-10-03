import os
import os.path
import sys
import math

from PIL import Image, ImageDraw
from struct import Struct, unpack_from
from util import NamedObject

#little endian byte order
endianness = "< "

class SLPFrame:
	#shadow and transparency colors
	shadow      = NamedObject("#")
	transparent = NamedObject("( )")

	#player color class to preserve the player number variable
	class SpecialColor:
		player_color = NamedObject("P")
		black_color = NamedObject("||")

		#base_color: value for the base player color
		#used for outlines.
		#2 is lighter and suits better for outline display.
		#try to experiment with [0,7]..
		def __init__(self, special_id, base_color=2):
			#print("creating special color " + str(base_color))
			self.special_id = special_id
			self.base_color = base_color

		def get_pcolor_for_player(self, player):
			if self.special_id == 2 or self.special_id == self.black_color:
				return -16 #this ensures palette[16 -16] will be taken
			elif self.special_id == 1 or self.special_id == self.player_color:
				return 16 * player + self.base_color #return final color for outline or player
			else:
				raise Exception("unknown special color")

		def get_pcolor(self):
			#@returns (base_color, is_outline_pixel)
			if self.special_id == 2 or self.special_id == self.black_color:
				#black outline pixel, we will probably never encounter this.
				# -16 ensures palette[16 -16] will be used.
				return (-16, True)
			elif self.special_id == 1:
				return (self.base_color, True) #this is an player-colored outline pixel
			elif self.special_id == self.player_color:
				return (self.base_color, False) #this is a playercolor pixel base color
			else:
				raise Exception("unknown special color")

		def __repr__(self):
			return "S" + str(self.special_id) + str(self.base_color)

	#struct slp_frame_row_edge {
	# unsigned short left_space;
	# unsigned short right_space;
	#};
	slp_frame_row_edge = Struct(endianness + "H H")

	#struct slp_command_offset {
	# unsigned int offset;
	#}
	slp_command_offset = Struct(endianness + "I")

	def __init__(self, containingfile, frame_info, frame_id, data):

		self.slpfile = containingfile
		self.cmd_table_offset, self.outline_table_offset,\
		self.palette_offset, self.properties, self.width,\
		self.height, self.hotspot_x, self.hotspot_y = frame_info

		self.frame_id = frame_id

		self.boundaries = [] #for each row, contains the (left, right) number of boundary pixels
		self.cmd_offsets = [] #for each row, store the file offset to the first drawing command

		self.pcolor = [] #matrix that contains all the palette indices drawn by commands, key: rowid

		self.process(data)

	def process(self, data):
		self.process_boundary_tables(data)
		#print("boundary values:\n" + str(self.boundaries))

		self.process_cmd_table(data)
		#print("cmd_offsets:\n" + str(self.cmd_offsets))

		self.create_palette_color_table(data)

	def process_boundary_tables(self, data):
		for i in range(self.height):
			outline_entry_position = self.outline_table_offset + i * SLPFrame.slp_frame_row_edge.size

			left, right = SLPFrame.slp_frame_row_edge.unpack_from(data, outline_entry_position)

			#is this row completely transparent?
			if left == 0x8000 or right == 0x8000:
				self.boundaries.append( self.transparent ) #TODO: -1 or like should be enough
			else:
				self.boundaries.append( (left, right) )

	def process_cmd_table(self, data):
		for i in range(self.height):
			cmd_table_position = self.cmd_table_offset + i * SLPFrame.slp_command_offset.size

			cmd_offset, = SLPFrame.slp_command_offset.unpack_from(data, cmd_table_position)

			self.cmd_offsets.append(cmd_offset)

	def create_palette_color_table(self, data):
		self.pcolor = []
		for i in range(self.height):
			palette_color_row = self.create_palette_color_row(data, i)

			self.pcolor.append( palette_color_row )

	def create_palette_color_row(self, data, rowid):

		first_cmd_offset = self.cmd_offsets[rowid]

		bounds = self.boundaries[rowid]
		if bounds == self.transparent:
			return [self.transparent] * self.width

		left_boundary, right_boundary = bounds

		missing_pixels = self.width - left_boundary - right_boundary

		#start drawing the left transparent space
		pcolor_row_beginning = [ self.transparent ] * left_boundary

		#process the drawing commands for this row.
		pcolor_row_content = self.process_drawing_cmds(data, rowid, first_cmd_offset, missing_pixels, left_boundary)

		#finish by filling up the right transparent space
		pcolor_row_trailing = [ self.transparent ] * right_boundary

		pcolor = pcolor_row_beginning + pcolor_row_content + pcolor_row_trailing

		got = len(pcolor)
		if got != self.width:
			tooless = self.width - got
			summary = "%d/%d -> row %d, offset %d / %#x" % (got, self.width, rowid, first_cmd_offset, first_cmd_offset)
			txt = " pixels than expected: "
			if got < self.width:
				txt = "LESS" + txt
				raise Exception("got " + txt + summary + ", missing: %d" % tooless)
			else:
				txt = "MORE" + txt
				raise Exception("got " + txt + summary + ": %d " % (-tooless))


		return pcolor

	def process_drawing_cmds(self, data, rowid, first_cmd_offset, missing_pixels, leftpx):
		dpos = first_cmd_offset #position in the data blob, we start at the first command of this row

		pcolor_list = [] #this array gets filled with palette indices by the cmds

		eor = False
		#work through commands till end of row.
		while not eor:

			if len(pcolor_list) > missing_pixels:
				print("#### row is getting too long!!")
				#raise Exception("Only %d pixels should be drawn in row %d!" % (missing_pixels, rowid))
			cmd = self.get_byte_at(data, dpos)

			lower_nibble  = 0x0f       & cmd
			higher_nibble = 0xf0       & cmd
			lower_bits    = 0b00000011 & cmd

			#print("opcode: %#x, rowlength: %d, rowid: %d" % (cmd, len(pcolor_list) + leftpx, rowid))

			if lower_nibble == 0x0f:
				#eol command, this row is finished now.

				#print("end of row reached.")
				eor = True
				continue

			elif lower_bits == 0b00000000:
				#color_list command
				#draw the following bytes as palette colors

				pixel_count = cmd >> 2
				for i in range(pixel_count):
					dpos = dpos + 1
					color = self.get_byte_at(data, dpos)
					pcolor_list = pcolor_list + [ int(color) ]

			elif lower_bits == 0b00000001:
				#skip command
				#draw 'count' transparent pixels
				#count = cmd >> 2; if count == 0: count = nextbyte

				pixel_count, dpos = self.cmd_or_next(cmd, 2, data, dpos)
				pcolor_list = pcolor_list + [self.transparent] * pixel_count

			elif lower_nibble == 0x02:
				#big_color_list command
				#draw higher_nibble << 4 + nextbyte times the following palette colors

				dpos = dpos + 1
				nextbyte = self.get_byte_at(data, dpos)
				pixel_count = (higher_nibble << 4) + nextbyte

				for i in range(pixel_count):
					dpos = dpos + 1
					color = self.get_byte_at(data, dpos)
					pcolor_list = pcolor_list + [ int(color) ]

			elif lower_nibble == 0x03:
				#big_skip command
				#draw higher_nibble << 4 + nextbyte times the transparent pixel

				dpos = dpos + 1
				nextbyte = self.get_byte_at(data, dpos)
				pixel_count = (higher_nibble << 4) + nextbyte

				pcolor_list = pcolor_list + [self.transparent] * pixel_count

			elif lower_nibble == 0x06:
				#player_color_list command
				#we have to draw the player color for cmd>>4 times,
				#or if that is 0, as often as the next byte says.

				pixel_count, dpos = self.cmd_or_next(cmd, 4, data, dpos)
				for i in range(pixel_count):
					dpos = dpos + 1
					color = self.get_byte_at(data, dpos)

					#the SpecialColor class preserves the calculation with player*16+color
					entry = SLPFrame.SpecialColor(special_id = SLPFrame.SpecialColor.player_color, base_color = color)
					pcolor_list = pcolor_list + [ entry ]

			elif lower_nibble == 0x07:
				#fill command
				#draw 'count' pixels with color of next byte

				pixel_count, dpos = self.cmd_or_next(cmd, 4, data, dpos)

				dpos = dpos + 1
				color = self.get_byte_at(data, dpos)

				pcolor_list = pcolor_list + [color] * pixel_count

			elif lower_nibble == 0x0A:
				#fill player color command
				#draw the player color for 'count' times

				pixel_count, dpos = self.cmd_or_next(cmd, 4, data, dpos)

				dpos = dpos + 1
				color = self.get_byte_at(data, dpos)

				#TODO: verify this. might be incorrect.
				#color = ((color & 0b11001100) | 0b00110011)

				#SpecialColor class preserves the calculation of player*16 + color
				entry = SLPFrame.SpecialColor(special_id = SLPFrame.SpecialColor.player_color, base_color = color)
				pcolor_list = pcolor_list + [ entry ] * pixel_count

			elif lower_nibble == 0x0B:
				#shadow command
				#draw a transparent shadow pixel for 'count' times

				pixel_count, dpos = self.cmd_or_next(cmd, 4, data, dpos)

				pcolor_list = pcolor_list + [ self.shadow ] * pixel_count

			elif lower_nibble == 0x0E:
				if higher_nibble == 0x00:
					#render hint xflip command
					#render hint: only draw the following command, if this sprite is not flipped left to right
					print("render hint: xfliptest")

				elif higher_nibble == 0x10:
					#render h notxflip command
					#render hint: only draw the following command, if this sprite IS flipped left to right.
					print("render hint: !xfliptest")

				elif higher_nibble == 0x20:
					#table use normal command
					#set the transform color table to normal, for the standard drawing commands
					print("image wants normal color table now")

				elif higher_nibble == 0x30:
					#table use alternat command
					#set the transform color table to alternate, this affects all following standard commands
					print("image wants alternate color table now")

				elif higher_nibble == 0x40:
					#outline_1 command
					#the next pixel shall be drawn as special color 1, if it is obstructed later in rendering
					pcolor_list = pcolor_list + [ SLPFrame.SpecialColor(1) ]

				elif higher_nibble == 0x60:
					#outline_2 command
					#same as above, but special color 2
					pcolor_list = pcolor_list + [ SLPFrame.SpecialColor(2) ]

				elif higher_nibble == 0x50:
					#outline_span_1 command
					#same as above, but span special color 1 nextbyte times.

					dpos = dpos + 1
					pixel_count = self.get_byte_at(data, dpos)

					pcolor_list = pcolor_list + [ SLPFrame.SpecialColor(1) ] * pixel_count


				elif higher_nibble == 0x70:
					#outline_span_2 command
					#same as above, using special color 2

					dpos = dpos + 1
					pixel_count = self.get_byte_at(data, dpos)

					pcolor_list = pcolor_list + [ SLPFrame.SpecialColor(2) ] * pixel_count

			else:
				print("stored in this row so far: " + str(pcolor_list))
				raise Exception("wtf! unknown slp drawing command read: %#x in row %d" % (cmd, rowid) )

			dpos = dpos + 1

		#print("file %d, frame %d, row %d: " % (self.slpfile.file_id, self.frame_id, rowid) + str(pcolor_list))
		#end of row reached, return the created pixel array.
		return pcolor_list

	def get_byte_at(self, data, offset):
		return int(unpack_from("B", data, offset)[0]) #unpack returns len==1 tuple..

	def cmd_or_next(self, cmd, n, data, pos):
		packed_in_cmd = cmd >> n

		if packed_in_cmd != 0:
			return packed_in_cmd, pos

		else:
			pos = pos + 1
			return self.get_byte_at(data, pos), pos

	def draw_pcolor_to_row(self, rowid, color_list, count=1):
		#print("drawing " + str(count) + " times " + repr(color))
		if type(color_list) != list:
			color_list = [color_list] * count

		self.pcolor[rowid] = self.pcolor[rowid] + color_list

	def get_picture_data(self):
		return self.pcolor


class ColorTable():
	def __init__(self, raw_color_palette):
		self.process(raw_color_palette.decode("utf-8"))

	def process(self, raw_color_palette):
		palette_lines = raw_color_palette.split("\r\n") #WHYYYY WINDOWS LINE ENDINGS YOU IDIOTS

		self.header = palette_lines[0]
		self.version = palette_lines[1]

		# check for palette header
		if self.header != "JASC-PAL":
			raise Exception("No palette header 'JASC-PAL' found, instead: %r" % palette_lines[0])
		if self.version != "0100":
			raise Exception("palette version mispatch, got %s" % palette_lines[1])

		self.num_entries = int(palette_lines[2])

		self.palette = []

		for i in range(self.num_entries):
			#one entry looks like "13 37 42", where 13 is the red value, 37 green and 42 blue.
			self.palette.append(tuple(map(int, palette_lines[i+3].split(' '))))

	def to_image(self, filename, draw_text=True, squaresize=100):
		#writes this color table (palette) to a png image.
		imgside_length = math.ceil(math.sqrt(self.num_entries))
		imgsize = imgside_length * squaresize

		print("generating palette image with size %dx%d" % (imgsize, imgsize))

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

		palette_image.save(filename)


	def __getitem__(self, index):
		return self.palette[index]

	def __repr__(self):
		return "color palette: %d entries." % self.num_entries

	def __str__(self):
		return repr(self) + "\n" + str(self.palette)


class PNG():
	def __init__(self, player_number, color_table, picture_data):
		self.player_number = player_number
		self.color_table = color_table
		self.picture_data = picture_data

	def create(self):
		width = len(self.picture_data[0])
		height = len(self.picture_data)
		self.image = Image.new('RGBA', (width, height), (255, 255, 255, 0))
		draw = ImageDraw.ImageDraw(self.image)
		self.draw_picture(draw)

	def draw_picture(self, draw):
		# TODO draw lines, more efficient picture_data
		for y, picture_row in enumerate(self.picture_data):
			for x, color_data in enumerate(picture_row):
				if type(color_data) == int:
					color = self.color_table[color_data]
				elif type(color_data) == SLPFrame.SpecialColor:
					base_pcolor, is_outline = color_data.get_pcolor()
					if is_outline:
						alpha = 253  #mark this pixel as outline
						#print("drawing outline base %d" % base_pcolor)
					else:
						alpha = 254  #mark this pixel as player color

					#get rgb base color from the color table
					#store it the preview player color (in the table: [16*player,16*player+7]
					r, g, b = self.color_table[base_pcolor + 16 * self.player_number]
					color = (r, g, b, alpha)

					#this now ensures the fragment shader is able to replace the base_color
					#with the real player color, and only display outline pixels,
					#if they are obstructed.

				elif color_data == SLPFrame.transparent:
					color = (0, 0, 0, 0)
				elif color_data == SLPFrame.shadow:
					color = (0, 0, 0, 100)
				else:
					raise Exception("Unknown color: " + str(color_data))

				draw.point((x, y), fill=color)

				#self.draw.line((x, y, x + amount, y), fill=color)

	def get_image(self):
		return self.image

	def save_to(self, filename):
		return self.image.save(filename)



def main():
	print("welcome to the extaordinary epic age2 media file converter")

	graphics_drs_file = DRS("../resources/age2/graphics.drs")
	graphics_drs_file.read()

	interfac_drs_file = DRS("../resources/age2/interfac.drs")
	interfac_drs_file.read()

	# the ingame graphics color palette is stored in the interfac.drs file at file index 50500
	palette_index = 50500
	color_table = ColorTable(interfac_drs_file.get_raw_file(palette_index))

	color_table.to_image("../resources/colortable" + str(palette_index) + ".pal.png")
	#print("using " + str(color_table))

	#print("\n\nfile ids in this drs:" + str(sorted(drs_file.files.keys())))

	print("\n=========\nfound " + str(len(graphics_drs_file.files)) + " files in the graphics.drs.\n=========\n")

	create_all = False
	create_player_color_entries = False

	if len(sys.argv) > 1:
		if sys.argv[1] == "all":
			create_all = True
		elif sys.argv[1] == "playercolorentries":
			create_player_color_entries = True

	create_single = (not create_all) and (not create_player_color_entries)

	#test university and hussar creation
	test_building = True
	test_unit = True

	export_path = "../resources/age2_generated"
	export_graphics_path = os.path.join(export_path, "graphics.drs")

	#create all graphics?
	if create_all:
		for table in graphics_drs_file.table_info:
			extension = table[1]
			if extension == 'slp':
				for file_info in graphics_drs_file.file_info[table]:
					file_id = file_info[0]
					slp_file = SLP(graphics_drs_file.get_raw_file(file_id), file_id)
					slp_file.save_pngs(export_graphics_path, color_table)
			else:
				pass

	elif create_single:
		if test_building:
			#only create a university
			uni_id = 3836
			uni_slp = SLP(graphics_drs_file.get_raw_file(uni_id), uni_id)

			uni_slp.save_pngs(export_graphics_path, color_table, True)

		if test_unit:
			#only create a hussar
			hussar_id = 4857
			hussar_slp = SLP(graphics_drs_file.get_raw_file(hussar_id), hussar_id)

			hussar_slp.save_pngs(export_graphics_path, color_table, True)

	elif create_player_color_entries:
		#experiments with the color table, getting the math behind player colors:

		#each player has 8 subcolors, where 0 is the darkest and 7 is the lightest
		players = range(1, 9);
		psubcolors = range(8);
		for i in players:
			print("//colors for player %d" % i)
			for subcol in psubcolors:
				r,g,b = color_table[16 * i + subcol]
				print("const vec4 player%d_color_%d = vec4(%d.0/255.0, %d.0/255.0, %d.0/255.0, 1.0);" % (i, subcol, r, g, b))

		print("\n\n//generating get_color(basecolor, playernum):\n")
		print("vec4 get_color(int base, int playernum) {")


		#TODO: use switch()
		for i in players:
			if i != 1:
				p_else = "else "
			else:
				p_else = ""

			print("\t%sif (playernum == %d) {" % (p_else, i))
			for subcol in psubcolors:
				if subcol != 0:
					s_else = "else "
				else:
					s_else = ""
				print("\t\t%sif (base == %d) {" % (s_else, subcol))
				print("\t\t\treturn player%d_color_%d;" % (i, subcol))
				print("\t\t}")

			print("\t}")

		print("}")

main()
