from struct import Struct, unpack_from
from util import dbg, zstr, check_file, offset_info
import math


from .empiresdat import endianness


class UnitHeaderData:
	def read(self, raw, offset):
		#uint32_t unit_count;
		header_struct = Struct(endianness + "I")

		pc = header_struct.unpack_from(raw, offset)
		offset += header_struct.size

		self.unit_count = pc[0]

		self.unit_header = list()
		for i in range(self.unit_count):
			t = UnitHeader()
			offset = t.read(raw, offset)
			self.unit_header.append(t)

		return offset


class UnitHeader:
	def read(self, raw, offset):
		#bool exists;
		unit_header_header_struct0 = Struct(endianness + "?")

		pc = unit_header_header_struct0.unpack_from(raw, offset)
		offset += unit_header_header_struct0.size

		self.exists = pc[0]

		if self.exists == True:
			unit_header_header_struct1 = Struct(endianness + "H")

			pc = unit_header_header_struct1.unpack_from(raw, offset)
			offset += unit_header_header_struct1.size

			self.unit_command_count = pc[0]

			self.unit_command = list()
			for i in range(self.unit_command_count):
				t = UnitCommand()
				offset = t.read(raw, offset)
				self.unit_command.append(t)

		return offset


class UnitCommand:
	def read(self, raw, offset):
		#int16_t one;
		#int16_t uid;
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

		self.one = pc[0]
		self.uid = pc[1]
		#self. = pc[2]
		self.class_id = pc[3]
		self.unit_id = pc[4]
		#self. = pc[5]
		self.ressource_in = pc[6]
		self.ressource_out = pc[7]
		#self. = pc[8]
		self.work_rate_multiplier = pc[9]
		self.execution_radius = pc[10]
		self.extra_range = pc[11]
		#self. = pc[12]
		#self. = pc[13]
		#self. = pc[14]
		#self. = pc[15]
		#self. = pc[16]
		#self. = pc[17]
		#self. = pc[18]
		#self. = pc[19]
		self.graphic = pc[20:(20+6)]

		return offset


