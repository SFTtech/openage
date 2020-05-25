# Copyright 2019-2020 the openage authors. See copying.md for legal info.


from ...dataformat.converter_object import ConverterObject,\
    ConverterObjectGroup


class GenieTechObject(ConverterObject):
    """
    Technology in AoE2.

    Techs are not limited to researchable technologies. They also
    unlock the unique units of civs and contain the civ bonuses
    (excluding team boni).
    """

    __slots__ = ('data')

    def __init__(self, tech_id, full_data_set, members=None):
        """
        Creates a new Genie tech object.

        :param tech_id: The internal tech_id from the .dat file.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :param members: An already existing member dict.
        """

        super().__init__(tech_id, members=members)

        self.data = full_data_set

    def __repr__(self):
        return "GenieTechObject<%s>" % (self.get_id())


class GenieTechEffectBundleGroup(ConverterObjectGroup):
    """
    A tech and the collection of its effects.
    """

    __slots__ = ('data', 'tech', 'effects')

    def __init__(self, tech_id, full_data_set):
        """
        Creates a new Genie tech group object.

        :param tech_id: The internal tech_id from the .dat file.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """

        super().__init__(tech_id)

        self.data = full_data_set

        # The tech that belongs to the tech id
        self.tech = self.data.genie_techs[tech_id]

        # Effects of the tech
        effect_bundle_id = self.tech.get_member("tech_effect_id").get_value()

        if effect_bundle_id > -1:
            self.effects = self.data.genie_effect_bundles[effect_bundle_id]

        else:
            self.effects = None

    def is_researchable(self):
        """
        Techs are researchable if they are associated with an ingame tech.
        This is the case if the research time is greater than 0.

        :returns: True if the research time is greater than zero.
        """
        research_time = self.tech.get_member("research_time").get_value()

        # -1 = no train location
        return research_time > 0

    def is_unique(self):
        """
        Techs are unique if they belong to a specific civ.

        :returns: True if the civilization id is greater than zero.
        """
        civilization_id = self.tech.get_member("civilization_id").get_value()

        # -1 = no train location
        return civilization_id > -1

    def get_civilization(self):
        """
        Returns the civilization id if the tech is unique, otherwise return None.
        """
        if self.is_unique():
            return self.tech.get_member("civilization_id").get_value()

        return None

    def get_effects(self, effect_type=None):
        """
        Returns the associated effects.
        """
        if self.effects:
            return self.effects.get_effects(effect_type=effect_type)

        return []

    def get_required_techs(self):
        """
        Returns the techs that are required for this tech.
        """
        required_tech_ids = self.tech.get_member("required_techs").get_value()

        required_techs = []

        for tech_id_member in required_tech_ids:
            tech_id = tech_id_member.get_value()
            if tech_id == -1:
                break

            required_techs.append(self.data.genie_techs[tech_id])

        return required_techs

    def get_required_tech_count(self):
        """
        Returns the number of required techs necessary to unlock this  tech.
        """
        return self.tech.get_member("required_tech_count").get_value()

    def get_research_location_id(self):
        """
        Returns the group_id for a building line if the tech is
        researchable, otherwise return None.
        """
        if self.is_researchable():
            return self.tech.get_member("research_location_id").get_value()

        return None

    def has_effect(self):
        """
        Returns True if the techology's effects do anything.
        """
        if self.effects:
            return len(self.effects.get_effects()) > 0
        else:
            return False

    def __repr__(self):
        return "GenieTechEffectBundleGroup<%s>" % (self.get_id())


class StatUpgrade(GenieTechEffectBundleGroup):
    """
    Upgrades attributes of units/buildings or other stats in the game.
    """

    def __repr__(self):
        return "StatUpgrade<%s>" % (self.get_id())


class AgeUpgrade(GenieTechEffectBundleGroup):
    """
    Researches a new Age.

    openage actually does not care about Ages, so this will
    not be different from any other Tech API object. However,
    we will use this object to push all Age-related upgrades
    here and create a Tech from it.
    """

    __slots__ = ('age_id')

    def __init__(self, tech_id, age_id, full_data_set):
        """
        Creates a new Genie tech group object.

        :param tech_id: The internal tech_id from the .dat file.
        :param age_id: The index of the Age. (First Age = 0)
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """
        super().__init__(tech_id, full_data_set)

        self.age_id = age_id

    def __repr__(self):
        return "AgeUpgrade<%s>" % (self.get_id())


