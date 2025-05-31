# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Link trade posts to the building lines that they trade with.
"""
from __future__ import annotations
import typing

if typing.TYPE_CHECKING:
    from ......entity_object.conversion.aoc.genie_object_container import GenieObjectContainer


def link_trade_posts(full_data_set: GenieObjectContainer) -> None:
    """
    Link a trade post building to the lines that it trades with.

    :param full_data_set: GenieObjectContainer instance that
                            contains all relevant data for the conversion
                            process.
    :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
    """
    unit_lines = full_data_set.unit_lines.values()

    for unit_line in unit_lines:
        if unit_line.has_command(111):
            head_unit = unit_line.get_head_unit()
            unit_commands = head_unit["unit_commands"].value
            trade_post_id = -1
            for command in unit_commands:
                # Find the trade command and the trade post id
                type_id = command["type"].value

                if type_id != 111:
                    continue

                trade_post_id = command["unit_id"].value

                # Notify buiding
                if trade_post_id in full_data_set.building_lines.keys():
                    full_data_set.building_lines[trade_post_id].add_trading_line(unit_line)
