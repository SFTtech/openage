# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create ambient groups from genie units.
"""
from __future__ import annotations
import typing

import openage.convert.value_object.conversion.aoc.internal_nyan_names as aoc_internal
import openage.convert.value_object.conversion.de2.internal_nyan_names as de2_internal

from .......util.ordered_set import OrderedSet
from ......entity_object.conversion.aoc.genie_unit import GenieAmbientGroup

if typing.TYPE_CHECKING:
    from ......entity_object.conversion.aoc.genie_object_container import GenieObjectContainer


def create_ambient_groups(full_data_set: GenieObjectContainer) -> None:
    """
    Create ambient groups, mostly for resources and scenery.

    :param full_data_set: GenieObjectContainer instance that
                            contains all relevant data for the conversion
                            process.
    """
    ambient_ids = OrderedSet()
    ambient_ids.update(aoc_internal.AMBIENT_GROUP_LOOKUPS.keys())
    ambient_ids.update(de2_internal.AMBIENT_GROUP_LOOKUPS.keys())
    genie_units = full_data_set.genie_units

    for ambient_id in ambient_ids:
        ambient_group = GenieAmbientGroup(ambient_id, full_data_set)
        ambient_group.add_unit(genie_units[ambient_id])
        full_data_set.ambient_groups.update({ambient_group.get_id(): ambient_group})
        full_data_set.unit_ref.update({ambient_id: ambient_group})
