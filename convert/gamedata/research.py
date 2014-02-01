from struct import Struct, unpack_from
from util import dbg, zstr

endianness = '< '


class ResearchData:
	def read(self, raw, offset):
		self.data = dict()

		#uint16_t research_count;
		header_struct = Struct(endianness + "H")

		header = header_struct.unpack_from(raw, offset)
		offset += header_struct.size
		self.data["research_count"], = header

		self.data["research"] = list()
		for i in range(self.data["research_count"]):
			t = Research()
			offset = t.read(raw, offset)
			self.data["research"] += [t.data]

		return offset


class Research:
	def read(self, raw, offset):
		self.data = dict()

		#int16_t[6] required_techs;
		research0_struct = Struct(endianness + "6h")

		pc = research0_struct.unpack_from(raw, offset)
		offset += research0_struct.size

		self.data["required_techs"] = pc[0]

		self.data["research_ressource_cost"] = list()
		for i in range(3):
			t = ResearchRessourceCost()
			offset = t.read(raw, offset)
			self.data["research_ressource_cost"] += [t.data]


		#int16_t required_tech_count;
		#int16_t civilisation_id;
		#int16_t full_tech_mode;
		#int16_t research_location_id;
		#uint16_t language_dll_name;
		#uint16_t language_dll_description;
		#int16_t research_time;
		#int16_t tech_id;
		#int16_t tech_type;
		#int16_t icon_id;
		#int8_t button_id;
		#int32_t pointers[3];
		#uint16_t name_length;
		research1_struct = Struct(endianness + "4h 2H 4h b 3i H")

		pc = research1_struct.unpack_from(raw, offset)
		offset += research1_struct.size

		self.data["required_tech_count"] = pc[0]
		self.data["civilisation_id"] = pc[1]
		self.data["full_tech_mode"] = pc[2]
		self.data["research_location_id"] = pc[3]
		self.data["language_dll_name"] = pc[4]
		self.data["language_dll_description"] = pc[5]
		self.data["research_time"] = pc[6]
		self.data["tech_id"] = pc[7]
		self.data["tech_type"] = pc[8]
		self.data["icon_id"] = pc[9]
		self.data["button_id"] = pc[10]
		self.data["pointers"] = pc[11:(11+3)]
		self.data["name_length"] = pc[14]

		research_name_struct = Struct(endianness + "%ds" % self.data["name_length"])

		pc = research_name_struct.unpack_from(raw, offset)
		offset += research_name_struct.size

		#char name[name_length];
		self.data["name"] = zstr(pc[0])

		return offset

class ResearchRessourceCost:
	def read(self, raw, offset):
		self.data = dict()

		#int16_t a;
		#int16_t b;
		#int8_t c;
		research_ressource_cost_struct = Struct(endianness + "2h b")

		pc = research_ressource_cost_struct.unpack_from(raw, offset)
		offset += research_ressource_cost_struct.size

		self.data["a"] = pc[0]
		self.data["b"] = pc[1]
		self.data["c"] = pc[2]

		return offset
