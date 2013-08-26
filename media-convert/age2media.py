#!/usr/bin/env python

import sys

from struct import Struct

#version of the drs file, hardcoded for now
file_version = 57

if file_version == 57:
	copyright_size = 40
elif file_version == 59:
	copyright_size = 60


#we force the endianness to little endian by "<",
#as this is the byte-order in the drs file


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

		print("header:\n\t" + str(self.header))
		print("tables:")
		for table in self.table_info:
			print("\t" + str(table))

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
			file_extension = file_extension.decode('utf8')[::-1]
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

	def get_file_table(self, fid):
		self.exists_file(fid, True)
		return self.file_table[fid]

	def get_raw_file(self, fid):
		self.exists_file(fid, True)

		file_data_offset, file_size, file_extension = self.files[fid]

		self.in_file.seek(file_data_offset)
		return self.in_file.read(file_size)




class SLP:

	#struct slp_header {
	# char version[4];
	# int num_frames;
	# char comment[24];
	#};
	slp_header = Struct("< 4s i 24s")

	#struct slp_frame_info {
	#unsigned int	qdl_table_offset;
	#unsigned int	outline_table_offset;
	#unsigned int	palette_offset;
	#unsigned int	properties;
	#int	width;
	#int	height;
	#int	hotspot_x;
	#int	hotspot_y;
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
			self.frames.append(SLPFrame(frame_info, self.rawdata))
			fcnt = fcnt + 1

	def __repr__(self):
		#TODO: lookup the image content description
		return "SLP image, " + str(len(self.frames)) + " Frames"


class SLPFrame:

	def __init__(self, frame_info, data):

		self.cmd_table_offset, self.outline_table_offset,\
		self.palette_offset, self.properties, self.width,\
		self.height, self.hotspot_x, self.hotspot_y = frame_info


	def create_palette_index_table(self):
		pass

	def create_palette_index_row(self):
		pass



def main():
	print("welcome to the extaordinary epic age2 media file converter")

	drs_file = DRS("../resources/age2/graphics.drs")
	#drs_file = DRS("../resources/age2/interfac.drs")
	drs_file.read()

	print("\n\nfile ids in this drs:" + str(sorted(drs_file.files.keys())))

	print("\n=========\nfound " + str(len(drs_file.files)) + " files in the drs.\n=========\n")


	print("\n\neuropean castle:")
	#slp_castle = SLP(drs_file.get_raw_file(302), 302) #get european castle
	print("done with the european castle.\n\n")


	create_all = ( len(sys.argv) > 1 and sys.argv[1] == "all")

	#create all graphics?
	if create_all:
		graphics_slps = {}

		for i in drs_file.files.keys():
			#only process slp files so far
			file_extension = drs_file.get_file_table(i)[1]
			if "slp" == file_extension:
				gslp = SLP(drs_file.get_raw_file(i), i)
				graphics_slps[i] = gslp

			else:
				raise Exception("file" + str(i) + " has a unknown file extension: " + file_extension)


main()
