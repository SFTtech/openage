# Copyright 2020-2025 the openage authors. See copying.md for legal info.

"""
Derives and adds abilities to lines. Subroutine of the
nyan subprocessor.

For SWGB we use the functions of the AoCAbilitySubprocessor, but additionally
create a diff for every civ line.
"""
from .ability.active_transform_to import active_transform_to_ability
from .ability.apply_continuous_effect import apply_continuous_effect_ability
from .ability.apply_discrete_effect import apply_discrete_effect_ability
from .ability.attribute_change_tracker import attribute_change_tracker_ability
from .ability.collision import collision_ability
from .ability.constructable import constructable_ability
from .ability.death import death_ability
from .ability.exchange_resources import exchange_resources_ability
from .ability.gather import gather_ability
from .ability.harvestable import harvestable_ability
from .ability.idle import idle_ability
from .ability.line_of_sight import line_of_sight_ability
from .ability.live import live_ability
from .ability.move import move_ability
from .ability.named import named_ability
from .ability.provide_contingent import provide_contingent_ability
from .ability.regenerate_attribute import regenerate_attribute_ability
from .ability.resource_storage import resource_storage_ability
from .ability.restock import restock_ability
from .ability.selectable import selectable_ability
from .ability.send_back_to_task import send_back_to_task_ability
from .ability.shoot_projectile import shoot_projectile_ability
from .ability.trade import trade_ability
from .ability.trade_post import trade_post_ability
from .ability.turn import turn_ability


class SWGBCCAbilitySubprocessor:
    """
    Creates raw API objects for abilities in SWGB.
    """

    active_transform_to_ability = staticmethod(active_transform_to_ability)
    apply_continuous_effect_ability = staticmethod(apply_continuous_effect_ability)
    apply_discrete_effect_ability = staticmethod(apply_discrete_effect_ability)
    attribute_change_tracker_ability = staticmethod(attribute_change_tracker_ability)
    collision_ability = staticmethod(collision_ability)
    constructable_ability = staticmethod(constructable_ability)
    death_ability = staticmethod(death_ability)
    exchange_resources_ability = staticmethod(exchange_resources_ability)
    gather_ability = staticmethod(gather_ability)
    harvestable_ability = staticmethod(harvestable_ability)
    idle_ability = staticmethod(idle_ability)
    live_ability = staticmethod(live_ability)
    los_ability = staticmethod(line_of_sight_ability)
    move_ability = staticmethod(move_ability)
    named_ability = staticmethod(named_ability)
    provide_contingent_ability = staticmethod(provide_contingent_ability)
    regenerate_attribute_ability = staticmethod(regenerate_attribute_ability)
    resource_storage_ability = staticmethod(resource_storage_ability)
    restock_ability = staticmethod(restock_ability)
    selectable_ability = staticmethod(selectable_ability)
    send_back_to_task_ability = staticmethod(send_back_to_task_ability)
    shoot_projectile_ability = staticmethod(shoot_projectile_ability)
    trade_ability = staticmethod(trade_ability)
    trade_post_ability = staticmethod(trade_post_ability)
    turn_ability = staticmethod(turn_ability)
