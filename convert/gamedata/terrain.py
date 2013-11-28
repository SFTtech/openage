from struct import Struct, unpack_from
from util import dbg

endianness = '< '


class TerrainHeaderData:
	def read(self, raw, offset):
		self.data = dict()

		#uint16_t terrain_restriction_count;
		#uint16_t terrain_count;
		header_struct = Struct(endianness + "h h")

		header = header_struct.unpack_from(raw, offset)
		offset += header_struct.size
		self.data["terrain_restriction_count"], self.data["terrain_count"] = header

		#int32_t terrain_restriction_offset0[terrain_restriction_count];
		#int32_t terrain_restriction_offset1[terrain_restriction_count];
		tr_offset_struct = Struct(endianness + "%di" % self.data["terrain_restriction_count"])

		self.data["terrain_restriction_offset0"] = tr_offset_struct.unpack_from(raw, offset)
		offset += tr_offset_struct.size
		self.data["terrain_restriction_offset1"] = tr_offset_struct.unpack_from(raw, offset)
		offset += tr_offset_struct.size

		self.data["terrain_restriction"] = list()
		for i in range(self.data["terrain_restriction_count"]):
			t = TerrainRestriction(self.data["terrain_count"])
			offset = t.read(raw, offset)
			self.data["terrain_restriction"] += [t.data]

		return offset


class TerrainRestriction:
	def __init__(self, terrain_count):
		self.terrain_count = terrain_count

		#float terrain_accessible[terrain_count];
		self.terrain_accessible_struct = Struct(endianness + "%df" % terrain_count)

	def read(self, raw, offset):
		self.data = dict()

		self.data["terrain_accessible"] = self.terrain_accessible_struct.unpack_from(raw, offset)
		offset += self.terrain_accessible_struct.size

		self.data["terrain_pass_graphic"] = list()
		for i in range(self.terrain_count):
			t = TerrainPassGraphic()
			offset = t.read(raw, offset)
			self.data["terrain_pass_graphic"] += [t.data]

		return offset


class TerrainPassGraphic:
	def read(self, raw, offset):
		self.data = dict()

		#int32_t buildable;
		#int32_t graphic_id_first;
		#int32_t graphic_id_second;
		#int32_t replication_amount;
		terrain_pass_graphic_struct = Struct(endianness + "i i i i")

		pg = terrain_pass_graphic_struct.unpack_from(raw, offset)
		offset += terrain_pass_graphic_struct.size

		self.data["buildable"]          = pg[0]
		self.data["graphic_id0"]        = pg[1]
		self.data["graphic_id1"]        = pg[2]
		self.data["replication_amount"] = pg[3]

		return offset
