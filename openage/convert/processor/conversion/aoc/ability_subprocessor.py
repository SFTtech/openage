# Copyright 2020-2025 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-few-public-methods, invalid-name

"""
Derives and adds abilities to lines. Subroutine of the
nyan subprocessor.
"""

from .ability.active_transform_to import active_transform_to_ability
from .ability.activity import activity_ability
from .ability.apply_continuous_effect import apply_continuous_effect_ability
from .ability.apply_discrete_effect import apply_discrete_effect_ability
from .ability.attribute_change_tracker import attribute_change_tracker_ability
from .ability.collect_storage import collect_storage_ability
from .ability.collision import collision_ability
from .ability.constructable import constructable_ability
from .ability.create import create_ability
from .ability.death import death_ability
from .ability.delete import delete_ability
from .ability.despawn import despawn_ability
from .ability.drop_resources import drop_resources_ability
from .ability.drop_site import drop_site_ability
from .ability.enter_container import enter_container_ability
from .ability.exchange_resources import exchange_resources_ability
from .ability.exit_container import exit_container_ability
from .ability.game_entity_stance import game_entity_stance_ability
from .ability.formation import formation_ability
from .ability.foundation import foundation_ability
from .ability.gather import gather_ability
from .ability.harvestable import harvestable_ability
from .ability.herd import herd_ability
from .ability.herdable import herdable_ability
from .ability.idle import idle_ability
from .ability.line_of_sight import line_of_sight_ability
from .ability.live import live_ability
from .ability.move import move_ability, move_projectile_ability
from .ability.named import named_ability
from .ability.overlay_terrain import overlay_terrain_ability
from .ability.pathable import pathable_ability
from .ability.production_queue import production_queue_ability
from .ability.projectile import projectile_ability
from .ability.provide_contingent import provide_contingent_ability
from .ability.rally_point import rally_point_ability
from .ability.regenerate_attribute import regenerate_attribute_ability
from .ability.regenerate_resource_spot import regenerate_resource_spot_ability
from .ability.remove_storage import remove_storage_ability
from .ability.research import research_ability
from .ability.resistance import resistance_ability
from .ability.resource_storage import resource_storage_ability
from .ability.restock import restock_ability
from .ability.selectable import selectable_ability
from .ability.send_back_to_task import send_back_to_task_ability
from .ability.shoot_projectile import shoot_projectile_ability
from .ability.stop import stop_ability
from .ability.storage import storage_ability
from .ability.terrain_requirement import terrain_requirement_ability
from .ability.trade import trade_ability
from .ability.trade_post import trade_post_ability
from .ability.transfer_storage import transfer_storage_ability
from .ability.turn import turn_ability
from .ability.use_contingent import use_contingent_ability
from .ability.visibility import visibility_ability

from .ability.util import create_animation, create_civ_animation, create_language_strings, \
    create_sound


class AoCAbilitySubprocessor:
    """
    Creates raw API objects for abilities in AoC.
    """

    active_transform_to_ability = staticmethod(active_transform_to_ability)
    activity_ability = staticmethod(activity_ability)
    apply_continuous_effect_ability = staticmethod(apply_continuous_effect_ability)
    apply_discrete_effect_ability = staticmethod(apply_discrete_effect_ability)
    attribute_change_tracker_ability = staticmethod(attribute_change_tracker_ability)
    collect_storage_ability = staticmethod(collect_storage_ability)
    collision_ability = staticmethod(collision_ability)
    constructable_ability = staticmethod(constructable_ability)
    create_ability = staticmethod(create_ability)
    death_ability = staticmethod(death_ability)
    delete_ability = staticmethod(delete_ability)
    despawn_ability = staticmethod(despawn_ability)
    drop_resources_ability = staticmethod(drop_resources_ability)
    drop_site_ability = staticmethod(drop_site_ability)
    enter_container_ability = staticmethod(enter_container_ability)
    exchange_resources_ability = staticmethod(exchange_resources_ability)
    exit_container_ability = staticmethod(exit_container_ability)
    game_entity_stance_ability = staticmethod(game_entity_stance_ability)
    formation_ability = staticmethod(formation_ability)
    foundation_ability = staticmethod(foundation_ability)
    gather_ability = staticmethod(gather_ability)
    harvestable_ability = staticmethod(harvestable_ability)
    herd_ability = staticmethod(herd_ability)
    herdable_ability = staticmethod(herdable_ability)
    idle_ability = staticmethod(idle_ability)
    los_ability = staticmethod(line_of_sight_ability)
    live_ability = staticmethod(live_ability)
    move_ability = staticmethod(move_ability)
    move_projectile_ability = staticmethod(move_projectile_ability)
    named_ability = staticmethod(named_ability)
    overlay_terrain_ability = staticmethod(overlay_terrain_ability)
    pathable_ability = staticmethod(pathable_ability)
    production_queue_ability = staticmethod(production_queue_ability)
    projectile_ability = staticmethod(projectile_ability)
    provide_contingent_ability = staticmethod(provide_contingent_ability)
    rally_point_ability = staticmethod(rally_point_ability)
    regenerate_attribute_ability = staticmethod(regenerate_attribute_ability)
    regenerate_resource_spot_ability = staticmethod(regenerate_resource_spot_ability)
    remove_storage_ability = staticmethod(remove_storage_ability)
    research_ability = staticmethod(research_ability)
    resource_storage_ability = staticmethod(resource_storage_ability)
    resistance_ability = staticmethod(resistance_ability)
    restock_ability = staticmethod(restock_ability)
    selectable_ability = staticmethod(selectable_ability)
    send_back_to_task_ability = staticmethod(send_back_to_task_ability)
    shoot_projectile_ability = staticmethod(shoot_projectile_ability)
    stop_ability = staticmethod(stop_ability)
    storage_ability = staticmethod(storage_ability)
    terrain_requirement_ability = staticmethod(terrain_requirement_ability)
    trade_ability = staticmethod(trade_ability)
    trade_post_ability = staticmethod(trade_post_ability)
    transfer_storage_ability = staticmethod(transfer_storage_ability)
    turn_ability = staticmethod(turn_ability)
    use_contingent_ability = staticmethod(use_contingent_ability)
    visibility_ability = staticmethod(visibility_ability)

    create_animation = staticmethod(create_animation)
    create_civ_animation = staticmethod(create_civ_animation)
    create_sound = staticmethod(create_sound)
    create_language_strings = staticmethod(create_language_strings)
