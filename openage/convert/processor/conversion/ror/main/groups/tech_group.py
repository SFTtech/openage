# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create tech groups from genie techs.
"""
from __future__ import annotations
import typing

from ......entity_object.conversion.aoc.genie_tech import InitiatedTech
from ......entity_object.conversion.ror.genie_tech import RoRStatUpgrade, \
    RoRBuildingLineUpgrade, RoRUnitLineUpgrade, RoRBuildingUnlock, RoRUnitUnlock, \
    RoRAgeUpgrade


if typing.TYPE_CHECKING:
    from ......entity_object.conversion.aoc.genie_object_container import GenieObjectContainer


def create_tech_groups(full_data_set: GenieObjectContainer) -> None:
    """
    Create techs from tech connections and unit upgrades/unlocks
    from unit connections.

    :param full_data_set: GenieObjectContainer instance that
                            contains all relevant data for the conversion
                            process.
    """
    genie_techs = full_data_set.genie_techs

    for tech_id, tech in genie_techs.items():
        tech_type = tech["tech_type"].value

        # Test if a tech exist and skip it if it doesn't
        required_techs = tech["required_techs"].value
        if all(required_tech.value == 0 for required_tech in required_techs):
            # If all required techs are tech ID 0, the tech doesnt exist
            continue

        effect_bundle_id = tech["tech_effect_id"].value

        if effect_bundle_id == -1:
            continue

        effect_bundle = full_data_set.genie_effect_bundles[effect_bundle_id]

        # Ignore techs without effects
        if len(effect_bundle.get_effects()) == 0:
            continue

        # Town Center techs (only age ups)
        if tech_type == 12:
            # Age ID is set as resource value
            setr_effects = effect_bundle.get_effects(effect_type=1)
            for effect in setr_effects:
                resource_id = effect["attr_a"].value

                if resource_id == 6:
                    age_id = int(effect["attr_d"].value)
                    break

            age_up = RoRAgeUpgrade(tech_id, age_id, full_data_set)
            full_data_set.tech_groups.update({age_up.get_id(): age_up})
            full_data_set.age_upgrades.update({age_up.get_id(): age_up})

        else:
            effects = effect_bundle.get_effects()
            for effect in effects:
                # Enabling techs
                if effect.get_type() == 2:
                    unit_id = effect["attr_a"].value
                    unit = full_data_set.genie_units[unit_id]
                    unit_type = unit["unit_type"].value

                    if unit_type == 70:
                        unit_unlock = RoRUnitUnlock(tech_id, unit_id, full_data_set)
                        full_data_set.tech_groups.update(
                            {unit_unlock.get_id(): unit_unlock}
                        )
                        full_data_set.unit_unlocks.update(
                            {unit_unlock.get_id(): unit_unlock}
                        )
                        break

                    if unit_type == 80:
                        building_unlock = RoRBuildingUnlock(tech_id, unit_id, full_data_set)
                        full_data_set.tech_groups.update(
                            {building_unlock.get_id(): building_unlock}
                        )
                        full_data_set.building_unlocks.update(
                            {building_unlock.get_id(): building_unlock}
                        )
                        break

                # Upgrades
                elif effect.get_type() == 3:
                    source_unit_id = effect["attr_a"].value
                    target_unit_id = effect["attr_b"].value
                    unit = full_data_set.genie_units[source_unit_id]
                    unit_type = unit["unit_type"].value

                    if unit_type == 70:
                        unit_upgrade = RoRUnitLineUpgrade(tech_id,
                                                          source_unit_id,
                                                          target_unit_id,
                                                          full_data_set)
                        full_data_set.tech_groups.update(
                            {unit_upgrade.get_id(): unit_upgrade}
                        )
                        full_data_set.unit_upgrades.update(
                            {unit_upgrade.get_id(): unit_upgrade}
                        )
                        break

                    if unit_type == 80:
                        building_upgrade = RoRBuildingLineUpgrade(tech_id,
                                                                  source_unit_id,
                                                                  target_unit_id,
                                                                  full_data_set)
                        full_data_set.tech_groups.update(
                            {building_upgrade.get_id(): building_upgrade}
                        )
                        full_data_set.building_upgrades.update(
                            {building_upgrade.get_id(): building_upgrade}
                        )
                        break

            else:
                # Anything else must be a stat upgrade
                stat_up = RoRStatUpgrade(tech_id, full_data_set)
                full_data_set.tech_groups.update({stat_up.get_id(): stat_up})
                full_data_set.stat_upgrades.update({stat_up.get_id(): stat_up})

    # Initiated techs are stored with buildings
    genie_units = full_data_set.genie_units

    for genie_unit in genie_units.values():
        if not genie_unit.has_member("research_id"):
            continue

        building_id = genie_unit["id0"].value
        initiated_tech_id = genie_unit["research_id"].value

        if initiated_tech_id == -1:
            continue

        if building_id not in full_data_set.building_lines.keys():
            # Skips upgraded buildings (which initiate the same techs)
            continue

        initiated_tech = InitiatedTech(initiated_tech_id, building_id, full_data_set)
        full_data_set.tech_groups.update({initiated_tech.get_id(): initiated_tech})
        full_data_set.initiated_techs.update({initiated_tech.get_id(): initiated_tech})
