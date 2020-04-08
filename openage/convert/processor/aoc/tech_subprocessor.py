# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Creates patches for technologies.
"""
from openage.convert.processor.aoc.upgrade_ability_subprocessor import AoCUgradeAbilitySubprocessor


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
        if upgrade_source_id not in dataset.unit_ref.keys():
            return patches

        line = dataset.unit_ref[upgrade_source_id]
        upgrade_source = line.line[line.get_unit_position(upgrade_source_id)]
        upgrade_target_id = effect["attr_b"].get_value()
        if upgrade_target_id not in dataset.unit_ref.keys():
            return patches

        upgrade_target = line.line[line.get_unit_position(upgrade_target_id)]

        diff = upgrade_source.diff(upgrade_target)

        patches.extend(AoCUgradeAbilitySubprocessor.idle_ability(tech_group, line, diff))

        return patches
