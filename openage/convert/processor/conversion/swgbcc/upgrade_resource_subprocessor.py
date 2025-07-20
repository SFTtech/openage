# Copyright 2020-2025 the openage authors. See copying.md for legal info.

"""
Creates upgrade patches for resource modification effects in SWGB.
"""
from .upgrade_resource.assault_mech_anti_air import assault_mech_anti_air_upgrade
from .upgrade_resource.berserk_heal_rate import berserk_heal_rate_upgrade
from .upgrade_resource.cloak import cloak_upgrade, detect_cloak_upgrade
from .upgrade_resource.concentration import concentration_upgrade
from .upgrade_resource.convert_building import convert_building_upgrade
from .upgrade_resource.convert_monk import convert_monk_upgrade
from .upgrade_resource.faith_recharge_rate import faith_recharge_rate_upgrade
from .upgrade_resource.heal_range import heal_range_upgrade
from .upgrade_resource.shield import shield_air_units_upgrade, shield_dropoff_time_upgrade, \
    shield_power_core_upgrade, shield_recharge_rate_upgrade
from .upgrade_resource.submarine_detect import submarine_detect_upgrade
from .upgrade_resource.convert_range import convert_range_upgrade


class SWGBCCUpgradeResourceSubprocessor:
    """
    Creates raw API objects for resource upgrade effects in SWGB.
    """

    assault_mech_anti_air_upgrade = staticmethod(assault_mech_anti_air_upgrade)
    berserk_heal_rate_upgrade = staticmethod(berserk_heal_rate_upgrade)
    cloak_upgrade = staticmethod(cloak_upgrade)
    detect_cloak_upgrade = staticmethod(detect_cloak_upgrade)
    concentration_upgrade = staticmethod(concentration_upgrade)
    building_conversion_upgrade = staticmethod(convert_building_upgrade)
    monk_conversion_upgrade = staticmethod(convert_monk_upgrade)
    faith_recharge_rate_upgrade = staticmethod(faith_recharge_rate_upgrade)
    heal_range_upgrade = staticmethod(heal_range_upgrade)
    shield_air_units_upgrade = staticmethod(shield_air_units_upgrade)
    shield_dropoff_time_upgrade = staticmethod(shield_dropoff_time_upgrade)
    shield_power_core_upgrade = staticmethod(shield_power_core_upgrade)
    shield_recharge_rate_upgrade = staticmethod(shield_recharge_rate_upgrade)
    submarine_detect_upgrade = staticmethod(submarine_detect_upgrade)
    conversion_range_upgrade = staticmethod(convert_range_upgrade)
