from struct import Struct, unpack_from
from util import dbg, zstr

endianness = '< '


class CivData:
	def read(self, raw, offset):
		self.data = dict()

		#uint16_t civ_count;
		header_struct = Struct(endianness + "H")

		header = header_struct.unpack_from(raw, offset)
		offset += header_struct.size
		self.data["civ_count"], = header

		self.data["civ"] = list()
		for i in range(self.data["civ_count"]):
			t = Civ()
			offset = t.read(raw, offset)
			self.data["civ"] += [t.data]

		return offset


class Civ:
	def read(self, raw, offset):
		self.data = dict()

		#int8_t one;
		#char name[20];
		#uint16_t ressources_count;
		#int16_t tech_tree_id;
		#int16_t team_bonus_id;
		civ_header0_struct = Struct(endianness + "b 20s H 2h")

		pc = civ_header0_struct.unpack_from(raw, offset)
		offset += civ_header0_struct.size

		self.data["one"]             = pc[0]
		self.data["name"]            = zstr(pc[1])
		self.data["ressource_count"] = pc[2]
		self.data["tech_tree_id"]    = pc[3]
		self.data["team_bonus_id"]   = pc[4]


		#float[ressources_count] ressources;
		civ_ressources_struct = Struct(endianness + "%df" % self.data["ressource_count"])

		pc = civ_ressources_struct.unpack_from(raw, offset)
		offset += civ_ressources_struct.size

		self.data["ressources"] = pc


		#int8_t graphic_set;
		#uint16_t unit_count;
		civ_header1_struct = Struct(endianness + "b H")

		pc = civ_header1_struct.unpack_from(raw, offset)
		offset += civ_header1_struct.size

		self.data["graphic_set"] = pc[0]
		self.data["unit_count"]  = pc[1]


		#int32_t unit_offsets[unit_count];
		civ_header2_struct = Struct(endianness + "%di" % self.data["unit_count"])
		pc = civ_header2_struct.unpack_from(raw, offset)
		offset += civ_header2_struct.size

		self.data["unit_offsets"] = pc


		self.data["unit"] = list()
		#for i in range(self.data["unit_count"]):
		for i in range(10):
			print("%%%%%%%%%%%%%%%%%%%%%%%%%%%% UNIT entry %d" % i)
			from gamedata import unit
			t = unit.Unit()
			offset = t.read(raw, offset)
			#self.data["unit"] += [t.data]

			import pprint
			pprint.pprint(t.data)

		raise Exception("lol")


		return offset
