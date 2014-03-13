from struct import Struct, unpack_from
from util import dbg, zstr

from .empiresdat import endianness


class GraphicData:
	def read(self, raw, offset):
		#uint16_t graphic_count;
		header_struct = Struct(endianness + "H")

		self.graphic_count, = header_struct.unpack_from(raw, offset)
		offset += header_struct.size

		#int32_t graphic_offset[graphic_count];
		offset_struct = Struct(endianness + "%di" % self.graphic_count)

		self.graphic_offset = offset_struct.unpack_from(raw, offset)
		offset += offset_struct.size

		self.graphic = list()
		for i in range(self.graphic_count):
			g_offset = self.graphic_offset[i]
			if g_offset == 0:
				#dbg("SKIPPING graphic %d" % i)
				continue

			t = Graphic()
			offset = t.read(raw, offset)
			self.graphic.append(t)

		#int8_t[138] rendering_data;
		rendering_data_struct = Struct(endianness + "138c")

		self.rendering_data = rendering_data_struct.unpack_from(raw, offset)
		offset += rendering_data_struct.size

		return offset


class Graphic:
	def read(self, raw, offset):
		t = GraphicHeader()
		offset = t.read(raw, offset)
		self.graphic_header = t

		self.graphic_delta = list()
		for i in range(self.graphic_header.delta_count):
			t = GraphicDelta()
			offset = t.read(raw, offset)
			self.graphic_delta.append(t)

		if self.graphic_header.attack_sound_used == True:
			self.graphic_attack_sound = list()
			for i in range(self.graphic_header.angle_count):
				t = GraphicAttackSound()
				offset = t.read(raw, offset)
				self.graphic_attack_sound.append(t)

		return offset


class GraphicHeader:
	def read(self, raw, offset):
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
		#int16_t uid;
		#int16_t mirroring_mode;
		graphic_header_struct = Struct(endianness + "21s 13s i 3b h ? 4h H h ? 2H 3f b 2h")

		pc = graphic_header_struct.unpack_from(raw, offset)
		offset += graphic_header_struct.size

		self.name0             = zstr(pc[0])
		self.name1             = zstr(pc[1])
		self.slp_id            = pc[2]
		#self. = pc[3]
		#self. = pc[4]
		self.layer             = pc[5]
		self.player_color      = pc[6]
		self.replay            = pc[7]
		self.coordinates       = pc[8:12]
		self.delta_count       = pc[12]
		self.sound_id          = pc[13]
		self.attack_sound_used = pc[14]
		self.frame_count       = pc[15]
		self.angle_count       = pc[16]
		self.new_speed         = pc[17]
		self.frame_rate        = pc[18]
		self.replay_rate       = pc[19]
		self.sequence_type     = pc[20]
		self.uid               = pc[21]
		self.mirroring_mode    = pc[22]

		return offset


class GraphicDelta:
	def read(self, raw, offset):
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

		self.graphic_id  = pc[0]
		#self. = pc[1]
		#self. = pc[2]
		#self. = pc[3]
		self.direction_x = pc[4]
		self.direction_y = pc[5]
		#self. = pc[6]
		#self. = pc[7]

		return offset


class GraphicAttackSound:
	def read(self, raw, offset):
		self.sound_prop = list()
		for i in range(3):
			t = SoundProp()
			offset = t.read(raw, offset)
			self.sound_prop.append(t)

		return offset


class SoundProp:
	def read(self, raw, offset):
		#int16_t sound_delay;
		#int16_t sound_id;
		sound_prop_struct = Struct(endianness + "2h")

		pc = sound_prop_struct.unpack_from(raw, offset)
		offset += sound_prop_struct.size

		self.sound_delay = pc[0]
		self.sound_id    = pc[1]

		return offset
