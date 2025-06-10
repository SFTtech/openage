# Copyright 2020-2025 the openage authors. See copying.md for legal info.

"""
Creates patches for technologies.
"""
from __future__ import annotations
import typing

from .tech.attribute_modify import attribute_modify_effect
from .tech.resource_modify import resource_modify_effect
from .tech.unit_upgrade import upgrade_unit_effect
from .tech.upgrade_funcs import UPGRADE_ATTRIBUTE_FUNCS, UPGRADE_RESOURCE_FUNCS

if typing.TYPE_CHECKING:
    from ....entity_object.conversion.converter_object import ConverterObjectGroup
    from ....value_object.conversion.forward_ref import ForwardRef


class RoRTechSubprocessor:
    """
    Creates raw API objects and patches for techs and civ setups in RoR.
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
        effects = converter_group.get_effects()
        for effect in effects:
            type_id = effect.get_type()

            if type_id in (0, 4, 5):
                patches.extend(attribute_modify_effect(converter_group, effect))

            elif type_id == 1:
                patches.extend(resource_modify_effect(converter_group, effect))

            elif type_id == 2:
                # Enabling/disabling units: Handled in creatable conditions
                pass

            elif type_id == 3:
                patches.extend(upgrade_unit_effect(converter_group, effect))

        return patches

    attribute_modify_effect = staticmethod(attribute_modify_effect)
    resource_modify_effect = staticmethod(resource_modify_effect)
    upgrade_unit_effect = staticmethod(upgrade_unit_effect)
