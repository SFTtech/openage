# Copyright 2020-2025 the openage authors. See copying.md for legal info.

"""
Creates upgrade patches for resource modification effects in RoR.
"""
from .upgrade_resource.convert_building import convert_building_upgrade
from .upgrade_resource.heal_bonus import heal_bonus_upgrade
from .upgrade_resource.martyrdom import martyrdom_upgrade


class RoRUpgradeResourceSubprocessor:
    """
    Creates raw API objects for resource upgrade effects in RoR.
    """

    building_conversion_upgrade = staticmethod(convert_building_upgrade)
    heal_bonus_upgrade = staticmethod(heal_bonus_upgrade)
    martyrdom_upgrade = staticmethod(martyrdom_upgrade)
