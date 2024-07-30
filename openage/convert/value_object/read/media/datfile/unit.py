# Copyright 2013-2024 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R,too-many-lines
from __future__ import annotations
import typing

from functools import cache

from ...genie_structure import GenieStructure
from ....read.member_access import READ, READ_GEN, SKIP
from ....read.read_members import EnumLookupMember, ContinueReadMember, IncludeMembers, SubdataMember
from ....read.value_members import StorageType
from .lookup_dicts import COMMAND_ABILITY, OWNER_TYPE, RESOURCE_HANDLING, RESOURCE_TYPES, \
    DAMAGE_DRAW_TYPE, ARMOR_CLASS, UNIT_CLASSES, ELEVATION_MODES, FOG_VISIBILITY, \
    TERRAIN_RESTRICTIONS, BLAST_DEFENSE_TYPES, COMBAT_LEVELS, INTERACTION_MODES, \
    MINIMAP_MODES, UNIT_LEVELS, OBSTRUCTION_TYPES, SELECTION_EFFECTS, \
    ATTACK_MODES, BOUNDARY_IDS, BLAST_OFFENSE_TYPES, CREATABLE_TYPES, \
    GARRISON_TYPES

if typing.TYPE_CHECKING:
    from openage.convert.value_object.init.game_version import GameVersion
    from openage.convert.value_object.read.member_access import MemberAccess
    from openage.convert.value_object.read.read_members import ReadMember


class UnitCommand(GenieStructure):
    """
    also known as "Task" according to ES debug code,
    this structure is the master for spawn-unit actions.
    """

    @classmethod
    @cache
    def get_data_format_members(
        cls,
        game_version: GameVersion
    ) -> list[tuple[MemberAccess, str, StorageType, typing.Union[str, ReadMember]]]:
        """
        Return the members in this struct.
        """
        data_format = [
            # Type (0 = Generic, 1 = Tribe)
            (READ_GEN, "command_used", StorageType.INT_MEMBER, "int16_t"),
            (READ_GEN, "command_id", StorageType.ID_MEMBER, "int16_t"),
            (SKIP, "is_default", StorageType.BOOLEAN_MEMBER, "int8_t"),
            (READ_GEN, "type", StorageType.ID_MEMBER, EnumLookupMember(
                raw_type="int16_t",
                type_name="command_ability",
                lookup_dict=COMMAND_ABILITY
            )),
            (READ_GEN, "class_id", StorageType.ID_MEMBER, "int16_t"),
            (READ_GEN, "unit_id", StorageType.ID_MEMBER, "int16_t"),
            (SKIP, "terrain_id", StorageType.ID_MEMBER, "int16_t"),
            (READ_GEN, "resource_in", StorageType.INT_MEMBER, "int16_t"),        # carry resource
            # resource that multiplies the amount you can gather
            (READ_GEN, "resource_multiplier", StorageType.INT_MEMBER, "int16_t"),
            (READ_GEN, "resource_out", StorageType.INT_MEMBER, "int16_t"),       # drop resource
            (SKIP, "unused_resource", StorageType.INT_MEMBER, "int16_t"),
            (READ_GEN, "work_value1", StorageType.FLOAT_MEMBER, "float"),        # quantity
            (READ_GEN, "work_value2", StorageType.FLOAT_MEMBER, "float"),        # execution radius?
            (SKIP, "work_range", StorageType.FLOAT_MEMBER, "float"),
            (SKIP, "search_mode", StorageType.BOOLEAN_MEMBER, "int8_t"),
            (SKIP, "search_time", StorageType.FLOAT_MEMBER, "float"),
            (SKIP, "enable_targeting", StorageType.BOOLEAN_MEMBER, "int8_t"),
            (SKIP, "combat_level_flag", StorageType.ID_MEMBER, "int8_t"),
            (SKIP, "gather_type", StorageType.INT_MEMBER, "int16_t"),
            (SKIP, "work_mode2", StorageType.INT_MEMBER, "int16_t"),
            (SKIP, "owner_type", StorageType.ID_MEMBER, EnumLookupMember(
                # what can be selected as a target for the unit command?
                raw_type="int8_t",
                type_name="selection_type",
                lookup_dict=OWNER_TYPE
            )),
            # checks if the targeted unit has > 0 resources
            (SKIP, "carry_check", StorageType.BOOLEAN_MEMBER, "int8_t"),
            (SKIP, "state_build", StorageType.BOOLEAN_MEMBER, "int8_t"),
            # walking with tool but no resource
            (READ_GEN, "move_sprite_id", StorageType.ID_MEMBER, "int16_t"),
            # proceeding resource gathering or attack
            (READ_GEN, "proceed_sprite_id", StorageType.ID_MEMBER, "int16_t"),
            # actual execution or transformation graphic
            (READ_GEN, "work_sprite_id", StorageType.ID_MEMBER, "int16_t"),
            # display resources in hands
            (READ_GEN, "carry_sprite_id", StorageType.ID_MEMBER, "int16_t"),
            # sound to play when execution starts
            (READ_GEN, "resource_gather_sound_id", StorageType.ID_MEMBER, "int16_t"),
            # sound to play on resource drop
            (READ_GEN, "resource_deposit_sound_id", StorageType.ID_MEMBER, "int16_t"),
        ]

        if game_version.edition.game_id == "AOE2DE":
            data_format.extend([
                (READ_GEN, "wwise_resource_gather_sound_id", StorageType.ID_MEMBER, "uint32_t"),
                # sound to play on resource drop
                (READ_GEN, "wwise_resource_deposit_sound_id", StorageType.ID_MEMBER, "uint32_t"),
            ])

        return data_format


class UnitHeader(GenieStructure):

    @classmethod
    @cache
    def get_data_format_members(
        cls,
        game_version: GameVersion
    ) -> list[tuple[MemberAccess, str, StorageType, typing.Union[str, ReadMember]]]:
        """
        Return the members in this struct.
        """
        data_format = [
            (READ, "exists", StorageType.BOOLEAN_MEMBER, ContinueReadMember("uint8_t")),
            (READ, "unit_command_count", StorageType.INT_MEMBER, "uint16_t"),
            (READ_GEN, "unit_commands", StorageType.ARRAY_CONTAINER, SubdataMember(
                ref_type=UnitCommand,
                length="unit_command_count",
            )),
        ]

        return data_format


