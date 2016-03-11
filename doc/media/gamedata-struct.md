empires2_x1_p1.dat
==================

```cpp
struct {
	char versionstr[8];

	uint16_t terrain_restriction_count;
	uint16_t terrain_count;
	int32_t terrain_restriction_offset0[terrain_restriction_count];
	int32_t terrain_restriction_offset1[terrain_restriction_count];
	struct {
		float terrain_accessible[terrain_count];
		struct {
			int32_t buildable;
			int32_t graphic_id0;
			int32_t graphic_id1;
			int32_t replication_amount;
		} terrain_pass_graphic[terrain_count];
	} terrain_restriction[terrain_restriction_count];

	uint16_t player_color_count;
	struct {
		int32_t id;
		int32_t palette;
		int32_t color;
		int32_t unknown;
		int32_t unknown;
		int32_t minimap_color;
		int32_t unknown;
		int32_t unknown;
		int32_t statistics_text_color;
	} player_color[player_color_count];

	uint16_t sound_count;
	struct {
		int32_t id;
		int16_t unknown;
		uint16_t item_count;
		int32_t unknown;
		struct {
			char filename[13];
			int32_t resource_id;
			int16_t probability;
			int16_t civilisation;
			int16_t unknown;
		} sound_item[item_count];
	} sound[sound_count];

	int16_t graphic_count;
	int32_t graphic_offsets[graphic_count];
	struct {
		struct {
			char name0[21];
			char name1[13];
			int32_t slp_id;
			int8_t unknown;
			int8_t unknown;
			int8_t layer;
			int16_t player_color;
			int8_t adapt_color;
			bool replay;
			int16_t coordinates[4];
			uint16_t delta_count;
			int16_t sound_id;
			uint8_t attack_sound_used;
			uint16_t frame_count;
			uint16_t angle_count;
			float speed_adjust;
			float frame_rate;
			float replay_delay;
			int8_t sequence_type;
			int16_t id;
			int16_t mirroring_mode;
			int8_t unknown;
		} graphic_header;

		struct {
			int16_t graphic_id;
			int16_t unknown;
			int16_t unknown;
			int16_t unknown;
			int16_t direction_x;
			int16_t direction_y;
			int16_t unknown;
			int16_t unknown;
		} graphic_delta[graphic_header.delta_count];

		//if(graphic_header.attack_sound_used) {
		struct {
			struct {
				int16_t sound_delay;
				int16_t sound_id;
			} sound_prop[3];
		} graphic_attack_sound[graphic_header.angle_count];
		//}
	} graphic[graphic_count];
	int8_t[138] rendering_data;

	struct {
		int16_t unknown;
		int16_t unknown;
		char name0[13];
		char name1[13];
		int32_t slp_id;
		int32_t unknown;
		int32_t sound_id;
		int32_t blend_priority;
		int32_t blend_mode;
		uint8_t color[3];
		uint8_t unknown[5];
		float unknown;
		int8_t unknown[18];
		int16_t frame_count;
		int16_t angle_count;
		int16_t terrain_id;
		int16_t elevation_graphic[54];
		int16_t terrain_replacement_id;
		int16_t terrain_dimensions0;
		int16_t terrain_dimensions1;
		int8_t terrain_border_id[84];
		int16_t terrain_unit_id[30];
		int16_t terrain_unit_density[30];
		int8_t terrain_unit_priority[30];
		int16_t terrain_units_used_count;
	} terrain[terrain_count];

	int8_t unknown[438];

	struct {
		int16_t enabled;
		char name0[13];
		char name1[13];
		int32_t resource_id;
		int32_t unknown;
		int32_t unknown;
		uint8_t color[3];
		int8_t unknown;
		int32_t unknown;
		int32_t unknown;
		struct {
			int16_t frame_id;
			int16_t flag0;
			int16_t flag1;
		} frame_data[230];
		int16_t frame_count;
		int16_t unknown;
		int16_t unknown;
		int16_t unknown;
	} terrain_border[16];

	int8_t zero[28];
	uint16_t terrain_count_additional;
	int8_t rendering_etc[12722];

	uint32_t tech_count;
	struct {
		char name[31];
		uint16_t effect_count;
		struct {
			int8_t type_id;
			int16_t unit;
			int16_t unit_class_id;
			int16_t attribute_id;
			float amount;
		} effects[effect_count];
	} tech[tech_count];

	uint32_t unit_count;
	struct {
		uint8_t exists;
		//if(exists) {
		uint16_t unit_command_count;
		struct {
			int16_t command_used;
			int16_t id;
			int8_t unknown;
			int16_t type;
			int16_t class_id;
			int16_t unit_id;
			int16_t unknown;
			int16_t resource_in;
			int16_t resource_productivity;
			int16_t resource_out;
			int16_t resource;
			float work_rate_multiplier;
			float execution_radius;
			float extra_range;
			int8_t unknown;
			float unknown;
			int8_t selection_enabled;
			int8_t unknown;
			int16_t unknown;
			int16_t unknown;
			int8_t targets_allowed;
			int8_t unknown;
			int8_t unknown;
			int16_t tool_graphic_id;
			int16_t proceed_graphic_id;
			int16_t action_graphic_id;
			int16_t carrying_graphic_id;
			int16_t execution_sound_id;
			int16_t resource_deposit_sound_id;
		} unit_command[command_count];
		//}
	} unit_header[unit_count];

	uint16_t civ_count;
	struct {
		int8_t enabled;
		char name[20];
		uint16_t resources_count;
		int16_t tech_tree_id;
		int16_t team_bonus_id;
		float[resources_count] resources;
		int8_t icon_set;
		uint16_t unit_count;
		int32_t unit_offsets[unit_count];

		struct {
			/*
			unit type:
			eye_candy         = 10
			flag              = 20
			doppelganger      = 25
			dead_or_fish_unit = 30
			bird              = 40
			projectile        = 60
			living_unit       = 70
			building          = 80
			tree              = 90
			*/
			int8_t type_id;

			struct {
				uint16_t name_length;
				int16_t id0;
				uint16_t language_dll_name;
				uint16_t language_dll_creation;
				int16_t unit_class;
				int16_t graphic_standing0;
				int16_t graphic_standing1;
				int16_t graphic_dying0;
				int16_t graphic_dying1;
				int8_t death_mode;
				int16_t hit_points;
				float line_of_sight;
				int8_t garnison_capacity;
				float radius_size0;
				float radius_size1;
				float hp_bar_height0;
				int16_t sound_creation0;
				int16_t sound_creation1;
				int16_t dead_unit_id;
				int8_t placement_mode;
				int8_t air_mode;
				int16_t icon_id;
				int8_t hidden_in_editor;
				int16_t unknown;
				int16_t enabled;
				int16_t placement_by_pass_terrain0;
				int16_t placement_by_pass_terrain1;
				int16_t placement_terrain0;
				int16_t placement_terrain1;
				float editor_radius0;
				float editor_radius1;
				int8_t building_mode;
				int8_t visible_in_fog;
				int16_t terrain_restriction;
				int8_t fly_mode;
				int16_t resource_capacity;
				float resource_decay;
				int8_t blast_armor_level;
				int8_t trigger_type;
				int8_t interaction_mode;
				int8_t minimap_mode;
				int16_t command_attribute;
				float unknown;
				int8_t minimap_color;
				uint16_t language_dll_help;
				int16_t[4] hot_keys;
				int8_t unknown;
				int8_t unknown;
				uint8_t unselectable;
				int8_t unknown;
				int8_t selection_mode;
				int8_t unknown;
				int8_t selection_mask;
				int8_t selection_shape_type;
				int8_t selection_shape;
				uint8_t attribute;
				int8_t civilisation;
				int16_t unknown;
				int8_t selection_effect;
				uint8_t editor_selection_color;
				float selection_radius0;
				float selection_radius1;
				float hp_bar_height1;

				struct {
					int16_t type;
					float amount;
					int8_t used_mode;
				} resource_storage[3];

				int8_t damage_graphic_count;
				struct {
					int16_t graphic_id;
					int8_t damage_percent;
					int8_t apply_mode;
					int8_t unknown;
				} damage_graphic[damage_graphic_count];

				int16_t sound_selection;
				int16_t sound_dying;
				int8_t attack_mode;
				int8_t is_edible_meat;
				char name[name_length];
				int16_t id1;
				int16_t id2;
			} unit_header;

			if (type_id >= unit_type.flag = 20 && type_id != unit_type.tree = 90) {
				float speed;
			} else {
				unit_done;
			}

			if (type_id >= unit_type.dead_or_fish_unit = 30) {
				int16_t walking_graphics0;
				int16_t walking_graphics1;
				float rotation_speed;
				int8_t unknown;
				int16_t tracking_unit_id;
				uint8_t tracking_unit_used;
				float tracking_unit_density;
				float unknown;
				float rotation_angles[5];
			}

			if (type_id >= unit_type.bird = 40) {
				int16_t sheep_conversion;
				float search_radius;
				float work_rate;
				int16_t drop_site0;
				int16_t drop_site1;
				int8_t villager_mode;
				int16_t move_sound;
				int16_t stop_sound;
				int8_t animal_mode;
			}

			if (type_id >= unit_type.projectile = 60) {
				int16_t default_armor;
				uint16_t attack_count;
				struct {
					int16_t type_id;
					int16_t amount;
				} attack[attack_count];
				uint16_t armor_count;
				struct {
					int16_t type_id;
					int16_t amount;
				} armor[armor_count];
				int16_t interaction_type;
				float max_range;
				float blast_radius;
				float reload_time0;
				int16_t projectile_unit_id;
				int16_t accuracy_percent;
				int8_t tower_mode;
				int16_t delay;
				float graphics_displacement_lr;
				float graphics_displacement_distance;
				float graphics_displacement_height;
				int8_t blast_attack_level;
				float min_range;
				float garrison_recovery_rate;
				int16_t attack_graphic;
				int16_t melee_armor_displayed;
				int16_t attack_displayed;
				float range_displayed;
				float reload_time1;
			}

			if (type_id == unit_type.projectile = 60) {
				int8_t stretch_mode;
				int8_t compensation_mode;
				int8_t drop_animation_mode;
				int8_t penetration_mode;
				int8_t unknown;
				float projectile_arc;
			}

			if (type_id >= unit_type.living_unit = 70) {
				struct {
					int16_t type_id;
					int16_t amount;
					int16_t enabled;
				} resource_cost[3];
				int16_t creation_time;
				int16_t creation_location_id;
				int8_t creation_button_id;
				float unknown;
				float unknown;
				int8_t missile_graphic_delay;
				int8_t hero_mode;
				int32_t garrison_graphic;
				float attack_missile_count;
				int8_t attack_missile_max_count;
				float attack_missile_duplication_spawning_width;
				float attack_missile_duplication_spawning_length;
				float attack_missile_duplication_spawning_randomness;
				int32_t attack_missile_duplication_unit_id;
				int32_t attack_missile_duplication_graphic_id;
				int8_t dynamic_image_update;
				int16_t pierce_armor_displayed;
			}

			if (type_id >= unit_type.building = 80) {
				int16_t construction_graphic_id;
				int16_t snow_graphic_id;
				int16_t adjacent_mode;
				int16_t icon_disabler;
				int8_t disappears_when_built;
				int16_t stack_unit_id;
				int16_t terrain_id;
				int16_t resource_id;
				int16_t research_id;
				int8_t unknown;
				struct {
					int16_t unit_id;
					float misplaced0;
					float misplaced1;
				} building_annex[4];
				int16_t head_unit_id;
				int16_t transform_unit_id;
				int16_t unknown;
				int16_t construction_sound_id;
				int8_t garrison_type;
				float garrison_heal_rate;
				int32_t unknown;
				int16_t unknown;
				int8_t unknown[6];
			}
		} unit[unit_count];
	} civ[civ_count];

	uint16_t research_count;
	struct {
		int16_t[6] required_techs;
		struct {
			int16_t resource_id;
			int16_t amount;
			int8_t enabled;
		} research_resource_cost[3];
		int16_t required_tech_count;
		int16_t civilisation_id;
		int16_t full_tech_mode;
		int16_t research_location_id;
		uint16_t language_dll_name;
		uint16_t language_dll_description;
		int16_t research_time;
		int16_t tech_effect_id;
		int16_t tech_type;
		int16_t icon_id;
		int8_t button_id;
		int32_t pointers[3];
		uint16_t name_length;
		char name[name_length];
	} research[research_count];

	uint32_t unknown[7];

	struct {
		int8_t age_entry_count;
		int8_t building_connection_count;
		int8_t unit_connection_count;
		int8_t research_connection_count;

		struct {
			int32_t unknown;
			int32_t id;
			int8_t unknown;
			int8_t building_count;
			int32_t buildings[building_count];
			int8_t unit_count;
			int32_t units[unit_count];
			int8_t research_count;
			int32_t research[research_count];
			int32_t unknown;
			int32_t second_age_id;
			int16_t zeroes[49];
		} age_tech_tree[age_entry_count];

		int32_t unknown;

		struct {
			int32_t id;
			int8_t prerequisite_count;
			int8_t building_count;
			int32_t building[building_count];
			int8_t unit_count;
			int32_t unit[unit_count];
			int8_t research_count;
			int32_t research[research_count];
			int32_t age;
			int32_t unit_or_research0;
			int32_t unit_or_research1;
			int32_t unknown[8];
			int32_t mode0;
			int32_t mode1;
			int32_t unknown[8];
			int8_t unknown[11];
			int32_t line_mode;
			int32_t enabled_by_research_id;
		} building_connection[building_entry_count];

		struct {
			int32_t id;
			int8_t unknown;
			int32_t upper_building;
			int32_t required_researches;
			int32_t age;
			int32_t unit_or_research0;
			int32_t unit_or_research1;
			int32_t unknown[8];
			int32_t mode0;
			int32_t mode1;
			int32_t unknown[7];
			int32_t vertical_lines;
			int8_t unit_count;
			int32_t unit[unit_count];
			int32_t location_in_age;
			int32_t required_research;
			int32_t line_mode;
			int32_t enabling_research;
		} unit_connection[unit_entry_count];

		struct {
			int32_t id;
			int8_t unknown;
			int32_t upper_building;
			int8_t building_count;
			int32_t building[building_count];
			int8_t unit_count;
			int32_t unit[unit_count];
			int8_t research_count;
			int32_t research[research_count];
			int32_t required_research;
			int32_t age;
			int32_t upper_research;
			int32_t unknown[9];
			int32_t line_mode;
			int32_t unknown[8];
			int32_t vertical_line;
			int32_t location_in_age;
			int32_t unknown;
		} research_connection[research_entry_count];
	} tech_tree;
} empires2_x1_p1;
```
