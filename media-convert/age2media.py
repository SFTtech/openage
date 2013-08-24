#!/usr/bin/env python

import Struct from struct

#version of the drs file, hardcoded for now
file_version = 59

if file_version == 57:
	copyright_size = 40
else:
	copyright_size = 60



#struct drs_header {
# char copyright[copyright-size];
# char version[4];
# char type[12];
# long table_count;
# long file_offset;
#};
#
drs_header = Struct(copyright_size + "s 4s 12s l l")

#struct table_info {
# char file_type;
# char ext3;  //extension string not null terminated
# char ext2;  //so read character by character-exactly 3 & reverse order
# char ext1;
# int fileInfoOffset;
# int numFiles;
#};
table_info = Struct("c c c c i i")

#struct file_info {
# long file_id;
# long file_data_offset;
# long file_size;
#};
file_info = Struct("l l l")

#struct drs_table {
# int numfiles;
# fileInfo* files;  //number of files uncertain-array for holding file info
# long lower_resid; //the resid range stored in this table
# long upper_resid; //useful for quickly locating table containing a res.
#};
drs_table = Struct("i P l l")




def main():
	print("welcome to the extaordinary epic age2 media file converter")


main()
