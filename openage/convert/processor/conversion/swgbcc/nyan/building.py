# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Convert building lines to openage game entities.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.aoc.genie_unit import GenieGarrisonMode
from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from ...aoc.ability_subprocessor import AoCAbilitySubprocessor
from ..ability_subprocessor import SWGBCCAbilitySubprocessor
from ..auxiliary_subprocessor import SWGBCCAuxiliarySubprocessor
from .projectile import projectiles_from_line

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_unit import GenieBuildingLineGroup


def building_line_to_game_entity(building_line: GenieBuildingLineGroup) -> None:
    """
    Creates raw API objects for a building line.

    :param building_line: Building line that gets converted to a game entity.
    """
    current_building = building_line.line[0]
    current_building_id = building_line.get_head_unit_id()
    dataset = building_line.data

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
    class_lookup_dict = internal_name_lookups.get_class_lookups(dataset.game_version)

    # Start with the generic GameEntity
    game_entity_name = name_lookup_dict[current_building_id][0]
    obj_location = f"data/game_entity/generic/{name_lookup_dict[current_building_id][1]}/"
    raw_api_object = RawAPIObject(game_entity_name, game_entity_name,
                                  dataset.nyan_api_objects)
    raw_api_object.add_raw_parent("engine.util.game_entity.GameEntity")
    raw_api_object.set_location(obj_location)
    raw_api_object.set_filename(name_lookup_dict[current_building_id][1])
    building_line.add_raw_api_object(raw_api_object)

    # =======================================================================
    # Game Entity Types
    # =======================================================================
    # we give a building two types
    #    - util.game_entity_type.types.Building (if unit_type >= 80)
    #    - util.game_entity_type.types.<Class> (depending on the class)
    # and additionally
    #    - util.game_entity_type.types.DropSite (only if this is used as a drop site)
    # =======================================================================
    # Create or use existing auxiliary types
    types_set = []
    unit_type = current_building["unit_type"].value

    if unit_type >= 80:
        type_obj = dataset.pregen_nyan_objects[
            "util.game_entity_type.types.Building"
        ].get_nyan_object()
        types_set.append(type_obj)

    unit_class = current_building["unit_class"].value
    class_name = class_lookup_dict[unit_class]
    class_obj_name = f"util.game_entity_type.types.{class_name}"
    type_obj = dataset.pregen_nyan_objects[class_obj_name].get_nyan_object()
    types_set.append(type_obj)

    if building_line.is_dropsite():
        type_obj = dataset.pregen_nyan_objects[
            "util.game_entity_type.types.DropSite"
        ].get_nyan_object()
        types_set.append(type_obj)

    raw_api_object.add_raw_member("types", types_set, "engine.util.game_entity.GameEntity")

    # =======================================================================
    # Abilities
    # =======================================================================
    abilities_set = []

    abilities_set.append(
        SWGBCCAbilitySubprocessor.attribute_change_tracker_ability(building_line))
    abilities_set.append(SWGBCCAbilitySubprocessor.death_ability(building_line))
    abilities_set.append(AoCAbilitySubprocessor.delete_ability(building_line))
    abilities_set.append(AoCAbilitySubprocessor.despawn_ability(building_line))
    abilities_set.append(AoCAbilitySubprocessor.idle_ability(building_line))
    abilities_set.append(AoCAbilitySubprocessor.collision_ability(building_line))
    abilities_set.append(AoCAbilitySubprocessor.live_ability(building_line))
    abilities_set.append(AoCAbilitySubprocessor.los_ability(building_line))
    abilities_set.append(AoCAbilitySubprocessor.named_ability(building_line))
    abilities_set.append(AoCAbilitySubprocessor.resistance_ability(building_line))
    abilities_set.extend(AoCAbilitySubprocessor.selectable_ability(building_line))
    abilities_set.append(AoCAbilitySubprocessor.stop_ability(building_line))
    abilities_set.append(AoCAbilitySubprocessor.terrain_requirement_ability(building_line))
    abilities_set.append(AoCAbilitySubprocessor.visibility_ability(building_line))

    # Config abilities
    # if building_line.is_creatable():
    #     abilities_set.append(SWGBCCAbilitySubprocessor.constructable_ability(building_line))

    if not building_line.is_passable():
        abilities_set.append(AoCAbilitySubprocessor.pathable_ability(building_line))

    if building_line.has_foundation():
        if building_line.get_class_id() == 7:
            # Use OverlayTerrain for the farm terrain
            abilities_set.append(AoCAbilitySubprocessor.overlay_terrain_ability(building_line))
            abilities_set.append(AoCAbilitySubprocessor.foundation_ability(building_line,
                                                                           terrain_id=7))

        else:
            abilities_set.append(AoCAbilitySubprocessor.foundation_ability(building_line))

    # Creation/Research abilities
    if len(building_line.creates) > 0:
        abilities_set.append(AoCAbilitySubprocessor.create_ability(building_line))
        abilities_set.append(AoCAbilitySubprocessor.production_queue_ability(building_line))

    if len(building_line.researches) > 0:
        abilities_set.append(AoCAbilitySubprocessor.research_ability(building_line))

    # Effect abilities
    if building_line.is_projectile_shooter():
        abilities_set.append(AoCAbilitySubprocessor.shoot_projectile_ability(building_line, 7))
        abilities_set.append(AoCAbilitySubprocessor.game_entity_stance_ability(building_line))
        projectiles_from_line(building_line)

    # Storage abilities
    if building_line.is_garrison():
        abilities_set.append(AoCAbilitySubprocessor.storage_ability(building_line))
        abilities_set.append(AoCAbilitySubprocessor.remove_storage_ability(building_line))

        garrison_mode = building_line.get_garrison_mode()

        if garrison_mode == GenieGarrisonMode.NATURAL:
            abilities_set.append(
                SWGBCCAbilitySubprocessor.send_back_to_task_ability(building_line))

        if garrison_mode in (GenieGarrisonMode.NATURAL, GenieGarrisonMode.SELF_PRODUCED):
            abilities_set.append(AoCAbilitySubprocessor.rally_point_ability(building_line))

    # Resource abilities
    if building_line.is_harvestable():
        abilities_set.append(AoCAbilitySubprocessor.harvestable_ability(building_line))

    if building_line.is_dropsite():
        abilities_set.append(AoCAbilitySubprocessor.drop_site_ability(building_line))

    ability = AoCAbilitySubprocessor.provide_contingent_ability(building_line)
    if ability:
        abilities_set.append(ability)

    # Trade abilities
    if building_line.is_trade_post():
        abilities_set.append(SWGBCCAbilitySubprocessor.trade_post_ability(building_line))

    if building_line.get_id() == 84:
        # Spaceport trading
        abilities_set.extend(
            SWGBCCAbilitySubprocessor.exchange_resources_ability(building_line))

    raw_api_object.add_raw_member("abilities", abilities_set,
                                  "engine.util.game_entity.GameEntity")

    # =======================================================================
    # Modifiers
    # =======================================================================
    raw_api_object.add_raw_member("modifiers", [], "engine.util.game_entity.GameEntity")

    # =======================================================================
    # TODO: Variants
    # =======================================================================
    raw_api_object.add_raw_member("variants", [], "engine.util.game_entity.GameEntity")

    # =======================================================================
    # Misc (Objects that are not used by the unit line itself, but use its values)
    # =======================================================================
    if building_line.is_creatable():
        SWGBCCAuxiliarySubprocessor.get_creatable_game_entity(building_line)