# Only used in SWGB
class UnitLine(GenieStructure):

    @classmethod
    @cache
    def get_data_format_members(
        cls,
        game_version: GameVersion
    ) -> list[tuple[MemberAccess, str, StorageType, typing.Union[str, ReadMember]]]:
        """
        Return the members in this struct.
        """
        data_format = [
            (READ_GEN, "id", StorageType.ID_MEMBER, "int16_t"),
            (READ, "name_length", StorageType.INT_MEMBER, "uint16_t"),
            (SKIP, "name", StorageType.STRING_MEMBER, "char[name_length]"),
            (READ, "unit_ids_count", StorageType.INT_MEMBER, "uint16_t"),
            (READ_GEN, "unit_ids", StorageType.ARRAY_ID, "int16_t[unit_ids_count]"),
        ]

        return data_format


class ResourceStorage(GenieStructure):

    @classmethod
    @cache
    def get_data_format_members(
        cls,
        game_version: GameVersion
    ) -> list[tuple[MemberAccess, str, StorageType, typing.Union[str, ReadMember]]]:
        """
        Return the members in this struct.
        """
        data_format = [
            (READ_GEN, "type", StorageType.ID_MEMBER, "int16_t"),
            (READ_GEN, "amount", StorageType.FLOAT_MEMBER, "float"),
            (SKIP, "used_mode", StorageType.ID_MEMBER, EnumLookupMember(
                raw_type="int8_t",
                type_name="resource_handling",
                lookup_dict=RESOURCE_HANDLING
            )),
        ]

        return data_format


class DamageGraphic(GenieStructure):

    @classmethod
    @cache
    def get_data_format_members(
        cls,
        game_version: GameVersion
    ) -> list[tuple[MemberAccess, str, StorageType, typing.Union[str, ReadMember]]]:
        """
        Return the members in this struct.
        """
        data_format = [
            (READ_GEN, "graphic_id", StorageType.ID_MEMBER, "int16_t"),
            (READ_GEN, "damage_percent", StorageType.INT_MEMBER, "int8_t"),
            # gets overwritten in aoe memory by the real apply_mode:
            (SKIP, "old_apply_mode", StorageType.ID_MEMBER, "int8_t"),
            (SKIP, "apply_mode", StorageType.ID_MEMBER, EnumLookupMember(
                raw_type="int8_t",
                type_name="damage_draw_type",
                lookup_dict=DAMAGE_DRAW_TYPE
            )),
        ]

        return data_format


class HitType(GenieStructure):

    @classmethod
    @cache
    def get_data_format_members(
        cls,
        game_version: GameVersion
    ) -> list[tuple[MemberAccess, str, StorageType, typing.Union[str, ReadMember]]]:
        """
        Return the members in this struct.
        """
        data_format = [
            (READ_GEN, "type_id", StorageType.ID_MEMBER, EnumLookupMember(
                raw_type="int16_t",
                type_name="hit_class",
                lookup_dict=ARMOR_CLASS
            )),
            (READ_GEN, "amount", StorageType.INT_MEMBER, "int16_t"),
        ]

        return data_format


class ResourceCost(GenieStructure):

    @classmethod
    @cache
    def get_data_format_members(
        cls,
        game_version: GameVersion
    ) -> list[tuple[MemberAccess, str, StorageType, typing.Union[str, ReadMember]]]:
        """
        Return the members in this struct.
        """
        data_format = [
            (READ_GEN, "type_id", StorageType.ID_MEMBER, EnumLookupMember(
                raw_type="int16_t",
                type_name="resource_types",
                lookup_dict=RESOURCE_TYPES
            )),
            (READ_GEN, "amount", StorageType.INT_MEMBER, "int16_t"),
            (READ_GEN, "enabled", StorageType.BOOLEAN_MEMBER, "int16_t"),
        ]

        return data_format


class BuildingAnnex(GenieStructure):

    @classmethod
    @cache
    def get_data_format_members(
        cls,
        game_version: GameVersion
    ) -> list[tuple[MemberAccess, str, StorageType, typing.Union[str, ReadMember]]]:
        """
        Return the members in this struct.
        """
        data_format = [
            (SKIP, "unit_id", StorageType.ID_MEMBER, "int16_t"),
            (SKIP, "misplaced0", StorageType.FLOAT_MEMBER, "float"),
            (SKIP, "misplaced1", StorageType.FLOAT_MEMBER, "float"),
        ]

        return data_format


