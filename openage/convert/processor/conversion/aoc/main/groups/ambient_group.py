# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create ambient groups from genie units.
"""
from __future__ import annotations
import typing

from ......entity_object.conversion.aoc.genie_unit import GenieAmbientGroup
from ......value_object.conversion.aoc.internal_nyan_names import AMBIENT_GROUP_LOOKUPS

if typing.TYPE_CHECKING:
    from ......entity_object.conversion.aoc.genie_object_container import GenieObjectContainer


def create_ambient_groups(full_data_set: GenieObjectContainer) -> None:
    """
    Create ambient groups, mostly for resources and scenery.

    :param full_data_set: GenieObjectContainer instance that
                            contains all relevant data for the conversion
                            process.
    :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
    """
    ambient_ids = AMBIENT_GROUP_LOOKUPS.keys()
    genie_units = full_data_set.genie_units

    for ambient_id in ambient_ids:
        ambient_group = GenieAmbientGroup(ambient_id, full_data_set)
        ambient_group.add_unit(genie_units[ambient_id])
        full_data_set.ambient_groups.update({ambient_group.get_id(): ambient_group})
        full_data_set.unit_ref.update({ambient_id: ambient_group})
