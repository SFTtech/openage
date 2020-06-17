# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Provides functions that retrieve name lookup dicts for internal nyan object
names or filenames.
"""
import openage.convert.dataformat.aoc.internal_nyan_names as aoc_internal
import openage.convert.dataformat.de2.internal_nyan_names as de2_internal
import openage.convert.dataformat.hd.ak.internal_nyan_names as ak_internal
import openage.convert.dataformat.hd.fgt.internal_nyan_names as fgt_internal
import openage.convert.dataformat.hd.raj.internal_nyan_names as raj_internal
import openage.convert.dataformat.ror.internal_nyan_names as ror_internal
import openage.convert.dataformat.swgbcc.internal_nyan_names as swgbcc_internal
from openage.convert.dataformat.version_detect import GameEdition


def get_armor_class_lookups(game_version):
    """
    Return the name lookup dicts for armor classes.

    :param game_version: Game edition and expansions for which the lookups should be.
    :type game_version: tuple
    """
    game_edition = game_version[0]
    # game_expansions = game_version[1]

    if game_edition is GameEdition.ROR:
        return ror_internal.ARMOR_CLASS_LOOKUPS

    elif game_edition is GameEdition.AOC:
        return aoc_internal.ARMOR_CLASS_LOOKUPS

    elif game_edition is GameEdition.AOE2DE:
        armor_lookup_dict = {}
        armor_lookup_dict.update(aoc_internal.ARMOR_CLASS_LOOKUPS)
        armor_lookup_dict.update(fgt_internal.ARMOR_CLASS_LOOKUPS)
        armor_lookup_dict.update(ak_internal.ARMOR_CLASS_LOOKUPS)
        armor_lookup_dict.update(raj_internal.ARMOR_CLASS_LOOKUPS)
        armor_lookup_dict.update(de2_internal.ARMOR_CLASS_LOOKUPS)

        return armor_lookup_dict

    elif game_edition is GameEdition.SWGB:
        return swgbcc_internal.ARMOR_CLASS_LOOKUPS

    else:
        raise Exception("No lookup dict found for game version %s"
                        % game_edition.edition_name)


def get_civ_lookups(game_version):
    """
    Return the name lookup dicts for civs.

    :param game_version: Game edition and expansions for which the lookups should be.
    :type game_version: tuple
    """
    game_edition = game_version[0]
    # game_expansions = game_version[1]

    if game_edition is GameEdition.ROR:
        return ror_internal.CIV_GROUP_LOOKUPS

    elif game_edition is GameEdition.AOC:
        return aoc_internal.CIV_GROUP_LOOKUPS

    elif game_edition is GameEdition.AOE2DE:
        civ_lookup_dict = {}
        civ_lookup_dict.update(aoc_internal.CIV_GROUP_LOOKUPS)
        civ_lookup_dict.update(fgt_internal.CIV_GROUP_LOOKUPS)
        civ_lookup_dict.update(ak_internal.CIV_GROUP_LOOKUPS)
        civ_lookup_dict.update(raj_internal.CIV_GROUP_LOOKUPS)
        civ_lookup_dict.update(de2_internal.CIV_GROUP_LOOKUPS)

        return civ_lookup_dict

    elif game_edition is GameEdition.SWGB:
        return swgbcc_internal.CIV_GROUP_LOOKUPS

    else:
        raise Exception("No lookup dict found for game version %s"
                        % game_edition.edition_name)


def get_class_lookups(game_version):
    """
    Return the name lookup dicts for unit classes.

    :param game_version: Game edition and expansions for which the lookups should be.
    :type game_version: tuple
    """
    game_edition = game_version[0]
    # game_expansions = game_version[1]

    if game_edition is GameEdition.ROR:
        return ror_internal.CLASS_ID_LOOKUPS

    elif game_edition in (GameEdition.AOC, GameEdition.HDEDITION, GameEdition.AOE2DE):
        return aoc_internal.CLASS_ID_LOOKUPS

    elif game_edition is GameEdition.SWGB:
        return swgbcc_internal.CLASS_ID_LOOKUPS

    else:
        raise Exception("No lookup dict found for game version %s"
                        % game_edition.edition_name)


def get_command_lookups(game_version):
    """
    Return the name lookup dicts for unit commands.

    :param game_version: Game edition and expansions for which the lookups should be.
    :type game_version: tuple
    """
    game_edition = game_version[0]
    # game_expansions = game_version[1]

    if game_edition is GameEdition.ROR:
        return ror_internal.COMMAND_TYPE_LOOKUPS

    elif game_edition in (GameEdition.AOC, GameEdition.HDEDITION, GameEdition.AOE2DE):
        return aoc_internal.COMMAND_TYPE_LOOKUPS

    elif game_edition is GameEdition.SWGB:
        return swgbcc_internal.COMMAND_TYPE_LOOKUPS

    else:
        raise Exception("No lookup dict found for game version %s"
                        % game_edition.edition_name)


def get_entity_lookups(game_version):
    """
    Return the name lookup dicts for game entities.

    :param game_version: Game edition and expansions for which the lookups should be.
    :type game_version: tuple
    """
    game_edition = game_version[0]
    # game_expansions = game_version[1]

    entity_lookup_dict = {}

    if game_edition is GameEdition.ROR:
        entity_lookup_dict.update(ror_internal.UNIT_LINE_LOOKUPS)
        entity_lookup_dict.update(ror_internal.BUILDING_LINE_LOOKUPS)
        entity_lookup_dict.update(ror_internal.AMBIENT_GROUP_LOOKUPS)
        entity_lookup_dict.update(ror_internal.VARIANT_GROUP_LOOKUPS)

        return entity_lookup_dict

    elif game_edition is GameEdition.AOC:
        entity_lookup_dict.update(aoc_internal.UNIT_LINE_LOOKUPS)
        entity_lookup_dict.update(aoc_internal.BUILDING_LINE_LOOKUPS)
        entity_lookup_dict.update(aoc_internal.AMBIENT_GROUP_LOOKUPS)
        entity_lookup_dict.update(aoc_internal.VARIANT_GROUP_LOOKUPS)

        return entity_lookup_dict

    elif game_edition is GameEdition.AOE2DE:
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

        return entity_lookup_dict

    elif game_edition is GameEdition.SWGB:
        entity_lookup_dict.update(swgbcc_internal.UNIT_LINE_LOOKUPS)
        entity_lookup_dict.update(swgbcc_internal.BUILDING_LINE_LOOKUPS)
        entity_lookup_dict.update(swgbcc_internal.AMBIENT_GROUP_LOOKUPS)
        entity_lookup_dict.update(swgbcc_internal.VARIANT_GROUP_LOOKUPS)

        return entity_lookup_dict

    else:
        raise Exception("No lookup dict found for game version %s"
                        % game_edition.edition_name)


def get_gather_lookups(game_version):
    """
    Return the name lookup dicts for gather tasks.

    :param game_version: Game edition and expansions for which the lookups should be.
    :type game_version: tuple
    """
    game_edition = game_version[0]
    # game_expansions = game_version[1]

    if game_edition is GameEdition.ROR:
        return ror_internal.GATHER_TASK_LOOKUPS

    elif game_edition in (GameEdition.AOC, GameEdition.HDEDITION, GameEdition.AOE2DE):
        return aoc_internal.GATHER_TASK_LOOKUPS

    elif game_edition is GameEdition.SWGB:
        return swgbcc_internal.GATHER_TASK_LOOKUPS

    else:
        raise Exception("No lookup dict found for game version %s"
                        % game_edition.edition_name)


def get_graphic_set_lookups(game_version):
    """
    Return the name lookup dicts for civ graphic sets.

    :param game_version: Game edition and expansions for which the lookups should be.
    :type game_version: tuple
    """
    game_edition = game_version[0]
    # game_expansions = game_version[1]

    if game_edition is GameEdition.ROR:
        return ror_internal.GRAPHICS_SET_LOOKUPS

    elif game_edition is GameEdition.AOC:
        return aoc_internal.GRAPHICS_SET_LOOKUPS

    elif game_edition is GameEdition.AOE2DE:
        graphic_set_lookup_dict = {}
        graphic_set_lookup_dict.update(aoc_internal.GRAPHICS_SET_LOOKUPS)
        graphic_set_lookup_dict.update(fgt_internal.GRAPHICS_SET_LOOKUPS)
        graphic_set_lookup_dict.update(ak_internal.GRAPHICS_SET_LOOKUPS)
        graphic_set_lookup_dict.update(raj_internal.GRAPHICS_SET_LOOKUPS)
        graphic_set_lookup_dict.update(de2_internal.GRAPHICS_SET_LOOKUPS)

        return graphic_set_lookup_dict

    elif game_edition is GameEdition.SWGB:
        return swgbcc_internal.GRAPHICS_SET_LOOKUPS

    else:
        raise Exception("No lookup dict found for game version %s"
                        % game_edition.edition_name)


def get_restock_lookups(game_version):
    """
    Return the name lookup dicts for restock targets.

    :param game_version: Game edition and expansions for which the lookups should be.
    :type game_version: tuple
    """
    game_edition = game_version[0]
    # game_expansions = game_version[1]

    if game_edition is GameEdition.ROR:
        return None

    elif game_edition in (GameEdition.AOC, GameEdition.HDEDITION, GameEdition.AOE2DE):
        return aoc_internal.RESTOCK_TARGET_LOOKUPS

    elif game_edition is GameEdition.SWGB:
        return swgbcc_internal.RESTOCK_TARGET_LOOKUPS

    else:
        raise Exception("No lookup dict found for game version %s"
                        % game_edition.edition_name)


def get_tech_lookups(game_version):
    """
    Return the name lookup dicts for tech groups.

    :param game_version: Game edition and expansions for which the lookups should be.
    :type game_version: tuple
    """
    game_edition = game_version[0]
    # game_expansions = game_version[1]

    if game_edition is GameEdition.ROR:
        return ror_internal.TECH_GROUP_LOOKUPS

    elif game_edition is GameEdition.AOC:
        return aoc_internal.TECH_GROUP_LOOKUPS

    elif game_edition is GameEdition.AOE2DE:
        tech_lookup_dict = {}
        tech_lookup_dict.update(aoc_internal.TECH_GROUP_LOOKUPS)
        tech_lookup_dict.update(fgt_internal.TECH_GROUP_LOOKUPS)
        tech_lookup_dict.update(ak_internal.TECH_GROUP_LOOKUPS)
        tech_lookup_dict.update(raj_internal.TECH_GROUP_LOOKUPS)
        tech_lookup_dict.update(de2_internal.TECH_GROUP_LOOKUPS)

        return tech_lookup_dict

    elif game_edition is GameEdition.SWGB:
        return swgbcc_internal.TECH_GROUP_LOOKUPS

    else:
        raise Exception("No lookup dict found for game version %s"
                        % game_edition.edition_name)


def get_terrain_lookups(game_version):
    """
    Return the name lookup dicts for terrain groups.

    :param game_version: Game edition and expansions for which the lookups should be.
    :type game_version: tuple
    """
    game_edition = game_version[0]
    # game_expansions = game_version[1]

    if game_edition is GameEdition.ROR:
        return ror_internal.TERRAIN_GROUP_LOOKUPS

    elif game_edition is GameEdition.AOC:
        return aoc_internal.TERRAIN_GROUP_LOOKUPS

    elif game_edition is GameEdition.AOE2DE:
        terrain_lookup_dict = {}
        terrain_lookup_dict.update(aoc_internal.TERRAIN_GROUP_LOOKUPS)
        terrain_lookup_dict.update(fgt_internal.TERRAIN_GROUP_LOOKUPS)
        terrain_lookup_dict.update(ak_internal.TERRAIN_GROUP_LOOKUPS)
        terrain_lookup_dict.update(raj_internal.TERRAIN_GROUP_LOOKUPS)
        terrain_lookup_dict.update(de2_internal.TERRAIN_GROUP_LOOKUPS)

        return terrain_lookup_dict

    elif game_edition is GameEdition.SWGB:
        return swgbcc_internal.TERRAIN_GROUP_LOOKUPS

    else:
        raise Exception("No lookup dict found for game version %s"
                        % game_edition.edition_name)


def get_terrain_type_lookups(game_version):
    """
    Return the name lookup dicts for terrain types.

    :param game_version: Game edition and expansions for which the lookups should be.
    :type game_version: tuple
    """
    game_edition = game_version[0]
    # game_expansions = game_version[1]

    if game_edition is GameEdition.ROR:
        return ror_internal.TERRAIN_TYPE_LOOKUPS

    elif game_edition is GameEdition.AOC:
        return aoc_internal.TERRAIN_TYPE_LOOKUPS

    elif game_edition is GameEdition.AOE2DE:
        terrain_type_lookup_dict = {}
        terrain_type_lookup_dict.update(aoc_internal.TERRAIN_TYPE_LOOKUPS)
        terrain_type_lookup_dict.update(fgt_internal.TERRAIN_TYPE_LOOKUPS)
        terrain_type_lookup_dict.update(ak_internal.TERRAIN_TYPE_LOOKUPS)
        terrain_type_lookup_dict.update(raj_internal.TERRAIN_TYPE_LOOKUPS)
        terrain_type_lookup_dict.update(de2_internal.TERRAIN_TYPE_LOOKUPS)

        return terrain_type_lookup_dict

    elif game_edition is GameEdition.SWGB:
        return swgbcc_internal.TERRAIN_TYPE_LOOKUPS

    else:
        raise Exception("No lookup dict found for game version %s"
                        % game_edition.edition_name)
