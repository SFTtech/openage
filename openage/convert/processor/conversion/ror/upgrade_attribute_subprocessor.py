# Copyright 2020-2025 the openage authors. See copying.md for legal info.

"""
Creates upgrade patches for attribute modification effects in RoR.
"""
from .upgrade_attribute.ballistics import ballistics_upgrade
from .upgrade_attribute.population import population_upgrade


class RoRUpgradeAttributeSubprocessor:
    """
    Creates raw API objects for attribute upgrade effects in RoR.
    """

    ballistics_upgrade = staticmethod(ballistics_upgrade)
    population_upgrade = staticmethod(population_upgrade)