class UnitObject(GenieStructure):
    """
    base properties for every unit entry.
    """

    @classmethod
    @cache
    def get_data_format_members(
        cls,
        game_version: GameVersion
    ) -> list[tuple[MemberAccess, str, StorageType, typing.Union[str, ReadMember]]]:
        """
        Return the members in this struct.
        """
        if game_version.edition.game_id not in ("AOE1DE", "AOE2DE"):
            data_format = [
                (READ, "name_length", StorageType.INT_MEMBER, "uint16_t"),
            ]
        else:
            data_format = []

        data_format.extend([
            (READ_GEN, "id0", StorageType.ID_MEMBER, "int16_t"),
        ])

        if game_version.edition.game_id == "AOE2DE":
            data_format.extend([
                (READ_GEN, "language_dll_name", StorageType.ID_MEMBER, "uint32_t"),
                (SKIP, "language_dll_creation", StorageType.ID_MEMBER, "uint32_t"),
            ])
        else:
            data_format.extend([
                (READ_GEN, "language_dll_name", StorageType.ID_MEMBER, "uint16_t"),
                (SKIP, "language_dll_creation", StorageType.ID_MEMBER, "uint16_t"),
            ])

        data_format.extend([
            (READ_GEN, "unit_class", StorageType.ID_MEMBER, EnumLookupMember(
                raw_type="int16_t",
                type_name="unit_classes",
                lookup_dict=UNIT_CLASSES
            )),
            (READ_GEN, "idle_graphic0", StorageType.ID_MEMBER, "int16_t"),
        ])

        if game_version.edition.game_id not in ("ROR", "AOE1DE"):
            data_format.extend([
                (SKIP, "idle_graphic1", StorageType.ID_MEMBER, "int16_t"),
            ])

        data_format.extend([
            (READ_GEN, "dying_graphic", StorageType.ID_MEMBER, "int16_t"),
            (SKIP, "undead_graphic", StorageType.ID_MEMBER, "int16_t"),
            # 1 = become `dead_unit_id` (reviving does not make it usable again)
            (SKIP, "death_mode", StorageType.ID_MEMBER, "int8_t"),
            # unit health. -1=insta-die
            (READ_GEN, "hit_points", StorageType.INT_MEMBER, "int16_t"),
            (READ_GEN, "line_of_sight", StorageType.FLOAT_MEMBER, "float"),
            # number of units that can garrison in there
            (READ_GEN, "garrison_capacity", StorageType.INT_MEMBER, "int8_t"),
            # size of the unit
            (READ_GEN, "radius_x", StorageType.FLOAT_MEMBER, "float"),
            (READ_GEN, "radius_y", StorageType.FLOAT_MEMBER, "float"),
            (READ_GEN, "radius_z", StorageType.FLOAT_MEMBER, "float"),
            (READ_GEN, "train_sound_id", StorageType.ID_MEMBER, "int16_t"),
        ])

        if game_version.edition.game_id not in ("ROR", "AOE1DE"):
            data_format.append((READ_GEN, "damage_sound_id", StorageType.ID_MEMBER, "int16_t"))

        data_format.extend([
            # unit id to become on death
            (READ_GEN, "dead_unit_id", StorageType.ID_MEMBER, "int16_t"),
        ])

        if game_version.edition.game_id in ("AOE1DE", "AOE2DE"):
            data_format.extend([
                (READ_GEN, "blood_unit_id", StorageType.ID_MEMBER, "int16_t"),
            ])

        data_format.extend([
            # 0=placable on top of others in scenario editor, 5=can't
            (SKIP, "placement_mode", StorageType.ID_MEMBER, "int8_t"),
            (SKIP, "can_be_built_on", StorageType.BOOLEAN_MEMBER, "int8_t"),  # 1=no footprints
            # frame id of the icon slp (57029) to place on the creation button
            (SKIP, "icon_id", StorageType.ID_MEMBER, "int16_t"),
            (SKIP, "hidden_in_editor", StorageType.BOOLEAN_MEMBER, "int8_t"),
            (SKIP, "old_portrait_icon_id", StorageType.ID_MEMBER, "int16_t"),
            # 0=unlocked by research, 1=insta-available
            (READ_GEN, "enabled", StorageType.BOOLEAN_MEMBER, "int8_t"),
        ])

        if game_version.edition.game_id not in ("ROR", "AOE1DE"):
            data_format.append((READ, "disabled", StorageType.BOOLEAN_MEMBER, "int8_t"))

        data_format.extend([
            # terrain id that's needed somewhere on the foundation (e.g. dock
            # water)
            (SKIP, "placement_side_terrain0", StorageType.ID_MEMBER, "int16_t"),
            (SKIP, "placement_side_terrain1", StorageType.ID_MEMBER, "int16_t"),    # second slot for ^
            # terrain needed for placement (e.g. dock: water)
            (SKIP, "placement_terrain0", StorageType.ID_MEMBER, "int16_t"),
            # alternative terrain needed for placement (e.g. dock: shallows)
            (SKIP, "placement_terrain1", StorageType.ID_MEMBER, "int16_t"),
            # minimum space required to allow placement in editor
            (READ_GEN, "clearance_size_x", StorageType.FLOAT_MEMBER, "float"),
            (READ_GEN, "clearance_size_y", StorageType.FLOAT_MEMBER, "float"),
            # determines the maxmimum elevation difference for terrain under the unit
            (READ_GEN, "elevation_mode", StorageType.ID_MEMBER, EnumLookupMember(
                raw_type="int8_t",
                type_name="elevation_modes",
                lookup_dict=ELEVATION_MODES
            )),
            (READ_GEN, "visible_in_fog", StorageType.ID_MEMBER, EnumLookupMember(
                raw_type="int8_t",
                type_name="fog_visibility",
                lookup_dict=FOG_VISIBILITY
            )),
            (READ_GEN, "terrain_restriction", StorageType.ID_MEMBER, EnumLookupMember(
                raw_type="int16_t",      # determines on what type of ground the unit can be placed/walk
                type_name="ground_type",  # is actually the id of the terrain_restriction entry!
                lookup_dict=TERRAIN_RESTRICTIONS
            )),
            # determines whether the unit can fly
            (READ_GEN, "fly_mode", StorageType.BOOLEAN_MEMBER, "int8_t"),
            (READ_GEN, "resource_capacity", StorageType.INT_MEMBER, "int16_t"),
            # when animals rot, their resources decay
            (READ_GEN, "resource_decay", StorageType.FLOAT_MEMBER, "float"),
            (READ_GEN, "blast_defense_level", StorageType.ID_MEMBER, EnumLookupMember(
                # receive blast damage from units that have lower or same
                # blast_attack_level.
                raw_type="int8_t",
                type_name="blast_types",
                lookup_dict=BLAST_DEFENSE_TYPES
            )),
            (SKIP, "combat_level", StorageType.ID_MEMBER, EnumLookupMember(
                raw_type="int8_t",
                type_name="combat_levels",
                lookup_dict=COMBAT_LEVELS
            )),
            (READ_GEN, "interaction_mode", StorageType.ID_MEMBER, EnumLookupMember(
                # what can be done with this unit?
                raw_type="int8_t",
                type_name="interaction_modes",
                lookup_dict=INTERACTION_MODES
            )),
            (SKIP, "map_draw_level", StorageType.ID_MEMBER, EnumLookupMember(
                # how does the unit show up on the minimap?
                raw_type="int8_t",
                type_name="minimap_modes",
                lookup_dict=MINIMAP_MODES
            )),
            (SKIP, "unit_level", StorageType.ID_MEMBER, EnumLookupMember(
                # selects the available ui command buttons for the unit
                raw_type="int8_t",
                type_name="command_attributes",
                lookup_dict=UNIT_LEVELS
            )),
            (SKIP, "attack_reaction", StorageType.FLOAT_MEMBER, "float"),
            # palette color id for the minimap
            (SKIP, "minimap_color", StorageType.ID_MEMBER, "int8_t"),
            # help text for this unit, stored in the translation dll.
            (SKIP, "language_dll_help", StorageType.ID_MEMBER, "int32_t"),
            (SKIP, "language_dll_hotkey_text", StorageType.ID_MEMBER, "int32_t"),
            # language dll dependent (kezb lazouts!)
            (SKIP, "hot_keys", StorageType.ID_MEMBER, "int32_t"),
            (SKIP, "recyclable", StorageType.BOOLEAN_MEMBER, "int8_t"),
            (SKIP, "enable_auto_gather", StorageType.BOOLEAN_MEMBER, "int8_t"),
            (SKIP, "doppelgaenger_on_death", StorageType.BOOLEAN_MEMBER, "int8_t"),
            (SKIP, "resource_gather_drop", StorageType.INT_MEMBER, "int8_t"),
        ])

        if game_version.edition.game_id not in ("ROR", "AOE1DE"):
            # bit 0 == 1 && val != 7: mask shown behind buildings,
            # bit 0 == 0 && val != {6, 10}: no mask displayed,
            # val == {-1, 7}: in open area mask is partially displayed
            # val == {6, 10}: building, causes mask to appear on units behind it
            data_format.extend([
                (SKIP, "occlusion_mode", StorageType.ID_MEMBER, "uint8_t"),
                (READ_GEN, "obstruction_type", StorageType.ID_MEMBER, EnumLookupMember(
                    raw_type="int8_t",
                    type_name="obstruction_types",
                    lookup_dict=OBSTRUCTION_TYPES
                )),
                (READ_GEN, "obstruction_class", StorageType.ID_MEMBER, "int8_t"),

                # bitfield of unit attributes:
                # bit 0: allow garrison,
                # bit 1: don't join formation,
                # bit 2: stealth unit,
                # bit 3: detector unit,
                # bit 4: mechanical unit,
                # bit 5: biological unit,
                # bit 6: self-shielding unit,
                # bit 7: invisible unit
                (READ_GEN, "trait", StorageType.ID_MEMBER, "uint8_t"),
                (READ_GEN, "civilization_id", StorageType.ID_MEMBER, "int8_t"),
                # leftover from trait+civ variable
                (SKIP, "attribute_piece", StorageType.INT_MEMBER, "int16_t"),
            ])
        elif game_version.edition.game_id == "AOE1DE":
            data_format.extend([
                (READ_GEN, "obstruction_type", StorageType.ID_MEMBER, EnumLookupMember(
                    raw_type="int8_t",
                    type_name="obstruction_types",
                    lookup_dict=OBSTRUCTION_TYPES
                )),
                (READ_GEN, "obstruction_class", StorageType.ID_MEMBER, "int8_t"),
            ])

        data_format.extend([
            (SKIP, "selection_effect", StorageType.ID_MEMBER, EnumLookupMember(
                # things that happen when the unit was selected
                raw_type="int8_t",
                type_name="selection_effects",
                lookup_dict=SELECTION_EFFECTS
            )),
            # 0: default, -16: fish trap, farm, 52: deadfarm, OLD-*, 116: flare,
            # whale, dolphin -123: fish
            (READ, "editor_selection_color", StorageType.ID_MEMBER, "uint8_t"),
            (READ_GEN, "selection_shape_x", StorageType.FLOAT_MEMBER, "float"),
            (READ_GEN, "selection_shape_y", StorageType.FLOAT_MEMBER, "float"),
            (READ_GEN, "selection_shape_z", StorageType.FLOAT_MEMBER, "float"),
        ])

        if game_version.edition.game_id == "AOE2DE":
            data_format.extend([
                (READ, "scenario_trigger_data0", StorageType.ID_MEMBER, "uint32_t"),
                (READ, "scenario_trigger_data1", StorageType.ID_MEMBER, "uint32_t"),
            ])

        data_format.extend([
            (READ_GEN, "resource_storage", StorageType.ARRAY_CONTAINER, SubdataMember(
                ref_type=ResourceStorage,
                length=3,
            )),
            (READ, "damage_graphic_count", StorageType.INT_MEMBER, "int8_t"),
            (READ_GEN, "damage_graphics", StorageType.ARRAY_CONTAINER, SubdataMember(
                ref_type=DamageGraphic,
                length="damage_graphic_count",
            )),
            (READ_GEN, "selection_sound_id", StorageType.ID_MEMBER, "int16_t"),
            (READ_GEN, "dying_sound_id", StorageType.ID_MEMBER, "int16_t"),
        ])

        if game_version.edition.game_id == "AOE2DE":
            data_format.extend([
                (READ_GEN, "wwise_creation_sound_id", StorageType.ID_MEMBER, "uint32_t"),
                (READ_GEN, "wwise_damage_sound_id", StorageType.ID_MEMBER, "uint32_t"),
                (READ_GEN, "wwise_selection_sound_id", StorageType.ID_MEMBER, "uint32_t"),
                (READ_GEN, "wwise_dying_sound_id", StorageType.ID_MEMBER, "uint32_t"),
            ])

        data_format.extend([
            (SKIP, "old_attack_mode", StorageType.ID_MEMBER, EnumLookupMember(  # obsolete, as it's copied when converting the unit
                raw_type="int8_t",     # things that happen when the unit was selected
                type_name="attack_modes",
                lookup_dict=ATTACK_MODES
            )),
            # leftover from alpha. would et units change terrain under them
            (SKIP, "convert_terrain", StorageType.INT_MEMBER, "int8_t"),
        ])

        if game_version.edition.game_id in ("AOE1DE", "AOE2DE"):
            data_format.extend([
                (SKIP, "name_len_debug", StorageType.INT_MEMBER, "uint16_t"),
                (READ, "name_len", StorageType.INT_MEMBER, "uint16_t"),
                (SKIP, "name", StorageType.STRING_MEMBER, "char[name_len]"),
            ])

        else:
            data_format.extend([
                (SKIP, "name", StorageType.STRING_MEMBER, "char[name_length]"),
            ])

            if game_version.edition.game_id == "SWGB":
                data_format.extend([
                    (READ, "name2_length", StorageType.INT_MEMBER, "uint16_t"),
                    (READ_GEN, "name2", StorageType.STRING_MEMBER, "char[name2_length]"),
                    (READ_GEN, "unit_line_id", StorageType.ID_MEMBER, "int16_t"),
                    (READ_GEN, "min_tech_level", StorageType.ID_MEMBER, "int8_t"),
                ])

        data_format.append((SKIP, "id1", StorageType.ID_MEMBER, "int16_t"))

        if game_version.edition.game_id not in ("ROR", "AOE1DE"):
            data_format.append((SKIP, "id2", StorageType.ID_MEMBER, "int16_t"))
        elif game_version.edition.game_id == "AOE1DE":
            data_format.append((READ_GEN, "telemetry_id", StorageType.ID_MEMBER, "int16_t"))

        return data_format


