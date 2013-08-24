#!/usr/bin/env python

from struct import Struct

#version of the drs file, hardcoded for now
file_version = 57

if file_version == 57:
	copyright_size = 40
elif file_version == 59:
	copyright_size = 60


#struct drs_header {
# char copyright[copyright-size];
# char version[4];
# char type[12];
# long table_count;
# long file_offset; //offset of first file
#};
#
drs_header = Struct(copyright_size + "s 4s 12s l l")

#struct table_info {
# char file_type;
# char file_extension[3]; //reversed extension
# int file_info_offset;   //table offset
# int num_files;          //number of files in table
#};
drs_table_info = Struct("c 3s i i")

#struct drs_table {
# int numfiles;
# file_info* files;  //number of files uncertain-array for holding file info
# long lower_resid; //the resid range stored in this table
# long upper_resid; //useful for quickly locating table containing a res.
#};
drs_table = Struct("i P l l")

#struct file_info {
# long file_id;
# long file_data_offset;
# long file_size;
#};
drs_file_info = Struct("l l l")

def main():
	print("welcome to the extaordinary epic age2 media file converter")


main()
