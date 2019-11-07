# Copyright 2019-2019 the openage authors. See copying.md for legal info.

from ...dataformat.converter_object import ConverterObject,\
    ConverterObjectGroup


class GenieCivilizationObject(ConverterObject):
    """
    Civilization in AoE2.
    """

    def __init__(self, civ_id, full_data_set, members=None):
        """
        Creates a new Genie civilization object.

        :param civ_id: The index of the civilization in the .dat file's civilization
                       block. (the index is referenced as civilization_id by techs)
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :param members: An already existing member dict.
        """

        super().__init__(civ_id, members=members)

        self.data = full_data_set


class GenieCivilizationGroup(ConverterObjectGroup):
    """
    All necessary civiization data.

    This will become a Civilization API object.
    """

    def __init__(self, civ_id, full_data_set):
        """
        Creates a new Genie civ group line.

        :param civ_id: The index of the civilization in the .dat file's civilization
                       block. (the index is referenced as civ_id by techs)
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """

        super().__init__(civ_id)

        # Reference to everything else in the gamedata
        self.data = full_data_set

        self.civ = self.data.genie_civs[civ_id]

        team_bonus_id = self.civ.get_member("team_bonus_id").get_value()
        if civ_id == 0:
            # Gaia civ has no team bonus
            self.team_bonus = None
        else:
            self.team_bonus = self.data.genie_effect_bundles[team_bonus_id]

        tech_tree_id = self.civ.get_member("tech_tree_id").get_value()
        self.disabled_techs = self.data.genie_effect_bundles[tech_tree_id]
