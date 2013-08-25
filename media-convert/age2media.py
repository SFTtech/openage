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


def main():
	print("welcome to the extaordinary epic age2 media file converter")

	with open("../resources/age2/graphics.drs", "rb") as drsfile:
		# read header size bytes from file
		buf = drsfile.read(drs_header.size)

		header = drs_header.unpack(buf)
		print("header of graphics.drs:\n\t" + str(header))
		copyright, version, ftype, table_count, file_offset = header

		table_infos = []

		#the main header defines the number of tables, each for one file type
		for i in range(table_count):
			buf = drsfile.read(drs_table_info.size)

			#the table_info describes one file type, and stores the number of files saved for it
			table_info = drs_table_info.unpack(buf)
			table_infos.append(table_info)
			print("table info %d:\n\t%s" % (i, str(table_info)))

		table_file_infos = {}

		for table_info in table_infos:
			file_type, file_extension, file_info_offset, num_files = table_info

			infos = []

			#goto the beginning of the file_info tables
			drsfile.seek(file_info_offset)
			buf = drsfile.read(num_files * drs_file_info.size)

			#iterate over all saved files, and store their file_info data
			for j in range(num_files):
				file_info = drs_file_info.unpack_from(buf, j * drs_file_info.size)
				infos.append(file_info)
				
			table_file_infos[table_info] = infos

		for table_info, file_infos in table_file_infos.items():
			print("table info: " + str(table_info))
			
			file_type, file_extension, file_info_offset, num_files = table_info
			ext = file_extension.decode("utf-8")[::-1]

			i = 0
			for file_info in file_infos:
				if i == 5:
					break
			
				file_id, file_data_offset, file_size = file_info
				
				drsfile.seek(file_data_offset)
				buf = drsfile.read(file_size)

				convert_slp(file_id, buf)
				i = i + 1



main()
