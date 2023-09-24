# Copyright 2019-2023 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-instance-attributes,too-few-public-methods

"""
Object for comparing and passing around data from a dataset.
"""
from __future__ import annotations
import typing


from ..converter_object import ConverterObjectContainer

if typing.TYPE_CHECKING:
    from engine.convert.entity_object.conversion.converter_object import RawAPIObject
    from engine.convert.entity_object.conversion.combined_sound import CombinedSound
    from engine.convert.entity_object.conversion.combined_sprite import CombinedSprite
    from engine.convert.entity_object.conversion.combined_terrain import CombinedTerrain
    from engine.convert.entity_object.conversion.stringresource import StringResource
    from engine.convert.entity_object.conversion.aoc.genie_civ import GenieCivilizationObject, \
        GenieCivilizationGroup
    from engine.convert.entity_object.conversion.aoc.genie_connection import GenieAgeConnection, \
        GenieBuildingConnection, GenieTechConnection, GenieUnitConnection
    from engine.convert.entity_object.conversion.aoc.genie_effect import GenieEffectBundle
    from engine.convert.entity_object.conversion.aoc.genie_graphic import GenieGraphic
    from engine.convert.entity_object.conversion.aoc.genie_sound import GenieSound
    from engine.convert.entity_object.conversion.aoc.genie_tech import GenieTechObject, \
        AgeUpgrade, BuildingLineUpgrade, BuildingUnlock, CivBonus, GenieTechEffectBundleGroup, \
        InitiatedTech, StatUpgrade, UnitLineUpgrade, UnitUnlock
    from engine.convert.entity_object.conversion.aoc.genie_terrain import GenieTerrainObject, \
        GenieTerrainGroup
    from engine.convert.entity_object.conversion.aoc.genie_unit import GenieUnitObject, \
        GenieAmbientGroup, GenieBuildingLineGroup, GenieMonkGroup, GenieUnitLineGroup, \
        GenieUnitTaskGroup, GenieUnitTransformGroup, GenieVariantGroup, GenieVillagerGroup, \
        GenieGameEntityGroup
    from engine.convert.entity_object.export.media_export_request import MediaExportRequest
    from engine.convert.entity_object.export.metadata_export import MetadataExport
    from engine.convert.value_object.init.game_version import GameVersion
    from engine.nyan.nyan_structs import NyanObject


class GenieObjectContainer(ConverterObjectContainer):
    """
    Contains everything from the dat file, sorted into several
    categories.
    """

    def __init__(self):

        # Game version
        self.game_version: GameVersion = None

        # API reference
        self.nyan_api_objects: dict[str, NyanObject] = None

        # Things that don't exist in the game, e.g. Attributes
        # saved as RawAPIObjects
        self.pregen_nyan_objects: dict[str, RawAPIObject] = {}

        # Auxiliary
        self.strings: StringResource = None
        self.existing_graphics: list[str] = None

        # Phase 1: Genie-like objects
        # ConverterObject types (the data from the game)
        # key: obj_id; value: ConverterObject instance
        self.genie_units: dict[int, GenieUnitObject] = {}
        self.genie_techs: dict[int, GenieTechObject]  = {}
        self.genie_effect_bundles: dict[int, GenieEffectBundle]  = {}
        self.genie_civs: dict[int, GenieCivilizationObject]  = {}
        self.age_connections: dict[int, GenieAgeConnection]  = {}
        self.building_connections: dict[int, GenieBuildingConnection]  = {}
        self.tech_connections: dict[int, GenieTechConnection]  = {}
        self.unit_connections: dict[int, GenieUnitConnection]  = {}
        self.genie_graphics: dict[int, GenieGraphic]  = {}
        self.genie_sounds: dict[int, GenieSound]  = {}
        self.genie_terrains: dict[int, GenieTerrainObject]  = {}

        # Phase 2: API-like objects
        # ConverterObjectGroup types (things that will become
        # nyan objects)
        # key: group_id; value: ConverterObjectGroup instance

        # Keys are the ID of the first unit in line
        self.unit_lines: dict[int, GenieUnitLineGroup] = {}
        # Keys are the line ID of the unit connection
        self.unit_lines_vertical_ref: dict[int, GenieUnitLineGroup] = {}
        self.building_lines: dict[int, GenieBuildingLineGroup] = {}
        self.task_groups: dict[int, GenieUnitTaskGroup] = {}
        self.transform_groups: dict[int, GenieUnitTransformGroup] = {}
        self.villager_groups: dict[int, GenieVillagerGroup] = {}
        self.monk_groups: dict[int, GenieMonkGroup] = {}
        self.ambient_groups: dict[int, GenieAmbientGroup] = {}
        self.variant_groups: dict[int, GenieVariantGroup] = {}

        self.civ_groups: dict[int, GenieCivilizationGroup]  = {}

        self.tech_groups: dict[int, GenieTechEffectBundleGroup]  = {}
        self.age_upgrades: dict[int, AgeUpgrade]  = {}
        self.unit_upgrades: dict[int, UnitLineUpgrade]  = {}
        self.building_upgrades: dict[int, BuildingLineUpgrade] = {}
        self.stat_upgrades: dict[int, StatUpgrade] = {}
        self.unit_unlocks: dict[int, UnitUnlock] = {}
        self.building_unlocks: dict[int, BuildingUnlock] = {}
        self.civ_boni: dict[int, CivBonus] = {}
        self.initiated_techs: dict[int, InitiatedTech] = {}

        self.terrain_groups: dict[int, GenieTerrainGroup] = {}

        # Stores which line a unit is part of
        self.unit_ref: dict[int, GenieGameEntityGroup] = {}

        # Phase 3: sprites, sounds
        # Animation or Terrain graphics
        self.combined_sprites: dict[int, CombinedSprite] = {}
        self.combined_sounds: dict[int, CombinedSound] = {}
        self.combined_terrains: dict[int, CombinedTerrain] = {}

        self.graphics_exports: dict[int, MediaExportRequest] = {}
        self.blend_exports: dict[int, MediaExportRequest] = {}
        self.sound_exports: dict[int, MediaExportRequest] = {}
        self.metadata_exports: list[MetadataExport] = []

    def __repr__(self):
        return "GenieObjectContainer"
