# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create patches for upgrading the Resistance ability.
"""
from __future__ import annotations
import typing

from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef
from .....value_object.read.value_members import NoDiffMember
from ..upgrade_effect_subprocessor import AoCUpgradeEffectSubprocessor

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.converter_object import ConverterObject, \
        ConverterObjectGroup
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup


def resistance_ability(
    converter_group: ConverterObjectGroup,
    line: GenieGameEntityGroup,
    container_obj_ref: str,
    diff: ConverterObject = None
) -> list[ForwardRef]:
    """
    Creates a patch for the Resistance ability of a line.

    :param converter_group: Group that gets the patch.
    :param line: Unit/Building line that has the ability.
    :param container_obj_ref: Reference of the raw API object the patch is nested in.
    :param diff: A diff between two ConvertObject instances.
    :returns: The forward references for the generated patches.
    """
    head_unit_id = line.get_head_unit_id()
    dataset = line.data

    patches = []

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

    game_entity_name = name_lookup_dict[head_unit_id][0]

    diff_armors = diff["armors"]
    if not isinstance(diff_armors, NoDiffMember):
        patch_target_ref = f"{game_entity_name}.Resistance"
        patches.extend(AoCUpgradeEffectSubprocessor.get_attack_resistances(converter_group,
                                                                           line, diff,
                                                                           patch_target_ref))

    # TODO: Other resistance types

    return patches
