from struct import Struct, unpack_from
from util import dbg, zstr
import math


endianness = '< '

def check_file(fid):
	import filelist
	if fid in filelist.avail_files:
		arch, extension = filelist.avail_files[fid]
		print("%d.%s in %s" % (fid, extension, arch))
	elif fid in [-1, 0]:
		print("***** graphic is %d!!" % fid)
		pass
	else:
		msg = "##### file %d not found" % (fid)
		#raise Exception(msg)
		print(msg)


def offset_info(offset, data, msg="", s=None, mode=0):
	ret = "====== @%d = %#x ======" % (offset, offset)
	if s != None:
		ret += " \"" + str(s.format.decode("utf-8")) + "\" =="

	if mode == 0:
		ret += " %s \n" % msg
		ret += str(data) + "\n" #============="
	elif mode == 1:
		ret += " %s " % msg + str(data)
	print(ret)


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


class Unit:
	def read(self, raw, offset):
		self.data = dict()

		#unit type==:
		#object = 10
		#flag = 20
		#dead_fish = 30
		#bird = 40
		#projectile = 60
		#creatable = 70
		#building = 80
		#tree = 90

		#int8_t type;
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


		self.data["type"]                       = pc[0]
		self.data["name_length"]                = pc[1]

		if self.data["name_length"] > 100:
			raise Exception("name is longer than 100, very unlikely: %d" % self.data["name_length"])

		self.data["id0"]                        = pc[2]
		self.data["language_dll_name"]          = pc[3]
		self.data["language_dll_creation"]      = pc[4]
		self.data["unit_class"]                 = pc[5]
		self.data["graphic_standing0"]          = pc[6]
		check_file(pc[6])
		self.data["graphic_standing1"]          = pc[7]
		check_file(pc[7])
		self.data["graphic_dying0"]             = pc[8]
		self.data["graphic_dying1"]             = pc[9]
		self.data["death_mode"]                 = pc[10]
		self.data["hit_points"]                 = pc[11]
		self.data["line_of_sight"]              = pc[12]
		self.data["garnison_capacity"]          = pc[13]
		self.data["radius_size0"]               = pc[14]
		self.data["radius_size1"]               = pc[15]
		self.data["hp_bar_height0"]             = pc[16]
		self.data["sound_train0"]               = pc[17]
		check_file(pc[17])
		self.data["sound_train1"]               = pc[18]
		check_file(pc[18])
		self.data["dead_unit_id"]               = pc[19]
		self.data["placement_mode"]             = pc[20]
		self.data["air_mode"]                   = pc[21]
		self.data["icon_id"]                    = pc[22]
		self.data["hidden_in_editor"]           = pc[23]
		#self.data[""] = pc[24]
		self.data["enabled"]                    = pc[25]
		self.data["placement_by_pass_terrain0"] = pc[26]
		self.data["placement_by_pass_terrain1"] = pc[27]
		self.data["placement_terrain0"]         = pc[28]
		self.data["placement_terrain1"]         = pc[29]
		self.data["editor_radius0"]             = pc[30]
		self.data["editor_radius1"]             = pc[31]
		self.data["building_mode"]              = pc[32]
		self.data["visible_in_fog"]             = pc[33]
		self.data["terrain_restriction"]        = pc[34]
		self.data["fly_mode"]                   = pc[35]
		self.data["ressource_capacity"]         = pc[36]
		self.data["ressource_decay"]            = pc[37]
		self.data["blast_type"]                 = pc[38]
		#self.data[""] = pc[39]
		self.data["interaction_mode"]           = pc[40]
		self.data["minimap_mode"]               = pc[41]
		self.data["command_attribute"]          = pc[42]
		#self.data[""] = pc[43]
		#self.data[""] = pc[44]
		self.data["language_dll_help"]          = pc[45]
		self.data["hot_keys"]                   = pc[46:(46+4)]
		#self.data[""] = pc[50]
		#self.data[""] = pc[51]
		self.data["unselectable"]               = pc[52]
		#self.data[""] = pc[53]
		#self.data[""] = pc[54]
		#self.data[""] = pc[55]
		self.data["selection_mask"]             = pc[56]
		self.data["selection_shape_type"]       = pc[57]
		self.data["selection_shape"]            = pc[58]
		self.data["attribute"]                  = pc[59]
		self.data["civilisation"]               = pc[60]
		#self.data[""] = pc[61]
		self.data["selection_effect"]           = pc[61]
		self.data["editor_selection_color"]     = pc[62]
		self.data["selection_radius0"]          = pc[63]
		self.data["selection_radius1"]          = pc[64]
		self.data["hp_bar_height1"]             = pc[65]


		self.data["ressource_storage"] = list()
		for i in range(3):
			t = RessourceStorage()
			offset = t.read(raw, offset)
			self.data["ressource_storage"] += [t.data]

		#int8_t damage_graphic_count
		unit1_struct = Struct(endianness + "b")

		pc = unit1_struct.unpack_from(raw, offset)
		offset += unit1_struct.size

		self.data["damage_graphic_count"], = pc

		self.data["damage_graphic"] = list()
		for i in range(self.data["damage_graphic_count"]):
			t = DamageGraphic()
			offset = t.read(raw, offset)
			self.data["damage_graphic"] += [t.data]


		#int16_t selection_sound;
		#int16_t dying_sound;
		#int16_t attack_sound;
		#char name[name_length];
		#int16_t id1;
		#int16_t id2;
		unit2_struct = Struct(endianness + "3h %ds 2h" % self.data["name_length"])

		pc = unit2_struct.unpack_from(raw, offset)
		offset_info(offset, pc, "unit header 2", unit2_struct)
		offset += unit2_struct.size


		self.data["sound_selection"] = pc[0]
		check_file(pc[0])
		self.data["sound_dying"]     = pc[1]
		check_file(pc[1])
		self.data["sound_attack"]    = pc[2]
		check_file(pc[2])
		self.data["name"]            = zstr(pc[3])
		self.data["id1"]             = pc[4]
		self.data["id2"]             = pc[5]


		print("unit type = %d\nname = %s" % (self.data["type"], self.data["name"]))

		#every thing else only exists for non-trees (!= 90)
		#and things with id >= 20
		if self.data["type"] != 90 and self.data["type"] >= 20:
			#float speed;
			tmp_struct = Struct(endianness + "f")
			pc = tmp_struct.unpack_from(raw, offset)
			offset_info(offset, pc, ">=20 SPEED", tmp_struct)
			offset += tmp_struct.size

			self.data["speed"], = pc

			if math.isnan(self.data["speed"]):
				raise Exception("Speed is NaN!")

			print("speed:" + str(pc))


			if self.data["type"] >= 30:
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

				self.data["walking_graphics0"]     = pc[0]
				check_file(pc[0])
				self.data["walking_graphics1"]     = pc[1]
				check_file(pc[1])
				self.data["rotation_speed"]        = pc[2]
				#self.data[""] = pc[2]
				self.data["tracking_unit"]         = pc[3]
				self.data["tracking_unit_used"]    = pc[4]
				self.data["tracking_unit_density"] = pc[5]
				#self.data[""] = pc[6]
				#self.data[""] = pc[7:(7+17)]


			if self.data["type"] >= 40:
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


				self.data["sheep_conversion"] = pc[0]
				self.data["search_radius"]    = pc[1]
				self.data["work_rate"]        = pc[2]
				self.data["drop_site0"]       = pc[3]
				self.data["drop_site1"]       = pc[4]
				self.data["villager_mode"]    = pc[5]
				self.data["move_sound"]       = pc[6]
				check_file(pc[6])
				self.data["stop_sound"]       = pc[7]
				check_file(pc[7])
				self.data["animal_mode"]      = pc[8]


			if self.data["type"] >= 60:
				#int8_t unknown;
				#int8_t unknown;
				tmp_struct = Struct(endianness + "2b")
				pc = tmp_struct.unpack_from(raw, offset)
				offset_info(offset, pc, ">=60 header0", tmp_struct)
				offset += tmp_struct.size

				#self.data[""] = pc[0]
				#self.data[""] = pc[1]

				#uint16_t attack_count;
				tmp_struct = Struct(endianness + "H")
				pc = tmp_struct.unpack_from(raw, offset)
				offset += tmp_struct.size

				self.data["attack_count"] = pc[0]

				self.data["attack"] = list()
				for i in range(self.data["attack_count"]):
					t = HitType()
					offset_tmp = t.read(raw, offset)
					self.data["attack"] += [t.data]
					offset = offset_tmp

				#uint16_t armor_count;
				tmp_struct = Struct(endianness + "H")
				pc = tmp_struct.unpack_from(raw, offset)
				offset += tmp_struct.size

				self.data["armor_count"] = pc[0]

				self.data["armor"] = list()
				for i in range(self.data["armor_count"]):
					t = HitType()
					offset_tmp = t.read(raw, offset)
					self.data["armor"] += [t.data]
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
				#int8_t unknown;
				#float min_range;
				#float garnison_recovery_rate;
				#int16_t attack_graphic;
				#int16_t melee_armor_displayed;
				#int16_t attack_displayed;
				#int16_t range_displayed;
				#float reload_time1;
				tmp_struct = Struct(endianness + "h 3f 2h b h 3f b 2f 4h f")
				pc = tmp_struct.unpack_from(raw, offset)
				offset_info(offset, pc, ">=60 data", tmp_struct)
				offset += tmp_struct.size


				#self.data[""] = pc[0]
				self.data["max_range"]              = pc[1]
				self.data["blast_radius"]           = pc[2]
				self.data["reload_time0"]           = pc[3]
				self.data["projectile_unit_id"]     = pc[4]
				self.data["accuracy_percent"]       = pc[5]
				self.data["tower_mode"]             = pc[6]
				self.data["delay"]                  = pc[7]
				self.data["graphics_displacement"]  = pc[8:(8+3)]

				#self.data[""] = pc[11]
				self.data["min_range"]              = pc[12]
				self.data["garnison_recovery_rate"] = pc[13]
				self.data["attack_graphic"]         = pc[14]
				self.data["melee_armor_displayed"]  = pc[15]
				self.data["attack_displayed"]       = pc[16]
				self.data["range_displayed"]        = pc[17]
				self.data["reload_time1"]           = pc[18]


			if self.data["type"] == 60:
				#int8_t stretch_mode;
				#int8_t compensation_mode;
				#int8_t drop_animation_mode;
				#int8_t penetration_mode;
				#int8_t unknown;
				#float projectile_arc;
				tmp_struct = Struct(endianness + "5b f")
				pc = tmp_struct.unpack_from(raw, offset)
				offset_info(offset, pc, "==60", tmp_struct)
				offset += tmp_struct.size

				self.data["stretch_mode"]        = pc[0]
				self.data["compensation_mode"]   = pc[1]
				self.data["drop_animation_mode"] = pc[2]
				self.data["penetration_mode"]    = pc[3]
				#self.data[""] = pc[4]
				self.data["projectile_arc"]      = pc[5]


			if self.data["type"] >= 70:
				self.data["ressource_cost"] = list()
				for i in range(3):
					t = RessourceCost()
					offset = t.read(raw, offset)
					self.data["ressource_cost"] += [t.data]

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


				self.data["train_time"]                          = pc[0]
				self.data["train_location_id"]                   = pc[1]
				self.data["button_id"]                           = pc[2]
				#self.data[""] = pc[3]
				#self.data[""] = pc[4]
				self.data["missile_graphic_delay"]               = pc[5]
				self.data["hero_mode"]                           = pc[6]
				self.data["garnison_graphic0"]                   = pc[7]
				self.data["garnison_graphic1"]                   = pc[8]
				self.data["attack_missile_duplication0"]         = pc[9]
				self.data["attack_missile_duplication1"]         = pc[10]
				self.data["attack_missile_duplication_spawning"] = pc[11:(11+3)]
				self.data["attack_missile_duplication_unit"]     = pc[14]
				self.data["attack_missile_duplication_graphic"]  = pc[15]
				#self.data[""] = pc[16]
				self.data["pierce_armor_displayed"]              = pc[17]

				#raise Exception()


			if self.data["type"] >= 80:
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

				self.data["construction_graphic_id"] = pc[0]
				check_file(pc[0])
				self.data["snow_graphic_id"]         = pc[1]
				check_file(pc[1])
				self.data["adjacent_mode"]           = pc[2]
				#self.data[""] = pc[3]
				#self.data[""] = pc[4]
				self.data["stack_unit_id"]           = pc[5]
				self.data["terrain_id"]              = pc[6]
				#self.data[""] = pc[7]
				#self.data["research_id"]             = pc[8]
				#self.data[""] = pc[9]

				self.data["building_annex"] = list()
				for i in range(4):
					t = BuildingAnnex()
					offset_tmp = t.read(raw, offset)
					offset = offset_tmp

					self.data["building_annex"] += [t.data]

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

				self.data["head_unit"] = pc[0]
				self.data["transform_unit"] = pc[1]
				#self.data[""] = pc[2]
				self.data["construction_sound_id"] = pc[3]
				check_file(pc[3])
				self.data["garnison_type"] = pc[4]
				self.data["garnison_heal_rate"] = pc[5]
				#self.data[""] = pc[6]
				#self.data[""] = pc[7]
				#self.data[""] = pc[8:(8+6)]

		return offset