class TreeUnit(UnitObject):
    """
    type_id == 90
    """

    @classmethod
    @cache
    def get_data_format_members(
        cls,
        game_version: GameVersion
    ) -> list[tuple[MemberAccess, str, StorageType, typing.Union[str, ReadMember]]]:
        """
        Return the members in this struct.
        """
        data_format = [
            (READ_GEN, None, None, IncludeMembers(cls=UnitObject)),
        ]

        return data_format


class AnimatedUnit(UnitObject):
    """
    type_id >= 20
    Animated master object
    """

    @classmethod
    @cache
    def get_data_format_members(
        cls,
        game_version: GameVersion
    ) -> list[tuple[MemberAccess, str, StorageType, typing.Union[str, ReadMember]]]:
        """
        Return the members in this struct.
        """
        data_format = [
            (READ_GEN, None, None, IncludeMembers(cls=UnitObject)),
            (READ_GEN, "speed", StorageType.FLOAT_MEMBER, "float"),
        ]

        return data_format


class DoppelgangerUnit(AnimatedUnit):
    """
    type_id >= 25
    """

    @classmethod
    @cache
    def get_data_format_members(
        cls,
        game_version: GameVersion
    ) -> list[tuple[MemberAccess, str, StorageType, typing.Union[str, ReadMember]]]:
        """
        Return the members in this struct.
        """
        data_format = [
            (READ_GEN, None, None, IncludeMembers(cls=AnimatedUnit)),
        ]

        return data_format


