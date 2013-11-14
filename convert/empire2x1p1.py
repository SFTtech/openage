import os
import sys
import zlib

from struct import Struct, unpack_from
from util import dbg, file_get_path, file_open

class Empires2X1P1Convert:

	def __init__(self,fname):
		self.fname = fname
		print("reading empires2x1p1 from %s" % fname)

		#fname = file_get_path(fname, write = False)
		#f = file_open(fname, binary = True, write = False)
		f = open(fname, mode='rb')

		content = f.read()
		print( "length of compressed data: %d" % sys.getsizeof(content) )
		zlib.decompress( content, -15)
		print("decompressed data from %s" % fname)
		print( "length of that data: %d" % sys.getsizeof(content) )


if __name__ == "__main__":
	tryit = Empires2X1P1Convert("../Age_of_Empires_Files/Data/empires2_x1_p1.dat")