class RessourceStorage:
	def read(self, raw, offset):
		self.data = dict()

		#int16_t a;
		#float b;
		#int8_t c;
		ressource_storage_struct = Struct(endianness + "h f b")

		pc = ressource_storage_struct.unpack_from(raw, offset)
		offset_info(offset, pc, "ressource storage", ressource_storage_struct, mode=1)
		offset += ressource_storage_struct.size

		self.data["a"] = pc[0]
		self.data["b"] = pc[1]
		self.data["c"] = pc[2]

		return offset


class DamageGraphic:
	def read(self, raw, offset):
		self.data = dict()

		#int16_t graphic_id;
		#int8_t damage_percent;
		#int8_t unknown;
		#int8_t unknown;
		damage_graphic_struct = Struct(endianness + "h 3b")

		pc = damage_graphic_struct.unpack_from(raw, offset)
		offset_info(offset, pc, "damage graphic", hit_type_struct, mode=1)
		offset += damage_graphic_struct.size

		self.data["graphic_id"]     = pc[0]
		check_file(pc[0])
		self.data["damage_percent"] = pc[1]
		#self.data[""] = pc[2]
		#self.data[""] = pc[3]

		return offset


class HitType:
	def read(self, raw, offset):
		self.data = dict()

		#int16_t type;
		#int16_t amount;
		hit_type_struct = Struct(endianness + "2h")

		pc = hit_type_struct.unpack_from(raw, offset)
		offset_info(offset, pc, ">=60 hittype", hit_type_struct, mode=1)
		offset += hit_type_struct.size

		self.data["type"]   = pc[0]
		self.data["amount"] = pc[1]

		return offset


class RessourceCost:
	def read(self, raw, offset):
		self.data = dict()

		#int16_t type;
		#int16_t amount;
		#int16_t enabled;
		ressource_cost_struct = Struct(endianness + "3h")

		pc = ressource_cost_struct.unpack_from(raw, offset)
		offset_info(offset, pc, ">=70 ressource cost", ressource_cost_struct, mode=1)
		offset += ressource_cost_struct.size

		self.data["type"]    = pc[0]
		self.data["amount"]  = pc[1]
		self.data["enabled"] = pc[2]

		return offset


class BuildingAnnex:
	def read(self, raw, offset):
		self.data = dict()

		#int16_t unit_id;
		#float misplaced0;
		#float misplaced1;
		building_annex_struct = Struct(endianness + "h 2f")

		pc = building_annex_struct.unpack_from(raw, offset)
		offset_info(offset, pc, ">=80 b_annex", building_annex_struct, mode=1)
		offset += building_annex_struct.size

		self.data["unit_id"]    = pc[0]
		self.data["misplaced0"] = pc[1]
		self.data["misplaced1"] = pc[2]

		return offset