class MovingUnit(DoppelgangerUnit):
    """
    type_id >= 30
    Moving master object
    """

    @classmethod
    @cache
    def get_data_format_members(
        cls,
        game_version: GameVersion
    ) -> list[tuple[MemberAccess, str, StorageType, typing.Union[str, ReadMember]]]:
        """
        Return the members in this struct.
        """
        data_format = [
            (READ_GEN, None, None, IncludeMembers(cls=DoppelgangerUnit)),
            (READ_GEN, "move_graphics", StorageType.ID_MEMBER, "int16_t"),
            (SKIP, "run_graphics", StorageType.ID_MEMBER, "int16_t"),
            (READ_GEN, "turn_speed", StorageType.FLOAT_MEMBER, "float"),
            (SKIP, "old_size_class", StorageType.ID_MEMBER, "int8_t"),
            # unit id for the ground traces
            (SKIP, "trail_unit_id", StorageType.ID_MEMBER, "int16_t"),
            # ground traces: -1: no tracking present, 2: projectiles with tracking unit
            (SKIP, "trail_opsions", StorageType.ID_MEMBER, "uint8_t"),
            # ground trace spacing: 0: no tracking, 0.5: trade cart, 0.12: some
            # projectiles, 0.4: other projectiles
            (SKIP, "trail_spacing", StorageType.FLOAT_MEMBER, "float"),
            (SKIP, "old_move_algorithm", StorageType.ID_MEMBER, "int8_t"),
        ]

        if game_version.edition.game_id not in ("ROR", "AOE1DE"):
            data_format.extend([
                (SKIP, "turn_radius", StorageType.FLOAT_MEMBER, "float"),
                (SKIP, "turn_radius_speed", StorageType.FLOAT_MEMBER, "float"),
                (READ_GEN, "max_yaw_per_sec_moving", StorageType.FLOAT_MEMBER, "float"),
                (SKIP, "stationary_yaw_revolution_time", StorageType.FLOAT_MEMBER, "float"),
                (SKIP, "max_yaw_per_sec_stationary", StorageType.FLOAT_MEMBER, "float"),
            ])

            if game_version.edition.game_id == "AOE2DE":
                data_format.extend([
                    (READ_GEN, "min_collision_size_multiplier", StorageType.FLOAT_MEMBER, "float"),
                ])

        return data_format


