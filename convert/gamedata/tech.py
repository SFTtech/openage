from struct import Struct, unpack_from
from util import dbg, zstr, offset_info

endianness = '< '


class TechData:
	def read(self, raw, offset):
		self.data = dict()

		#uint32_t tech_count;
		header_struct = Struct(endianness + "I")

		pc = header_struct.unpack_from(raw, offset)
		offset += header_struct.size

		self.data["tech_count"] = pc[0]

		self.data["tech"] = list()
		for i in range(self.data["tech_count"]):
			t = Tech()
			offset = t.read(raw, offset)
			self.data["tech"] += [t.data]

		return offset


class Tech:
	def read(self, raw, offset):
		self.data = dict()

		#char name[31];
		#uint16_t effect_count;
		tech_struct = Struct(endianness + "31s H")

		pc = tech_struct.unpack_from(raw, offset)
		offset += tech_struct.size

		self.data["name"]         = zstr(pc[0])
		self.data["effect_count"] = pc[1]

		self.data["effect"] = list()
		for i in range(self.data["effect_count"]):
			t = Effect()
			offset = t.read(raw, offset)
			self.data["effect"] += [t.data]

		return offset


class Effect:
	def read(self, raw, offset):
		self.data = dict()

		#int8_t type;
		#int16_t unit;
		#int16_t class;
		#int16_t attribute;
		#float amount;
		effect_struct = Struct(endianness + "b 3h f")

		pc = effect_struct.unpack_from(raw, offset)
		offset += effect_struct.size

		self.data["type"]      = pc[0]
		self.data["unit"]      = pc[1]
		self.data["class"]     = pc[2]
		self.data["attribute"] = pc[3]
		self.data["amount"]    = pc[4]

		return offset


class TechtreeData:
	def read(self, raw, offset):
		self.data = dict()

		#int8_t age_entry_count;
		#int8_t building_connection_count;
		#int8_t unit_connection_count;
		#int8_t research_connection_count;
		header_struct = Struct(endianness + "4b")

		header = header_struct.unpack_from(raw, offset)
		offset_info(offset, header, "techtree: entries[age, building, unit, research]", header_struct)
		offset += header_struct.size

		self.data["age_entry_count"]      = header[0]
		self.data["building_connection_count"] = header[1]
		self.data["unit_connection_count"]     = header[2]
		self.data["research_connection_count"] = header[3]

		self.data["age_tech_tree"] = list()
		for i in range(self.data["age_entry_count"]):
			t = AgeTechTree()
			offset = t.read(raw, offset)
			self.data["age_tech_tree"] += [t.data]

		tmp_struct = Struct(endianness + "i")
		offset += tmp_struct.size

		self.data["building_connection"] = list()
		for i in range(self.data["building_connection_count"]):
			dbg("=== building_connection %d" % i)
			t = BuildingConnection()
			offset = t.read(raw, offset)
			self.data["building_connection"] += [t.data]

		self.data["unit_connection"] = list()
		for i in range(self.data["unit_connection_count"]):
			dbg("=== unit_connection %d" % i)
			t = UnitConnection()
			offset = t.read(raw, offset)
			self.data["unit_connection"] += [t.data]

		self.data["research_connection"] = list()
		for i in range(self.data["research_connection_count"]):
			dbg("=== research_connection %d" % i)
			t = ResearchConnection()
			offset = t.read(raw, offset)
			self.data["research_connection"] += [t.data]

		return offset


class AgeTechTree:
	def read(self, raw, offset):
		self.data = dict()

		#int32_t unknown;
		#int32_t id;
		#int8_t unknown;
		#int8_t building_count;
		age_tech_tree_struct = Struct(endianness + "2i 2b")

		pc = age_tech_tree_struct.unpack_from(raw, offset)
		offset_info(offset, pc, "techtree: X id X buildingcount", age_tech_tree_struct, 1)
		offset += age_tech_tree_struct.size

		#self.data[""] = pc[0]
		self.data["id"]             = pc[1]
		#self.data[""] = pc[2]
		self.data["building_count"] = pc[3]

		#int32_t building[building_count];
		age_tech_tree_struct = Struct(endianness + "%di" % self.data["building_count"])
		pc = age_tech_tree_struct.unpack_from(raw, offset)
		offset_info(offset, pc, "techtree: building", age_tech_tree_struct, 1)
		offset += age_tech_tree_struct.size
		self.data["building"] = pc

		#int8_t unit_count;
		age_tech_tree_struct = Struct(endianness + "b")
		pc = age_tech_tree_struct.unpack_from(raw, offset)
		offset_info(offset, pc, "techtree: unitcount", age_tech_tree_struct, 1)
		offset += age_tech_tree_struct.size
		self.data["unit_count"] = pc[0]

		#int32_t unit[unit_count];
		age_tech_tree_struct = Struct(endianness + "%di" % self.data["unit_count"])
		pc = age_tech_tree_struct.unpack_from(raw, offset)
		offset_info(offset, pc, "techtree: unit", age_tech_tree_struct, 1)
		offset += age_tech_tree_struct.size
		self.data["unit"] = pc

		#int8_t research_count;
		age_tech_tree_struct = Struct(endianness + "b")
		pc = age_tech_tree_struct.unpack_from(raw, offset)
		offset_info(offset, pc, "techtree: researchcount", age_tech_tree_struct, 1)
		offset += age_tech_tree_struct.size
		self.data["research_count"] = pc[0]

		#int32_t research[research_count];
		age_tech_tree_struct = Struct(endianness + "%di" % self.data["research_count"])
		pc = age_tech_tree_struct.unpack_from(raw, offset)
		offset_info(offset, pc, "techtree: research", age_tech_tree_struct, 1)
		offset += age_tech_tree_struct.size
		self.data["research"] = pc

		#int32_t unknown;
		#int32_t unknown;
		#int16_t zeroes[49];
		age_tech_tree_struct = Struct(endianness + "2i 49h")

		pc = age_tech_tree_struct.unpack_from(raw, offset)
		offset_info(offset, pc, "techtree: X X zeroes", age_tech_tree_struct)
		offset += age_tech_tree_struct.size

		#self.data[""] = pc[0]
		#self.data[""] = pc[1]
		#self.data[""] = pc[2:(2+49)]

		return offset



