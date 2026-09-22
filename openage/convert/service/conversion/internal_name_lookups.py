# Copyright 2020-2026 the openage authors. See copying.md for legal info.

"""
Provides functions that retrieve name lookup dicts for internal nyan object
names or filenames.
"""
from __future__ import annotations
import typing

from functools import cache

import openage.convert.value_object.conversion.aoc.internal_nyan_names as aoc_internal
import openage.convert.value_object.conversion.de1.internal_nyan_names as de1_internal
import openage.convert.value_object.conversion.de2.internal_nyan_names as de2_internal
import openage.convert.value_object.conversion.hd.ak.internal_nyan_names as ak_internal
import openage.convert.value_object.conversion.hd.fgt.internal_nyan_names as fgt_internal
import openage.convert.value_object.conversion.hd.raj.internal_nyan_names as raj_internal
import openage.convert.value_object.conversion.ror.internal_nyan_names as ror_internal
import openage.convert.value_object.conversion.swgb.internal_nyan_names as swgb_internal

if typing.TYPE_CHECKING:
    from openage.convert.value_object.init.game_version import GameVersion


@cache
def get_armor_class_lookups(game_version: GameVersion) -> dict[int, str]:
    """
    Return the name lookup dicts for armor classes.

    :param game_version: Game edition and expansions for which the lookups should be.
    :type game_version: GameVersion
    """
    game_edition = game_version.edition
    # game_expansions = game_version.expansions

    if game_edition.game_id == "ROR":
        return ror_internal.ARMOR_CLASS_LOOKUPS

    if game_edition.game_id == "AOC":
        return aoc_internal.ARMOR_CLASS_LOOKUPS

    if game_edition.game_id == "HDEDITION":
        armor_lookup_dict = {}
        armor_lookup_dict.update(aoc_internal.ARMOR_CLASS_LOOKUPS)

        # TODO: Include expansion lookups

        return armor_lookup_dict

    if game_edition.game_id == "AOE1DE":
        armor_lookup_dict = {}
        armor_lookup_dict.update(ror_internal.ARMOR_CLASS_LOOKUPS)
        armor_lookup_dict.update(de1_internal.ARMOR_CLASS_LOOKUPS)

        return armor_lookup_dict

    if game_edition.game_id == "AOE2DE":
        armor_lookup_dict = {}
        armor_lookup_dict.update(aoc_internal.ARMOR_CLASS_LOOKUPS)
        armor_lookup_dict.update(fgt_internal.ARMOR_CLASS_LOOKUPS)
        armor_lookup_dict.update(ak_internal.ARMOR_CLASS_LOOKUPS)
        armor_lookup_dict.update(raj_internal.ARMOR_CLASS_LOOKUPS)
        armor_lookup_dict.update(de2_internal.ARMOR_CLASS_LOOKUPS)

        # armor classes used by recent DE2 builds that have no name yet; they
        # have to be present up front so that the pregen processor creates the
        # matching attribute change types
        for unnamed_class in (41,):
            armor_lookup_dict.setdefault(unnamed_class, f"ArmorClass{unnamed_class}")

        return GeneratedLabels(armor_lookup_dict, prefix="ArmorClass")

    if game_edition.game_id == "SWGB":
        return swgb_internal.ARMOR_CLASS_LOOKUPS

    raise RuntimeError(f"No lookup dict found for game version {game_edition.edition_name}")


@cache
def get_civ_lookups(game_version: GameVersion) -> dict[int, tuple[str, str]]:
    """
    Return the name lookup dicts for civs.

    :param game_version: Game edition and expansions for which the lookups should be.
    :type game_version: GameVersion
    """
    game_edition = game_version.edition
    # game_expansions = game_version.expansions

    if game_edition.game_id == "ROR":
        return ror_internal.CIV_GROUP_LOOKUPS

    if game_edition.game_id == "AOC":
        return aoc_internal.CIV_GROUP_LOOKUPS

    if game_edition.game_id == "HDEDITION":
        civ_lookup_dict = {}
        civ_lookup_dict.update(aoc_internal.CIV_GROUP_LOOKUPS)

        # TODO: Include expansion lookups

        return civ_lookup_dict

    if game_edition.game_id == "AOE1DE":
        return ror_internal.CIV_GROUP_LOOKUPS

    if game_edition.game_id == "AOE2DE":
        civ_lookup_dict = {}
        civ_lookup_dict.update(aoc_internal.CIV_GROUP_LOOKUPS)
        civ_lookup_dict.update(fgt_internal.CIV_GROUP_LOOKUPS)
        civ_lookup_dict.update(ak_internal.CIV_GROUP_LOOKUPS)
        civ_lookup_dict.update(raj_internal.CIV_GROUP_LOOKUPS)
        civ_lookup_dict.update(de2_internal.CIV_GROUP_LOOKUPS)

        return GeneratedNames(civ_lookup_dict, prefix="Civ", snake="civ")

    if game_edition.game_id == "SWGB":
        return swgb_internal.CIV_GROUP_LOOKUPS

    raise RuntimeError(f"No lookup dict found for game version {game_edition.edition_name}")


