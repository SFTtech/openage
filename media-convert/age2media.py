#!/usr/bin/env python

from struct import Struct

#version of the drs file, hardcoded for now
file_version = 57

if file_version == 57:
	copyright_size = 40
elif file_version == 59:
	copyright_size = 60


#we force the endianness to little endian by "<",
#as this is the byte-order in the drs file



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


def convert_slp(file_id, buf):
	header = slp_header.unpack_from(buf)
	print("slp %d header:\n\t%s" % (file_id, str(header)))

	version, num_frames, comment = header
	
	frame_infos = []

	for i in range(num_frames):
		frame_info = slp_frame_info.unpack_from(buf,
				slp_header.size + i * slp_frame_info.size)
		frame_infos.append(frame_info)
	
	for frame_info in frame_infos:
		print("\t" + str(frame_info))


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
			self.table_info.append(table_info)
	
	def read_file_info(self):
		self.file_info = {}

		for table in self.table_info:
			file_type, file_extension, file_info_offset, num_files = table
			file_info = []
			
			self.in_file.seek(file_info_offset)
			buf = self.in_file.read(num_files * DRS.drs_file_info.size)
			
			for i in range(num_files):
				info = DRS.drs_file_info.unpack_from(buf, i * DRS.drs_file_info.size)
				file_info.append(info)

			self.file_info[table] = file_info





def main():
	print("welcome to the extaordinary epic age2 media file converter")

	drs_file = DRS("../resources/age2/graphics.drs")
	drs_file.read()


main()
