import os
import sys
import math

from PIL import Image, ImageDraw
from struct import Struct, unpack_from
from util import NamedObject, dbg, ifdbg

#little endian byte order
endianness = "< "

class SLP:
	#struct slp_header {
	# char version[4];
	# int num_frames;
	# char comment[24];
	#};
	slp_header = Struct(endianness + "4s i 24s")

	#struct slp_frame_info {
	# unsigned int qdl_table_offset;
	# unsigned int outline_table_offset;
	# unsigned int palette_offset;
	# unsigned int properties;
	# int          width;
	# int          height;
	# int          hotspot_x;
	# int          hotspot_y;
	#};
	slp_frame_info = Struct(endianness + "I I I I i i i i")

	def __init__(self, data):
		self.data = data
		header = SLP.slp_header.unpack_from(self.data)
		version, num_frames, comment = header

		dbg("SLP header", 2, push = "slp")
		dbg("version:     " + version.decode('ascii'))
		dbg("frame count: " + str(num_frames))
		dbg("comment:     " + comment.decode('ascii'))
		dbg("")

		self.frames = []

		dbg(FrameInfo.repr_header())

		#read all slp_frame_info structs
		for i in range(num_frames):
			frame_header_offset = SLP.slp_header.size + i * SLP.slp_frame_info.size

			frame_info = FrameInfo(*SLP.slp_frame_info.unpack_from(self.data, frame_header_offset))
			dbg(frame_info)
			self.frames.append(SLPFrame(frame_info, self.data))

		dbg("", pop = "slp")

	def draw_frames(self, color_table):
		#player-specific colors will be in color blue, but with an alpha of 254
		player_id = 1

		for frame in self.frames:
			png = PNG(player_id, color_table, frame.get_picture_data())
			png.create()

			yield png, frame.info.size, frame.info.hotspot

	def draw_frames_merged(self, color_table):
		#merge all frames of this slp to a single png file.

		#TODO: actually optimize free space on the texture.
		#if you ever wanted to implement a combinatoric optimisation
		#algorithm, go for it, this function just waits for you.
		#https://en.wikipedia.org/wiki/Bin_packing_problem

		#for now, using max values for solving bin packing problem


		#player-specific colors will be in color blue, but with an alpha of 254
		player_id = 1

		max_width  = 0
		max_height = 0

		slp_pngs = []
		for frame in self.frames:
			png = PNG(player_id, color_table, frame.get_picture_data())
			png.create()

			if png.width > max_width:
				max_width = png.width

			if png.height > max_height:
				max_height = png.height

			slp_pngs.append((png, frame.info.size, frame.info.hotspot))

		#now we collected all sprites and can start merging them to one
		#big texture atlas.

		max_per_row = math.ceil(math.sqrt(len(slp_pngs)))

		width  = math.ceil(max_width * max_per_row)
		height = math.ceil(max_height * (len(slp_pngs) / max_per_row))

		atlas = Image.new('RGBA', (width, height), (0, 0, 0, 0))

		pos_x = 0
		pos_y = 0

		drawn_pngs = []
		drawn_current_row = 0

		for (sub_png, size, hotspot) in slp_pngs:
			subtexture = sub_png.image
			sub_w = subtexture.size[0]
			sub_h = subtexture.size[1]
			box = (pos_x, pos_y, pos_x + sub_w, pos_y + sub_h)

			atlas.paste(subtexture, box, mask=subtexture)
			dbg("drew frame %d at %dx%d " % (len(drawn_pngs), pos_x, pos_y), 2)

			drawn_subtexture_meta = {
				'tx': pos_x,
				'ty': pos_y,
				'tw': sub_w,
				'th': sub_h,
				'hx': hotspot[0],
				'hy': hotspot[1]
			}
			drawn_pngs.append(drawn_subtexture_meta)

			drawn_current_row = drawn_current_row + 1

			pos_x = pos_x + sub_w
			if drawn_current_row > max_per_row - 1:
				drawn_current_row = 0
				pos_x = 0
				pos_y = pos_y + sub_h


		meta_out = "#texture meta information: subtexid=x,y,w,h,hotspot_x,hotspot_y\n"
		meta_out = meta_out + "n=%d\n" % (len(slp_pngs))

		for idx, drawn_st_meta in enumerate(drawn_pngs):
			tx = drawn_st_meta["tx"]
			ty = drawn_st_meta["ty"]
			tw = drawn_st_meta["tw"]
			th = drawn_st_meta["th"]
			hotspot_x = drawn_st_meta["hx"]
			hotspot_y = drawn_st_meta["hy"]

			meta_out = meta_out + "%d=" % idx
			meta_out = meta_out + "%d,%d,%d,%d," % (tx, ty, tw, th)
			meta_out = meta_out + "%d,%d\n" % (hotspot_x, hotspot_y)


		return atlas, (width, height), meta_out

	def __str__(self):
		ret = repr(self) + "\n"

		ret = ret + FrameInfo.repr_header() + "\n"
		for frame in self.frames:
			ret = ret + repr(frame) + "\n"
		return ret

	def __repr__(self):
		#TODO: lookup the image content description
		return "<SLP image, " + str(len(self.frames)) + " Frames>"