class BuildingConnection:
	def read(self, raw, offset):
		self.data = dict()

		#int32_t id;
		#int8_t unknown;
		#int8_t building_count;
		building_connection_struct = Struct(endianness + "i 2b")

		pc = building_connection_struct.unpack_from(raw, offset)
		offset_info(offset, pc, "bconnection: id X building_count", building_connection_struct, 1)
		offset += building_connection_struct.size

		self.data["id"]             = pc[0]
		#self.data[""] = pc[1]
		self.data["building_count"] = pc[2]

		#int32_t building[building_count];
		building_connection_struct = Struct(endianness + "%di" % self.data["building_count"])
		pc = building_connection_struct.unpack_from(raw, offset)
		offset_info(offset, pc, "bconnection: buildings", building_connection_struct, 1)
		offset += building_connection_struct.size
		self.data["building"] = pc

		#int8_t unit_count;
		building_connection_struct = Struct(endianness + "b")
		pc = building_connection_struct.unpack_from(raw, offset)
		offset_info(offset, pc, "bconnection: unit_count", building_connection_struct, 1)
		offset += building_connection_struct.size
		self.data["unit_count"] = pc[0]

		#int32_t unit[unit_count];
		building_connection_struct = Struct(endianness + "%di" % self.data["unit_count"])
		pc = building_connection_struct.unpack_from(raw, offset)
		offset_info(offset, pc, "bconnection: unit", building_connection_struct, 1)
		offset += building_connection_struct.size
		self.data["unit"] = pc

		#int8_t research_count;
		building_connection_struct = Struct(endianness + "b")
		pc = building_connection_struct.unpack_from(raw, offset)
		offset_info(offset, pc, "bconnection: research_count", building_connection_struct, 1)
		offset += building_connection_struct.size
		self.data["research_count"] = pc[0]

		#int32_t research[research_count];
		building_connection_struct = Struct(endianness + "%di" % self.data["research_count"])
		pc = building_connection_struct.unpack_from(raw, offset)
		offset_info(offset, pc, "bconnection: research", building_connection_struct, 1)
		offset += building_connection_struct.size
		self.data["research"] = pc

		#int32_t age;
		#int32_t unit_or_research0;
		#int32_t unit_or_research1;
		#int32_t unknown[8];
		#int32_t mode0;
		#int32_t mode1;
		#int32_t unknown[7];
		#int8_t unknown[11];
		#int32_t connection;
		#int32_t enabling_research;
		building_connection_struct = Struct(endianness + "3i 8i 2i 7i xxxx 11b 2i")

		pc = building_connection_struct.unpack_from(raw, offset)
		offset_info(offset, pc, "bconnection: age ur0 ur1 X[8] mode0 mode1 X[18] connection enablingresearch", building_connection_struct)
		offset += building_connection_struct.size

		self.data["age"]               = pc[0]
		self.data["unit_or_research0"] = pc[1]
		self.data["unit_or_research1"] = pc[2]
		#self.data[""] = pc[3:(3+8)]
		self.data["mode0"]             = pc[11]
		self.data["mode1"]             = pc[12]
		#self.data[""] = pc[13:(13+7)]
		#self.data[""] = pc[20:(20+11)]
		self.data["connection"]        = pc[31]
		self.data["enabling_research"] = pc[32]

		return offset


