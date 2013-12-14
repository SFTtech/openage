from struct import Struct, unpack_from
from util import dbg, zstr
from util import file_get_path, file_write

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
		#uint8_t unknown[5];
		#float unknown;
		#int8_t unknown[18];
		#int16_t frame_count;
		#int16_t angle_count;
		#int16_t terrain_id;
		#int16_t elevation_graphic[54];
		#int16_t terrain_replacement_id;
		#int16_t terrain_dimensions0;
		#int16_t terrain_dimensions1;
		#int8_t terrain_border_id[84];
		#int16_t terrain_unit_id[30];
		#int16_t terrain_unit_density[30];
		#int8_t terrain_unit_priority[30];
		#int16_t terrain_units_used_count;
		terrain_struct = Struct(endianness + "2h 13s 13s 5i 3B 5b f 18b 3h 54h 3h 84b 30h 30h 30b h")

		pc = terrain_struct.unpack_from(raw, offset)
		offset += terrain_struct.size

		#self.data[""] = pc[0]
		#self.data[""] = pc[1]
		self.data["name0"]                    = zstr(pc[2])
		self.data["name1"]                    = zstr(pc[3])
		self.data["slp_id"]                   = pc[4]
		#self.data[""] = pc[5]
		self.data["sound_id"]                 = pc[6]
		self.data["blend_priority"]           = pc[7]
		self.data["blend_mode"]               = pc[8]
		self.data["color"]                    = pc[9:(9+3)]
		#self.data[""] = pc[12:(12+5)]
		#self.data[""] = pc[17]
		#self.data[""] = pc[18:(18+18)]

		self.data["frame_count"]              = pc[36]
		self.data["angle_count"]              = pc[37]
		self.data["terrain_id"]               = pc[38]
		self.data["elevation_graphic"]        = pc[39:(39+54)]
		self.data["terrain_replacement_id"]   = pc[93]
		self.data["terrain_dimensions0"]      = pc[94]
		self.data["terrain_dimensions1"]      = pc[95]
		self.data["terrain_border_id"]        = pc[96:(96+84)]
		self.data["terrain_unit_id"]          = pc[180:(180+30)]
		self.data["terrain_unit_density"]     = pc[210:(210+30)]
		self.data["terrain_unit_priority"]    = pc[240:(240+30)]
		self.data["terrain_units_used_count"] = pc[270]

		return offset


class TerrainBorderData:
	def read(self, raw, offset):
		self.data = dict()

		self.data["terrain_border"] = list()
		for i in range(16):
			t = TerrainBorder()
			offset = t.read(raw, offset)
			self.data["terrain_border"] += [t.data]

		#int8_t zero[28];
		#uint16_t terrain_count_additional;
		zero_terrain_count_struct = Struct(endianness + "28c H")
		pc = zero_terrain_count_struct.unpack_from(raw, offset)
		offset += zero_terrain_count_struct.size

		self.data["terrain_count_additional"] = pc[28]

		tmp_struct = Struct(endianness + "12722s")
		t = tmp_struct.unpack_from(raw, offset)
		offset_begin = offset
		offset += tmp_struct.size

		fname = 'raw/terrain_render_data_%d_to_%d.raw' % (offset_begin, offset)
		filename = file_get_path(fname, write=True)
		file_write(filename, t[0])

		return offset


class TerrainBorder:
	def read(self, raw, offset):
		self.data = dict()

		#int16_t enabled;
		#char name0[13];
		#char name1[13];
		#int32_t ressource_id;
		#int32_t unknown;
		#int32_t unknown;
		#uint8_t color[3];
		#int8_t unknown;
		#int32_t unknown;
		#int32_t unknown;
		terrain_border_struct0 = Struct(endianness + "h 13s 13s 3i 3B b 2i")

		pc = terrain_border_struct0.unpack_from(raw, offset)
		offset += terrain_border_struct0.size

		self.data["enabled"]      = pc[0]
		self.data["name0"]        = zstr(pc[1])
		self.data["name1"]        = zstr(pc[2])
		self.data["ressource_id"] = pc[3]
		#self.data[""] = pc[4]
		#self.data[""] = pc[5]
		self.data["color"]        = pc[6:(6+3)]
		#self.data[""] = pc[9]
		#self.data[""] = pc[10]
		#self.data[""] = pc[11]

		self.data["frame_data"] = list()
		for i in range(230):
			t = FrameData()
			offset = t.read(raw, offset)
			self.data["frame_data"] += [t.data]

		#int16_t frame_count;
		#int16_t unknown;
		#int16_t unknown;
		#int16_t unknown;
		terrain_border_struct1 = Struct(endianness + "4h")

		pc = terrain_border_struct1.unpack_from(raw, offset)
		offset += terrain_border_struct1.size

		self.data["frame_count"] = pc[0]
		#self.data[""] = pc[1]
		#self.data[""] = pc[2]
		#self.data[""] = pc[3]

		return offset


class FrameData:
	def read(self, raw, offset):
		self.data = dict()

		#int16_t frame_id;
		#int16_t flag0;
		#int16_t flag1;
		frame_data_struct = Struct(endianness + "3h")

		pc = frame_data_struct.unpack_from(raw, offset)
		offset += frame_data_struct.size

		self.data["frame_id"] = pc[0]
		self.data["flag0"]    = pc[1]
		self.data["flag1"]    = pc[2]

		return offset