class ActionUnit(MovingUnit):
    """
    type_id >= 40
    Action master object
    """

    @classmethod
    @cache
    def get_data_format_members(
        cls,
        game_version: GameVersion
    ) -> list[tuple[MemberAccess, str, StorageType, typing.Union[str, ReadMember]]]:
        """
        Return the members in this struct.
        """
        data_format = [
            (READ_GEN, None, None, IncludeMembers(cls=MovingUnit)),
            # callback unit action id when found.
            # monument and sheep: 107 = enemy convert.
            # all auto-convertible units: 0, most other units: -1
            # e.g. when sheep are discovered
            (SKIP, "default_task_id", StorageType.ID_MEMBER, "int16_t"),
            (READ_GEN, "search_radius", StorageType.FLOAT_MEMBER, "float"),
            (READ_GEN, "work_rate", StorageType.FLOAT_MEMBER, "float"),
        ]

        if game_version.edition.game_id == "AOE2DE":
            data_format.extend([
                (READ, "drop_sites_count", StorageType.INT_MEMBER, "int16_t"),
                (READ_GEN, "drop_sites", StorageType.ARRAY_ID, "int16_t[drop_sites_count]"),
            ])
        else:
            data_format.extend([
                (READ_GEN, "drop_sites", StorageType.ARRAY_ID, "int16_t[2]"),
            ])

        data_format.extend([
            # 1: male villager; 2: female villager; 3+: free slots
            (READ_GEN, "task_group", StorageType.ID_MEMBER, "int8_t"),
            # basically this
            # creates a "swap
            # group id" where you
            # can place
            # different-graphic
            # units together.
            # sound played when a command is instanciated
            (READ_GEN, "command_sound_id", StorageType.ID_MEMBER, "int16_t"),
            # sound when the command is done (e.g. unit stops at target position)
            (SKIP, "stop_sound_id", StorageType.ID_MEMBER, "int16_t"),
        ])

        if game_version.edition.game_id == "AOE2DE":
            data_format.extend([
                (READ_GEN, "wwise_command_sound_id", StorageType.ID_MEMBER, "uint32_t"),
                (READ_GEN, "wwise_stop_sound_id", StorageType.ID_MEMBER, "uint32_t"),
            ])

        data_format.extend([
            # how animals run around randomly
            (SKIP, "run_pattern", StorageType.ID_MEMBER, "int8_t"),
        ])

        if game_version.edition.game_id in ("ROR", "AOE1DE", "AOE2DE"):
            data_format.extend([
                (READ, "unit_command_count", StorageType.INT_MEMBER, "uint16_t"),
                (READ_GEN, "unit_commands", StorageType.ARRAY_CONTAINER, SubdataMember(
                    ref_type=UnitCommand,
                    length="unit_command_count",
                )),
            ])

        return data_format


class ProjectileUnit(ActionUnit):
    """
    type_id >= 60
    Projectile master object
    """

    @classmethod
    @cache
    def get_data_format_members(
        cls,
        game_version: GameVersion
    ) -> list[tuple[MemberAccess, str, StorageType, typing.Union[str, ReadMember]]]:
        """
        Return the members in this struct.
        """
        data_format = [
            (READ_GEN, None, None, IncludeMembers(cls=ActionUnit)),
        ]

        if game_version.edition.game_id == "ROR":
            data_format.append((SKIP, "default_armor", StorageType.INT_MEMBER, "uint8_t"))
        else:
            data_format.append((SKIP, "default_armor", StorageType.INT_MEMBER, "int16_t"))

        data_format.extend([
            (READ, "attack_count", StorageType.INT_MEMBER, "uint16_t"),
            (READ_GEN, "attacks", StorageType.ARRAY_CONTAINER, SubdataMember(
                ref_type=HitType,
                length="attack_count",
            )),
            (READ, "armor_count", StorageType.INT_MEMBER, "uint16_t"),
            (READ_GEN, "armors", StorageType.ARRAY_CONTAINER, SubdataMember(
                ref_type=HitType,
                length="armor_count",
            )),
            (SKIP, "boundary_id", StorageType.ID_MEMBER, EnumLookupMember(
                # the damage received by this unit is multiplied by
                # the accessible values on the specified terrain restriction
                raw_type="int16_t",
                type_name="boundary_ids",
                lookup_dict=BOUNDARY_IDS
            )),
        ])

        if game_version.edition.game_id == "AOE2DE":
            data_format.append((READ_GEN, "bonus_damage_resistance",
                               StorageType.FLOAT_MEMBER, "float"))

        data_format.extend([
            (READ_GEN, "weapon_range_max", StorageType.FLOAT_MEMBER, "float"),
            (READ_GEN, "blast_range", StorageType.FLOAT_MEMBER, "float"),
            (READ_GEN, "attack_speed", StorageType.FLOAT_MEMBER, "float"),  # = "reload time"
            # which projectile to use?
            (READ_GEN, "projectile_id0", StorageType.ID_MEMBER, "int16_t"),
            # probablity of attack hit in percent
            (READ_GEN, "accuracy", StorageType.INT_MEMBER, "int16_t"),
            # = tower mode?; not used anywhere
            (SKIP, "break_off_combat", StorageType.INT_MEMBER, "int8_t"),
            # the frame number at which the missile is fired, = delay
            (READ_GEN, "frame_delay", StorageType.INT_MEMBER, "int16_t"),
            # graphics displacement in x, y and z
            (READ_GEN, "weapon_offset", StorageType.ARRAY_FLOAT, "float[3]"),
            (READ_GEN, "blast_level_offence", StorageType.ID_MEMBER, EnumLookupMember(
                # blasts damage units that have higher or same blast_defense_level
                raw_type="uint8_t",
                type_name="range_damage_type",
                lookup_dict=BLAST_OFFENSE_TYPES
            )),
            # minimum range that this projectile requests for display
            (READ_GEN, "weapon_range_min", StorageType.FLOAT_MEMBER, "float"),
        ])

        if game_version.edition.game_id not in ("ROR", "AOE1DE"):
            data_format.append((READ_GEN, "accuracy_dispersion", StorageType.FLOAT_MEMBER, "float"))

        data_format.extend([
            (READ_GEN, "attack_sprite_id", StorageType.ID_MEMBER, "int16_t"),
            (SKIP, "melee_armor_displayed", StorageType.INT_MEMBER, "int16_t"),
            (SKIP, "attack_displayed", StorageType.INT_MEMBER, "int16_t"),
            (SKIP, "range_displayed", StorageType.FLOAT_MEMBER, "float"),
            (SKIP, "reload_time_displayed", StorageType.FLOAT_MEMBER, "float"),
        ])

        if game_version.edition.game_id == "AOE2DE":
            data_format.append((READ_GEN, "blast_damage", StorageType.FLOAT_MEMBER, "float"))

        return data_format


