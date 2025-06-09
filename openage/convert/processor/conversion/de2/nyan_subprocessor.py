# Copyright 2020-2025 the openage authors. See copying.md for legal info.

"""
Convert API-like objects to nyan objects. Subroutine of the
main DE2 processor.
"""
from __future__ import annotations
import typing

from ..aoc.nyan_subprocessor import AoCNyanSubprocessor

from .nyan.building import building_line_to_game_entity
from .nyan.civ import civ_group_to_civ
from .nyan.tech import tech_group_to_tech
from .nyan.terrain import terrain_group_to_terrain
from .nyan.unit import unit_line_to_game_entity

if typing.TYPE_CHECKING:
    from ....entity_object.conversion.aoc.genie_object_container import GenieObjectContainer


class DE2NyanSubprocessor:
    """
    Transform a DE2 dataset to nyan objects.
    """

    @classmethod
    def convert(cls, full_data_set: GenieObjectContainer) -> None:
        """
        Create nyan objects from the given dataset.
        """
        cls._process_game_entities(full_data_set)
        cls._create_nyan_objects(full_data_set)
        cls._create_nyan_members(full_data_set)

        cls._check_objects(full_data_set)

    @classmethod
    def _check_objects(cls, full_data_set: GenieObjectContainer) -> None:
        """
        Check if objects are valid.
        """
        for unit_line in full_data_set.unit_lines.values():
            unit_line.check_readiness()

        for building_line in full_data_set.building_lines.values():
            building_line.check_readiness()

        for ambient_group in full_data_set.ambient_groups.values():
            ambient_group.check_readiness()

        for variant_group in full_data_set.variant_groups.values():
            variant_group.check_readiness()

        for tech_group in full_data_set.tech_groups.values():
            tech_group.check_readiness()

        for terrain_group in full_data_set.terrain_groups.values():
            terrain_group.check_readiness()

        for civ_group in full_data_set.civ_groups.values():
            civ_group.check_readiness()

    @classmethod
    def _create_nyan_objects(cls, full_data_set: GenieObjectContainer) -> None:
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
    def _create_nyan_members(cls, full_data_set: GenieObjectContainer) -> None:
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
    def _process_game_entities(cls, full_data_set: GenieObjectContainer) -> None:
        """
        Create the RawAPIObject representation of the objects.
        """
        for unit_line in full_data_set.unit_lines.values():
            cls.unit_line_to_game_entity(unit_line)

        for building_line in full_data_set.building_lines.values():
            cls.building_line_to_game_entity(building_line)

        for ambient_group in full_data_set.ambient_groups.values():
            AoCNyanSubprocessor.ambient_group_to_game_entity(ambient_group)

        for variant_group in full_data_set.variant_groups.values():
            AoCNyanSubprocessor.variant_group_to_game_entity(variant_group)

        for tech_group in full_data_set.tech_groups.values():
            if tech_group.is_researchable():
                cls.tech_group_to_tech(tech_group)

        for terrain_group in full_data_set.terrain_groups.values():
            cls.terrain_group_to_terrain(terrain_group)

        for civ_group in full_data_set.civ_groups.values():
            cls.civ_group_to_civ(civ_group)

    building_line_to_game_entity = staticmethod(building_line_to_game_entity)
    civ_group_to_civ = staticmethod(civ_group_to_civ)
    tech_group_to_tech = staticmethod(tech_group_to_tech)
    terrain_group_to_terrain = staticmethod(terrain_group_to_terrain)
    unit_line_to_game_entity = staticmethod(unit_line_to_game_entity)
