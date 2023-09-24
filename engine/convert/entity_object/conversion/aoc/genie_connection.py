# Copyright 2019-2022 the openage authors. See copying.md for legal info.

"""
Contains structures and API-like objects for connections from AoC.
"""

from __future__ import annotations
import typing


from ..converter_object import ConverterObject

if typing.TYPE_CHECKING:
    from engine.convert.entity_object.conversion.aoc.genie_object_container\
        import GenieObjectContainer
    from engine.convert.value_object.read.value_members import ValueMember


class GenieAgeConnection(ConverterObject):
    """
    A relation between an Age and buildings/techs/units in AoE.
    """

    __slots__ = ('data',)

    def __init__(
        self,
        age_id: int,
        full_data_set: GenieObjectContainer,
        members: dict[str, ValueMember] = None
    ):
        """
        Creates a new Genie age connection.

        :param age_id: The index of the Age. (First Age = 0)
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :param members: An already existing member dict.
        """

        super().__init__(age_id, members=members)

        self.data = full_data_set

    def __repr__(self):
        return f"GenieAgeConnection<{self.get_id()}>"


class GenieBuildingConnection(ConverterObject):
    """
    A relation between a building and other buildings/techs/units in AoE.
    """

    __slots__ = ('data',)

    def __init__(
        self,
        building_id: int,
        full_data_set: GenieObjectContainer,
        members: dict[str, ValueMember] = None
    ):
        """
        Creates a new Genie building connection.

        :param building_id: The id of the building from the .dat file.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :param members: An already existing member dict.
        """

        super().__init__(building_id, members=members)

        self.data = full_data_set

    def __repr__(self):
        return f"GenieBuildingConnection<{self.get_id()}>"


class GenieTechConnection(ConverterObject):
    """
    A relation between a tech and other buildings/techs/units in AoE.
    """

    __slots__ = ('data',)

    def __init__(
        self,
        tech_id: int,
        full_data_set: GenieObjectContainer,
        members: dict[str, ValueMember] = None
    ):
        """
        Creates a new Genie tech connection.

        :param tech_id: The id of the tech from the .dat file.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :param members: An already existing member dict.
        """

        super().__init__(tech_id, members=members)

        self.data = full_data_set

    def __repr__(self):
        return f"GenieTechConnection<{self.get_id()}>"


class GenieUnitConnection(ConverterObject):
    """
    A relation between a unit and other buildings/techs/units in AoE.
    """

    __slots__ = ('data',)

    def __init__(
        self,
        unit_id: int,
        full_data_set: GenieObjectContainer,
        members: dict[str, ValueMember] = None
    ):
        """
        Creates a new Genie unit connection.

        :param unit_id: The id of the unit from the .dat file.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :param members: An already existing member dict.
        """

        super().__init__(unit_id, members=members)

        self.data = full_data_set

    def __repr__(self):
        return f"GenieUnitConnection<{self.get_id()}>"