class Unit:
	def read(self, raw, offset):
		#unit type==:
		#object = 10
		#flag = 20
		#dead_fish = 30
		#bird = 40
		#projectile = 60
		#creatable = 70
		#building = 80
		#tree = 90

		#int8_t type_id;
		#uint16_t name_length;
		#int16_t id0;
		#uint16_t language_dll_name;
		#uint16_t language_dll_creation;
		#int16_t unit_class;
		#int16_t graphic_standing0;
		#int16_t graphic_standing1;
		#int16_t graphic_dying0;
		#int16_t graphic_dying1;
		#int8_t death_mode;
		#int16_t hit_points;
		#float line_of_sight;
		#int8_t garnison_capacity;
		#float radius_size0;
		#float radius_size1;
		#float hp_bar_height0;
		#int16_t sound_train0;
		#int16_t sound_train1;
		#int16_t dead_unit_id;
		#int8_t placement_mode;
		#int8_t air_mode;
		#int16_t icon_id;
		#int8_t hidden_in_editor;
		#int16_t unknown;
		#int16_t enabled;
		#int16_t placement_by_pass_terrain0;
		#int16_t placement_by_pass_terrain1;
		#int16_t placement_terrain0;
		#int16_t placement_terrain1;
		#float editor_radius0;
		#float editor_radius1;
		#int8_t building_mode;
		#int8_t visible_in_fog;
		#int16_t terrain_restriction;
		#int8_t fly_mode;
		#int16_t ressource_capacity;
		#float ressource_decay;
		#int8_t blast_type;
		#int8_t unknown;
		#int8_t interaction_mode;
		#int8_t minimap_mode;
		#int16_t command_attribute;
		#int16_t unknown;
		#int16_t unknown;
		#uint16_t language_dll_help;
		#int16_t[4] hot_keys;
		#int8_t unknown;
		#int8_t unknown;
		#bool unselectable;
		#int8_t unknown;
		#int8_t unknown;
		#int8_t unknown;
		#int8_t selection_mask;
		#int8_t selection_shape_type;
		#int8_t selection_shape;
		#int8_t attribute;
		#int8_t civilisation;
		#int16_t unknown;
		#int8_t selection_effect;
		#uint8_t editor_selection_color;
		#float selection_radius0;
		#float selection_radius1;
		#float hp_bar_height1;
		unit0_struct = Struct(endianness + "b H h 2H 5h b h f b 3f 3h 2b h b 6h 2f 2b h b h f 4b 3h H 4h 2b ? 8b h b B 3f")

		pc = unit0_struct.unpack_from(raw, offset)
		offset_info(offset, pc, "unit header0", unit0_struct)
		offset += unit0_struct.size


		self.type_id                    = pc[0]
		self.name_length                = pc[1]

		if self.name_length > 100:
			raise Exception("name is longer than 100, very unlikely: %d" % self.name_length)

		self.id0                        = pc[2]
		self.language_dll_name          = pc[3]
		self.language_dll_creation      = pc[4]
		self.unit_class                 = pc[5]
		self.graphic_standing0          = pc[6]
		check_file(pc[6])
		self.graphic_standing1          = pc[7]
		check_file(pc[7])
		self.graphic_dying0             = pc[8]
		self.graphic_dying1             = pc[9]
		self.death_mode                 = pc[10]
		self.hit_points                 = pc[11]
		self.line_of_sight              = pc[12]
		self.garnison_capacity          = pc[13]
		self.radius_size0               = pc[14]
		self.radius_size1               = pc[15]
		self.hp_bar_height0             = pc[16]
		self.sound_train0               = pc[17]
		check_file(pc[17])
		self.sound_train1               = pc[18]
		check_file(pc[18])
		self.dead_unit_id               = pc[19]
		self.placement_mode             = pc[20]
		self.air_mode                   = pc[21]
		self.icon_id                    = pc[22]
		self.hidden_in_editor           = pc[23]
		#self. = pc[24]
		self.enabled                    = pc[25]
		self.placement_by_pass_terrain0 = pc[26]
		self.placement_by_pass_terrain1 = pc[27]
		self.placement_terrain0         = pc[28]
		self.placement_terrain1         = pc[29]
		self.editor_radius0             = pc[30]
		self.editor_radius1             = pc[31]
		self.building_mode              = pc[32]
		self.visible_in_fog             = pc[33]
		self.terrain_restriction        = pc[34]
		self.fly_mode                   = pc[35]
		self.ressource_capacity         = pc[36]
		self.ressource_decay            = pc[37]
		self.blast_type                 = pc[38]
		#self. = pc[39]
		self.interaction_mode           = pc[40]
		self.minimap_mode               = pc[41]
		self.command_attribute          = pc[42]
		#self. = pc[43]
		#self. = pc[44]
		self.language_dll_help          = pc[45]
		self.hot_keys                   = pc[46:(46+4)]
		#self. = pc[50]
		#self. = pc[51]
		self.unselectable               = pc[52]
		#self. = pc[53]
		#self. = pc[54]
		#self. = pc[55]
		self.selection_mask             = pc[56]
		self.selection_shape_type       = pc[57]
		self.selection_shape            = pc[58]
		self.attribute                  = pc[59]
		self.civilisation               = pc[60]
		#self. = pc[61]
		self.selection_effect           = pc[61]
		self.editor_selection_color     = pc[62]
		self.selection_radius0          = pc[63]
		self.selection_radius1          = pc[64]
		self.hp_bar_height1             = pc[65]


		self.ressource_storage = list()
		for i in range(3):
			t = RessourceStorage()
			offset = t.read(raw, offset)
			self.ressource_storage.append(t)

		#int8_t damage_graphic_count
		unit1_struct = Struct(endianness + "b")

		pc = unit1_struct.unpack_from(raw, offset)
		offset += unit1_struct.size

		self.damage_graphic_count, = pc

		self.damage_graphic = list()
		for i in range(self.damage_graphic_count):
			t = DamageGraphic()
			offset = t.read(raw, offset)
			self.damage_graphic.append(t)


		#int16_t selection_sound;
		#int16_t dying_sound;
		#int16_t attack_sound;
		#char name[name_length];
		#int16_t id1;
		#int16_t id2;
		unit2_struct = Struct(endianness + "3h %ds 2h" % self.name_length)

		pc = unit2_struct.unpack_from(raw, offset)
		offset_info(offset, pc, "unit header 2", unit2_struct)
		offset += unit2_struct.size


		self.sound_selection = pc[0]
		check_file(pc[0])
		self.sound_dying     = pc[1]
		check_file(pc[1])
		self.sound_attack    = pc[2]
		check_file(pc[2])
		self.name            = zstr(pc[3])
		self.id1             = pc[4]
		self.id2             = pc[5]


		dbg("unit type = %d\nname = %s" % (self.type_id, self.name), 2)

		#every thing else only exists for non-trees (!= 90)
		#and things with id >= 20
		if self.type_id != 90 and self.type_id >= 20:
			#float speed;
			tmp_struct = Struct(endianness + "f")
			pc = tmp_struct.unpack_from(raw, offset)
			offset_info(offset, pc, ">=20 SPEED", tmp_struct)
			offset += tmp_struct.size

			self.speed, = pc

			if math.isnan(self.speed):
				raise Exception("Speed is NaN!")

			dbg("speed:" + str(pc), 2)


			if self.type_id >= 30:
				#int16_t walking_graphics0;
				#int16_t walking_graphics1;
				#float rotation_speed;
				#int8_t unknown;
				#int16_t tracking_unit;
				#bool tracking_unit_used;
				#float tracking_unit_density;
				#float unknown;
				#int8_t unknown[17];
				tmp_struct = Struct(endianness + "2h f b h ? 2f 17b")
				pc = tmp_struct.unpack_from(raw, offset)
				offset_info(offset, pc, ">=30", tmp_struct)
				offset += tmp_struct.size

				self.walking_graphics0     = pc[0]
				check_file(pc[0])
				self.walking_graphics1     = pc[1]
				check_file(pc[1])
				self.rotation_speed        = pc[2]
				#self. = pc[2]
				self.tracking_unit         = pc[3]
				self.tracking_unit_used    = pc[4]
				self.tracking_unit_density = pc[5]
				#self. = pc[6]
				#self. = pc[7:(7+17)]


			if self.type_id >= 40:
				#int16_t sheep_conversion;
				#float search_radius;
				#float work_rate;
				#int16_t drop_site0;
				#int16_t drop_site1;
				#int8_t villager_mode;
				#int16_t move_sound;
				#int16_t stop_sound;
				#int8_t animal_mode;
				tmp_struct = Struct(endianness + "h 2f 2h b 2h b")
				pc = tmp_struct.unpack_from(raw, offset)
				offset_info(offset, pc, ">=40", tmp_struct)
				offset += tmp_struct.size


				self.sheep_conversion = pc[0]
				self.search_radius    = pc[1]
				self.work_rate        = pc[2]
				self.drop_site0       = pc[3]
				self.drop_site1       = pc[4]
				self.villager_mode    = pc[5]
				self.move_sound       = pc[6]
				check_file(pc[6])
				self.stop_sound       = pc[7]
				check_file(pc[7])
				self.animal_mode      = pc[8]


			if self.type_id >= 60:
				#int16_t default_armor;
				#uint16_t attack_count;
				tmp_struct = Struct(endianness + "h H")
				pc = tmp_struct.unpack_from(raw, offset)
				offset_info(offset, pc, ">=60", tmp_struct)
				offset += tmp_struct.size

				self.default_armor = pc[0]
				self.attack_count  = pc[1]

				self.attack = list()
				for i in range(self.attack_count):
					t = HitType()
					offset_tmp = t.read(raw, offset)
					self.attack.append(t)
					offset = offset_tmp

				#uint16_t armor_count;
				tmp_struct = Struct(endianness + "H")
				pc = tmp_struct.unpack_from(raw, offset)
				offset += tmp_struct.size

				self.armor_count = pc[0]

				self.armor = list()
				for i in range(self.armor_count):
					t = HitType()
					offset_tmp = t.read(raw, offset)
					self.armor.append(t)
					offset = offset_tmp

				#int16_t unknown;
				#float max_range;
				#float blast_radius;
				#float reload_time0;
				#int16_t projectile_unit_id;
				#int16_t accuracy_percent;
				#int8_t tower_mode;
				#int16_t delay;
				#float graphics_displacement[3];
				#int8_t blast_level;
				#float min_range;
				#float garnison_recovery_rate;
				#int16_t attack_graphic;
				#int16_t melee_armor_displayed;
				#int16_t attack_displayed;
				#int16_t range_displayed;
				#float reload_time1;
				tmp_struct = Struct(endianness + "h 3f 2h b h 3f b 2f 4h xx f")
				pc = tmp_struct.unpack_from(raw, offset)
				offset_info(offset, pc, ">=60 data", tmp_struct)
				offset += tmp_struct.size - 2


				#self. = pc[0]
				self.max_range              = pc[1]
				self.blast_radius           = pc[2]
				self.reload_time0           = pc[3]
				self.projectile_unit_id     = pc[4]
				self.accuracy_percent       = pc[5]
				self.tower_mode             = pc[6]
				self.delay                  = pc[7]
				self.graphics_displacement  = pc[8:(8+3)]
				self.blast_level            = pc[11]
				self.min_range              = pc[12]
				self.garnison_recovery_rate = pc[13]
				self.attack_graphic         = pc[14]
				self.melee_armor_displayed  = pc[15]
				self.attack_displayed       = pc[16]
				self.range_displayed        = pc[17]
				self.reload_time1           = pc[18]


			if self.type_id == 60:

				#int8_t stretch_mode;
				#int8_t compensation_mode;
				#int8_t drop_animation_mode;
				#int8_t penetration_mode;
				#int8_t unknown;
				#float projectile_arc;
				tmp_struct = Struct(endianness + "5b x x f")
				pc = tmp_struct.unpack_from(raw, offset)
				offset_info(offset, pc, "==60", tmp_struct)
				offset += tmp_struct.size

				self.stretch_mode        = pc[0]
				self.compensation_mode   = pc[1]
				self.drop_animation_mode = pc[2]
				self.penetration_mode    = pc[3]
				#self. = pc[4]
				self.projectile_arc      = pc[5]


			if self.type_id >= 70:
				self.ressource_cost = list()
				for i in range(3):
					t = RessourceCost()
					offset = t.read(raw, offset)
					self.ressource_cost.append(t)

				#int16_t train_time;
				#int16_t train_location_id;
				#int8_t button_id;
				#int32_t unknown;
				#int32_t unknown;
				#int8_t missile_graphic_delay;
				#int8_t hero_mode;
				#int16_t garnison_graphic0;
				#int16_t garnison_graphic1;
				#float attack_missile_duplication0;
				#int8_t attack_missile_duplication1;
				#float attack_missile_duplication_spawning[3];
				#int32_t attack_missile_duplication_unit;
				#int32_t attack_missile_duplication_graphic;
				#int8_t unknown;
				#int16_t pierce_armor_displayed;
				tmp_struct = Struct(endianness + "2h b 2i 2b 2h xx f b 3f 2i b h")
				pc = tmp_struct.unpack_from(raw, offset)
				offset_info(offset, pc, ">=70", tmp_struct)
				offset += tmp_struct.size


				self.train_time                          = pc[0]
				self.train_location_id                   = pc[1]
				self.button_id                           = pc[2]
				#self. = pc[3]
				#self. = pc[4]
				self.missile_graphic_delay               = pc[5]
				self.hero_mode                           = pc[6]
				self.garnison_graphic0                   = pc[7]
				self.garnison_graphic1                   = pc[8]
				self.attack_missile_duplication0         = pc[9]
				self.attack_missile_duplication1         = pc[10]
				self.attack_missile_duplication_spawning = pc[11:(11+3)]
				self.attack_missile_duplication_unit     = pc[14]
				self.attack_missile_duplication_graphic  = pc[15]
				#self. = pc[16]
				self.pierce_armor_displayed              = pc[17]

				#raise Exception()


			if self.type_id >= 80:
				#int16_t construction_graphic_id;
				#int16_t snow_graphic_id;
				#int16_t adjacent_mode;
				#int8_t unknown;
				#int8_t unknown;
				#int16_t stack_unit_id;
				#int16_t terrain_id;
				#int16_t unknown;
				#int16_t research_id;
				#int8_t unknown;
				tmp_struct = Struct(endianness + "3h 2b 4h b")
				pc = tmp_struct.unpack_from(raw, offset)
				offset_info(offset, pc, ">=80 header 0", tmp_struct)
				offset += tmp_struct.size

				self.construction_graphic_id = pc[0]
				check_file(pc[0])
				self.snow_graphic_id         = pc[1]
				check_file(pc[1])
				self.adjacent_mode           = pc[2]
				#self. = pc[3]
				#self. = pc[4]
				self.stack_unit_id           = pc[5]
				self.terrain_id              = pc[6]
				#self. = pc[7]
				#self.research_id             = pc[8]
				#self. = pc[9]

				self.building_annex = list()
				for i in range(4):
					t = BuildingAnnex()
					offset_tmp = t.read(raw, offset)
					offset = offset_tmp

					self.building_annex.append(t)

				#int16_t head_unit;
				#int16_t transform_unit;
				#int16_t unknown;
				#int16_t construction_sound_id;
				#int8_t garnison_type;
				#float garnison_heal_rate;
				#int32_t unknown;
				#int16_t unknown;
				#int8_t unknown[6];
				tmp_struct = Struct(endianness + "4h b f i h 6b")
				pc = tmp_struct.unpack_from(raw, offset)
				offset_info(offset, pc, ">=80 header 1", tmp_struct)
				offset += tmp_struct.size

				self.head_unit = pc[0]
				self.transform_unit = pc[1]
				#self. = pc[2]
				self.construction_sound_id = pc[3]
				check_file(pc[3])
				self.garnison_type = pc[4]
				self.garnison_heal_rate = pc[5]
				#self. = pc[6]
				#self. = pc[7]
				#self. = pc[8:(8+6)]

		return offset


