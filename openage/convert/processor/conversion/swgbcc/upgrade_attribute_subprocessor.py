# Copyright 2020-2025 the openage authors. See copying.md for legal info.

"""
Creates upgrade patches for attribute modification effects in SWGB.
"""
from .upgrade_attribute.cost_carbon import cost_carbon_upgrade
from .upgrade_attribute.cost_nova import cost_nova_upgrade
from .upgrade_attribute.cost_ore import cost_ore_upgrade
from .upgrade_attribute.resource_cost import resource_cost_upgrade


class SWGBCCUpgradeAttributeSubprocessor:
    """
    Creates raw API objects for attribute upgrade effects in SWGB.
    """
    cost_carbon_upgrade = staticmethod(cost_carbon_upgrade)
    cost_nova_upgrade = staticmethod(cost_nova_upgrade)
    cost_ore_upgrade = staticmethod(cost_ore_upgrade)
    resource_cost_upgrade = staticmethod(resource_cost_upgrade)
