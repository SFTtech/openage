# Copyright 2019-2019 the openage authors. See copying.md for legal info.


from openage.convert.dataformat.converter_object import ConverterObject,\
    ConverterObjectGroup


class GenieTechObject(ConverterObject):
    """
    Technology in AoE2.

    Techs are not limited to researchable technologies. They also
    unlock the unique units of civs and contain the civ bonuses
    (excluding team boni).
    """

    def __init__(self, tech_id, full_data_set):
        """
        Creates a new Genie tech object.

        :param tech_id: The internal tech_id from the .dat file.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """

        super().__init__(tech_id)

        self.data = full_data_set
        self.data.genie_techs.update({self.get_id(): self})


class GenieTechEffectBundleGroup(ConverterObjectGroup):
    """
    A tech and the collection of its effects.
    """

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
        self.data.tech_groups.update({self.get_id(): self})

        self.effects = []

        effect_bundle_id = self.data.genie_techs[tech_id].get_member("tech_effect_id").get_value()
        tech_effects = self.data.genie_effect_bundles[effect_bundle_id].get_effects()

        self.effects.extend(tech_effects)


class TechLineGroup(ConverterObjectGroup):
    """
    Collection of GenieTechGroups that form a line (i.e. they unlock each other
    consecutively).

    Example: Double-bit axe->Bow Saw-> Two-man saw

    Each of the techs in line will become individual Tech API objects.
    """

    def __init__(self, line_id, full_data_set):
        """
        Creates a new Genie tech group object.

        :param line_id: The internal line_id from the .dat file (ResearchConnection).
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """

        super().__init__(line_id)

        # The line is stored as an ordered list of GenieTechEffectBundleGroups.
        self.line = []

        self.data = full_data_set
        self.data.tech_lines.update({self.get_id(): self})


class AgeUpgrade(GenieTechEffectBundleGroup):
    """
    Researches a new Age.

    openage actually does not care about Ages, so this will
    not be different from any other Tech API object. However,
    we will use this object to push all Age-related upgrades
    here and create a Tech from it.
    """

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


class UnitLineUpgrade(GenieTechEffectBundleGroup):
    """
    Upgrades a unit in a line.

    This will become a Tech API object targeted at the line's game entity.
    """

    def __init__(self, tech_id, unit_line_id, upgrade_target_id, full_data_set):
        """
        Creates a new Genie tech group object.

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


class UnitUnlock(GenieTechEffectBundleGroup):
    """
    Unlocks units and buildings for an Age, sometimes with additional
    requirements like (266 - Castle built).

    This will become one or more patches for an AgeUpgrade Tech. If the unlock
    is civ-specific, two patches (one for the age, one for the civ)
    will be created.
    """

    def __init__(self, tech_id, unit_type, line_id, full_data_set):
        """
        Creates a new Genie tech group object.

        :param tech_id: The internal tech_id from the .dat file.
        :param unit_type: Type of the unit (unit=70,building=80).
        :param line_id: The unit line that is unlocked.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """

        super().__init__(tech_id, full_data_set)

        self.unit_type = unit_type
        self.line_id = line_id


class CivBonus(GenieTechEffectBundleGroup):
    """
    Gives one specific civilization a bonus. Not the team bonus
    because that's not a Tech in Genie.

    This will become patches in the Civilization API object.
    """

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
