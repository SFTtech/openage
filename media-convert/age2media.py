#!/usr/bin/env python

import os
import sys

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

		def __init__(self, special_id, base_color=0):
			#print("creating special color " + str(color))
			self.special_id = special_id
			self.base_color = base_color

		def get_pcolor_for_player(self, player):
			if self.special_id == 1 or self.special_id == self.black_color:
				return 0
			elif self.special_id == 2 or self.special_id == self.player_color:
				return 16 * player + self.base_color
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
		self.num_entries = int(palette_lines[2])

		# check for palette header
		if self.header != "JASC-PAL":
			raise Exception("No palette header 'JASC-PAL' found, instead: %r" % palette_lines[0])
		if self.version != "0100":
			raise Exception("palette version mispatch, got %s" % palette_lines[1])

		self.palette = []

		for i in range(self.num_entries):
			#one entry looks like "13 37 42", where 13 is the red value, 37 green and 42 blue.
			self.palette.append(tuple(map(int, palette_lines[i+3].split(' '))))

	def __getitem__(self, index):
		return self.palette[index]

	def __str__(self):
		return "color palette: \n" + str(self.palette)


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
				#elif type(color_data) == SLPFrame.PlayerColor:
				#	color = self.color_table[color_data.get_pcolor_for_player(self.player_number)]
				elif type(color_data) == SLPFrame.SpecialColor:
					color = self.color_table[color_data.get_pcolor_for_player(self.player_number)]
				elif color_data == SLPFrame.transparent:
					color = (0, 0, 0, 0)
				elif color_data == SLPFrame.shadow:
					color = (0, 0, 0, 100)
				else:
					raise Exception("Unknown color: " + str(color_data))

				draw.point((x, y), fill=color)

		#	self.draw.line((x, y, x + amount, y), fill=color)

	def get_image(self):
		return self.image

	def store_image_to(self, filename):
		return self.image.save(filename)


base_path = "../resources/age2_generated"
base_graphics_path = os.path.join(base_path, "graphics.drs")


def main():
	print("welcome to the extaordinary epic age2 media file converter")

	graphics_drs_file = DRS("../resources/age2/graphics.drs")
	graphics_drs_file.read()

	interfac_drs_file = DRS("../resources/age2/interfac.drs")
	interfac_drs_file.read()

	# the ingame graphics color palette is stored in the interfac.drs file at file index 50500
	palette_index = 50500
	color_table = ColorTable(interfac_drs_file.get_raw_file(palette_index))

	#print(str(color_table))

	#print("\n\nfile ids in this drs:" + str(sorted(drs_file.files.keys())))

	print("\n=========\nfound " + str(len(graphics_drs_file.files)) + " files in the graphics.drs.\n=========\n")

	create_all = ( len(sys.argv) > 1 and sys.argv[1] == "all")

	#create all graphics?
	if create_all:
		os.makedirs(base_graphics_path, exist_ok=True)

		for table in graphics_drs_file.table_info:
			extension = table[1]
			if extension == 'slp':
				for file_info in graphics_drs_file.file_info[table]:
					file_id = file_info[0]
					slp_file = SLP(graphics_drs_file.get_raw_file(file_id), file_id)
					create_slp(slp_file, color_table)
			else:
				pass


def create_slp(slp_file, color_table):
	base_slp_path = os.path.join(base_graphics_path, "%06d.slp" % slp_file.file_id)
	os.makedirs(base_slp_path)

	player_id = 4 #yellow

	for frame in slp_file.get_frames():
		frame_path = os.path.join(base_slp_path, "%06d_%03d_%02d.png" %
				(slp_file.file_id, frame.frame_id, player_id))

		png = PNG(player_id, color_table, frame.get_picture_data())
		png.create()
		png.get_image().save(frame_path)


main()
