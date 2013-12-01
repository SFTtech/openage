from struct import Struct, unpack_from
from util import dbg, zstr

endianness = '< '


class UnitHeaderData:
	def read(self, raw, offset):
		self.data = dict()

		#uint32_t unit_count;
		header_struct = Struct(endianness + "I")

		pc = header_struct.unpack_from(raw, offset)
		offset += header_struct.size

		self.data["unit_count"] = pc[0]

		self.data["unit_header"] = list()
		for i in range(self.data["unit_count"]):
			t = UnitHeader()
			offset = t.read(raw, offset)
			self.data["unit_header"] += [t.data]

		return offset


class UnitHeader:
	def read(self, raw, offset):
		self.data = dict()

		#bool exists;
		unit_header_header_struct0 = Struct(endianness + "?")

		pc = unit_header_header_struct0.unpack_from(raw, offset)
		offset += unit_header_header_struct0.size

		self.data["exists"] = pc[0]

		if self.data["exists"] == True:
			unit_header_header_struct1 = Struct(endianness + "H")

			pc = unit_header_header_struct1.unpack_from(raw, offset)
			offset += unit_header_header_struct1.size

			self.data["unit_command_count"] = pc[0]

			self.data["unit_command"] = list()
			for i in range(self.data["unit_command_count"]):
				t = UnitCommand()
				offset = t.read(raw, offset)
				self.data["unit_command"] += [t.data]

		return offset


class UnitCommand:
	def read(self, raw, offset):
		self.data = dict()

		#int16_t one;
		#int16_t id;
		#int8_t unknown;
		#int16_t type;
		#int16_t class_id;
		#int16_t unit_id;
		#int16_t unknown;
		#int16_t ressource_in;
		#int16_t sub_type;
		#int16_t ressource_out;
		#int16_t unknown;
		#float work_rate_multiplier;
		#float execution_radius;
		#float extra_range;
		#int8_t unknown;
		#float unknown;
		#int8_t unknown;
		#int8_t unknown;
		#int32_t unknown;
		#int8_t unknown;
		#int8_t unknown;
		#int8_t unknown;
		#int16_t graphic[6];
		unit_command_struct = Struct(endianness + "2h b 8h 3f b f 2b i 3b 6h")

		pc = unit_command_struct.unpack_from(raw, offset)
		offset += unit_command_struct.size

		self.data["one"] = pc[0]
		self.data["id"] = pc[1]
		#self.data[""] = pc[2]
		self.data["class_id"] = pc[3]
		self.data["unit_id"] = pc[4]
		#self.data[""] = pc[5]
		self.data["ressource_in"] = pc[6]
		self.data["ressource_out"] = pc[7]
		#self.data[""] = pc[8]
		self.data["work_rate_multiplier"] = pc[9]
		self.data["execution_radius"] = pc[10]
		self.data["extra_range"] = pc[11]
		#self.data[""] = pc[12]
		#self.data[""] = pc[13]
		#self.data[""] = pc[14]
		#self.data[""] = pc[15]
		#self.data[""] = pc[16]
		#self.data[""] = pc[17]
		#self.data[""] = pc[18]
		#self.data[""] = pc[19]
		self.data["graphic"] = pc[20:(20+6)]

		return offset