class MissileUnit(ProjectileUnit):
    """
    type_id == 60
    Missile master object
    """

    @classmethod
    @cache
    def get_data_format_members(
        cls,
        game_version: GameVersion
    ) -> list[tuple[MemberAccess, str, StorageType, typing.Union[str, ReadMember]]]:
        """
        Return the members in this struct.
        """
        data_format = [
            (READ_GEN, None, None, IncludeMembers(cls=ProjectileUnit)),
            # 0 = default; 1 = projectile falls vertically to the bottom of the
            # map; 3 = teleporting projectiles
            (READ_GEN, "projectile_type", StorageType.ID_MEMBER, "int8_t"),
            # "better aiming". tech attribute 19 changes this: 0 = shoot at current pos; 1 = shoot at predicted pos
            (READ_GEN, "smart_mode", StorageType.BOOLEAN_MEMBER, "int8_t"),
            (READ_GEN, "drop_animation_mode", StorageType.ID_MEMBER, "int8_t"),  # 1 = disappear on hit
            # 1 = pass through hit object; 0 = stop projectile on hit; (only for
            # graphics, not pass-through damage)
            (READ_GEN, "penetration_mode", StorageType.ID_MEMBER, "int8_t"),
            (READ_GEN, "area_of_effect_special", StorageType.INT_MEMBER, "int8_t"),
            (READ_GEN, "projectile_arc", StorageType.FLOAT_MEMBER, "float"),
        ]

        return data_format


class LivingUnit(ProjectileUnit):
    """
    type_id >= 70
    """

    @classmethod
    @cache
    def get_data_format_members(
        cls,
        game_version: GameVersion
    ) -> list[tuple[MemberAccess, str, StorageType, typing.Union[str, ReadMember]]]:
        """
        Return the members in this struct.
        """
        data_format = [
            (READ_GEN, None, None, IncludeMembers(cls=ProjectileUnit)),
            (READ_GEN, "resource_cost", StorageType.ARRAY_CONTAINER, SubdataMember(
                ref_type=ResourceCost,
                length=3,
            )),
            (READ_GEN, "creation_time", StorageType.INT_MEMBER, "int16_t"),     # in seconds
            (READ_GEN, "train_location_id", StorageType.ID_MEMBER,
             "int16_t"),  # e.g. 118 = villager builder

            # where to place the button with the given icon
            # creation page:
            # +------------------------+
            # | 01 | 02 | 03 | 04 | 05 |
            # |----|----|----|----|----|
            # | 06 | 07 | 08 | 09 | 10 |
            # |----|----|----|----|----|
            # | 11 | 12 | 13 | 14 | 15 |
            # +------------------------+
            #
            # additional page (dock):
            # +------------------------+
            # | 21 | 22 | 23 | 24 | 25 |
            # |----|----|----|----|----|
            # | 26 | 27 | 28 | 29 | 30 |
            # |----|----|----|----|----|
            # | 31 | 32 | 33 | 34 | 35 |
            # +------------------------+
            (READ, "creation_button_id", StorageType.ID_MEMBER, "int8_t"),
        ]

        if game_version.edition.game_id not in ("ROR", "AOE1DE"):
            if game_version.edition.game_id == "AOE2DE":
                data_format.extend([
                    (READ_GEN, "heal_timer", StorageType.FLOAT_MEMBER, "float"),
                ])
            else:
                data_format.extend([
                    (SKIP, "rear_attack_modifier", StorageType.FLOAT_MEMBER, "float"),
                ])

            data_format.extend([
                (SKIP, "flank_attack_modifier", StorageType.FLOAT_MEMBER, "float"),
                (READ_GEN, "creatable_type", StorageType.ID_MEMBER, EnumLookupMember(
                    raw_type="uint8_t",
                    type_name="creatable_types",
                    lookup_dict=CREATABLE_TYPES
                )),
                # if building: "others" tab in editor, if living unit: "heroes" tab,
                # regenerate health + monk immunity
                (SKIP, "hero_mode", StorageType.BOOLEAN_MEMBER, "int8_t"),
                # graphic to display when units are garrisoned
                (READ_GEN, "garrison_graphic", StorageType.ID_MEMBER, "int32_t"),
                # projectile count when nothing garrisoned, including both normal and
                # duplicated projectiles
            ])

            if game_version.edition.game_id == "AOE2DE":
                data_format.extend([
                    (READ_GEN, "spawn_graphic_id", StorageType.ID_MEMBER, "int16_t"),
                    (READ_GEN, "upgrade_graphic_id", StorageType.ID_MEMBER, "int16_t"),
                    (READ_GEN, "hero_glow_graphic_id", StorageType.ID_MEMBER, "int16_t"),
                    (READ_GEN, "max_charge", StorageType.FLOAT_MEMBER, "float"),
                    (READ_GEN, "charge_regen_rate", StorageType.FLOAT_MEMBER, "float"),
                    (READ_GEN, "charge_cost", StorageType.ID_MEMBER, "int16_t"),
                    (READ_GEN, "charge_type", StorageType.ID_MEMBER, "int16_t"),
                    (READ_GEN, "min_convert_mod", StorageType.FLOAT_MEMBER, "float"),
                    (READ_GEN, "max_convert_mod", StorageType.FLOAT_MEMBER, "float"),
                    (READ_GEN, "convert_chance_mod", StorageType.FLOAT_MEMBER, "float"),
                ])

            data_format.extend([
                (READ_GEN, "projectile_min_count", StorageType.INT_MEMBER, "float"),
                # total projectiles when fully garrisoned
                (READ_GEN, "projectile_max_count", StorageType.INT_MEMBER, "int8_t"),
                (READ_GEN, "projectile_spawning_area_width", StorageType.FLOAT_MEMBER, "float"),
                (READ_GEN, "projectile_spawning_area_length", StorageType.FLOAT_MEMBER, "float"),
                # placement randomness, 0=from single spot, 1=random, 1<less random
                (READ_GEN, "projectile_spawning_area_randomness", StorageType.FLOAT_MEMBER, "float"),
                # uses its own attack values
                (READ_GEN, "projectile_id1", StorageType.ID_MEMBER, "int32_t"),
                # used just before unit reaches its target enemy, configuration:
                (SKIP, "special_graphic_id", StorageType.ID_MEMBER, "int32_t"),
                # determines adjacent unit graphics, if 1: building can adapt graphics
                # by adjacent buildings
                (SKIP, "special_activation", StorageType.ID_MEMBER, "int8_t"),
                # 0: default: only works when facing the hit angle.
                # 1: block: activates special graphic when receiving damage and not pursuing the attacker.
                # while idle, blocking decreases damage taken by 1/3.
                # also: a wall changes the graphics (when not-an-end piece) because of this.
                # 2: counter charge: activates special graphic when idle and enemy is near.
                # while idle, attacks back once on first received hit.
                # enemy must be unit type 70 and have less than 0.2 max range.
                # 3: charge: activates special graphic when closer than two tiles to the target.
                # deals 2X damage on 1st
                # hit
            ])

        # unit stats display of pierce armor
        data_format.append((SKIP, "pierce_armor_displayed", StorageType.INT_MEMBER, "int16_t"))

        return data_format


