import os
import sys
import zlib

from struct import Struct, unpack_from
from util import dbg, file_get_path, file_open

endianness = '< '

class Empires2X1P1Convert:
	#version, NumTerRest, NumTer
	empires_header = Struct(endianness + "8s H H")
	#TerRestOff:Int[NumTerRest]
	#TerRestOff2:Int[NumTerRest]
	#DatTerRest[NumTerRest]: float[NumTer]TerAccessibles, TerPassGraphic[NumTer]
	#TerPassGraphic: Buildable, GraphicIdF, GraphicIdS, ReplicationNum
	terrain_pass_graphics = Struct(endianness + "i i i i")

	def __init__(self,fname):
		self.fname = fname
		print("reading empires2x1p1 from %s" % fname)

		#fname = file_get_path(fname, write = False)
		#f = file_open(fname, binary = True, write = False)
		f = open(fname, mode='rb')

		content = zlib.decompress( f.read(), -15)
		print("decompressed data from %s" % fname)
		print( "length of that data: %d" % sys.getsizeof(content) )

		#do the extracting
		#header
		offset = 0
		self.empires_header = Empires2X1P1Convert.empires_header.unpack_from(content,offset)
		offset += Empires2X1P1Convert.empires_header.size
		version, terrain_restriction_count, terrain_count = self.empires_header
		print("version: %s, TerRestCount: %d, TerCount: %d" % (version, terrain_restriction_count, terrain_count))

		#terrain_rest_offsets
		struct_terrain_restriction_offset = Struct(endianness + "%di %di" % (terrain_restriction_count, terrain_restriction_count))
		#theoratically there are 2 different sets of offsets
		self.terrain_restriction_offsets = struct_terrain_restriction_offset.unpack_from(content,offset)
		offset += struct_terrain_restriction_offset.size
		print("%s ", self.terrain_restriction_offsets)

		#TerrainRestrictions
		self.terrain_restrictions = []
		for i in range(terrain_count):
			self.terrain_restrictions.append(TerrainRestriction())
			offset = self.terrain_restrictions[i].read_data(terrain_count, content, offset)


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






if __name__ == "__main__":
	tryit = Empires2X1P1Convert("../Age_of_Empires_Files/Data/empires2_x1_p1.dat")