@cache
def get_class_lookups(game_version: GameVersion) -> dict[int, str]:
    """
    Return the name lookup dicts for unit classes.

    :param game_version: Game edition and expansions for which the lookups should be.
    :type game_version: GameVersion
    """
    game_edition = game_version.edition
    # game_expansions = game_version.expansions

    if game_edition.game_id in ("ROR", "AOE1DE"):
        return ror_internal.CLASS_ID_LOOKUPS

    if game_edition.game_id in ("AOC", "HDEDITION", "AOE2DE"):
        return aoc_internal.CLASS_ID_LOOKUPS

    if game_edition.game_id == "SWGB":
        return swgb_internal.CLASS_ID_LOOKUPS

    raise RuntimeError(f"No lookup dict found for game version {game_edition.edition_name}")


@cache
def get_command_lookups(game_version: GameVersion) -> dict[int, tuple[str, str]]:
    """
    Return the name lookup dicts for unit commands.

    :param game_version: Game edition and expansions for which the lookups should be.
    :type game_version: GameVersion
    """
    game_edition = game_version.edition
    # game_expansions = game_version.expansions

    if game_edition.game_id in ("ROR", "AOE1DE"):
        return ror_internal.COMMAND_TYPE_LOOKUPS

    if game_edition.game_id in ("AOC", "HDEDITION", "AOE2DE"):
        return aoc_internal.COMMAND_TYPE_LOOKUPS

    if game_edition.game_id == "SWGB":
        return swgb_internal.COMMAND_TYPE_LOOKUPS

    raise RuntimeError(f"No lookup dict found for game version {game_edition.edition_name}")


class GeneratedLabels(dict):
    """
    Single-string name lookup that generates a label for unknown ids.

    Used for the lookups that map an id to one name instead of a name pair.
    """

    def __init__(self, *args, prefix: str = "Class"):
        super().__init__(*args)
        self.prefix = prefix

    def __missing__(self, key: int) -> str:
        generated = f"{self.prefix}{key}"
        self[key] = generated
        return generated


class GeneratedNames(dict):
    """
    Name lookup that generates a placeholder name for unknown ids.

    Recent AoE2: DE builds contain game entities and civs that the converter
    has no curated nyan name for yet. Generating a placeholder keeps the
    conversion going instead of aborting on the first unknown id.
    """

    def __init__(self, *args, prefix: str = "GameEntity", snake: str = "game_entity"):
        super().__init__(*args)
        self.prefix = prefix
        self.snake = snake

    def __missing__(self, key: int) -> tuple[str, str]:
        generated = (f"{self.prefix}{key}", f"{self.snake}_{key}")
        self[key] = generated
        return generated


