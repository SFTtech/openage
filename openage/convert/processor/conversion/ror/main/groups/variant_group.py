# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create variant groups from genie units.
"""
from __future__ import annotations
import typing

from ......entity_object.conversion.ror.genie_unit import RoRVariantGroup
from ......value_object.conversion.ror.internal_nyan_names import VARIANT_GROUP_LOOKUPS

if typing.TYPE_CHECKING:
    from ......entity_object.conversion.aoc.genie_object_container import GenieObjectContainer


def create_variant_groups(full_data_set: GenieObjectContainer) -> None:
    """
    Create variant groups.

    :param full_data_set: GenieObjectContainer instance that
                            contains all relevant data for the conversion
                            process.
    """
    variants = VARIANT_GROUP_LOOKUPS

    for group_id, variant in variants.items():
        variant_group = RoRVariantGroup(group_id, full_data_set)
        full_data_set.variant_groups.update({variant_group.get_id(): variant_group})

        for variant_id in variant[2]:
            variant_group.add_unit(full_data_set.genie_units[variant_id])
            full_data_set.unit_ref.update({variant_id: variant_group})
