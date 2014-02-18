from struct import Struct, unpack_from
from util import dbg, zstr

from .empiresdat import endianness


class SoundData:
	def read(self, raw, offset):
		#uint16_t sound_count;
		header_struct = Struct(endianness + "H")

		self.sound_count, = header_struct.unpack_from(raw, offset)
		offset += header_struct.size

		self.sound = list()
		for i in range(self.sound_count):
			t = Sound()
			offset = t.read(raw, offset)
			self.sound.append(t)

		return offset


class Sound:
	def read(self, raw, offset):
		#int32_t id;
		#uint16_t item_count;
		#int32_t unknown;
		sound_struct = Struct(endianness + "i H i")

		snd = sound_struct.unpack_from(raw, offset)
		offset += sound_struct.size

		self.id            = snd[0]
		self.item_count    = snd[1]
		#self. = snd[2]

		self.sound_item = list()
		for i in range(self.item_count):
			t = SoundItem()
			offset = t.read(raw, offset)
			self.sound_item.append(t)

		return offset


class SoundItem:
	def read(self, raw, offset):
		#char filename[13];
		#int32_t resource_id;
		#int16_t probability;
		#int16_t civilisation;
		#int16_t unknown;
		sound_item_struct = Struct(endianness + "13s i 3h")

		pc = sound_item_struct.unpack_from(raw, offset)
		offset += sound_item_struct.size

		self.filename      = zstr(pc[0])
		self.resource_id   = pc[1]
		self.probablilty   = pc[2]
		self.civilisation  = pc[3]
		#self. = pc[4]

		return offset