@cache
def get_entity_lookups(game_version: GameVersion) -> dict[int, tuple[str, str]]:
    """
    Return the name lookup dicts for game entities.

    :param game_version: Game edition and expansions for which the lookups should be.
    :type game_version: GameVersion
    """
    game_edition = game_version.edition
    # game_expansions = game_version.expansions

    entity_lookup_dict = {}

    if game_edition.game_id == "ROR":
        entity_lookup_dict.update(ror_internal.UNIT_LINE_LOOKUPS)
        entity_lookup_dict.update(ror_internal.BUILDING_LINE_LOOKUPS)
        entity_lookup_dict.update(ror_internal.AMBIENT_GROUP_LOOKUPS)
        entity_lookup_dict.update(ror_internal.VARIANT_GROUP_LOOKUPS)

        return entity_lookup_dict

    if game_edition.game_id == "AOC":
        entity_lookup_dict.update(aoc_internal.UNIT_LINE_LOOKUPS)
        entity_lookup_dict.update(aoc_internal.BUILDING_LINE_LOOKUPS)
        entity_lookup_dict.update(aoc_internal.AMBIENT_GROUP_LOOKUPS)
        entity_lookup_dict.update(aoc_internal.VARIANT_GROUP_LOOKUPS)

        return entity_lookup_dict

    if game_edition.game_id == "HDEDITION":
        entity_lookup_dict.update(aoc_internal.UNIT_LINE_LOOKUPS)
        entity_lookup_dict.update(aoc_internal.BUILDING_LINE_LOOKUPS)
        entity_lookup_dict.update(aoc_internal.AMBIENT_GROUP_LOOKUPS)
        entity_lookup_dict.update(aoc_internal.VARIANT_GROUP_LOOKUPS)

        # TODO: Include expansion lookups

        return entity_lookup_dict

    if game_edition.game_id == "AOE1DE":
        entity_lookup_dict.update(ror_internal.UNIT_LINE_LOOKUPS)
        entity_lookup_dict.update(ror_internal.BUILDING_LINE_LOOKUPS)
        entity_lookup_dict.update(ror_internal.AMBIENT_GROUP_LOOKUPS)
        entity_lookup_dict.update(ror_internal.VARIANT_GROUP_LOOKUPS)

        return entity_lookup_dict

    if game_edition.game_id == "AOE2DE":
        entity_lookup_dict.update(aoc_internal.UNIT_LINE_LOOKUPS)
        entity_lookup_dict.update(aoc_internal.BUILDING_LINE_LOOKUPS)
        entity_lookup_dict.update(aoc_internal.AMBIENT_GROUP_LOOKUPS)
        entity_lookup_dict.update(aoc_internal.VARIANT_GROUP_LOOKUPS)

        entity_lookup_dict.update(fgt_internal.UNIT_LINE_LOOKUPS)
        entity_lookup_dict.update(fgt_internal.BUILDING_LINE_LOOKUPS)
        entity_lookup_dict.update(fgt_internal.AMBIENT_GROUP_LOOKUPS)
        entity_lookup_dict.update(fgt_internal.VARIANT_GROUP_LOOKUPS)

        entity_lookup_dict.update(ak_internal.UNIT_LINE_LOOKUPS)
        entity_lookup_dict.update(ak_internal.BUILDING_LINE_LOOKUPS)
        entity_lookup_dict.update(ak_internal.AMBIENT_GROUP_LOOKUPS)
        entity_lookup_dict.update(ak_internal.VARIANT_GROUP_LOOKUPS)

        entity_lookup_dict.update(raj_internal.UNIT_LINE_LOOKUPS)
        entity_lookup_dict.update(raj_internal.BUILDING_LINE_LOOKUPS)
        entity_lookup_dict.update(raj_internal.AMBIENT_GROUP_LOOKUPS)
        entity_lookup_dict.update(raj_internal.VARIANT_GROUP_LOOKUPS)

        entity_lookup_dict.update(de2_internal.UNIT_LINE_LOOKUPS)
        entity_lookup_dict.update(de2_internal.BUILDING_LINE_LOOKUPS)
        entity_lookup_dict.update(de2_internal.AMBIENT_GROUP_LOOKUPS)
        entity_lookup_dict.update(de2_internal.VARIANT_GROUP_LOOKUPS)

        return GeneratedNames(entity_lookup_dict)

    if game_edition.game_id == "SWGB":
        entity_lookup_dict.update(swgb_internal.UNIT_LINE_LOOKUPS)
        entity_lookup_dict.update(swgb_internal.BUILDING_LINE_LOOKUPS)
        entity_lookup_dict.update(swgb_internal.AMBIENT_GROUP_LOOKUPS)
        entity_lookup_dict.update(swgb_internal.VARIANT_GROUP_LOOKUPS)

        return entity_lookup_dict

    raise RuntimeError(f"No lookup dict found for game version {game_edition.edition_name}")


@cache
def get_gather_lookups(game_version: GameVersion) -> dict[int, tuple[str, str]]:
    """
    Return the name lookup dicts for gather tasks.

    :param game_version: Game edition and expansions for which the lookups should be.
    :type game_version: GameVersion
    """
    game_edition = game_version.edition
    # game_expansions = game_version.expansions

    if game_edition.game_id in ("ROR", "AOE1DE"):
        return ror_internal.GATHER_TASK_LOOKUPS

    if game_edition.game_id in ("AOC", "HDEDITION", "AOE2DE"):
        return aoc_internal.GATHER_TASK_LOOKUPS

    if game_edition.game_id == "SWGB":
        return swgb_internal.GATHER_TASK_LOOKUPS

    raise RuntimeError(f"No lookup dict found for game version {game_edition.edition_name}")


