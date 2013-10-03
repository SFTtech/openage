#!/usr/bin/env python

import os
import os.path
import sys
import math

from PIL import Image, ImageDraw
from struct import Struct, unpack_from

#version of the drs file, hardcoded for now
file_version = 57

if file_version == 57:
	copyright_size = 40
elif file_version == 59:
	copyright_size = 60


#we force the endianness to little endian by "<" in the Struct() format
#as this is the byte-order in the drs file


class EnumVal:
	def __init__(self, representation, **kw):
		self.representation = representation
		self.__dict__.update(kw)

	def __repr__(self):
		return self.representation

class DRS:
	#struct drs_header {
	# char copyright[copyright-size];
	# char version[4];
	# char ftype[12];
	# int table_count;
	# int file_offset; //offset of first file
	#};
	#
	drs_header = Struct("< " + str(copyright_size) + "s 4s 12s i i")

	#struct table_info {
	# char file_type;
	# char file_extension[3]; //reversed extension
	# int file_info_offset;   //table offset
	# int num_files;          //number of files in table
	#};
	drs_table_info = Struct("< c 3s i i")

	#struct file_info {
	# int file_id;
	# int file_data_offset;
	# int file_size;
	#};
	drs_file_info = Struct("< i i i")

	def __init__(self, path):
		self.path = path
		self.in_file = None
		self.files = {}       #key: file id, value: file_data_offset, file_size, file_extension
		self.file_info = {}   #key: table, value: list of drs_file_info tuples for this table
		self.file_table = {}  #key: id, value: drs_table_info tuple of this file

	def __del__(self):
		if self.in_file:
			self.in_file.close()

	def read(self):
		self.in_file = open(self.path, "rb")

		self.read_header()
		self.read_table_info()
		self.read_file_info()
		print("========= found " + str(len(self.files)) + " files in " + self.path + ". =========")

	def read_header(self):
		buf = self.in_file.read(DRS.drs_header.size)
		self.header = DRS.drs_header.unpack(buf)

	def read_table_info(self):
		table_count = self.header[3]

		self.table_info = []

		buf = self.in_file.read(table_count * DRS.drs_table_info.size)
		for i in range(table_count):
			table_info = DRS.drs_table_info.unpack_from(buf, i * DRS.drs_table_info.size)

			file_type, file_extension, file_info_offset, num_files = table_info
			#flip the extension... it's stored like that..
			file_extension = file_extension.decode('utf-8')[::-1]
			table_info = file_type, file_extension, file_info_offset, num_files

			self.table_info.append(table_info)

	def read_file_info(self):

		for table in self.table_info:

			file_type, file_extension, file_info_offset, num_files = table

			file_info_list = []

			self.in_file.seek(file_info_offset)
			buf = self.in_file.read(num_files * DRS.drs_file_info.size)

			for i in range(num_files):
				info = DRS.drs_file_info.unpack_from(buf, i * DRS.drs_file_info.size)
				file_id, file_data_offset, file_size = info

				self.files[ int(file_id) ] = file_data_offset, file_size, file_extension
				self.file_table[ int(file_id) ] = table

				file_info_list.append(info)

			self.file_info[table] = file_info_list


	def exists_file(self, fid, abort=False):
		if fid not in self.files:
			if abort:
				raise Exception("id " + str(fid) + " not stored in this drs.")
			else:
				return True
		return True

	def get_file_table_info(self, fid):
		self.exists_file(fid, True)
		return self.file_table[fid]

	def get_file_info(self, fid):
		self.exists_file(fid, True)
		return self.files[fid]

	def get_raw_file(self, fid):
		self.exists_file(fid, True)

		file_data_offset, file_size, file_extension = self.files[fid]

		self.in_file.seek(file_data_offset)
		return self.in_file.read(file_size)

	def write_raw_file(self, fid):
		fo, fs, fe = self.get_file_info(fid)

		fname = "%09d-%d.%s" % (fid, fs, fe)
		with open(fname, "wb") as f:
			f.write(self.get_raw_file(fid))

		print(fname + " written")

	def print_file_info(self, fid):
		fo, fs, fe = self.get_file_info(fid)

		print("file %d = offset: %#x / %d, size = %#x / %d, extension = %s" % (fid, fo, fo, fs, fs, fe))

	def __repr__():
		return "DRS file %s" % self.path

	def __str__(self):
		ret = repr(self)
		ret = ret + "\nheader:\n\t" + str(self.header)
		ret = ret + "\ttables:\n"
		for table in self.table_info:
			ret = ret + "\t" + str(table)

		return res



