import dataformat
from struct import Struct, unpack_from
from util import dbg, zstr

from .empiresdat import endianness


class SoundItem:
	name_struct        = "sound_item"
	name_struct_file   = "sound"
	struct_description = "one possible file for a sound."

	data_format = {
		0: {"filename":       {"type": "char", "length": 13}},
		1: {"resource_id":     "int16_t"},
		2: {"probablilty":     "int16_t"},
		3: {"civilisation":    "int16_t"},
	}

	def dump(self):
		return dataformat.gather_data(self, self.data_format)

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


class Sound:
	name_struct        = "sound"
	name_struct_file   = "sound"
	struct_description = "describes a sound, consisting of several sound items."

	data_format = {
		0: {"uid":            "int32_t"},
		1: {"item_count":     "int32_t"},
		2: {"sound_item":     {"type": "subdata", "ref_type": SoundItem}}
	}

	def dump(self):
		return dataformat.gather_data(self, self.data_format)

	def read(self, raw, offset):
		#int32_t uid;
		#uint16_t item_count;
		#int32_t unknown;
		sound_struct = Struct(endianness + "i H i")

		snd = sound_struct.unpack_from(raw, offset)
		offset += sound_struct.size

		self.uid           = snd[0]
		self.item_count    = snd[1]
		#self. = snd[2]

		self.sound_item = list()
		for i in range(self.item_count):
			t = SoundItem()
			offset = t.read(raw, offset)
			self.sound_item.append(t)

		return offset


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

	def dump(self):
		ret = dict()

		ret.update(dataformat.gather_format(Sound))
		ret["name_table_file"] = "sound_data"
		ret["data"] = list()

		for sound in self.sound:
			#dump terrains
			ret["data"].append(sound.dump())

		return [ ret ]
