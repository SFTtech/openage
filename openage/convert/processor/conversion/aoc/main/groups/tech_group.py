# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create tech groups from genie techs.
"""
from __future__ import annotations
import typing

from ......entity_object.conversion.aoc.genie_tech import AgeUpgrade, BuildingUnlock, \
    BuildingLineUpgrade, CivBonus, InitiatedTech, StatUpgrade, UnitLineUpgrade, \
    UnitUnlock

if typing.TYPE_CHECKING:
    from ......entity_object.conversion.aoc.genie_object_container import GenieObjectContainer


def create_tech_groups(full_data_set: GenieObjectContainer) -> None:
    """
    Create techs from tech connections and unit upgrades/unlocks
    from unit connections.

    :param full_data_set: GenieObjectContainer instance that
                            contains all relevant data for the conversion
                            process.
    :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
    """
    tech_connections = full_data_set.tech_connections

    # In tech connection are age ups, building unlocks/upgrades and stat upgrades
    for connection in tech_connections.values():
        connected_buildings = connection["buildings"].value
        tech_id = connection["id"].value
        tech = full_data_set.genie_techs[tech_id]

        effect_id = tech["tech_effect_id"].value
        if effect_id < 0:
            continue

        tech_effects = full_data_set.genie_effect_bundles[effect_id]

        # Check if the tech is an age upgrade
        tech_found = False
        resource_effects = tech_effects.get_effects(effect_type=1)
        for effect in resource_effects:
            # Resource ID 6: Current Age
            if effect["attr_a"].value != 6:
                continue

            age_id = effect["attr_b"].value
            age_up = AgeUpgrade(tech_id, age_id, full_data_set)
            full_data_set.tech_groups.update({age_up.get_id(): age_up})
            full_data_set.age_upgrades.update({age_up.get_id(): age_up})
            tech_found = True
            break

        if tech_found:
            continue

        if len(connected_buildings) > 0:
            # Building unlock or upgrade
            unlock_effects = tech_effects.get_effects(effect_type=2)
            upgrade_effects = tech_effects.get_effects(effect_type=2)
            if len(unlock_effects) > 0:
                unlock = unlock_effects[0]
                unlock_id = unlock["attr_a"].value

                building_unlock = BuildingUnlock(tech_id, unlock_id, full_data_set)
                full_data_set.tech_groups.update(
                    {building_unlock.get_id(): building_unlock}
                )
                full_data_set.building_unlocks.update(
                    {building_unlock.get_id(): building_unlock}
                )
                continue

            if len(upgrade_effects) > 0:
                upgrade = upgrade_effects[0]
                line_id = upgrade["attr_a"].value
                upgrade_id = upgrade["attr_b"].value

                building_upgrade = BuildingLineUpgrade(
                    tech_id,
                    line_id,
                    upgrade_id,
                    full_data_set
                )
                full_data_set.tech_groups.update(
                    {building_upgrade.get_id(): building_upgrade}
                )
                full_data_set.building_upgrades.update(
                    {building_upgrade.get_id(): building_upgrade}
                )
                continue

        # Create a stat upgrade for other techs
        stat_up = StatUpgrade(tech_id, full_data_set)
        full_data_set.tech_groups.update({stat_up.get_id(): stat_up})
        full_data_set.stat_upgrades.update({stat_up.get_id(): stat_up})

    # Unit upgrades and unlocks are stored in unit connections
    unit_connections = full_data_set.unit_connections
    for connection in unit_connections.values():
        unit_id = connection["id"].value
        required_research_id = connection["required_research"].value
        enabling_research_id = connection["enabling_research"].value
        line_mode = connection["line_mode"].value
        line_id = full_data_set.unit_ref[unit_id].get_id()

        if required_research_id == -1 and enabling_research_id == -1:
            # Unit is unlocked from the start
            continue

        if line_mode == 2:
            # Unit is first in line, there should be an unlock tech id
            # This is usually the enabling tech id
            unlock_tech_id = enabling_research_id
            if unlock_tech_id == -1:
                # Battle elephant is a curious exception wher it's the required tech id
                unlock_tech_id = required_research_id

            unit_unlock = UnitUnlock(unlock_tech_id, line_id, full_data_set)
            full_data_set.tech_groups.update({unit_unlock.get_id(): unit_unlock})
            full_data_set.unit_unlocks.update({unit_unlock.get_id(): unit_unlock})

        elif line_mode == 3:
            # Units further down the line receive line upgrades
            unit_upgrade = UnitLineUpgrade(required_research_id, line_id,
                                           unit_id, full_data_set)
            full_data_set.tech_groups.update({unit_upgrade.get_id(): unit_upgrade})
            full_data_set.unit_upgrades.update({unit_upgrade.get_id(): unit_upgrade})

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

    # Civ boni have to be aquired from techs
    # Civ boni = ONLY passive boni (not unit unlocks, unit upgrades or team bonus)
    genie_techs = full_data_set.genie_techs

    for index, _ in enumerate(genie_techs):
        tech_id = index

        # Civ ID must be positive and non-zero
        civ_id = genie_techs[index]["civilization_id"].value
        if civ_id <= 0:
            continue

        # Passive boni are not researched anywhere
        research_location_id = genie_techs[index]["research_location_id"].value
        if research_location_id > 0:
            continue

        # Passive boni are not available in full tech mode
        full_tech_mode = genie_techs[index]["full_tech_mode"].value
        if full_tech_mode:
            continue

        civ_bonus = CivBonus(tech_id, civ_id, full_data_set)
        full_data_set.tech_groups.update({civ_bonus.get_id(): civ_bonus})
        full_data_set.civ_boni.update({civ_bonus.get_id(): civ_bonus})