class UnitLineUpgrade(GenieTechEffectBundleGroup):
    """
    Upgrades a unit in a line.

    This will become a Tech API object targeted at the line's game entity.
    """

    __slots__ = ('unit_line_id', 'upgrade_target_id')

    def __init__(self, tech_id, unit_line_id, upgrade_target_id, full_data_set):
        """
        Creates a new Genie line upgrade object.

        :param tech_id: The internal tech_id from the .dat file.
        :param unit_line_id: The unit line that is upgraded.
        :param upgrade_target_id: The unit that is the result of the upgrade.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """
        super().__init__(tech_id, full_data_set)

        self.unit_line_id = unit_line_id
        self.upgrade_target_id = upgrade_target_id

    def get_line_id(self):
        """
        Returns the line id of the upgraded line.
        """
        return self.unit_line_id

    def get_upgrade_target_id(self):
        """
        Returns the target unit that is upgraded to.
        """
        return self.upgrade_target_id

    def __repr__(self):
        return "UnitLineUpgrade<%s>" % (self.get_id())


class BuildingLineUpgrade(GenieTechEffectBundleGroup):
    """
    Upgrades a building in a line.

    This will become a Tech API object targeted at the line's game entity.
    """

    __slots__ = ('building_line_id', 'upgrade_target_id')

    def __init__(self, tech_id, building_line_id, upgrade_target_id, full_data_set):
        """
        Creates a new Genie line upgrade object.

        :param tech_id: The internal tech_id from the .dat file.
        :param building_line_id: The building line that is upgraded.
        :param upgrade_target_id: The unit that is the result of the upgrade.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """
        super().__init__(tech_id, full_data_set)

        self.building_line_id = building_line_id
        self.upgrade_target_id = upgrade_target_id

    def get_line_id(self):
        """
        Returns the line id of the upgraded line.
        """
        return self.building_line_id

    def get_upgrade_target_id(self):
        """
        Returns the target unit that is upgraded to.
        """
        return self.upgrade_target_id

    def __repr__(self):
        return "BuildingLineUpgrade<%s>" % (self.get_id())


class UnitUnlock(GenieTechEffectBundleGroup):
    """
    Unlocks units, sometimes with additional requirements like (266 - Castle built).

    This will become one or more patches for an AgeUpgrade Tech. If the unlock
    is civ-specific, two patches (one for the age, one for the civ)
    will be created.
    """

    __slots__ = ('line_id')

    def __init__(self, tech_id, line_id, full_data_set):
        """
        Creates a new Genie tech group object.

        :param tech_id: The internal tech_id from the .dat file.
        :param line_id: The id of the unlocked line.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """

        super().__init__(tech_id, full_data_set)

        self.line_id = line_id

    def get_line_id(self):
        """
        Returns the ID of the line that is unlocked by this tech.
        """
        return self.line_id

    def get_unlocked_line(self):
        """
        Returns the line that is unlocked by this tech.
        """
        return self.data.unit_lines_vertical_ref[self.line_id]

    def __repr__(self):
        return "UnitUnlock<%s>" % (self.get_id())


class BuildingUnlock(GenieTechEffectBundleGroup):
    """
    Unlocks buildings, sometimes with additional requirements like (266 - Castle built).

    This will become one or more patches for an AgeUpgrade Tech. If the unlock
    is civ-specific, two patches (one for the age, one for the civ)
    will be created.
    """

    __slots__ = ('head_unit_id')

    def __init__(self, tech_id, head_unit_id, full_data_set):
        """
        Creates a new Genie tech group object.

        :param tech_id: The internal tech_id from the .dat file.
        :param head_unit_id: The id of the unlocked line.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """

        super().__init__(tech_id, full_data_set)

        self.head_unit_id = head_unit_id

    def get_line_id(self):
        """
        Returns the ID of the line that is unlocked by this tech.
        """
        return self.head_unit_id

    def get_unlocked_line(self):
        """
        Returns the line that is unlocked by this tech.
        """
        return self.data.building_lines[self.head_unit_id]

    def __repr__(self):
        return "BuildingUnlock<%s>" % (self.get_id())


