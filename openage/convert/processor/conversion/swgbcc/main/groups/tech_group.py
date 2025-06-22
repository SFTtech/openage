# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create tech groups from genie techs.
"""
from __future__ import annotations
import typing

from ......entity_object.conversion.aoc.genie_tech import AgeUpgrade, StatUpgrade, InitiatedTech, CivBonus
from ......entity_object.conversion.swgbcc.genie_tech import SWGBUnitUnlock, SWGBUnitLineUpgrade
from ......entity_object.conversion.swgbcc.genie_unit import SWGBUnitTransformGroup
from ......value_object.conversion.swgb.internal_nyan_names import CIV_LINE_ASSOCS, CIV_TECH_ASSOCS

if typing.TYPE_CHECKING:
    from ......entity_object.conversion.aoc.genie_object_container import GenieObjectContainer


def create_tech_groups(full_data_set: GenieObjectContainer) -> None:
    """
    Create techs from tech connections and unit upgrades / unlocks
    from unit connections.

    :param full_data_set: GenieObjectContainer instance that
                            contains all relevant data for the conversion
                            process.
    """
    tech_connections = full_data_set.tech_connections

    for connection in tech_connections.values():
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

        # Building unlocks/upgrades are not in SWGB tech connections

        # Create a stat upgrade for other techs
        stat_up = StatUpgrade(tech_id, full_data_set)
        full_data_set.tech_groups.update({stat_up.get_id(): stat_up})
        full_data_set.stat_upgrades.update({stat_up.get_id(): stat_up})

    # Unit upgrades and unlocks are stored in unit connections
    unit_connections = full_data_set.unit_connections
    unit_unlocks = {}
    unit_upgrades = {}
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
            # Unit is first in line, there should be an unlock tech
            unit_unlock = SWGBUnitUnlock(enabling_research_id, line_id, full_data_set)
            unit_unlocks.update({unit_id: unit_unlock})

        elif line_mode == 3:
            # Units further down the line receive line upgrades
            unit_upgrade = SWGBUnitLineUpgrade(required_research_id, line_id,
                                               unit_id, full_data_set)
            unit_upgrades.update({required_research_id: unit_upgrade})

    # Unit unlocks for civ lines
    final_unit_unlocks = {}
    for unit_unlock in unit_unlocks.values():
        line = unit_unlock.get_unlocked_line()
        if line.get_head_unit_id() not in CIV_LINE_ASSOCS:
            for main_head_unit_id, civ_head_unit_ids in CIV_LINE_ASSOCS.items():
                if line.get_head_unit_id() in civ_head_unit_ids:
                    if isinstance(line, SWGBUnitTransformGroup):
                        main_head_unit_id = line.get_transform_unit_id()

                    # The line is an alternative civ line so the unlock
                    # is stored with the main unlock
                    main_unlock = unit_unlocks[main_head_unit_id]
                    main_unlock.add_civ_unlock(unit_unlock)
                    break

            else:
                # The unlock is for a line without alternative civ lines
                final_unit_unlocks.update({unit_unlock.get_id(): unit_unlock})

        else:
            # The unlock is for a main line
            final_unit_unlocks.update({unit_unlock.get_id(): unit_unlock})

    full_data_set.tech_groups.update(final_unit_unlocks)
    full_data_set.unit_unlocks.update(final_unit_unlocks)

    # Unit upgrades for civ lines
    final_unit_upgrades = {}
    for unit_upgrade in unit_upgrades.values():
        tech_id = unit_upgrade.tech.get_id()
        if tech_id not in CIV_TECH_ASSOCS:
            for main_tech_id, civ_tech_ids in CIV_TECH_ASSOCS.items():
                if tech_id in civ_tech_ids:
                    # The tech is upgrade for an alternative civ so the upgrade
                    # is stored with the main upgrade
                    main_upgrade = unit_upgrades[main_tech_id]
                    main_upgrade.add_civ_upgrade(unit_upgrade)
                    break

            else:
                # The upgrade is for a line without alternative civ lines
                final_unit_upgrades.update({unit_upgrade.get_id(): unit_upgrade})

        else:
            # The upgrade is for a main line
            final_unit_upgrades.update({unit_upgrade.get_id(): unit_upgrade})

    full_data_set.tech_groups.update(final_unit_upgrades)
    full_data_set.unit_upgrades.update(final_unit_upgrades)

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