class SLP:

	#struct slp_header {
	# char version[4];
	# int num_frames;
	# char comment[24];
	#};
	slp_header = Struct("< 4s i 24s")

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
	slp_frame_info = Struct("< I I I I i i i i")

	def __init__(self, buf, file_id):
		self.rawdata = buf
		self.file_id = file_id
		self.header = SLP.slp_header.unpack_from(self.rawdata)
		self.version, self.num_frames, self.comment = self.header

		self.read_header()
		self.create_frames()

	def read_header(self):
		print("slp " + str(self.file_id) + " header:")
		print("\t" + str(self.header))
		print("\t-> " + str(self.num_frames) + " frame(s):")

		self.frame_infos = []

		#read all slp_frame_info structs
		for i in range(self.num_frames):
			info_position = SLP.slp_header.size + i * SLP.slp_frame_info.size

			frame_info = SLP.slp_frame_info.unpack_from(self.rawdata, info_position)
			self.frame_infos.append(frame_info)

	def create_frames(self):
		print("\tcmd_table_offset, outline_table_offset, palette_offset, properties, width, height, hotspot_x, hotspot_y")

		#the list of all frames included in this slp
		self.frames = []

		fcnt = 0
		for frame_info in self.frame_infos:
			print("\t\t" + str(fcnt) + " => " + str(frame_info))

			#create the frame.
			self.frames.append(SLPFrame(self, frame_info, fcnt, self.rawdata))
			fcnt = fcnt + 1

	def get_frame_count(self):
		return len(self.frames)

	def get_frame(self, index):
		return self.frames[index]

	def get_frames(self):
		return self.frames

	def save_pngs(self, destination_path, color_table, overwrite_existing=False):

		destination_path = os.path.join(destination_path, "%06d.slp" % self.file_id)

		#create the folder where the pngs will be saved to
		os.makedirs(destination_path, exist_ok=True)

		#TODO: do something like
		#http://wiki.wesnoth.org/Team_Color_Shifting
		player_id = 1 #blue

		#TODO: another idea:
		#store the base_color at it's point (with alpha = 254 as marker?)
		#then let the fragment shader do the color transformation ((player*16)+base_color)
		#and let it only draw an outline pixel if there's an obstruction (use alpha = 253 as marker?)

		#TODO: combine all slp frames to one single png, much more efficient loading and editing possible..
		#-> another converter script for packing and unpacking those
		#the number of frames must be encoded in the filename ideally
		#
		#concept:
		#im = Image.new("RGB", (500,500), "white")
		#draw = ImageDraw.ImageDraw(im)
		#part = Image.open("part.png") #why that?
		#x, y = part.size
		#im.paste(part, (0,0,x,y))  [=part.getbbox()]
		#im.save("test.png")

		#TODO: save the hotspot_x and y in the filename


		for frame in self.get_frames():
			png_filename = "%06d_%03d_%02d.png" % (self.file_id, frame.frame_id, player_id)
			frame_path = os.path.join(destination_path, png_filename)

			png = PNG(player_id, color_table, frame.get_picture_data())
			png.create()

			if not overwrite_existing and os.path.exists(frame_path):
				raise Exception("file " + frame_path + " is already existing and I won't overwrite!")

			png.save_to(frame_path)


	def __repr__(self):
		#TODO: lookup the image content description
		return "SLP image, " + str(len(self.frames)) + " Frames"


