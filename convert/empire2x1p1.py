import os
import sys
import zlib

from struct import Struct, unpack_from
from util import dbg, file_get_path, file_open

endianness = '< '

class Empires2X1P1Convert:
	#version, NumTerRest, NumTer
	initial_information= Struct(endianness + "8s H H")

	def __init__(self,fname):
		self.fname = fname
		print("reading empires2x1p1 from %s" % fname)

		f = open(fname, mode='rb')

		content = zlib.decompress( f.read(), -15)
		print("decompressed data from %s" % fname)
		print( "length of that data: %d" % sys.getsizeof(content) )

		#do the extracting
		#header
		offset = 0
		empires_header = Empires2X1P1Convert.initial_information.unpack_from(content,offset)
		offset += Empires2X1P1Convert.initial_information.size
		self.version, self.terrain_restriction_count, self.terrain_count = empires_header
		print("version: %s, TerRestCount: %d, TerCount: %d" % (self.version, self.terrain_restriction_count, self.terrain_count))

		#terrain_rest_offsets
		struct_terrain_restriction_offset = Struct(endianness + "%di" % (2*self.terrain_restriction_count))
		#theoratically there are 2 different sets of offsets
		self.terrain_restriction_offsets = struct_terrain_restriction_offset.unpack_from(content,offset)
		offset += struct_terrain_restriction_offset.size
		print('TerrainRestrictionOffsets:', self.terrain_restriction_offsets)

		#TerrainRestrictions
		self.terrain_restrictions = []
		for i in range(self.terrain_count):
			self.terrain_restrictions.append(TerrainRestriction())
			offset = self.terrain_restrictions[i].read_data(self.terrain_count, content, offset)
		print('TerRest[0]: \n' + str(self.terrain_restrictions[0]))
		print('TerRest[1]: \n' + str(self.terrain_restrictions[1]))
		print('TerRest[2]: \n' + str(self.terrain_restrictions[2]))
		print('TerRest[3]: \n' + str(self.terrain_restrictions[3]))
		print('TerRest[4]: \n' + str(self.terrain_restrictions[4]))


class TerrainRestriction:
	def read_data(self, num_ter, content, offset):
		struct_terrain_accessibles = Struct(endianness + '%df' % (num_ter))
		struct_terrain_pass_graphic = Struct(endianness + 'i i i i')
		self.terrain_accessibles = struct_terrain_accessibles.unpack_from(content, offset)
		offset += struct_terrain_pass_graphic.size
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


if __name__ == "__main__":
	tryit = Empires2X1P1Convert("../Age_of_Empires_Files/Data/empires2_x1_p1.dat")

