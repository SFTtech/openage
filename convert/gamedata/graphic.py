from struct import Struct, unpack_from
from util import dbg, zstr

endianness = '< '


class GraphicData:
	def read(self, raw, offset):
		self.data = dict()

		#uint16_t graphic_count;
		header_struct = Struct(endianness + "H")

		self.data["graphic_count"], = header_struct.unpack_from(raw, offset)
		offset += header_struct.size

		#int32_t graphic_offset[graphic_count];
		offset_struct = Struct(endianness + "%di" % self.data["graphic_count"])

		self.data["graphic_offset"] = offset_struct.unpack_from(raw, offset)
		offset += offset_struct.size

		self.data["graphic"] = list()
		for i in range(self.data["graphic_count"]):
			g_offset = self.data["graphic_offset"][i]
			if g_offset == 0:
				#dbg("SKIPPING graphic %d" % i)
				continue

			t = Graphic()
			offset = t.read(raw, offset)
			self.data["graphic"] += [t.data]

			if i > 7016:
				break

		return offset


class Graphic:
	def read(self, raw, offset):
		global lol
		self.data = dict()

		t = GraphicHeader()
		offset = t.read(raw, offset)
		self.data["graphic_header"] = t.data

		self.data["graphic_delta"] = list()
		for i in range(self.data["graphic_header"]["delta_count"]):
			t = GraphicDelta()
			offset = t.read(raw, offset)
			self.data["graphic_delta"] += [t.data]

		if self.data["graphic_header"]["attack_sound_used"] == True:
			self.data["graphic_attack_sound"] = list()
			for i in range(self.data["graphic_header"]["angle_count"]):
				t = GraphicAttackSound()
				offset = t.read(raw, offset)
				self.data["graphic_attack_sound"] += [t.data]

		return offset


class GraphicHeader:
	def read(self, raw, offset):
		self.data = dict()

		#char name0[21];
		#char name1[13];
		#int32_t slp_id;
		#int8_t unknown;
		#int8_t unknown;
		#int8_t layer;
		#int16_t player_color;
		#bool replay;
		#int16_t coordinates[4];
		#uint16_t delta_count;
		#int16_t sound_id;
		#bool attack_sound_used;
		#uint16_t frame_count;
		#uint16_t angle_count;
		#float new_speed;
		#float frame_rate;
		#float replay_delay;
		#int8_t sequence_type;
		#int16_t id;
		#int16_t mirroring_mode;
		graphic_header_struct = Struct(endianness + "21s 13s i 3b h ? 4h H h ? 2H 3f b 2h")

		pc = graphic_header_struct.unpack_from(raw, offset)
		offset += graphic_header_struct.size

		self.data["name0"]             = zstr(pc[0])
		self.data["name1"]             = zstr(pc[1])
		self.data["slp_id"]            = pc[2]
		#self.data[""] = pc[3]
		#self.data[""] = pc[4]
		self.data["layer"]             = pc[5]
		self.data["player_color"]      = pc[6]
		self.data["replay"]            = pc[7]
		self.data["coordinates"]       = pc[8:12]
		self.data["delta_count"]       = pc[12]
		self.data["sound_id"]          = pc[13]
		self.data["attack_sound_used"] = pc[14]
		self.data["frame_count"]       = pc[15]
		self.data["angle_count"]       = pc[16]
		self.data["new_speed"]         = pc[17]
		self.data["frame_rate"]        = pc[18]
		self.data["replay_rate"]       = pc[19]
		self.data["sequence_type"]     = pc[20]
		self.data["id"]                = pc[21]
		self.data["mirroring_mode"]    = pc[22]

		return offset


class GraphicDelta:
	def read(self, raw, offset):
		self.data = dict()

		#int16_t graphic_id;
		#int16_t unknown;
		#int16_t unknown;
		#int16_t unknown;
		#int16_t direction_x;
		#int16_t direction_y;
		#int16_t unknown;
		#int16_t unknown;
		graphic_delta_struct = Struct(endianness + "8h")

		pc = graphic_delta_struct.unpack_from(raw, offset)
		offset += graphic_delta_struct.size

		self.data["graphic_id"]  = pc[0]
		#self.data[""] = pc[1]
		#self.data[""] = pc[2]
		#self.data[""] = pc[3]
		self.data["direction_x"] = pc[4]
		self.data["direction_y"] = pc[5]
		#self.data[""] = pc[6]
		#self.data[""] = pc[7]

		return offset


class GraphicAttackSound:
	def read(self, raw, offset):
		self.data = dict()

		self.data["sound_prop"] = list()
		for i in range(3):
			t = SoundProp()
			offset = t.read(raw, offset)
			self.data["sound_prop"] += [t.data]

		return offset


class SoundProp:
	def read(self, raw, offset):
		self.data = dict()

		#int16_t sound_delay;
		#int16_t sound_id;
		sound_prop_struct = Struct(endianness + "2h")

		pc = sound_prop_struct.unpack_from(raw, offset)
		offset += sound_prop_struct.size

		self.data["sound_delay"] = pc[0]
		self.data["sound_id"]    = pc[1]

		return offset