class BuildingUnit(LivingUnit):
    """
    type_id >= 80
    """

    @classmethod
    @cache
    def get_data_format_members(
        cls,
        game_version: GameVersion
    ) -> list[tuple[MemberAccess, str, StorageType, typing.Union[str, ReadMember]]]:
        """
        Return the members in this struct.
        """
        data_format = [
            (READ_GEN, None, None, IncludeMembers(cls=LivingUnit)),
            (READ_GEN, "construction_graphic_id", StorageType.ID_MEMBER, "int16_t"),
        ]

        if game_version.edition.game_id not in ("ROR", "AOE1DE"):
            data_format.append((SKIP, "snow_graphic_id", StorageType.ID_MEMBER, "int16_t"))

            if game_version.edition.game_id == "AOE2DE":
                data_format.extend([
                    (READ_GEN, "destruction_graphic_id", StorageType.ID_MEMBER, "int16_t"),
                    (READ_GEN, "destruction_rubble_graphic_id", StorageType.ID_MEMBER, "int16_t"),
                    (READ_GEN, "research_graphic_id", StorageType.ID_MEMBER, "int16_t"),
                    (READ_GEN, "research_complete_graphic_id", StorageType.ID_MEMBER, "int16_t"),
                ])

        data_format.extend([
            # 1=adjacent units may change the graphics
            (SKIP, "adjacent_mode", StorageType.BOOLEAN_MEMBER, "int8_t"),
            (SKIP, "graphics_angle", StorageType.INT_MEMBER, "int16_t"),
            (SKIP, "disappears_when_built", StorageType.BOOLEAN_MEMBER, "int8_t"),
            # second building to place directly on top
            (READ_GEN, "stack_unit_id", StorageType.ID_MEMBER, "int16_t"),
            # change underlying terrain to this id when building completed
            (READ_GEN, "foundation_terrain_id", StorageType.ID_MEMBER, "int16_t"),
            # deprecated terrain-like structures knowns as "Overlays" from alpha
            # AOE used for roads
            (SKIP, "old_overlay_id", StorageType.ID_MEMBER, "int16_t"),
            # research_id to be enabled when building creation
            (READ_GEN, "research_id", StorageType.ID_MEMBER, "int16_t"),
        ])

        if game_version.edition.game_id not in ("ROR", "AOE1DE"):
            data_format.extend([
                (SKIP, "can_burn", StorageType.BOOLEAN_MEMBER, "int8_t"),
                (SKIP, "building_annex", StorageType.ARRAY_CONTAINER, SubdataMember(
                    ref_type=BuildingAnnex,
                    length=4
                )),
                # building at which an annex building is attached to
                (READ_GEN, "head_unit_id", StorageType.ID_MEMBER, "int16_t"),
                # destination unit id when unit shall transform (e.g. unpack)
                (READ_GEN, "transform_unit_id", StorageType.ID_MEMBER, "int16_t"),
                (READ_GEN, "transform_sound_id", StorageType.ID_MEMBER, "int16_t"),
            ])

        data_format.append((READ_GEN, "construction_sound_id", StorageType.ID_MEMBER, "int16_t"))

        if game_version.edition.game_id not in ("ROR", "AOE1DE"):
            if game_version.edition.game_id == "AOE2DE":
                data_format.extend([
                    (READ_GEN, "wwise_construction_sound_id", StorageType.ID_MEMBER, "uint32_t"),
                    (READ_GEN, "wwise_transform_sound_id", StorageType.ID_MEMBER, "uint32_t"),
                ])

            data_format.extend([
                (READ_GEN, "garrison_type", StorageType.BITFIELD_MEMBER, EnumLookupMember(
                    raw_type="int8_t",
                    type_name="garrison_types",
                    lookup_dict=GARRISON_TYPES
                )),
                (READ_GEN, "garrison_heal_rate", StorageType.FLOAT_MEMBER, "float"),
                (SKIP, "garrison_repair_rate", StorageType.FLOAT_MEMBER, "float"),
                # id of the unit used for salvages
                (SKIP, "salvage_unit_id", StorageType.ID_MEMBER, "int16_t"),
                # list of attributes for salvages (looting table)
                (SKIP, "salvage_attributes", StorageType.ARRAY_INT, "int8_t[6]"),
            ])

        return data_format


# unit type id => human readable name
# used as member name in the resulting struct
unit_type_lookup = {
    10: "object",
    20: "animated",
    25: "doppelganger",
    30: "moving",
    40: "action",
    60: "missile",
    70: "living",
    80: "building",
    90: "tree",
}


# name => attribute class
unit_type_class_lookup = {
    "object":         UnitObject,
    "animated":       AnimatedUnit,
    "doppelganger":   DoppelgangerUnit,
    "moving":         MovingUnit,
    "action":         ActionUnit,
    "missile":        MissileUnit,
    "living":         LivingUnit,
    "building":       BuildingUnit,
    "tree":           TreeUnit,
}
