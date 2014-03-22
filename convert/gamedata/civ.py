import dataformat
from struct import Struct, unpack_from
from util import dbg, zstr

from .empiresdat import endianness


class CivData:
	def dump(self):
		ret = dict()

		ret.update(dataformat.gather_format(Civ))
		ret["name_table_file"] = "civilisations"
		ret["data"] = list()

		for terrain in self.terrains:
			#dump terrains
			ret["data"].append(terrain.dump())

		return [ ret ]

	def read(self, raw, offset):
		#uint16_t civ_count;
		header_struct = Struct(endianness + "H")

		header = header_struct.unpack_from(raw, offset)
		offset += header_struct.size
		self.civ_count, = header

		self.civ = dict()
		for i in range(self.civ_count):
			t = Civ()
			offset = t.read(raw, offset)
			self.civ[i] = t

		return offset


class Civ:
	name_struct        = "civilisation"
	name_struct_file   = name_struct
	struct_description = "describes one a civilisation."

	data_format = {
		0: {"name":            { "type": "char", "length": 20 }},
		1: {"graphic_set":     "int32_t"},
		2: {"team_bonus_id":   "int16_t"},
		3: {"tech_tree_id":    "int16_t"},
	}

	def read(self, raw, offset):
		#int8_t one;
		#char name[20];
		#uint16_t ressources_count;
		#int16_t tech_tree_id;
		#int16_t team_bonus_id;
		civ_header0_struct = Struct(endianness + "b 20s H 2h")

		pc = civ_header0_struct.unpack_from(raw, offset)
		offset += civ_header0_struct.size

		self.one             = pc[0]
		self.name            = zstr(pc[1])
		self.ressource_count = pc[2]
		self.tech_tree_id    = pc[3]
		self.team_bonus_id   = pc[4]


		#float[ressources_count] ressources;
		civ_ressources_struct = Struct(endianness + "%df" % self.ressource_count)

		pc = civ_ressources_struct.unpack_from(raw, offset)
		offset += civ_ressources_struct.size

		self.ressources = pc


		#int8_t graphic_set;
		#uint16_t unit_count;
		civ_header1_struct = Struct(endianness + "b H")

		pc = civ_header1_struct.unpack_from(raw, offset)
		offset += civ_header1_struct.size

		self.graphic_set = pc[0]
		self.unit_count  = pc[1]


		#int32_t unit_offsets[unit_count];
		civ_header2_struct = Struct(endianness + "%di" % self.unit_count)
		pc = civ_header2_struct.unpack_from(raw, offset)
		offset += civ_header2_struct.size

		self.unit_offsets = pc

		self.unit = list()
		dbg("unit count = %d" % self.unit_count, 2)
		for i in range(self.unit_count):
			dbg("%%%%%%%%%%%%%%%%%%%%%%%%%%%% UNIT entry %d" % i, 2)

			uo = self.unit_offsets[i]
			dbg("unit offset = %d" % uo, 3)
			if uo == 0:
				dbg("skipping UNIT entry %d" % i, 2)
				continue

			from gamedata import unit
			t = unit.Unit()
			offset = t.read(raw, offset)
			self.unit.append(t)

		return offset
