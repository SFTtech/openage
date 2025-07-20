# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Mappings of RoR upgrade IDs to their respective subprocessor functions.
"""

from ...aoc.upgrade_attribute_subprocessor import AoCUpgradeAttributeSubprocessor
from ...aoc.upgrade_resource_subprocessor import AoCUpgradeResourceSubprocessor
from ..upgrade_attribute_subprocessor import RoRUpgradeAttributeSubprocessor
from ..upgrade_resource_subprocessor import RoRUpgradeResourceSubprocessor

UPGRADE_ATTRIBUTE_FUNCS = {
    0: AoCUpgradeAttributeSubprocessor.hp_upgrade,
    1: AoCUpgradeAttributeSubprocessor.los_upgrade,
    2: AoCUpgradeAttributeSubprocessor.garrison_capacity_upgrade,
    3: AoCUpgradeAttributeSubprocessor.unit_size_x_upgrade,
    4: AoCUpgradeAttributeSubprocessor.unit_size_y_upgrade,
    5: AoCUpgradeAttributeSubprocessor.move_speed_upgrade,
    6: AoCUpgradeAttributeSubprocessor.rotation_speed_upgrade,
    8: AoCUpgradeAttributeSubprocessor.armor_upgrade,
    9: AoCUpgradeAttributeSubprocessor.attack_upgrade,
    10: AoCUpgradeAttributeSubprocessor.reload_time_upgrade,
    11: AoCUpgradeAttributeSubprocessor.accuracy_upgrade,
    12: AoCUpgradeAttributeSubprocessor.max_range_upgrade,
    13: AoCUpgradeAttributeSubprocessor.work_rate_upgrade,
    14: AoCUpgradeAttributeSubprocessor.carry_capacity_upgrade,
    16: AoCUpgradeAttributeSubprocessor.projectile_unit_upgrade,
    17: AoCUpgradeAttributeSubprocessor.graphics_angle_upgrade,
    18: AoCUpgradeAttributeSubprocessor.terrain_defense_upgrade,
    19: RoRUpgradeAttributeSubprocessor.ballistics_upgrade,
    100: AoCUpgradeAttributeSubprocessor.resource_cost_upgrade,
    101: RoRUpgradeAttributeSubprocessor.population_upgrade,
}

UPGRADE_RESOURCE_FUNCS = {
    4: AoCUpgradeResourceSubprocessor.starting_population_space_upgrade,
    27: AoCUpgradeResourceSubprocessor.monk_conversion_upgrade,
    28: RoRUpgradeResourceSubprocessor.building_conversion_upgrade,
    32: AoCUpgradeResourceSubprocessor.bonus_population_upgrade,
    35: AoCUpgradeResourceSubprocessor.faith_recharge_rate_upgrade,
    36: AoCUpgradeResourceSubprocessor.farm_food_upgrade,
    46: AoCUpgradeResourceSubprocessor.tribute_inefficiency_upgrade,
    47: AoCUpgradeResourceSubprocessor.gather_gold_efficiency_upgrade,
    50: AoCUpgradeResourceSubprocessor.reveal_ally_upgrade,
    56: RoRUpgradeResourceSubprocessor.heal_bonus_upgrade,
    57: RoRUpgradeResourceSubprocessor.martyrdom_upgrade,
}
