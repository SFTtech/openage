from struct import Struct, unpack_from
from util import dbg

endianness = '< '


class TerrainHeaderData:
	def read(self, raw, offset):
		self.data = dict()

		#uint16_t terrain_restriction_count;
		#uint16_t terrain_count;
		header_struct = Struct(endianness + "H H")

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
		#int32_t graphic_id0;
		#int32_t graphic_id1;
		#int32_t replication_amount;
		terrain_pass_graphic_struct = Struct(endianness + "i i i i")

		pg = terrain_pass_graphic_struct.unpack_from(raw, offset)
		offset += terrain_pass_graphic_struct.size

		self.data["buildable"]          = pg[0]
		self.data["graphic_id0"]        = pg[1]
		self.data["graphic_id1"]        = pg[2]
		self.data["replication_amount"] = pg[3]

		return offset


class TerrainData:
	def __init__(self, terrain_count):
		self.terrain_count = terrain_count

	def read(self, raw, offset):
		self.data = dict()

		self.data["terrain"] = list()
		for i in range(self.terrain_count):
			t = Terrain()
			offset = t.read(raw, offset)
			self.data["terrain"] += [t.data]

		return offset


class Terrain:
	def read(self, raw, offset):
		self.data = dict()

		#int16_t unknown;
		#int16_t unknown;
		#char name0[13];
		#char name1[13];
		#int32_t slp_id;
		#int32_t unknown;
		#int32_t sound_id;
		#int32_t blend_priority;
		#int32_t blend_mode;
		#uint8_t color[3];
		#int16_t unknown;
		#int16_t unknown;
		#int8_t unknown[23];
		#int16_t frame_count;
		#int16_t unknown[2];
		#int16_t unknown[54];
		#int16_t terrain_replacement_id;
		#int16_t terrain_dimensions0;
		#int16_t terrain_dimensions1;
		#int8_t unknown[84];
		#int16_t terrain_unit_id[30];
		#int16_t terrain_unit_density[30];
		#int8_t unknown[30];
		#int16_t terrain_units_used_count;
		terrain_struct = Struct(endianness + "2h 13s 13s 5i 3B 2h 23c h 2h 54h 3h 84c 30h 30h 30c h")

		pc = terrain_struct.unpack_from(raw, offset)
		offset += terrain_struct.size

		#self.data[""] = pc[0]
		#self.data[""] = pc[1]
		self.data["name0"]                    = pc[2]
		self.data["name1"]                    = pc[3]
		self.data["slp_id"]                   = pc[4]
		#self.data[""] = pc[5]
		self.data["sound_id"]                 = pc[6]
		self.data["blend_priority"]           = pc[7]
		self.data["blend_mode"]               = pc[8]
		self.data["color"]                    = pc[9:(9+3)]
		#self.data[""] = pc[12]
		#self.data[""] = pc[13]
		#self.data[""] = pc[14:(14+23)]

		self.data["frame_count"]              = pc[37]
		#self.data[""] = pc[38:(38+2)]
		#self.data[""] = pc[40:(40+54)]
		self.data["terrain_replacement_id"]   = pc[94]
		self.data["terrain_dimensions0"]      = pc[95]
		self.data["terrain_dimensions1"]      = pc[96]
		#self.data[""] = pc[97:(97+84)]
		self.data["terrain_unit_id"]          = pc[181:(181+30)]
		self.data["terrain_unit_density"]     = pc[211:(211+30)]
		#self.data[""] = pc[241:(241+30]
		self.data["terrain_units_used_count"] = pc[271]

		return offset
