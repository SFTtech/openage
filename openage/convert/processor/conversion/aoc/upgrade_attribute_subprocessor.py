# Copyright 2020-2025 the openage authors. See copying.md for legal info.

"""
Creates upgrade patches for attribute modification effects in AoC.
"""
from .upgrade_attribute.accuracy import accuracy_upgrade
from .upgrade_attribute.armor import armor_upgrade
from .upgrade_attribute.attack import attack_upgrade
from .upgrade_attribute.ballistics import ballistics_upgrade
from .upgrade_attribute.attack_warning_sound import attack_warning_sound_upgrade
from .upgrade_attribute.blast_radius import blast_radius_upgrade
from .upgrade_attribute.carry_capacity import carry_capacity_upgrade
from .upgrade_attribute.cost_food import cost_food_upgrade
from .upgrade_attribute.cost_wood import cost_wood_upgrade
from .upgrade_attribute.cost_gold import cost_gold_upgrade
from .upgrade_attribute.cost_stone import cost_stone_upgrade
from .upgrade_attribute.creation_time import creation_time_upgrade
from .upgrade_attribute.garrison_capacity import garrison_capacity_upgrade
from .upgrade_attribute.garrison_heal import garrison_heal_upgrade
from .upgrade_attribute.graphics_angle import graphics_angle_upgrade
from .upgrade_attribute.gold_counter import gold_counter_upgrade
from .upgrade_attribute.health import health_upgrade
from .upgrade_attribute.ignore_armor import ignore_armor_upgrade
from .upgrade_attribute.imperial_tech_id import imperial_tech_id_upgrade
from .upgrade_attribute.kidnap_storage import kidnap_storage_upgrade
from .upgrade_attribute.line_of_sight import line_of_sight_upgrade
from .upgrade_attribute.max_projectiles import max_projectiles_upgrade
from .upgrade_attribute.min_projectiles import min_projectiles_upgrade
from .upgrade_attribute.max_range import max_range_upgrade
from .upgrade_attribute.min_range import min_range_upgrade
from .upgrade_attribute.move_speed import move_speed_upgrade
from .upgrade_attribute.projectile_unit import projectile_unit_upgrade
from .upgrade_attribute.reload_time import reload_time_upgrade
from .upgrade_attribute.resource_cost import resource_cost_upgrade
from .upgrade_attribute.resource_storage_1 import resource_storage_1_upgrade
from .upgrade_attribute.rotation_speed import rotation_speed_upgrade
from .upgrade_attribute.search_radius import search_radius_upgrade
from .upgrade_attribute.standing_wonders import standing_wonders_upgrade
from .upgrade_attribute.tc_available import tc_available_upgrade
from .upgrade_attribute.terrain_defense import terrain_defense_upgrade
from .upgrade_attribute.train_button import train_button_upgrade
from .upgrade_attribute.tribute_inefficiency import tribute_inefficiency_upgrade
from .upgrade_attribute.unit_size import unit_size_x_upgrade, unit_size_y_upgrade
from .upgrade_attribute.work_rate import work_rate_upgrade


class AoCUpgradeAttributeSubprocessor:
    """
    Creates raw API objects for attribute upgrade effects in AoC.
    """

    accuracy_upgrade = staticmethod(accuracy_upgrade)
    armor_upgrade = staticmethod(armor_upgrade)
    attack_upgrade = staticmethod(attack_upgrade)
    ballistics_upgrade = staticmethod(ballistics_upgrade)
    attack_warning_sound_upgrade = staticmethod(attack_warning_sound_upgrade)
    blast_radius_upgrade = staticmethod(blast_radius_upgrade)
    carry_capacity_upgrade = staticmethod(carry_capacity_upgrade)
    cost_food_upgrade = staticmethod(cost_food_upgrade)
    cost_wood_upgrade = staticmethod(cost_wood_upgrade)
    cost_gold_upgrade = staticmethod(cost_gold_upgrade)
    cost_stone_upgrade = staticmethod(cost_stone_upgrade)
    creation_time_upgrade = staticmethod(creation_time_upgrade)
    garrison_capacity_upgrade = staticmethod(garrison_capacity_upgrade)
    garrison_heal_upgrade = staticmethod(garrison_heal_upgrade)
    graphics_angle_upgrade = staticmethod(graphics_angle_upgrade)
    gold_counter_upgrade = staticmethod(gold_counter_upgrade)
    hp_upgrade = staticmethod(health_upgrade)
    ignore_armor_upgrade = staticmethod(ignore_armor_upgrade)
    imperial_tech_id_upgrade = staticmethod(imperial_tech_id_upgrade)
    kidnap_storage_upgrade = staticmethod(kidnap_storage_upgrade)
    los_upgrade = staticmethod(line_of_sight_upgrade)
    max_projectiles_upgrade = staticmethod(max_projectiles_upgrade)
    min_projectiles_upgrade = staticmethod(min_projectiles_upgrade)
    max_range_upgrade = staticmethod(max_range_upgrade)
    min_range_upgrade = staticmethod(min_range_upgrade)
    move_speed_upgrade = staticmethod(move_speed_upgrade)
    projectile_unit_upgrade = staticmethod(projectile_unit_upgrade)
    reload_time_upgrade = staticmethod(reload_time_upgrade)
    resource_cost_upgrade = staticmethod(resource_cost_upgrade)
    resource_storage_1_upgrade = staticmethod(resource_storage_1_upgrade)
    rotation_speed_upgrade = staticmethod(rotation_speed_upgrade)
    search_radius_upgrade = staticmethod(search_radius_upgrade)
    standing_wonders_upgrade = staticmethod(standing_wonders_upgrade)
    tc_available_upgrade = staticmethod(tc_available_upgrade)
    terrain_defense_upgrade = staticmethod(terrain_defense_upgrade)
    train_button_upgrade = staticmethod(train_button_upgrade)
    tribute_inefficiency_upgrade = staticmethod(tribute_inefficiency_upgrade)
    unit_size_x_upgrade = staticmethod(unit_size_x_upgrade)
    unit_size_y_upgrade = staticmethod(unit_size_y_upgrade)
    work_rate_upgrade = staticmethod(work_rate_upgrade)
