import dataformat
from struct import Struct, unpack_from
from util import dbg, zstr
from util import file_get_path, file_write

from .empiresdat import endianness


class TerrainHeaderData:

	def read(self, raw, offset):
		#uint16_t terrain_restriction_count;
		#uint16_t terrain_count;
		header_struct = Struct(endianness + "H H")

		header = header_struct.unpack_from(raw, offset)
		offset += header_struct.size
		tr_count, t_count = header
		self.terrain_restriction_count = tr_count
		self.terrain_count = t_count

		#int32_t terrain_restriction_offset0[terrain_restriction_count];
		#int32_t terrain_restriction_offset1[terrain_restriction_count];
		tr_offset_struct = Struct(endianness + "%di" % self.terrain_restriction_count)

		self.terrain_restriction_offset0 = tr_offset_struct.unpack_from(raw, offset)
		offset += tr_offset_struct.size
		self.terrain_restriction_offset1 = tr_offset_struct.unpack_from(raw, offset)
		offset += tr_offset_struct.size

		self.terrain_restriction = list()
		for i in range(self.terrain_restriction_count):
			t = TerrainRestriction(self.terrain_count)
			offset = t.read(raw, offset)
			self.terrain_restriction.append(t)

		return offset


class TerrainRestriction:
	def __init__(self, terrain_count):
		self.terrain_count = terrain_count

		#float terrain_accessible[terrain_count];
		self.terrain_accessible_struct = Struct(endianness + "%df" % terrain_count)

	def read(self, raw, offset):
		self.terrain_accessible = self.terrain_accessible_struct.unpack_from(raw, offset)
		offset += self.terrain_accessible_struct.size

		self.terrain_pass_graphic = list()
		for i in range(self.terrain_count):
			t = TerrainPassGraphic()
			offset = t.read(raw, offset)
			self.terrain_pass_graphic.append(t)

		return offset


class TerrainPassGraphic:
	def read(self, raw, offset):
		#int32_t buildable;
		#int32_t graphic_id0;
		#int32_t graphic_id1;
		#int32_t replication_amount;
		terrain_pass_graphic_struct = Struct(endianness + "i i i i")

		pg = terrain_pass_graphic_struct.unpack_from(raw, offset)
		offset += terrain_pass_graphic_struct.size

		self.buildable          = pg[0]
		self.graphic_id0        = pg[1]
		self.graphic_id1        = pg[2]
		self.replication_amount = pg[3]

		return offset


class TerrainData:
	def __init__(self, terrain_count):
		self.terrain_count = terrain_count

	def dump(self):
		ret = dict()

		ret.update(dataformat.gather_format(Terrain))
		ret["name_table_file"] = "terrain_data"
		ret["data"] = list()

		for terrain in self.terrains:
			#dump terrains
			ret["data"].append(terrain.dump())

		return [ ret ]

	def read(self, raw, offset):
		self.terrains = list()
		for i in range(self.terrain_count):
			t = Terrain()
			offset = t.read(raw, offset)
			self.terrains.append(t)

		return offset


class Terrain:
	name_struct        = "terrain_type"
	name_struct_file   = "terrain"
	struct_description = "describes a terrain type, like water, ice, etc."

	data_format = {
		0: {"terrain_id":     "int32_t"},
		1: {"slp_id":         "int32_t"},
		2: {"blend_mode":     "int32_t"},
		3: {"blend_priority": "int32_t"},
		4: {"name0":          { "type": "char", "length": 13 }},
		5: {"name1":          { "type": "char", "length": 13 }},
	}

	def dump(self):
		return dataformat.gather_data(self, self.data_format)

	def read(self, raw, offset):
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

		#self. = pc[0]
		#self. = pc[1]
		self.name0                    = zstr(pc[2])
		self.name1                    = zstr(pc[3])
		self.slp_id                   = pc[4]
		#self. = pc[5]
		self.sound_id                 = pc[6]
		self.blend_priority           = pc[7]
		self.blend_mode               = pc[8]
		self.color                    = pc[9:(9+3)]
		#self. = pc[12:(12+5)]
		#self. = pc[17]
		#self. = pc[18:(18+18)]
		self.frame_count              = pc[36]
		self.angle_count              = pc[37]
		self.terrain_id               = pc[38]
		self.elevation_graphic        = pc[39:(39+54)]
		self.terrain_replacement_id   = pc[93]
		self.terrain_dimensions0      = pc[94]
		self.terrain_dimensions1      = pc[95]
		self.terrain_border_id        = pc[96:(96+84)]
		self.terrain_unit_id          = pc[180:(180+30)]
		self.terrain_unit_density     = pc[210:(210+30)]
		self.terrain_unit_priority    = pc[240:(240+30)]
		self.terrain_units_used_count = pc[270]

		return offset


class TerrainBorderData:
	def read(self, raw, offset):

		self.terrain_border = list()
		for i in range(16):
			t = TerrainBorder()
			offset = t.read(raw, offset)
			self.terrain_border.append(t)

		#int8_t zero[28];
		#uint16_t terrain_count_additional;
		zero_terrain_count_struct = Struct(endianness + "28c H")
		pc = zero_terrain_count_struct.unpack_from(raw, offset)
		offset += zero_terrain_count_struct.size

		self.terrain_count_additional = pc[28]

		tmp_struct = Struct(endianness + "12722s")
		t = tmp_struct.unpack_from(raw, offset)
		offset_begin = offset
		offset += tmp_struct.size

		#dump of unknown binary section.
		#it may contain unicorns, so don't hesitate to reverse it.
		fname = 'raw/terrain_render_data_%d_to_%d.raw' % (offset_begin, offset)
		filename = file_get_path(fname, write=True)
		file_write(filename, t[0])

		return offset


class TerrainBorder:
	def read(self, raw, offset):
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

		self.enabled      = pc[0]
		self.name0        = zstr(pc[1])
		self.name1        = zstr(pc[2])
		self.ressource_id = pc[3]
		#self. = pc[4]
		#self. = pc[5]
		self.color        = pc[6:(6+3)]
		#self. = pc[9]
		#self. = pc[10]
		#self. = pc[11]

		self.frame_data = list()
		for i in range(230):
			t = FrameData()
			offset = t.read(raw, offset)
			self.frame_data.append(t)

		#int16_t frame_count;
		#int16_t unknown;
		#int16_t unknown;
		#int16_t unknown;
		terrain_border_struct1 = Struct(endianness + "4h")

		pc = terrain_border_struct1.unpack_from(raw, offset)
		offset += terrain_border_struct1.size

		self.frame_count = pc[0]
		#self. = pc[1]
		#self. = pc[2]
		#self. = pc[3]

		return offset


class FrameData:
	def read(self, raw, offset):
		#int16_t frame_id;
		#int16_t flag0;
		#int16_t flag1;
		frame_data_struct = Struct(endianness + "3h")

		pc = frame_data_struct.unpack_from(raw, offset)
		offset += frame_data_struct.size

		self.frame_id = pc[0]
		self.flag0    = pc[1]
		self.flag1    = pc[2]

		return offset
