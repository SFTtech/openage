import os
import sys
import zlib

from struct import Struct, unpack_from
from util import dbg, file_get_path, file_open

endianness = '< '

class Empires2X1P1:
	"""represents the compressed empires2_x1_p1.dat"""

	#version, NumTerRest, NumTer
	initial_information= Struct(endianness + "8s H H")

	def __init__(self, fname):
		self.fname = fname
		dbg("reading empires2x1p1 from %s" % fname, 1)

		fname = file_get_path(fname, write = False)
		f = file_open(fname, binary = True, write = False)

		#decompress content with zlib (note the magic -15)
		#-15: - -> there is no header, 15 is the max windowsize
		content = zlib.decompress( f.read(), -15)
		f.close()

		dbg("decompressed data from %s" % fname, 1)
		dbg("length of that data: %d" % sys.getsizeof(content), 1)

		#do the extracting
		#header
		offset = 0
		empires_header = Empires2X1P1.initial_information.unpack_from(content,offset)
		offset += Empires2X1P1.initial_information.size
		self.version, self.terrain_restriction_count, self.terrain_count = empires_header
		dbg("version: %s, TerRestCount: %d, TerCount: %d" % (self.version, self.terrain_restriction_count, self.terrain_count), 1)

		#terrain_rest_offsets
		struct_terrain_restriction_offset = Struct(endianness + "%di" % (2*self.terrain_restriction_count))
		#theoratically there are 2 different sets of offsets
		self.terrain_restriction_offsets = struct_terrain_restriction_offset.unpack_from(content,offset)
		offset += struct_terrain_restriction_offset.size
		dbg("TerrainRestrictionOffsets: " + str(self.terrain_restriction_offsets), 1)

		#TerrainRestrictions
		self.terrain_restrictions = []
		for i in range(self.terrain_count):
			self.terrain_restrictions.append(TerrainRestriction())
			offset = self.terrain_restrictions[i].read_data(self.terrain_count, content, offset)

		#dump the first 5 terrain restrictions
		for i in range(5):
			dbg('TerRest[%d]: \n%s' % (i, self.terrain_restrictions[i]), 1)

	def __str__(self):
		ret = "[age2x1p1]" #TODO moar information
		return ret


class TerrainRestriction:
	def read_data(self, num_ter, content, offset):
		struct_terrain_accessibles = Struct(endianness + '%df' % (num_ter))
		struct_terrain_pass_graphic = Struct(endianness + 'i i i i')
		self.terrain_accessibles = struct_terrain_accessibles.unpack_from(content, offset)
		offset += struct_terrain_accessibles.size
		toplist = []
		for i in range(num_ter):
			sublist = struct_terrain_pass_graphic.unpack_from(content, offset)
			offset += struct_terrain_pass_graphic.size
			toplist.append(sublist)
		self.terrain_pass_graphic = toplist
		#return the updated offset
		return offset

	def __str__(self):
		return '[TerrainRestriction: ' + str(self.terrain_accessibles) + ' , ' + str(self.terrain_pass_graphic) + ' ]'


def test(datfile):
	dbg("converting the empires2_x1_p1 main game data file...")
	tryit = Empires2X1P1(datfile)
	dbg("result:\n" + str(tryit), 1)

if __name__ == "__main__":
	#fak u frank
	import os
	os.system("python convert -o ../../ -d -vv ../../Age_of_Empires_Files/")

