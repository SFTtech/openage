from struct import Struct, unpack_from
from util import dbg, zstr

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
