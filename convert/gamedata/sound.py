from struct import Struct, unpack_from
from util import dbg, zstr

endianness = '< '


class SoundData:
	def read(self, raw, offset):
		self.data = dict()

		#uint16_t sound_count;
		header_struct = Struct(endianness + "H")

		self.data["sound_count"], = header_struct.unpack_from(raw, offset)
		offset += header_struct.size

		self.data["sound"] = list()
		for i in range(self.data["sound_count"]):
			t = Sound()
			offset = t.read(raw, offset)
			self.data["sound"] += [t.data]

		return offset


class Sound:
	def read(self, raw, offset):
		self.data = dict()

		#int32_t id;
		#uint16_t item_count;
		#int32_t unknown;
		sound_struct = Struct(endianness + "i H i")

		snd = sound_struct.unpack_from(raw, offset)
		offset += sound_struct.size

		self.data["id"]            = snd[0]
		self.data["item_count"]    = snd[1]
		#self.data[""] = snd[2]

		self.data["sound_item"] = list()
		for i in range(self.data["item_count"]):
			t = SoundItem()
			offset = t.read(raw, offset)
			self.data["sound_item"] += [t.data]

		return offset


class SoundItem:
	def read(self, raw, offset):
		self.data = dict()

		#char filename[13];
		#int32_t resource_id;
		#int16_t probability;
		#int16_t civilisation;
		#int16_t unknown;
		sound_item_struct = Struct(endianness + "13s i 3h")

		pc = sound_item_struct.unpack_from(raw, offset)
		offset += sound_item_struct.size

		self.data["filename"]      = zstr(pc[0])
		self.data["resource_id"]   = pc[1]
		self.data["probablilty"]   = pc[2]
		self.data["civilisation"]  = pc[3]
		#self.data[""] = pc[4]

		return offset
