# Copyright 2019-2019 the openage authors. See copying.md for legal info.


from openage.convert.dataformat.converter_object import ConverterObject


class GenieAgeConnection(ConverterObject):
    """
    A relation between an Age and buildings/techs/units in AoE.
    """

    def __init__(self, age_id, full_data_set):
        """
        Creates a new Genie age connection.

        :param age_id: The index of the Age. (First Age = 0)
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """

        super().__init__(age_id)

        self.data = full_data_set
        self.data.age_connections.update({self.get_id(): self})


class GenieBuildingConnection(ConverterObject):
    """
    A relation between a building and other buildings/techs/units in AoE.
    """

    def __init__(self, building_id, full_data_set):
        """
        Creates a new Genie building connection.

        :param building_id: The id of the building from the .dat file.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """

        super().__init__(building_id)

        self.data = full_data_set
        self.data.building_connections.update({self.get_id(): self})


class GenieTechConnection(ConverterObject):
    """
    A relation between a tech and other buildings/techs/units in AoE.
    """

    def __init__(self, tech_id, full_data_set):
        """
        Creates a new Genie tech connection.

        :param tech_id: The id of the tech from the .dat file.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """

        super().__init__(tech_id)

        self.data = full_data_set
        self.data.tech_connections.update({self.get_id(): self})


class GenieUnitConnection(ConverterObject):
    """
    A relation between a unit and other buildings/techs/units in AoE.
    """

    def __init__(self, unit_id, full_data_set):
        """
        Creates a new Genie unit connection.

        :param unit_id: The id of the unit from the .dat file.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """

        super().__init__(unit_id)

        self.data = full_data_set
        self.data.unit_connections.update({self.get_id(): self})
