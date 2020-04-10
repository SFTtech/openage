# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Creates patches for technologies.
"""
from openage.convert.processor.aoc.upgrade_ability_subprocessor import AoCUgradeAbilitySubprocessor
from openage.convert.dataformat.aoc.genie_unit import GenieUnitLineGroup


class AoCTechSubprocessor:

    @classmethod
    def get_patches(cls, tech_group):
        """
        Returns the patches for a tech group, depending on the type
        of its effects.
        """
        patches = []
        for effect in tech_group.effects.get_effects():
            type_id = effect.get_type()

            if type_id == 3:
                patches.extend(cls._upgrade_unit_tech(tech_group, effect))

        return patches

    @staticmethod
    def _upgrade_unit_tech(tech_group, effect):
        """
        Creates the patches for upgrading entities in a line.
        """
        patches = []
        dataset = tech_group.data

        upgrade_source_id = effect["attr_a"].get_value()
        upgrade_target_id = effect["attr_b"].get_value()

        if upgrade_source_id not in dataset.unit_ref.keys() or\
                upgrade_target_id not in dataset.unit_ref.keys():
            # Skip annexes or transform units
            return patches

        line = dataset.unit_ref[upgrade_source_id]
        upgrade_source_pos = line.get_unit_position(upgrade_source_id)
        upgrade_target_pos = line.get_unit_position(upgrade_target_id)

        if upgrade_target_pos - upgrade_source_pos != 1:
            # Skip effects that upgrades entities not next to each other in
            # the line. This is not used in the games anyway and we would handle
            # it differently.
            return patches

        upgrade_source = line.line[upgrade_source_pos]
        upgrade_target = line.line[upgrade_target_pos]

        diff = upgrade_source.diff(upgrade_target)

        patches.extend(AoCUgradeAbilitySubprocessor.death_ability(tech_group, line, diff))
        patches.extend(AoCUgradeAbilitySubprocessor.despawn_ability(tech_group, line, diff))
        patches.extend(AoCUgradeAbilitySubprocessor.idle_ability(tech_group, line, diff))
        patches.extend(AoCUgradeAbilitySubprocessor.live_ability(tech_group, line, diff))
        patches.extend(AoCUgradeAbilitySubprocessor.los_ability(tech_group, line, diff))
        patches.extend(AoCUgradeAbilitySubprocessor.selectable_ability(tech_group, line, diff))
        patches.extend(AoCUgradeAbilitySubprocessor.turn_ability(tech_group, line, diff))

        if line.is_projectile_shooter():
            patches.extend(AoCUgradeAbilitySubprocessor.shoot_projectile_ability(tech_group, line,
                                                                                 upgrade_source,
                                                                                 upgrade_target,
                                                                                 7, diff))
        elif line.is_melee() or line.is_ranged():
            if line.has_command(7):
                # Attack
                patches.extend(AoCUgradeAbilitySubprocessor.apply_discrete_effect_ability(tech_group,
                                                                                          line,
                                                                                          7,
                                                                                          line.is_ranged(),
                                                                                          diff))

            # TODO: Other commands

        if isinstance(line, GenieUnitLineGroup):
            patches.extend(AoCUgradeAbilitySubprocessor.move_ability(tech_group, line, diff))

        return patches