@cache
def get_graphic_set_lookups(
    game_version: GameVersion
) -> dict[int, tuple[tuple[int, ...], str, str]]:
    """
    Return the name lookup dicts for civ graphic sets.

    :param game_version: Game edition and expansions for which the lookups should be.
    :type game_version: GameVersion
    """
    game_edition = game_version.edition
    # game_expansions = game_version.expansions

    if game_edition.game_id == "ROR":
        return ror_internal.GRAPHICS_SET_LOOKUPS

    if game_edition.game_id == "AOC":
        return aoc_internal.GRAPHICS_SET_LOOKUPS

    if game_edition.game_id == "HDEDITION":
        graphic_set_lookup_dict = {}
        graphic_set_lookup_dict.update(aoc_internal.GRAPHICS_SET_LOOKUPS)

        # TODO: Include expansion lookups

        return graphic_set_lookup_dict

    if game_edition.game_id == "AOE1DE":
        return ror_internal.GRAPHICS_SET_LOOKUPS

    if game_edition.game_id == "AOE2DE":
        graphic_set_lookup_dict = {}
        graphic_set_lookup_dict.update(aoc_internal.GRAPHICS_SET_LOOKUPS)
        graphic_set_lookup_dict.update(fgt_internal.GRAPHICS_SET_LOOKUPS)
        graphic_set_lookup_dict.update(ak_internal.GRAPHICS_SET_LOOKUPS)
        graphic_set_lookup_dict.update(raj_internal.GRAPHICS_SET_LOOKUPS)
        graphic_set_lookup_dict.update(de2_internal.GRAPHICS_SET_LOOKUPS)

        # civs introduced by recent DE2 expansions are not assigned to an
        # architecture set yet; collect them in a generic set instead of
        # aborting the conversion
        mapped_civs = set()
        for items in graphic_set_lookup_dict.values():
            mapped_civs.update(items[0])

        unmapped_civs = tuple(civ_id for civ_id in range(256)
                              if civ_id not in mapped_civs)
        graphic_set_lookup_dict[-1] = (unmapped_civs, "Generic", "generic")

        return graphic_set_lookup_dict

    if game_edition.game_id == "SWGB":
        return swgb_internal.GRAPHICS_SET_LOOKUPS

    raise KeyError(f"No lookup dict found for game version {game_edition.edition_name}")


@cache
def get_restock_lookups(game_version: GameVersion) -> dict[int, tuple[str, str]]:
    """
    Return the name lookup dicts for restock targets.

    :param game_version: Game edition and expansions for which the lookups should be.
    :type game_version: GameVersion
    """
    game_edition = game_version.edition
    # game_expansions = game_version.expansions

    if game_edition.game_id == "ROR":
        return None

    if game_edition.game_id in ("AOC", "HDEDITION", "AOE2DE"):
        return aoc_internal.RESTOCK_TARGET_LOOKUPS

    if game_edition.game_id == "AOE1DE":
        # TODO: Farms
        return None

    if game_edition.game_id == "SWGB":
        return swgb_internal.RESTOCK_TARGET_LOOKUPS

    raise RuntimeError(f"No lookup dict found for game version {game_edition.edition_name}")


@cache
def get_tech_lookups(game_version: GameVersion) -> dict[int, tuple[str, str]]:
    """
    Return the name lookup dicts for tech groups.

    :param game_version: Game edition and expansions for which the lookups should be.
    :type game_version: GameVersion
    """
    game_edition = game_version.edition
    # game_expansions = game_version.expansions

    if game_edition.game_id == "ROR":
        return ror_internal.TECH_GROUP_LOOKUPS

    if game_edition.game_id == "AOC":
        return aoc_internal.TECH_GROUP_LOOKUPS

    if game_edition.game_id == "HDEDITION":
        tech_lookup_dict = {}
        tech_lookup_dict.update(aoc_internal.TECH_GROUP_LOOKUPS)

        # TODO: Include expansion lookups

        return tech_lookup_dict

    if game_edition.game_id == "AOE1DE":
        return ror_internal.TECH_GROUP_LOOKUPS

    if game_edition.game_id == "AOE2DE":
        tech_lookup_dict = {}
        tech_lookup_dict.update(aoc_internal.TECH_GROUP_LOOKUPS)
        tech_lookup_dict.update(fgt_internal.TECH_GROUP_LOOKUPS)
        tech_lookup_dict.update(ak_internal.TECH_GROUP_LOOKUPS)
        tech_lookup_dict.update(raj_internal.TECH_GROUP_LOOKUPS)
        tech_lookup_dict.update(de2_internal.TECH_GROUP_LOOKUPS)

        return GeneratedNames(tech_lookup_dict, prefix="Tech", snake="tech")

    if game_edition.game_id == "SWGB":
        return swgb_internal.TECH_GROUP_LOOKUPS

    raise RuntimeError(f"No lookup dict found for game version {game_edition.edition_name}")


