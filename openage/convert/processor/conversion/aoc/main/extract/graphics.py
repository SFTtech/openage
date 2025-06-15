# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Extract graphics from the AoC data.
"""
from __future__ import annotations
import typing

from ......entity_object.conversion.aoc.genie_graphic import GenieGraphic

if typing.TYPE_CHECKING:
    from ......value_object.read.value_members import ArrayMember
    from ......entity_object.conversion.aoc.genie_object_container import GenieObjectContainer


def extract_genie_graphics(gamespec: ArrayMember, full_data_set: GenieObjectContainer) -> None:
    """
    Extract graphic definitions from the game data.

    :param gamespec: Gamedata from empires.dat file.
    :type gamespec: class: ...dataformat.value_members.ArrayMember
    """
    raw_graphics = gamespec[0]["graphics"].value

    for raw_graphic in raw_graphics:
        # Can be ignored if there is no filename associated
        filename = raw_graphic["filename"].value
        if not filename:
            continue

        graphic_id = raw_graphic["graphic_id"].value
        graphic_members = raw_graphic.value

        graphic = GenieGraphic(graphic_id, full_data_set, members=graphic_members)
        slp_id = raw_graphic["slp_id"].value
        if str(slp_id) not in full_data_set.existing_graphics:
            graphic.exists = False

        full_data_set.genie_graphics.update({graphic.get_id(): graphic})

    # Detect subgraphics
    for genie_graphic in full_data_set.genie_graphics.values():
        genie_graphic.detect_subgraphics()
