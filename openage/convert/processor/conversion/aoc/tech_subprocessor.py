# Copyright 2020-2025 the openage authors. See copying.md for legal info.

"""
Creates patches for technologies.
"""
from __future__ import annotations
import typing

from ....entity_object.conversion.aoc.genie_tech import CivTeamBonus, CivBonus
from ....value_object.conversion.forward_ref import ForwardRef

from .tech.attribute_modify import attribute_modify_effect
from .tech.resource_modify import resource_modify_effect
from .tech.tech_cost import tech_cost_modify_effect
from .tech.tech_time import tech_time_modify_effect
from .tech.unit_upgrade import upgrade_unit_effect
from .tech.upgrade_funcs import UPGRADE_ATTRIBUTE_FUNCS, UPGRADE_RESOURCE_FUNCS

if typing.TYPE_CHECKING:
    from openage.convert.entity_object.conversion.converter_object import ConverterObjectGroup


class AoCTechSubprocessor:
    """
    Creates raw API objects and patches for techs and civ setups in AoC.
    """

    upgrade_attribute_funcs = UPGRADE_ATTRIBUTE_FUNCS
    upgrade_resource_funcs = UPGRADE_RESOURCE_FUNCS

    @classmethod
    def get_patches(cls, converter_group: ConverterObjectGroup) -> list[ForwardRef]:
        """
        Returns the patches for a converter group, depending on the type
        of its effects.
        """
        patches = []
        dataset = converter_group.data
        team_bonus = False

        if isinstance(converter_group, CivTeamBonus):
            effects = converter_group.get_effects()

            # Change converter group here, so that the Civ object gets the patches
            converter_group = dataset.civ_groups[converter_group.get_civilization_id()]
            team_bonus = True

        elif isinstance(converter_group, CivBonus):
            effects = converter_group.get_effects()

            # Change converter group here, so that the Civ object gets the patches
            converter_group = dataset.civ_groups[converter_group.get_civilization_id()]

        else:
            effects = converter_group.get_effects()

        team_effect = False
        for effect in effects:
            type_id = effect.get_type()

            if team_bonus or type_id in (10, 11, 12, 13, 14, 15, 16):
                team_effect = True
                type_id -= 10

            if type_id in (0, 4, 5):
                patches.extend(attribute_modify_effect(converter_group,
                                                       effect,
                                                       team=team_effect))

            elif type_id in (1, 6):
                patches.extend(resource_modify_effect(converter_group,
                                                      effect,
                                                      team=team_effect))

            elif type_id == 2:
                # Enabling/disabling units: Handled in creatable conditions
                pass

            elif type_id == 3:
                patches.extend(upgrade_unit_effect(converter_group, effect))

            elif type_id == 101:
                patches.extend(tech_cost_modify_effect(converter_group,
                                                       effect,
                                                       team=team_effect))

            elif type_id == 102:
                # Tech disable: Only used for civ tech tree
                pass

            elif type_id == 103:
                patches.extend(tech_time_modify_effect(converter_group,
                                                       effect,
                                                       team=team_effect))

            team_effect = False

        return patches

    attribute_modify_effect = staticmethod(attribute_modify_effect)
    resource_modify_effect = staticmethod(resource_modify_effect)
    upgrade_unit_effect = staticmethod(upgrade_unit_effect)
    tech_cost_modify_effect = staticmethod(tech_cost_modify_effect)
    tech_time_modify_effect = staticmethod(tech_time_modify_effect)