@cache
def get_terrain_lookups(
    game_version: GameVersion
) -> dict[int, tuple[tuple[int, ...], str, str]]:
    """
    Return the name lookup dicts for terrain groups.

    :param game_version: Game edition and expansions for which the lookups should be.
    :type game_version: GameVersion
    """
    game_edition = game_version.edition
    # game_expansions = game_version.expansions

    if game_edition.game_id == "ROR":
        return ror_internal.TERRAIN_GROUP_LOOKUPS

    if game_edition.game_id == "AOC":
        return aoc_internal.TERRAIN_GROUP_LOOKUPS

    if game_edition.game_id == "HDEDITION":
        terrain_lookup_dict = {}
        terrain_lookup_dict.update(aoc_internal.TERRAIN_GROUP_LOOKUPS)

        # TODO: Include expansion lookups

        return terrain_lookup_dict

    if game_edition.game_id == "AOE1DE":
        return ror_internal.TERRAIN_GROUP_LOOKUPS

    if game_edition.game_id == "AOE2DE":
        terrain_lookup_dict = {}
        terrain_lookup_dict.update(aoc_internal.TERRAIN_GROUP_LOOKUPS)
        terrain_lookup_dict.update(fgt_internal.TERRAIN_GROUP_LOOKUPS)
        terrain_lookup_dict.update(ak_internal.TERRAIN_GROUP_LOOKUPS)
        terrain_lookup_dict.update(raj_internal.TERRAIN_GROUP_LOOKUPS)
        terrain_lookup_dict.update(de2_internal.TERRAIN_GROUP_LOOKUPS)

        return terrain_lookup_dict

    if game_edition.game_id == "SWGB":
        return swgb_internal.TERRAIN_GROUP_LOOKUPS

    raise RuntimeError(f"No lookup dict found for game version {game_edition.edition_name}")


@cache
def get_terrain_type_lookups(game_version: GameVersion) -> dict[int, tuple]:
    """
    Return the name lookup dicts for terrain types.

    :param game_version: Game edition and expansions for which the lookups should be.
    :type game_version: GameVersion
    """
    game_edition = game_version.edition
    # game_expansions = game_version.expansions

    if game_edition.game_id == "ROR":
        return ror_internal.TERRAIN_TYPE_LOOKUPS

    if game_edition.game_id == "AOC":
        return aoc_internal.TERRAIN_TYPE_LOOKUPS

    if game_edition.game_id == "HDEDITION":
        terrain_type_lookup_dict = {}
        terrain_type_lookup_dict.update(aoc_internal.TERRAIN_TYPE_LOOKUPS)

        # TODO: Include expansion lookups

        return terrain_type_lookup_dict

    if game_edition.game_id == "AOE1DE":
        return ror_internal.TERRAIN_TYPE_LOOKUPS

    if game_edition.game_id == "AOE2DE":
        terrain_type_lookup_dict = {}
        terrain_type_lookup_dict.update(aoc_internal.TERRAIN_TYPE_LOOKUPS)
        terrain_type_lookup_dict.update(fgt_internal.TERRAIN_TYPE_LOOKUPS)
        terrain_type_lookup_dict.update(ak_internal.TERRAIN_TYPE_LOOKUPS)
        terrain_type_lookup_dict.update(raj_internal.TERRAIN_TYPE_LOOKUPS)
        terrain_type_lookup_dict.update(de2_internal.TERRAIN_TYPE_LOOKUPS)

        return terrain_type_lookup_dict

    if game_edition.game_id == "SWGB":
        return swgb_internal.TERRAIN_TYPE_LOOKUPS

    raise RuntimeError(f"No lookup dict found for game version {game_edition.edition_name}")
