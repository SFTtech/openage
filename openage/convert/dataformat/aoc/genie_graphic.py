# Copyright 2019-2020 the openage authors. See copying.md for legal info.

from ...dataformat.converter_object import ConverterObject


class GenieGraphic(ConverterObject):
    """
    Graphic definition from a .dat file.
    """

    def __init__(self, graphic_id, full_data_set, members=None):
        """
        Creates a new Genie graphic object.

        :param graphic_id: The graphic id from the .dat file.
        :type graphic_id: int
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.converter_object.ConverterObjectContainer
        :param members: Members belonging to the graphic.
        :type members: dict, optional
        """

        super().__init__(graphic_id, members=members)

        self.data = full_data_set

        # Direct subgraphics (deltas) of this graphic
        self.subgraphics = []

        # Other graphics that have this graphic as a subgraphic (delta)
        self._refs = []

    def add_reference(self, referer):
        """
        Add another graphic that is referencing this sprite.
        """
        self._refs.append(referer)

    def detect_subgraphics(self):
        """
        Add references for the direct subgraphics to this object.
        """
        graphic_deltas = self.get_member("graphic_deltas").get_value()

        for subgraphic in graphic_deltas:
            graphic_id = subgraphic.get_value()["graphic_id"].get_value()

            # Ignore invalid IDs
            if graphic_id < 0:
                continue

            graphic = self.data.genie_graphics[graphic_id]

            self.subgraphics.append(graphic)
            graphic.add_reference(self)

    def get_subgraphics(self):
        """
        Return the subgraphics of this graphic
        """
        return self.subgraphics

    def is_shared(self):
        """
        Return True if the number of references to this graphic is >1.
        """
        return len(self._refs) > 1

    def __repr__(self):
        return "GenieGraphic<%s>" % (self.get_id())
