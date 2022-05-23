# Copyright 2019-2022 the openage authors. See copying.md for legal info.

"""
Contains structures and API-like objects for graphics from AoC.
"""
from __future__ import annotations
import typing

from ..converter_object import ConverterObject

if typing.TYPE_CHECKING:
    from openage.convert.entity_object.conversion.aoc.genie_object_container\
        import GenieObjectContainer
    from openage.convert.value_object.read.value_members import ValueMember


class GenieGraphic(ConverterObject):
    """
    Graphic definition from a .dat file.
    """

    __slots__ = ('exists', 'subgraphics', '_refs', 'data')

    def __init__(
        self,
        graphic_id: int,
        full_data_set: GenieObjectContainer,
        members: dict[str, ValueMember] = None
    ):
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

        # Should be set to False if no graphics file exists for it
        self.exists = True

        # Direct subgraphics (deltas) of this graphic
        self.subgraphics: list[GenieGraphic] = []

        # Other graphics that have this graphic as a subgraphic (delta)
        self._refs: list[GenieGraphic] = []

    def add_reference(self, referer: GenieGraphic) -> None:
        """
        Add another graphic that is referencing this sprite.
        """
        self._refs.append(referer)

    def detect_subgraphics(self) -> None:
        """
        Add references for the direct subgraphics to this object.
        """
        graphic_deltas = self["graphic_deltas"].value

        for subgraphic in graphic_deltas:
            graphic_id = subgraphic["graphic_id"].value

            # Ignore invalid IDs
            if graphic_id not in self.data.genie_graphics.keys():
                continue

            graphic = self.data.genie_graphics[graphic_id]

            self.subgraphics.append(graphic)
            graphic.add_reference(self)

    def get_animation_length(self) -> float:
        """
        Returns the time taken to display all frames in this graphic.
        """
        head_graphic = self.data.genie_graphics[self.get_id()]
        return head_graphic["frame_rate"].value * head_graphic["frame_count"].value

    def get_subgraphics(self) -> list[GenieGraphic]:
        """
        Return the subgraphics of this graphic
        """
        return self.subgraphics

    def get_frame_rate(self) -> float:
        """
        Returns the time taken to display a single frame in this graphic.
        """
        head_graphic = self.data.genie_graphics[self.get_id()]
        return head_graphic["frame_rate"].value

    def is_shared(self) -> bool:
        """
        Return True if the number of references to this graphic is >1.
        """
        return len(self._refs) > 1

    def __repr__(self):
        return f"GenieGraphic<{self.get_id()}>"
