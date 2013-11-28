from struct import Struct, unpack_from
from util import dbg

endianness = '< '


class PlayerColorData:
	def read(self, raw, offset):
		self.data = dict()

		#uint16_t player_color_count;
		header_struct = Struct(endianness + "h")

		self.data["player_color_count"], = header_struct.unpack_from(raw, offset)
		offset += header_struct.size

		self.data["player_color"] = list()
		for i in range(self.data["player_color_count"]):
			t = PlayerColor()
			offset = t.read(raw, offset)
			self.data["player_color"] += [t.data]

		return offset


class PlayerColor:
	def read(self, raw, offset):
		self.data = dict()

		#int32_t id;
		#int32_t palette;
		#int32_t color;
		#int32_t unknown;
		#int32_t unknown;
		#int32_t minimap_color;
		#int32_t unknown;
		#int32_t unknown;
		#int32_t unknown;
		player_color_struct = Struct(endianness + "9i")

		pc = player_color_struct.unpack_from(raw, offset)
		offset += player_color_struct.size

		self.data["id"]            = pc[0]
		self.data["palette"]       = pc[1]
		self.data["color"]         = pc[2]
		#self.data[""] = pc[0]
		#self.data[""] = pc[0]
		self.data["minimap_color"] = pc[5]
		#self.data[""] = pc[0]
		#self.data[""] = pc[0]

		return offset
