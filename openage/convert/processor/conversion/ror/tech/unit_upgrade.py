# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Creates patches for upgrading entities in a line.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.aoc.genie_unit import GenieBuildingLineGroup, \
    GenieUnitLineGroup
from .....service.conversion import internal_name_lookups
from ...aoc.upgrade_ability_subprocessor import AoCUpgradeAbilitySubprocessor
from ..upgrade_ability_subprocessor import RoRUpgradeAbilitySubprocessor

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.converter_object import ConverterObjectGroup
    from .....entity_object.conversion.aoc.genie_effect import GenieEffectObject
    from .....value_object.conversion.forward_ref import ForwardRef


def upgrade_unit_effect(
    converter_group: ConverterObjectGroup,
    effect: GenieEffectObject
) -> list[ForwardRef]:
    """
    Creates the patches for upgrading entities in a line.
    """
    patches = []
    tech_id = converter_group.get_id()
    dataset = converter_group.data

    tech_lookup_dict = internal_name_lookups.get_tech_lookups(dataset.game_version)

    head_unit_id = effect["attr_a"].value
    upgrade_target_id = effect["attr_b"].value

    if head_unit_id not in dataset.unit_ref.keys() or\
            upgrade_target_id not in dataset.unit_ref.keys():
        # Skip annexes or transform units
        return patches

    line = dataset.unit_ref[head_unit_id]
    upgrade_target_pos = line.get_unit_position(upgrade_target_id)
    upgrade_source_pos = upgrade_target_pos - 1

    upgrade_source = line.line[upgrade_source_pos]
    upgrade_target = line.line[upgrade_target_pos]
    tech_name = tech_lookup_dict[tech_id][0]

    diff = upgrade_source.diff(upgrade_target)

    patches.extend(
        AoCUpgradeAbilitySubprocessor.death_ability(
            converter_group,
            line,
            tech_name,
            diff
        )
    )
    patches.extend(
        AoCUpgradeAbilitySubprocessor.despawn_ability(
            converter_group,
            line,
            tech_name,
            diff
        )
    )
    patches.extend(
        AoCUpgradeAbilitySubprocessor.idle_ability(
            converter_group,
            line,
            tech_name,
            diff
        )
    )
    patches.extend(
        AoCUpgradeAbilitySubprocessor.live_ability(
            converter_group,
            line,
            tech_name,
            diff
        )
    )
    patches.extend(
        AoCUpgradeAbilitySubprocessor.los_ability(
            converter_group,
            line,
            tech_name,
            diff
        )
    )
    patches.extend(
        AoCUpgradeAbilitySubprocessor.named_ability(
            converter_group,
            line,
            tech_name,
            diff
        )
    )
    # patches.extend(
    #     AoCUpgradeAbilitySubprocessor.resistance_ability(
    #         converter_group,
    #         line,
    #         tech_name,
    #         diff
    #     )
    # )
    patches.extend(
        AoCUpgradeAbilitySubprocessor.selectable_ability(
            converter_group,
            line,
            tech_name,
            diff
        )
    )
    patches.extend(
        AoCUpgradeAbilitySubprocessor.turn_ability(
            converter_group,
            line,
            tech_name, diff
        )
    )

    if line.is_projectile_shooter():
        patches.extend(
            RoRUpgradeAbilitySubprocessor.shoot_projectile_ability(
                converter_group,
                line,
                tech_name,
                7,
                diff
            )
        )

    elif line.is_melee() or line.is_ranged():
        if line.has_command(7):
            # Attack
            patches.extend(
                AoCUpgradeAbilitySubprocessor.apply_discrete_effect_ability(
                    converter_group,
                    line,
                    tech_name,
                    7,
                    line.is_ranged(),
                    diff
                )
            )

    if isinstance(line, GenieUnitLineGroup):
        patches.extend(
            AoCUpgradeAbilitySubprocessor.move_ability(
                converter_group,
                line,
                tech_name,
                diff
            )
        )

    if isinstance(line, GenieBuildingLineGroup):
        # TODO: Damage percentages change
        # patches.extend(AoCUpgradeAbilitySubprocessor.attribute_change_tracker_ability(converter_group, line,
        #                                                                               tech_name, diff))
        pass

    return patches
