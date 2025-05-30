# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create the aliases for the nyan import tree.
"""
from __future__ import annotations
import typing

if typing.TYPE_CHECKING:
    from ......nyan.import_tree import ImportTree


def set_static_aliases(modpack_name: str, import_tree: ImportTree) -> None:
    """
    Set the aliases for the nyan import tree. The alias names
    and affected nodes are hardcoded in this function.

    :param modpack: Modpack that is being converted.
    :param import_tree: The import tree to set the aliases for.
    """
    # Prefix aliases to prevent naming conflicts
    prefix = modpack_name + "_"

    # Aliases for objects from the openage API
    _add_ability_aliases(import_tree)
    _add_utility_aliases(import_tree)
    _add_effect_aliases(import_tree)
    _add_modifier_aliases(import_tree)

    # Aliases for objects from the modpack itself
    # TODO: Make this less hacky
    _add_modpack_utility_aliases(modpack_name, prefix, import_tree)
    _add_modpack_effect_aliases(modpack_name, import_tree)
    _add_modpack_modifier_aliases(modpack_name, prefix, import_tree)
    _add_modpack_terrain_aliases(modpack_name, prefix, import_tree)
    _add_modpack_game_entity_aliases(modpack_name, import_tree)
    _add_modpack_tech_aliases(modpack_name, import_tree)
    _add_modpack_civ_aliases(modpack_name, import_tree)


def _add_ability_aliases(import_tree: ImportTree) -> None:
    """
    Add aliases for abilities from the openage API.

    :param import_tree: The import tree to set the aliases for.
    """
    # Abilities from the openage API
    import_tree.add_alias(("engine", "ability", "type"), "ability")
    import_tree.add_alias(("engine", "ability", "property", "type"), "ability_prop")


def _add_utility_aliases(import_tree: ImportTree) -> None:
    """
    Add aliases for utility objects from the openage API.

    :param import_tree: The import tree to set the aliases for.
    """
    import_tree.add_alias(
        ("engine", "util", "activity", "condition", "type"), "activity_condition"
    )
    import_tree.add_alias(("engine", "util", "activity", "event", "type"), "activity_event")
    import_tree.add_alias(("engine", "util", "activity", "node", "type"), "activity_node")
    import_tree.add_alias(("engine", "util", "accuracy"), "accuracy")
    import_tree.add_alias(
        ("engine", "util", "animation_override"), "animation_override"
    )
    import_tree.add_alias(("engine", "util", "attribute"), "attribute")
    import_tree.add_alias(("engine", "util", "attribute_change_type", "type"),
                          "attribute_change_type")
    import_tree.add_alias(("engine", "util", "calculation_type", "type"), "calculation_type")
    import_tree.add_alias(("engine", "util", "setup"), "civ")
    import_tree.add_alias(("engine", "util", "convert_type"), "convert_type")
    import_tree.add_alias(("engine", "util", "cost", "type"), "cost_type")
    import_tree.add_alias(("engine", "util", "create"), "create")
    import_tree.add_alias(("engine", "util", "diplomatic_stance"), "diplo_stance")
    import_tree.add_alias(
        ("engine", "util", "diplomatic_stance", "type"),
        "diplo_stance_type"
    )
    import_tree.add_alias(("engine", "util", "distribution_type", "type"), "distribution_type")
    import_tree.add_alias(("engine", "util", "dropoff_type", "type"), "dropoff_type")
    import_tree.add_alias(("engine", "util", "exchange_mode", "type"), "exchange_mode")
    import_tree.add_alias(("engine", "util", "exchange_rate"), "exchange_rate")
    import_tree.add_alias(("engine", "util", "formation"), "formation")
    import_tree.add_alias(("engine", "util", "game_entity"), "game_entity")
    import_tree.add_alias(
        ("engine", "util", "game_entity_formation"), "ge_formation"
    )
    import_tree.add_alias(("engine", "util", "game_entity_stance", "type"), "ge_stance")
    import_tree.add_alias(("engine", "util", "game_entity_type", "type"), "ge_type")
    import_tree.add_alias(
        ("engine", "util", "game_entity_type"), "game_entity_type"
    )
    import_tree.add_alias(("engine", "util", "graphics"), "graphics")
    import_tree.add_alias(("engine", "util", "herdable_mode", "type"), "herdable_mode")
    import_tree.add_alias(("engine", "util", "hitbox"), "hitbox")
    import_tree.add_alias(("engine", "util", "move_mode", "type"), "move_mode")
    import_tree.add_alias(("engine", "util", "language"), "lang")
    import_tree.add_alias(("engine", "util", "language", "translated", "type"), "translated")
    import_tree.add_alias(("engine", "util", "logic", "gate", "type"), "logic_gate")
    import_tree.add_alias(("engine", "util", "logic", "literal", "type"), "literal")
    import_tree.add_alias(("engine", "util", "logic", "literal_scope", "type"), "literal_scope")
    import_tree.add_alias(("engine", "util", "patch"), "patch")
    import_tree.add_alias(("engine", "util", "patch", "property", "type"), "patch_prop")
    import_tree.add_alias(("engine", "util", "path_type"), "path_type")
    import_tree.add_alias(("engine", "util", "payment_mode", "type"), "payment_mode")
    import_tree.add_alias(("engine", "util", "placement_mode", "type"), "placement_mode")
    import_tree.add_alias(("engine", "util", "price_mode", "type"), "price_mode")
    import_tree.add_alias(("engine", "util", "price_pool"), "price_pool")
    import_tree.add_alias(("engine", "util", "production_mode", "type"), "production_mode")
    import_tree.add_alias(("engine", "util", "progress"), "progress")
    import_tree.add_alias(("engine", "util", "progress", "property", "type"), "progress_prop")
    import_tree.add_alias(("engine", "util", "progress_status"), "progress_status")
    import_tree.add_alias(("engine", "util", "progress_type", "type"), "progress_type")
    import_tree.add_alias(("engine", "util", "research"), "research")
    import_tree.add_alias(("engine", "util", "resource"), "resource")
    import_tree.add_alias(("engine", "util", "resource_spot"), "resource_spot")
    import_tree.add_alias(("engine", "util", "selection_box", "type"), "selection_box")
    import_tree.add_alias(("engine", "util", "sound",), "sound")
    import_tree.add_alias(("engine", "util", "state_machine"), "state_machine")
    import_tree.add_alias(("engine", "util", "storage"), "storage")
    import_tree.add_alias(("engine", "util", "target_mode", "type"), "target_mode")
    import_tree.add_alias(("engine", "util", "tech"), "tech")
    import_tree.add_alias(("engine", "util", "terrain"), "terrain")
    import_tree.add_alias(("engine", "util", "terrain_type"), "terrain_type")
    import_tree.add_alias(("engine", "util", "trade_route", "type"), "trade_route")
    import_tree.add_alias(("engine", "util", "variant", "type"), "variant")


def _add_effect_aliases(import_tree: ImportTree) -> None:
    """
    Add aliases for effect objects from the openage API.

    :param import_tree: The import tree to set the aliases for.
    """
    import_tree.add_alias(("engine", "effect", "property", "type"), "effect_prop")
    import_tree.add_alias(
        ("engine", "effect", "continuous", "flat_attribute_change", "type"),
        "econt_flac"
    )
    import_tree.add_alias(
        ("engine", "effect", "continuous", "time_relative_progress", "type"),
        "econt_trp"
    )
    import_tree.add_alias(
        ("engine", "effect", "continuous", "time_relative_attribute", "type"),
        "econt_tra"
    )
    import_tree.add_alias(
        ("engine", "effect", "discrete", "convert", "type"),
        "edisc_conv"
    )
    import_tree.add_alias(
        ("engine", "effect", "discrete", "flat_attribute_change", "type"),
        "edisc_flac"
    )
    import_tree.add_alias(("engine", "resistance", "property", "type"), "resist_prop")
    import_tree.add_alias(
        ("engine", "resistance", "continuous", "flat_attribute_change", "type"),
        "rcont_flac"
    )
    import_tree.add_alias(
        ("engine", "resistance", "continuous", "time_relative_progress", "type"),
        "rcont_trp"
    )
    import_tree.add_alias(
        ("engine", "resistance", "continuous", "time_relative_attribute", "type"),
        "rcont_tra"
    )
    import_tree.add_alias(
        ("engine", "resistance", "discrete", "convert", "type"),
        "rdisc_conv"
    )
    import_tree.add_alias(
        ("engine", "resistance", "discrete", "flat_attribute_change", "type"),
        "rdisc_flac"
    )


def _add_modifier_aliases(import_tree: ImportTree) -> None:
    """
    Add aliases for modifier objects from the openage API.

    :param import_tree: The import tree to set the aliases for.
    """
    import_tree.add_alias(
        ("engine", "modifier", "effect", "flat_attribute_change", "type"),
        "me_flac"
    )


def _add_modpack_utility_aliases(modpack_name: str, prefix: str, import_tree: ImportTree) -> None:
    """
    Add aliases for utility objects from the modpack.

    :param modpack_name: The name of the modpack.
    :param prefix: The prefix to use for the aliases to prevent conflicts with the
                   openage API namespace 'engine'.
    :param import_tree: The import tree to set the aliases for.
    """
    import_tree.add_alias(
        (modpack_name, "data", "util", "attribute", "types"),
        prefix + "attribute"
    )
    import_tree.add_alias(
        (modpack_name, "data", "util", "attribute_change_type", "types"),
        prefix + "attr_change_type"
    )
    import_tree.add_alias(
        (modpack_name, "data", "util", "construct_type", "types"),
        prefix + "construct_type"
    )
    import_tree.add_alias(
        (modpack_name, "data", "util", "convert_type", "types"),
        prefix + "convert_type"
    )
    import_tree.add_alias(
        (modpack_name, "data", "util", "diplomatic_stance", "types"),
        prefix + "diplo_stance"
    )
    import_tree.add_alias(
        (modpack_name, "data", "util", "game_entity_type", "types"),
        prefix + "ge_type"
    )
    import_tree.add_alias(
        (modpack_name, "data", "util", "formation", "types"),
        prefix + "formation"
    )
    import_tree.add_alias(
        (modpack_name, "data", "util", "formation", "subformations"),
        prefix + "subformations"
    )
    import_tree.add_alias(
        (modpack_name, "data", "util", "language", "language"),
        prefix + "lang"
    )
    import_tree.add_alias(
        (modpack_name, "data", "util", "logic", "death", "death"),
        "death_condition"
    )
    import_tree.add_alias(
        (modpack_name, "data", "util", "logic", "garrison_empty",
            "garrison_empty"),
        "empty_garrison_condition"
    )
    import_tree.add_alias(
        (modpack_name, "data", "util", "path_type", "types"),
        prefix + "path_type"
    )
    import_tree.add_alias(
        (modpack_name, "data", "util", "resource", "market_trading"),
        prefix + "market_trading"
    )
    import_tree.add_alias(
        (modpack_name, "data", "util", "resource", "types"),
        prefix + "resource"
    )
    import_tree.add_alias(
        (modpack_name, "data", "util", "terrain_type", "types"),
        prefix + "terrain_type"
    )


def _add_modpack_effect_aliases(modpack_name: str, import_tree: ImportTree) -> None:
    """
    Add aliases for effect objects from the modpack.

    :param modpack_name: The name of the modpack.
    :param import_tree: The import tree to set the aliases for.
    """
    import_tree.add_alias(
        (modpack_name, "data", "effect", "discrete", "flat_attribute_change",
            "fallback"),
        "attack_fallback"
    )
    import_tree.add_alias(
        (modpack_name, "data", "effect", "discrete", "flat_attribute_change",
            "min_damage"),
        "min_damage"
    )
    import_tree.add_alias(
        (modpack_name, "data", "effect", "discrete", "flat_attribute_change",
            "min_heal"),
        "min_heal"
    )


def _add_modpack_modifier_aliases(modpack_name: str, prefix: str, import_tree: ImportTree) -> None:
    """
    Add aliases for modifier objects from the modpack.

    :param modpack_name: The name of the modpack.
    :param prefix: The prefix to use for the aliases to prevent conflicts with the
                   openage API namespace 'engine'.
    :param import_tree: The import tree to set the aliases for.
    """
    import_tree.add_alias(
        (modpack_name, "data", "util", "modifier", "elevation_difference",
            "elevation_difference"),
        prefix + "mme_elev_high"
    )
    import_tree.add_alias(
        (modpack_name, "data", "util", "modifier", "elevation_difference",
            "elevation_difference"),
        prefix + "mme_elev_low"
    )
    import_tree.add_alias(
        (modpack_name, "data", "util", "modifier", "flyover_cliff",
            "flyover_cliff"),
        prefix + "mme_cliff_attack"
    )


def _add_modpack_terrain_aliases(modpack_name: str, prefix: str, import_tree: ImportTree) -> None:
    """
    Add aliases for terrain objects from the modpack.

    :param modpack_name: The name of the modpack.
    :param prefix: The prefix to use for the aliases to prevent conflicts with the
                   openage API namespace 'engine'.
    :param import_tree: The import tree to set the aliases for.
    """
    import_tree.add_alias(
        (modpack_name, "data", "terrain", "foundation", "foundation"),
        prefix + "foundation"
    )

    # Modpack terrain objects
    fqon = (modpack_name, "data", "terrain")
    current_node = import_tree.root
    for part in fqon:
        current_node = current_node.get_child(part)

    for child in current_node.children.values():
        current_node = child

        # These are folders and should have unique names
        alias_name = "terrain_" + current_node.name

        # One level deeper: This should be the nyan file
        current_node = current_node.children[current_node.name]

        # Set the folder name as alias for the file
        current_node.set_alias(alias_name)


def _add_modpack_game_entity_aliases(modpack_name: str, import_tree: ImportTree) -> None:
    """
    Add aliases for game entity objects from the modpack.

    :param modpack_name: The name of the modpack.
    :param import_tree: The import tree to set the aliases for.
    """
    fqon = (modpack_name, "data", "game_entity", "generic")
    current_node = import_tree.root
    for part in fqon:
        current_node = current_node.get_child(part)

    for child in current_node.children.values():
        current_node = child

        # These are folders and should have unique names
        alias_name = f"ge_{current_node.name}"

        for subchild in current_node.children.values():
            if subchild.name in ("graphics", "sounds", "projectiles"):
                continue

            if subchild.name.endswith("upgrade"):
                alias = f"{alias_name}_{subchild.name}"
                subchild.set_alias(alias)
                continue

            # One level deeper: This should be the nyan file
            current_node = subchild

            alias = f"ge_{current_node.name}"

            # Use the file name as alias for the file
            current_node.set_alias(alias)


def _add_modpack_tech_aliases(modpack_name: str, import_tree: ImportTree) -> None:
    """
    Add aliases for tech objects from the modpack.

    :param modpack_name: The name of the modpack.
    :param prefix: The prefix to use for the aliases to prevent conflicts with the
                   openage API namespace 'engine'.
    :param import_tree: The import tree to set the aliases for.
    """
    fqon = (modpack_name, "data", "tech", "generic")
    current_node = import_tree.root
    for part in fqon:
        current_node = current_node.get_child(part)

    for child in current_node.children.values():
        current_node = child

        # These are folders and should have unique names
        alias_name = "tech_" + current_node.name

        # One level deeper: This should be the nyan file
        current_node = current_node.children[current_node.name]

        # Set the folder name as alias for the file
        current_node.set_alias(alias_name)


def _add_modpack_civ_aliases(modpack_name: str, import_tree: ImportTree) -> None:
    """
    Add aliases for civ objects from the modpack.

    :param modpack_name: The name of the modpack.
    :param import_tree: The import tree to set the aliases for.
    """
    fqon = (modpack_name, "data", "civ")
    current_node = import_tree.root
    for part in fqon:
        current_node = current_node.get_child(part)

    for child in current_node.children.values():
        current_node = child

        # These are folders and should have unique names
        alias_name = "civ_" + current_node.name

        # One level deeper: This should be the nyan file
        current_node = current_node.children[current_node.name]

        # Set the folder name as alias for the file
        current_node.set_alias(alias_name)