class FrameInfo:
	def __init__(self, qdl_table_offset, outline_table_offset, palette_offset, properties, width, height, hotspot_x, hotspot_y):
		self.qdl_table_offset = qdl_table_offset
		self.outline_table_offset = outline_table_offset
		#TODO this field is not checked
		self.palette_offset = palette_offset
		#TODO what are properties good for?
		self.properties = properties
		self.size = (width, height)
		self.hotspot = (hotspot_x, hotspot_y)

	def repr_header():
		return "offset (qdl table|outline table|palette) | properties | width x height | hotspot x/y"

	def __repr__(self):
		result = "        % 9d|" % self.qdl_table_offset
		result += "% 13d|" % self.outline_table_offset
		result += "% 7d) | " % self.palette_offset
		result += "% 10d | " % self.properties
		result += "% 5d x% 7d | " % self.size
		result += "% 4d /% 5d" % self.hotspot
		return result

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
		def __init__(self, special_id, base_color = 2):
			#dbg("creating special color " + str(base_color))
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
				# -16 ensures palette[16+(-16)=0] will be used.
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

	def __init__(self, frame_info, data):
		self.info = frame_info

		self.boundaries = [] #for each row, contains the (left, right) number of boundary pixels
		self.cmd_offsets = [] #for each row, store the file offset to the first drawing command

		self.pcolor = [] #matrix that contains all the palette indices drawn by commands, key: rowid

		dbg(push = "frame", lvl = 3)

		#process bondary table
		for i in range(self.info.size[1]):
			outline_entry_position = self.info.outline_table_offset + i * SLPFrame.slp_frame_row_edge.size

			left, right = SLPFrame.slp_frame_row_edge.unpack_from(data, outline_entry_position)

			#is this row completely transparent?
			if left == 0x8000 or right == 0x8000:
				self.boundaries.append( self.transparent ) #TODO: -1 or like should be enough
			else:
				self.boundaries.append( (left, right) )

		dbg("boundary values: " + str(self.boundaries), 3)

		#process cmd table
		for i in range(self.info.size[1]):
			cmd_table_position = self.info.qdl_table_offset + i * SLPFrame.slp_command_offset.size

			cmd_offset, = SLPFrame.slp_command_offset.unpack_from(data, cmd_table_position)

			self.cmd_offsets.append(cmd_offset)

		dbg("cmd_offsets:     " + str(self.cmd_offsets))

		self.pcolor = []
		for i in range(self.info.size[1]):
			palette_color_row = self.create_palette_color_row(data, i)

			self.pcolor.append( palette_color_row )

		if ifdbg(4):
			dbg("frame color index data:\n" + str(self.pcolor), 4)

		dbg(pop = "frame")

	def create_palette_color_row(self, data, rowid):

		first_cmd_offset = self.cmd_offsets[rowid]

		bounds = self.boundaries[rowid]
		if bounds == self.transparent:
			return [self.transparent] * self.info.size[0]

		left_boundary, right_boundary = bounds

		missing_pixels = self.info.size[0] - left_boundary - right_boundary

		#start drawing the left transparent space
		pcolor_row_beginning = [ self.transparent ] * left_boundary

		#process the drawing commands for this row.
		pcolor_row_content = self.process_drawing_cmds(data, rowid, first_cmd_offset, missing_pixels, left_boundary)

		#finish by filling up the right transparent space
		pcolor_row_trailing = [ self.transparent ] * right_boundary

		pcolor = pcolor_row_beginning + pcolor_row_content + pcolor_row_trailing

		got = len(pcolor)
		if got != self.info.size[0]:
			tooless = self.info.size[0] - got
			summary = "%d/%d -> row %d, offset %d / %#x" % (got, self.width, rowid, first_cmd_offset, first_cmd_offset)
			txt = " pixels than expected: "
			if got < self.info.size[0]:
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
				dbg("#### row is getting too long!!")
				#raise Exception("Only %d pixels should be drawn in row %d!" % (missing_pixels, rowid))
			cmd = self.get_byte_at(data, dpos)

			lower_nibble  = 0x0f       & cmd
			higher_nibble = 0xf0       & cmd
			lower_bits    = 0b00000011 & cmd

			dbg("opcode: %#x, rowlength: %d, rowid: %d" % (cmd, len(pcolor_list) + leftpx, rowid), 4)

			if lower_nibble == 0x0f:
				#eol command, this row is finished now.

				dbg("end of row reached.", 4)
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
					dbg("render hint: xfliptest", 2)

				elif higher_nibble == 0x10:
					#render h notxflip command
					#render hint: only draw the following command, if this sprite IS flipped left to right.
					dbg("render hint: !xfliptest", 2)

				elif higher_nibble == 0x20:
					#table use normal command
					#set the transform color table to normal, for the standard drawing commands
					dbg("image wants normal color table now", 2)

				elif higher_nibble == 0x30:
					#table use alternat command
					#set the transform color table to alternate, this affects all following standard commands
					dbg("image wants alternate color table now", 2)

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
				dbg("stored in this row so far: " + str(pcolor_list), 2)
				raise Exception("wtf! unknown slp drawing command read: %#x in row %d" % (cmd, rowid) )

			dpos = dpos + 1

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
		#dbg("drawing " + str(count) + " times " + repr(color))
		if type(color_list) != list:
			color_list = [color_list] * count

		self.pcolor[rowid] = self.pcolor[rowid] + color_list

	def get_picture_data(self):
		return self.pcolor

	def __repr__(self):
		return repr(self.info)

class PNG:
	def __init__(self, player_number, color_table, picture_data):
		self.player_number = player_number
		self.color_table = color_table
		self.picture_data = picture_data

	def create(self):
		self.width = len(self.picture_data[0])
		self.height = len(self.picture_data)
		self.image = Image.new('RGBA', (self.width, self.height), (255, 255, 255, 0))
		draw = ImageDraw.ImageDraw(self.image)
		self.draw_picture(draw)

	def draw_picture(self, draw):
		# TODO draw lines, more efficient picture_data
		for y, picture_row in enumerate(self.picture_data):
			for x, color_data in enumerate(picture_row):
				if type(color_data) == int:
					#simply look up the color index in the table
					color = self.color_table[color_data]
				elif type(color_data) == SLPFrame.SpecialColor:
					base_pcolor, is_outline = color_data.get_pcolor()
					if is_outline:
						alpha = 253  #mark this pixel as outline
						#dbg("drawing outline base %d" % base_pcolor)
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