class InitiatedTech(GenieTechEffectBundleGroup):
    """
    Techs initiated by buildings when they have finished constructing.

    This will used to determine requirements for the creatables.
    """

    __slots__ = ('building_id')

    def __init__(self, tech_id, building_id, full_data_set):
        """
        Creates a new Genie tech group object.

        :param tech_id: The internal tech_id from the .dat file.
        :param building_id: The id of the genie building initiatig this tech.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """

        super().__init__(tech_id, full_data_set)

        self.building_id = building_id

    def get_building_id(self):
        return self.building_id

    def __repr__(self):
        return "InitiatedTech<%s>" % (self.get_id())


class NodeTech(GenieTechEffectBundleGroup):
    """
    Techs that act as a container for other techs. It is used to form complex
    requirements for age upgrades.
    """

    def __init__(self, tech_id, full_data_set):
        """
        Creates a new Genie tech group object.

        :param tech_id: The internal tech_id from the .dat file.
        :param building_id: The id of the genie building initiatig this tech.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """

        super().__init__(tech_id, full_data_set)

    def __repr__(self):
        return "NodeTech<%s>" % (self.get_id())


class CivBonus(GenieTechEffectBundleGroup):
    """
    Gives one specific civilization a bonus. Not the team bonus or tech tree.

    This will become patches in the Civilization API object.
    """

    __slots__ = ('civ_id')

    def __init__(self, tech_id, civ_id, full_data_set):
        """
        Creates a new Genie tech group object.

        :param tech_id: The internal tech_id from the .dat file.
        :param civ_id: The index of the civ.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """

        super().__init__(tech_id, full_data_set)

        self.civ_id = civ_id

    def get_civilization(self):
        return self.civ_id

    def replaces_researchable_tech(self):
        """
        Checks if this bonus replaces a researchable Tech and returns the tech group
        if thats the case. Otherwise None is returned.
        """
        for tech_group in self.data.tech_groups.values():
            if tech_group.is_researchable():
                bonus_effect_id = self.tech.get_member("tech_effect_id").get_value()
                tech_group_effect_id = tech_group.tech.get_member("tech_effect_id").get_value()

                if bonus_effect_id == tech_group_effect_id:
                    return tech_group

        return None

    def __repr__(self):
        return "CivBonus<%s>" % (self.get_id())


class CivTeamBonus(ConverterObjectGroup):
    """
    Gives a civilization and all allies a bonus.

    This will become patches in the Civilization API object.
    """

    __slots__ = ('tech_id', 'data', 'civ_id', 'effects')

    def __init__(self, tech_id, civ_id, effect_bundle_id, full_data_set):
        """
        Creates a new Genie tech group object.

        :param tech_id: The internal tech_id from the .dat file.
        :param civ_id: The index of the civ.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """
        super().__init__(tech_id)

        self.tech_id = tech_id
        self.data = full_data_set
        self.civ_id = civ_id
        self.effects = self.data.genie_effect_bundles[effect_bundle_id]

    def get_effects(self):
        """
        Returns the associated effects.
        """
        return self.effects.get_effects()

    def get_civilization(self):
        return self.civ_id

    def __repr__(self):
        return "CivTeamBonus<%s>" % (self.get_id())


class CivTechTree(ConverterObjectGroup):
    """
    Tech tree of a civilization.

    This will become patches in the Civilization API object.
    """

    __slots__ = ('tech_id', 'data', 'civ_id', 'effects')

    def __init__(self, tech_id, civ_id, effect_bundle_id, full_data_set):
        """
        Creates a new Genie tech group object.

        :param tech_id: The internal tech_id from the .dat file.
        :param civ_id: The index of the civ.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """
        super().__init__(tech_id)

        self.tech_id = tech_id
        self.data = full_data_set
        self.civ_id = civ_id

        if effect_bundle_id > -1:
            self.effects = self.data.genie_effect_bundles[effect_bundle_id]

        else:
            self.effects = None

    def get_effects(self):
        """
        Returns the associated effects.
        """
        if self.effects:
            return self.effects.get_effects()

        return []

    def get_civilization(self):
        return self.civ_id

    def __repr__(self):
        return "CivTechTree<%s>" % (self.get_id())
