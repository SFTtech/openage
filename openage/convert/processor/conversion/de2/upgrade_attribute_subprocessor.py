# Copyright 2020-2025 the openage authors. See copying.md for legal info.
"""
Creates upgrade patches for attribute modification effects in DE2.
"""
from .upgrade_attribute.charge import charge_attack_upgrade, charge_event_upgrade, \
    charge_regen_upgrade, charge_type_upgrade
from .upgrade_attribute.convert_chance import convert_chance_upgrade
from .upgrade_attribute.herdable_capacity import herdable_capacity_upgrade
from .upgrade_attribute.max_convert import max_convert_upgrade
from .upgrade_attribute.min_convert import min_convert_upgrade
from .upgrade_attribute.regeneration_rate import regeneration_rate_upgrade
from .upgrade_attribute.villager_pop_space import villager_pop_space_upgrade
from .upgrade_attribute.unknown import bfg_unknown_51_upgrade, bfg_unknown_71_upgrade, \
    bfg_unknown_73_upgrade


class DE2UpgradeAttributeSubprocessor:
    """
    Creates raw API objects for attribute upgrade effects in DE2.
    """

    bfg_unknown_51_upgrade = staticmethod(bfg_unknown_51_upgrade)
    bfg_unknown_71_upgrade = staticmethod(bfg_unknown_71_upgrade)
    bfg_unknown_73_upgrade = staticmethod(bfg_unknown_73_upgrade)
    charge_attack_upgrade = staticmethod(charge_attack_upgrade)
    charge_event_upgrade = staticmethod(charge_event_upgrade)
    charge_regen_upgrade = staticmethod(charge_regen_upgrade)
    charge_type_upgrade = staticmethod(charge_type_upgrade)
    convert_chance_upgrade = staticmethod(convert_chance_upgrade)
    herdable_capacity_upgrade = staticmethod(herdable_capacity_upgrade)
    max_convert_upgrade = staticmethod(max_convert_upgrade)
    min_convert_upgrade = staticmethod(min_convert_upgrade)
    regeneration_rate_upgrade = staticmethod(regeneration_rate_upgrade)
    villager_pop_space_upgrade = staticmethod(villager_pop_space_upgrade)