class SLPFrame:
	#shadow and transparency colors
	shadow      = EnumVal("#")
	transparent = EnumVal("( )")

	#player color class to preserve the player number variable
	class SpecialColor:
		player_color = EnumVal("P")
		black_color = EnumVal("||")

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
	slp_frame_row_edge = Struct("< H H")

	#struct slp_command_offset {
	# unsigned int offset;
	#}
	slp_command_offset = Struct("< I")

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

		# check for palette header, JASC-PAL = Jasc Paint Shop Pro palette
		if self.header != "JASC-PAL":
			raise Exception("No palette header 'JASC-PAL' found, instead: %r" % self.header)
		if self.version != "0100":
			raise Exception("palette version mispatch, got %s" % self.version)

		self.num_entries = int(palette_lines[2])

		self.palette = []

		for i in range(self.num_entries):
			#one entry looks like "13 37 42", where 13 is the red value, 37 green and 42 blue.
			self.palette.append(tuple(map(int, palette_lines[i+3].split(' '))))

	def to_image(self, filename, draw_text=True, squaresize=100):
		#writes this color table (palette) to a png image.
		imgside_length = int(math.ceil(math.sqrt(self.num_entries)))
		imgsize = int(imgside_length * squaresize)

		print("generating palette image with size %dx%d..." % (imgsize, imgsize))

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
								tcolor = (255-r, 255-b, 255-g, 255) #'inverse' drawing color for the color id

								#draw the text  #TODO: use customsized font and size
								draw.text((sx+3, sy+1),ctext,fill=tcolor,font=None)

							drawn = drawn + 1


		else:
			raise Exception("fak u, no negative values for the squaresize pls.")

		palette_image.save(filename)

	def playercolors_to_file(self, filename):
		#exports the player colors of the game palette
		#each player has 8 subcolors, where 0 is the darkest and 7 is the lightest
		out = "#aoe player color palette\n"
		out = out + "#entry id = ((playernum-1) * 8) + subcolor\n"
		i = 0

		players = range(1, 9);
		psubcolors = range(8);

		numpcolors = len(players) * len(psubcolors)
		out = out + "n=%d\n" % numpcolors

		for i in players:
			for subcol in psubcolors:
				r,g,b = self.palette[16 * i + subcol] #player i subcolor
				cid = (i-1) * len(players) + subcol  #each entry has this index
				out = out + "%d=%d,%d,%d,255\n" % (cid, r, g, b)

		with open(filename, "w") as f:
			f.write(out)

	def to_file(self, filename):
		#exports the palette
		out = "#aoe player color palette\n"
		i = 0

		numpcolors = len(self.palette)
		out = out + "n=%d\n" % numpcolors

		for r,g,b in self.palette:
			out = out + "%d=%d,%d,%d,255\n" % (i, r, g, b)
			i = i + 1

		with open(filename, "w") as f:
			f.write(out)



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

	resource_files = {}

	#all unit graphics etc are stored in graphics.drs
	graphics_drs_file = DRS("../resources/age2/graphics.drs")
	graphics_drs_file.read()
	resource_files["graphics"] = graphics_drs_file

	#user interface stuff and color tables are in interfac.drs
	interfac_drs_file = DRS("../resources/age2/interfac.drs")
	interfac_drs_file.read()
	resource_files["interfac"] = interfac_drs_file

	#the ingame graphics color palette is stored in the interfac.drs file at file index 50500
	palette_index = 50500
	color_table = ColorTable(interfac_drs_file.get_raw_file(palette_index))
	resource_files["palette"] = color_table

	extract_all = False


	if len(sys.argv) > 1:
		if sys.argv[1] == "all":
			extract_all = True


	export_path = "../resources/age2_generated"
	os.makedirs(export_path, exist_ok=True)
	export_graphics_path = os.path.join(export_path, "graphics.drs")

	color_table.to_image(export_path + "/color_table" + str(palette_index) + ".pal.png")


	#extract all needed resources from aoc data files?
	if extract_all:
		#this routine exports all graphics, sounds and other shit
		#from the original media files. this mode must be used
		#for the installation procedure.
		for table in graphics_drs_file.table_info:
			extension = table[1]
			if extension == 'slp':
				for file_info in graphics_drs_file.file_info[table]:
					file_id = file_info[0]
					slp_file = SLP(graphics_drs_file.get_raw_file(file_id), file_id)
					slp_file.save_pngs(export_graphics_path, color_table)
			else:
				pass

	else:
		#test university and hussar creation
		test_building = True
		test_unit = True

		if test_building:
			#create a university
			uni_id = 3836
			uni_slp = SLP(graphics_drs_file.get_raw_file(uni_id), uni_id)

			uni_slp.save_pngs(export_graphics_path, color_table, True)

		if test_unit:
			#create a hussar
			hussar_id = 4857
			hussar_slp = SLP(graphics_drs_file.get_raw_file(hussar_id), hussar_id)

			hussar_slp.save_pngs(export_graphics_path, color_table, True)

		color_table.playercolors_to_file(export_path + "/player_color_palette.pal")


main()
