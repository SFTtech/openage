# Copyright 2020-2025 the openage authors. See copying.md for legal info.

"""
Creates upgrade patches for resource modification effects in AoC.
"""
from .upgrade_resource.berserk_heal_rate import berserk_heal_rate_upgrade
from .upgrade_resource.bonus_population import bonus_population_upgrade
from .upgrade_resource.building_conversion import building_conversion_upgrade
from .upgrade_resource.chinese_tech_discount import chinese_tech_discount_upgrade
from .upgrade_resource.construction_speed import construction_speed_upgrade
from .upgrade_resource.conversion_resistance import conversion_resistance_upgrade, \
    conversion_resistance_min_rounds_upgrade, conversion_resistance_max_rounds_upgrade
from .upgrade_resource.crenellations import crenellations_upgrade
from .upgrade_resource.faith_recharge_rate import faith_recharge_rate_upgrade
from .upgrade_resource.farm_food import farm_food_upgrade
from .upgrade_resource.gather_efficiency import gather_food_efficiency_upgrade, \
    gather_wood_efficiency_upgrade, gather_gold_efficiency_upgrade, gather_stone_efficiency_upgrade
from .upgrade_resource.heal_range import heal_range_upgrade
from .upgrade_resource.heal_rate import heal_rate_upgrade
from .upgrade_resource.herding_dominance import herding_dominance_upgrade
from .upgrade_resource.heresy import heresy_upgrade
from .upgrade_resource.monk_conversion import monk_conversion_upgrade
from .upgrade_resource.relic_gold_bonus import relic_gold_bonus_upgrade
from .upgrade_resource.research_time import research_time_upgrade
from .upgrade_resource.reveal_ally import reveal_ally_upgrade
from .upgrade_resource.reveal_enemy import reveal_enemy_upgrade
from .upgrade_resource.siege_conversion import siege_conversion_upgrade
from .upgrade_resource.ship_conversion import ship_conversion_upgrade
from .upgrade_resource.spies_discount import spies_discount_upgrade
from .upgrade_resource.starting_resources import starting_food_upgrade, \
    starting_wood_upgrade, starting_stone_upgrade, starting_gold_upgrade, \
    starting_villagers_upgrade, starting_population_space_upgrade
from .upgrade_resource.theocracy import theocracy_upgrade
from .upgrade_resource.trade_penalty import trade_penalty_upgrade
from .upgrade_resource.tribute_inefficiency import tribute_inefficiency_upgrade
from .upgrade_resource.wonder_time import wonder_time_increase_upgrade


class AoCUpgradeResourceSubprocessor:
    """
    Creates raw API objects for resource upgrade effects in AoC.
    """

    berserk_heal_rate_upgrade = staticmethod(berserk_heal_rate_upgrade)
    bonus_population_upgrade = staticmethod(bonus_population_upgrade)
    building_conversion_upgrade = staticmethod(building_conversion_upgrade)
    chinese_tech_discount_upgrade = staticmethod(chinese_tech_discount_upgrade)
    construction_speed_upgrade = staticmethod(construction_speed_upgrade)
    conversion_resistance_upgrade = staticmethod(conversion_resistance_upgrade)
    conversion_resistance_min_rounds_upgrade = staticmethod(conversion_resistance_min_rounds_upgrade)
    conversion_resistance_max_rounds_upgrade = staticmethod(conversion_resistance_max_rounds_upgrade)
    crenellations_upgrade = staticmethod(crenellations_upgrade)
    faith_recharge_rate_upgrade = staticmethod(faith_recharge_rate_upgrade)
    farm_food_upgrade = staticmethod(farm_food_upgrade)
    gather_food_efficiency_upgrade = staticmethod(gather_food_efficiency_upgrade)
    gather_wood_efficiency_upgrade = staticmethod(gather_wood_efficiency_upgrade)
    gather_gold_efficiency_upgrade = staticmethod(gather_gold_efficiency_upgrade)
    gather_stone_efficiency_upgrade = staticmethod(gather_stone_efficiency_upgrade)
    heal_range_upgrade = staticmethod(heal_range_upgrade)
    heal_rate_upgrade = staticmethod(heal_rate_upgrade)
    herding_dominance_upgrade = staticmethod(herding_dominance_upgrade)
    heresy_upgrade = staticmethod(heresy_upgrade)
    monk_conversion_upgrade = staticmethod(monk_conversion_upgrade)
    relic_gold_bonus_upgrade = staticmethod(relic_gold_bonus_upgrade)
    research_time_upgrade = staticmethod(research_time_upgrade)
    reveal_ally_upgrade = staticmethod(reveal_ally_upgrade)
    reveal_enemy_upgrade = staticmethod(reveal_enemy_upgrade)
    siege_conversion_upgrade = staticmethod(siege_conversion_upgrade)
    ship_conversion_upgrade = staticmethod(ship_conversion_upgrade)
    spies_discount_upgrade = staticmethod(spies_discount_upgrade)
    starting_food_upgrade = staticmethod(starting_food_upgrade)
    starting_wood_upgrade = staticmethod(starting_wood_upgrade)
    starting_stone_upgrade = staticmethod(starting_stone_upgrade)
    starting_gold_upgrade = staticmethod(starting_gold_upgrade)
    starting_villagers_upgrade = staticmethod(starting_villagers_upgrade)
    starting_population_space_upgrade = staticmethod(starting_population_space_upgrade)
    theocracy_upgrade = staticmethod(theocracy_upgrade)
    trade_penalty_upgrade = staticmethod(trade_penalty_upgrade)
    tribute_inefficiency_upgrade = staticmethod(tribute_inefficiency_upgrade)
    wonder_time_increase_upgrade = staticmethod(wonder_time_increase_upgrade)
