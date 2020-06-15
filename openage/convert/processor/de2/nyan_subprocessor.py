# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Convert API-like objects to nyan objects. Subroutine of the
main DE2 processor.
"""


class DE2NyanSubprocessor:

    @classmethod
    def convert(cls, gamedata):

        cls._process_game_entities(gamedata)
        cls._create_nyan_objects(gamedata)
        cls._create_nyan_members(gamedata)

    @classmethod
    def _create_nyan_objects(cls, full_data_set):
        """
        Creates nyan objects from the API objects.
        """
        for unit_line in full_data_set.unit_lines.values():
            unit_line.create_nyan_objects()
            unit_line.execute_raw_member_pushs()

        for building_line in full_data_set.building_lines.values():
            building_line.create_nyan_objects()
            building_line.execute_raw_member_pushs()

        for ambient_group in full_data_set.ambient_groups.values():
            ambient_group.create_nyan_objects()
            ambient_group.execute_raw_member_pushs()

        for variant_group in full_data_set.variant_groups.values():
            variant_group.create_nyan_objects()
            variant_group.execute_raw_member_pushs()

        for tech_group in full_data_set.tech_groups.values():
            tech_group.create_nyan_objects()
            tech_group.execute_raw_member_pushs()

        for terrain_group in full_data_set.terrain_groups.values():
            terrain_group.create_nyan_objects()
            terrain_group.execute_raw_member_pushs()

        for civ_group in full_data_set.civ_groups.values():
            civ_group.create_nyan_objects()
            civ_group.execute_raw_member_pushs()

    @classmethod
    def _create_nyan_members(cls, full_data_set):
        """
        Fill nyan member values of the API objects.
        """
        for unit_line in full_data_set.unit_lines.values():
            unit_line.create_nyan_members()

        for building_line in full_data_set.building_lines.values():
            building_line.create_nyan_members()

        for ambient_group in full_data_set.ambient_groups.values():
            ambient_group.create_nyan_members()

        for variant_group in full_data_set.variant_groups.values():
            variant_group.create_nyan_members()

        for tech_group in full_data_set.tech_groups.values():
            tech_group.create_nyan_members()

        for terrain_group in full_data_set.terrain_groups.values():
            terrain_group.create_nyan_members()

        for civ_group in full_data_set.civ_groups.values():
            civ_group.create_nyan_members()

    @classmethod
    def _process_game_entities(cls, full_data_set):

        for unit_line in full_data_set.unit_lines.values():
            cls._unit_line_to_game_entity(unit_line)

        for building_line in full_data_set.building_lines.values():
            cls._building_line_to_game_entity(building_line)

        for ambient_group in full_data_set.ambient_groups.values():
            cls._ambient_group_to_game_entity(ambient_group)

        for variant_group in full_data_set.variant_groups.values():
            cls._variant_group_to_game_entity(variant_group)

        for tech_group in full_data_set.tech_groups.values():
            if tech_group.is_researchable():
                cls._tech_group_to_tech(tech_group)

        for terrain_group in full_data_set.terrain_groups.values():
            cls._terrain_group_to_terrain(terrain_group)

        for civ_group in full_data_set.civ_groups.values():
            cls._civ_group_to_civ(civ_group)
