# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Convert projectiles to openage game entities.
"""
from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from ...aoc.ability_subprocessor import AoCAbilitySubprocessor
from ..ability_subprocessor import RoRAbilitySubprocessor


def projectiles_from_line(line):
    """
    Creates Projectile(GameEntity) raw API objects for a unit/building line.

    :param line: Line for which the projectiles are extracted.
    """
    current_unit = line.get_head_unit()
    current_unit_id = line.get_head_unit_id()
    dataset = line.data

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

    game_entity_name = name_lookup_dict[current_unit_id][0]
    game_entity_filename = name_lookup_dict[current_unit_id][1]

    projectiles_location = f"data/game_entity/generic/{game_entity_filename}/projectiles/"

    projectile_indices = []
    projectile_primary = current_unit["projectile_id0"].value
    if projectile_primary > -1:
        projectile_indices.append(0)

    for projectile_num in projectile_indices:
        obj_ref = f"{game_entity_name}.ShootProjectile.Projectile{projectile_num}"
        obj_name = f"Projectile{projectile_num}"
        proj_raw_api_object = RawAPIObject(obj_ref, obj_name, dataset.nyan_api_objects)
        proj_raw_api_object.add_raw_parent("engine.util.game_entity.GameEntity")
        proj_raw_api_object.set_location(projectiles_location)
        proj_raw_api_object.set_filename(f"{game_entity_filename}_projectiles")

        # =======================================================================
        # Types
        # =======================================================================
        types_set = [
            dataset.pregen_nyan_objects["util.game_entity_type.types.Projectile"].get_nyan_object()]
        proj_raw_api_object.add_raw_member(
            "types", types_set, "engine.util.game_entity.GameEntity")

        # =======================================================================
        # Abilities
        # =======================================================================
        abilities_set = []
        abilities_set.append(RoRAbilitySubprocessor.projectile_ability(
            line, position=projectile_num))
        abilities_set.append(AoCAbilitySubprocessor.move_projectile_ability(
            line, position=projectile_num))
        abilities_set.append(AoCAbilitySubprocessor.apply_discrete_effect_ability(
            line, 7, False, projectile_num))
        # TODO: Death, Despawn
        proj_raw_api_object.add_raw_member(
            "abilities", abilities_set, "engine.util.game_entity.GameEntity")

        # =======================================================================
        # Modifiers
        # =======================================================================
        modifiers_set = []

        proj_raw_api_object.add_raw_member(
            "modifiers", modifiers_set, "engine.util.game_entity.GameEntity")

        # =======================================================================
        # Variants
        # =======================================================================
        variants_set = []
        proj_raw_api_object.add_raw_member(
            "variants", variants_set, "engine.util.game_entity.GameEntity")

        line.add_raw_api_object(proj_raw_api_object)
