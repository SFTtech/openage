# Copyright 2020-2025 the openage authors. See copying.md for legal info.

"""
Creates upgrade patches for resource modification effects in DE2.
"""
from .upgrade_resource.bengali_conversion_resistance import bengali_conversion_resistance_upgrade
from .upgrade_resource.burgundian_vineyards import burgundian_vineyards_upgrade
from .upgrade_resource.chieftains import chieftains_upgrade
from .upgrade_resource.cliff import cliff_attack_upgrade, cliff_defense_upgrade
from .upgrade_resource.convert_adjust import convert_min_adjust_upgrade, \
    convert_max_adjust_upgrade
from .upgrade_resource.convert_building import convert_min_building_upgrade, \
    convert_max_building_upgrade, convert_building_chance_upgrade
from .upgrade_resource.convert_range import convert_range_upgrade
from .upgrade_resource.cuman_tc import cuman_tc_upgrade
from .upgrade_resource.current_resource_amount import current_food_amount_upgrade, \
    current_wood_amount_upgrade, current_stone_amount_upgrade, \
    current_gold_amount_upgrade
from .upgrade_resource.doi_paper_money import doi_paper_money_upgrade
from .upgrade_resource.elevation import elevation_attack_upgrade, \
    elevation_defense_upgrade
from .upgrade_resource.feitoria_gold import feitoria_gold_upgrade
from .upgrade_resource.first_crusade import first_crusade_upgrade
from .upgrade_resource.fish_trap_amount import fish_trap_food_amount_upgrade
from .upgrade_resource.folwark import folwark_collect_upgrade, \
    folwark_flag_upgrade, folwark_mill_id_upgrade
from .upgrade_resource.forager_wood_gather import forager_wood_gather_upgrade
from .upgrade_resource.free_kipchaks import free_kipchaks_upgrade
from .upgrade_resource.herdable_garrison import herdable_garrison_upgrade
from .upgrade_resource.relic_food_production import relic_food_production_upgrade
from .upgrade_resource.resource_decay import resource_decay_upgrade
from .upgrade_resource.reveal_enemy_tcs import reveal_enemy_tcs_upgrade
from .upgrade_resource.sheep_food_amount import sheep_food_amount_upgrade
from .upgrade_resource.stone_gold_gen import stone_gold_gen_upgrade
from .upgrade_resource.tech_reward import tech_reward_upgrade
from .upgrade_resource.trade_food_bonus import trade_food_bonus_upgrade
from .upgrade_resource.workshop_gen import workshop_food_gen_upgrade, \
    workshop_wood_gen_upgrade, workshop_stone_gen_upgrade, \
    workshop_gold_gen_upgrade


from .upgrade_resource.unknown import bfg_unknown_69_upgrade, \
    bfg_unknown_219_upgrade, bfg_unknown_502_upgrade, \
    bfg_unknown_507_upgrade, bfg_unknown_521_upgrade, \
    bfg_unknown_551_upgrade, unknown_recharge_rate_upgrade


class DE2UpgradeResourceSubprocessor:
    """
    Creates raw API objects for resource upgrade effects in DE2.
    """

    bengali_conversion_resistance_upgrade = staticmethod(
        bengali_conversion_resistance_upgrade)
    bfg_unknown_69_upgrade = staticmethod(bfg_unknown_69_upgrade)
    bfg_unknown_219_upgrade = staticmethod(bfg_unknown_219_upgrade)
    bfg_unknown_502_upgrade = staticmethod(bfg_unknown_502_upgrade)
    bfg_unknown_507_upgrade = staticmethod(bfg_unknown_507_upgrade)
    bfg_unknown_521_upgrade = staticmethod(bfg_unknown_521_upgrade)
    bfg_unknown_551_upgrade = staticmethod(bfg_unknown_551_upgrade)
    unknown_recharge_rate_upgrade = staticmethod(unknown_recharge_rate_upgrade)
    burgundian_vineyards_upgrade = staticmethod(burgundian_vineyards_upgrade)
    chieftains_upgrade = staticmethod(chieftains_upgrade)
    cliff_attack_upgrade = staticmethod(cliff_attack_upgrade)
    cliff_defense_upgrade = staticmethod(cliff_defense_upgrade)
    conversion_min_adjustment_upgrade = staticmethod(convert_min_adjust_upgrade)
    conversion_max_adjustment_upgrade = staticmethod(convert_max_adjust_upgrade)
    conversion_min_building_upgrade = staticmethod(convert_min_building_upgrade)
    conversion_max_building_upgrade = staticmethod(convert_max_building_upgrade)
    conversion_building_chance_upgrade = staticmethod(convert_building_chance_upgrade)
    conversion_range_upgrade = staticmethod(convert_range_upgrade)
    cuman_tc_upgrade = staticmethod(cuman_tc_upgrade)
    current_food_amount_upgrade = staticmethod(current_food_amount_upgrade)
    current_wood_amount_upgrade = staticmethod(current_wood_amount_upgrade)
    current_stone_amount_upgrade = staticmethod(current_stone_amount_upgrade)
    current_gold_amount_upgrade = staticmethod(current_gold_amount_upgrade)
    doi_paper_money_upgrade = staticmethod(doi_paper_money_upgrade)
    elevation_attack_upgrade = staticmethod(elevation_attack_upgrade)
    elevation_defense_upgrade = staticmethod(elevation_defense_upgrade)
    feitoria_gold_upgrade = staticmethod(feitoria_gold_upgrade)
    first_crusade_upgrade = staticmethod(first_crusade_upgrade)
    fish_trap_food_amount_upgrade = staticmethod(fish_trap_food_amount_upgrade)
    folwark_collect_upgrade = staticmethod(folwark_collect_upgrade)
    folwark_flag_upgrade = staticmethod(folwark_flag_upgrade)
    folwark_mill_id_upgrade = staticmethod(folwark_mill_id_upgrade)
    forager_wood_gather_upgrade = staticmethod(forager_wood_gather_upgrade)
    free_kipchaks_upgrade = staticmethod(free_kipchaks_upgrade)
    herdable_garrison_upgrade = staticmethod(herdable_garrison_upgrade)
    relic_food_production_upgrade = staticmethod(relic_food_production_upgrade)
    resource_decay_upgrade = staticmethod(resource_decay_upgrade)
    reveal_enemy_tcs_upgrade = staticmethod(reveal_enemy_tcs_upgrade)
    sheep_food_amount_upgrade = staticmethod(sheep_food_amount_upgrade)
    stone_gold_gen_upgrade = staticmethod(stone_gold_gen_upgrade)
    tech_reward_upgrade = staticmethod(tech_reward_upgrade)
    trade_food_bonus_upgrade = staticmethod(trade_food_bonus_upgrade)
    workshop_food_gen_upgrade = staticmethod(workshop_food_gen_upgrade)
    workshop_wood_gen_upgrade = staticmethod(workshop_wood_gen_upgrade)
    workshop_stone_gen_upgrade = staticmethod(workshop_stone_gen_upgrade)
    workshop_gold_gen_upgrade = staticmethod(workshop_gold_gen_upgrade)
