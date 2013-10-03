#!/usr/bin/python3
from struct import Struct, unpack_from
from util import NamedObject, args

#version of the drs file, hardcoded for now
file_version = 57

if file_version == 57:
	copyright_size = 40
elif file_version == 59:
	copyright_size = 60

#"<" in the Struct() format marks the integer format as little endian

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
		self.files = {}   #(extension, id): (data offset, size)

		f = open(path, "rb")

		#read header
		buf = f.read(DRS.drs_header.size)
		self.header = DRS.drs_header.unpack(buf)

		if args.verbose >= 1:
			print("DRS header [" + path + "]")
			print("  copyright:          " + self.header[0].decode('latin-1'))
			print("  version:            " + self.header[1].decode('latin-1'))
			print("  ftype:              " + self.header[2].decode('latin-1'))
			print("  table count:        " + str(self.header[3]))
			print("  file offset:        " + str(self.header[4]))
			print("")

		#read table info
		table_count = self.header[3]

		table_header_buf = f.read(table_count * DRS.drs_table_info.size)
		for i in range(table_count):
			table_header = DRS.drs_table_info.unpack_from(table_header_buf, i * DRS.drs_table_info.size)
			file_type, file_extension, file_info_offset, num_files = table_header

			#flip the extension... it's stored that way...
			file_extension = file_extension.decode('latin-1').lower()[::-1]

			if args.verbose >= 1:
				print("  Table header [" + str(i) + "]")
				print("    file type:        " + str(file_type))
				print("    file extension:   " + file_extension)
				print("    file_info_offset: " + str(file_info_offset))
				print("    num_files:        " + str(num_files))
				print("")

			f.seek(file_info_offset)
			file_info_buf = f.read(num_files * DRS.drs_file_info.size)

			for j in range(num_files):
				file_header = DRS.drs_file_info.unpack_from(file_info_buf, j * DRS.drs_file_info.size)
				file_id, file_data_offset, file_size = file_header

				if args.verbose >= 2:
					print("    File info header [" + str(j) + "]")
					print("      file id:        " + str(file_id))
					print("      data offset:    " + str(file_data_offset))
					print("      file size:      " + str(file_size))
					print("")

				self.files[(file_extension, file_id)] = file_data_offset, file_size

		self.f = f

	def get_file_data(self, file_extension, file_id):
		file_data_offset, file_size = self.files[(file_extension, file_id)]

		self.f.seek(file_data_offset)
		return self.f.read(file_size)

	def __repr__(self):
		return "DRS file (" + str(self.header[3]) + " tables, " + len(self.files) + " files)"
