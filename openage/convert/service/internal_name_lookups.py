# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Provides functions that retrieve name lookup dicts for internal nyan object
names or filenames.
"""
import openage.convert.dataformat.aoc.internal_nyan_names as aoc_internal
import openage.convert.dataformat.ror.internal_nyan_names as ror_internal
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

    elif game_edition is GameEdition.AOC:
        return aoc_internal.CLASS_ID_LOOKUPS


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

    elif game_edition is GameEdition.AOC:
        return aoc_internal.COMMAND_TYPE_LOOKUPS


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

    elif game_edition is GameEdition.AOC:
        return aoc_internal.GATHER_TASK_LOOKUPS


def get_graphic_set_lookups(game_version):
    """
    Return the name lookup dicts for civ graphic sets.

    :param game_version: Game edition and expansions for which the lookups should be.
    :type game_version: tuple
    """
    game_edition = game_version[0]
    # game_expansions = game_version[1]

    if game_edition is GameEdition.ROR:
        pass

    elif game_edition is GameEdition.AOC:
        return aoc_internal.GRAPHICS_SET_LOOKUPS


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

    elif game_edition is GameEdition.AOC:
        return aoc_internal.RESTOCK_TARGET_LOOKUPS


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


def get_terrain_lookups(game_version):
    """
    Return the name lookup dicts for terrain groups.

    :param game_version: Game edition and expansions for which the lookups should be.
    :type game_version: tuple
    """
    game_edition = game_version[0]
    # game_expansions = game_version[1]

    if game_edition is GameEdition.ROR:
        pass

    elif game_edition is GameEdition.AOC:
        return aoc_internal.TERRAIN_GROUP_LOOKUPS


def get_terrain_type_lookups(game_version):
    """
    Return the name lookup dicts for terrain types.

    :param game_version: Game edition and expansions for which the lookups should be.
    :type game_version: tuple
    """
    game_edition = game_version[0]
    # game_expansions = game_version[1]

    if game_edition is GameEdition.ROR:
        pass

    elif game_edition is GameEdition.AOC:
        return aoc_internal.TERRAIN_TYPE_LOOKUPS