class RessourceStorage:
	def read(self, raw, offset):
		#int16_t a;
		#float b;
		#int8_t c;
		ressource_storage_struct = Struct(endianness + "h f b")

		pc = ressource_storage_struct.unpack_from(raw, offset)
		offset_info(offset, pc, "ressource storage", ressource_storage_struct, mode=1)
		offset += ressource_storage_struct.size

		self.a = pc[0]
		self.b = pc[1]
		self.c = pc[2]

		return offset


class DamageGraphic:
	def read(self, raw, offset):
		#int16_t graphic_id;
		#int8_t damage_percent;
		#int8_t unknown;
		#int8_t unknown;
		damage_graphic_struct = Struct(endianness + "h 3b")

		pc = damage_graphic_struct.unpack_from(raw, offset)
		offset_info(offset, pc, "damage graphic", damage_graphic_struct, mode=1)
		offset += damage_graphic_struct.size

		self.graphic_id     = pc[0]
		check_file(pc[0])
		self.damage_percent = pc[1]
		#self. = pc[2]
		#self. = pc[3]

		return offset


class HitType:
	def read(self, raw, offset):
		#int16_t used_for_class_id;
		#int16_t amount;
		hit_type_struct = Struct(endianness + "2h")

		pc = hit_type_struct.unpack_from(raw, offset)
		offset_info(offset, pc, ">=60 hittype", hit_type_struct, mode=1)
		offset += hit_type_struct.size

		self.type_id = pc[0]
		self.amount  = pc[1]

		return offset


class RessourceCost:
	def read(self, raw, offset):
		#int16_t type;
		#int16_t amount;
		#int16_t enabled;
		ressource_cost_struct = Struct(endianness + "3h")

		pc = ressource_cost_struct.unpack_from(raw, offset)
		offset_info(offset, pc, ">=70 ressource cost", ressource_cost_struct, mode=1)
		offset += ressource_cost_struct.size

		self.type_id = pc[0]
		self.amount  = pc[1]
		self.enabled = pc[2]

		return offset


class BuildingAnnex:
	def read(self, raw, offset):
		#int16_t unit_id;
		#float misplaced0;
		#float misplaced1;
		building_annex_struct = Struct(endianness + "h 2f")

		pc = building_annex_struct.unpack_from(raw, offset)
		offset_info(offset, pc, ">=80 b_annex", building_annex_struct, mode=1)
		offset += building_annex_struct.size

		self.unit_id    = pc[0]
		self.misplaced0 = pc[1]
		self.misplaced1 = pc[2]

		return offset