class UnitConnection:
	def read(self, raw, offset):
		self.data = dict()

		#int32_t id;
		#int8_t unknown;
		#int32_t upper_building;
		#int32_t required_researches;
		#int32_t age;
		#int32_t unit_or_research0;
		#int32_t unit_or_research1;
		#int32_t unknown[8];
		#int32_t mode0;
		#int32_t mode1;
		#int32_t unknown[7];
		#int32_t vertical_line;
		#int8_t unit_count;
		unit_connection_struct = Struct(endianness + "i b 5i 8i 2i 7i i b")

		pc = unit_connection_struct.unpack_from(raw, offset)
		offset_info(offset, pc, "uconnection: id X upperbuilding req_researches age ur0 ur1 X[8] mode0 mode1 X[7] verticalline unitcount", unit_connection_struct)
		offset += unit_connection_struct.size

		self.data["id"]                  = pc[0]
		#self.data[""] = pc[1]
		self.data["upper_building"]      = pc[2]
		self.data["required_researches"] = pc[3]
		self.data["age"]                 = pc[4]
		self.data["unit_or_research0"]   = pc[5]
		self.data["unit_or_research1"]   = pc[6]
		#self.data[""] = pc[7:(7+8)]
		self.data["mode0"]               = pc[15]
		self.data["mode1"]               = pc[16]
		#self.data[""] = pc[17:(17+7)]
		self.data["vertical_line"]       = pc[24]
		self.data["unit_count"]          = pc[25]

		#int32_t unit[unit_count];
		unit_connection_struct = Struct(endianness + "%di" % self.data["unit_count"])
		pc = unit_connection_struct.unpack_from(raw, offset)
		offset_info(offset, pc, "uconnection: unit", unit_connection_struct, 1)
		offset += unit_connection_struct.size

		self.data["unit"] = pc

		#int32_t location_in_age;
		#int32_t required_research;
		#int32_t line_mode;
		#int32_t enabling_research;
		unit_connection_struct = Struct(endianness + "4i")

		pc = unit_connection_struct.unpack_from(raw, offset)
		offset_info(offset, pc, "uconnection: locationinage requiredresearch linemode enablingresearch", unit_connection_struct)
		offset += unit_connection_struct.size

		self.data["location_in_age"]   = pc[0]
		self.data["required_research"] = pc[1]
		self.data["line_mode"]         = pc[2]
		self.data["enabling_research"] = pc[3]

		return offset


class ResearchConnection:
	def read(self, raw, offset):
		self.data = dict()

		#int32_t id;
		#int8_t unknown;
		#int32_t upper_building;
		#int8_t building_count;
		research_connection_struct = Struct(endianness + "i b i b")

		pc = research_connection_struct.unpack_from(raw, offset)
		offset_info(offset, pc, "rconnection: id X upperbuilding building_count", research_connection_struct)
		offset += research_connection_struct.size

		self.data["id"]                  = pc[0]
		#self.data[""] = pc[1]
		self.data["upper_building"]      = pc[2]
		self.data["building_count"]      = pc[3]

		#int32_t building[building_count];
		research_connection_struct = Struct(endianness + "%di" % self.data["building_count"])
		pc = research_connection_struct.unpack_from(raw, offset)
		offset_info(offset, pc, "rconnection: building", research_connection_struct, 1)
		offset += research_connection_struct.size
		self.data["building"] = pc

		#int8_t unit_count;
		research_connection_struct = Struct(endianness + "b")
		pc = research_connection_struct.unpack_from(raw, offset)
		offset_info(offset, pc, "rconnection: unit_count", research_connection_struct, 1)
		offset += research_connection_struct.size
		self.data["unit_count"] = pc[0]

		#int32_t unit[unit_count];
		research_connection_struct = Struct(endianness + "%di" % self.data["unit_count"])
		pc = research_connection_struct.unpack_from(raw, offset)
		offset_info(offset, pc, "rconnection: unit", research_connection_struct, 1)
		offset += research_connection_struct.size
		self.data["unit"] = pc

		#int8_t research_count;
		research_connection_struct = Struct(endianness + "b")
		pc = research_connection_struct.unpack_from(raw, offset)
		offset_info(offset, pc, "rconnection: research_count", research_connection_struct, 1)
		offset += research_connection_struct.size
		self.data["research_count"] = pc[0]

		#int32_t research[research_count];
		research_connection_struct = Struct(endianness + "%di" % self.data["research_count"])
		pc = research_connection_struct.unpack_from(raw, offset)
		offset_info(offset, pc, "rconnection: research", research_connection_struct, 1)
		offset += research_connection_struct.size
		self.data["research"] = pc

		#int32_t required_research;
		#int32_t age;
		#int32_t upper_research;
		#int32_t unknown[9];
		#int32_t line_mode;
		#int32_t unknown[8];
		#int32_t vertical_line;
		#int32_t location_in_age;
		#int32_t unknown;
		research_connection_struct = Struct(endianness + "3i 9i i 8i 3i")
		pc = research_connection_struct.unpack_from(raw, offset)
		offset_info(offset, pc, "rconnection: requiredresearch age upperresearch X[9] linemode X[8] vericalline locationinage X", research_connection_struct)
		offset += research_connection_struct.size

		self.data["required_research"] = pc[0]
		self.data["age"] = pc[1]
		self.data["upper_research"] = pc[2]
		#self.data[""] = pc[3:(3+9)]
		self.data["line_mode"] = pc[12]
		#self.data[""] = pc[13:(13+8)]
		self.data["vertical_line"] = pc[21]
		self.data["location_in_age"] = pc[22]
		#self.data[""] = pc[23]

		return offset
