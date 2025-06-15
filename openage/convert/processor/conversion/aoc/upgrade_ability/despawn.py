# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create patches for upgrading the Despwan ability.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.aoc.genie_unit import GenieBuildingLineGroup
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef
from .....value_object.read.value_members import NoDiffMember
from .util import create_animation_patch

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.converter_object import ConverterObject, \
        ConverterObjectGroup
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup


def despawn_ability(
    converter_group: ConverterObjectGroup,
    line: GenieGameEntityGroup,
    container_obj_ref: str,
    diff: ConverterObject = None
) -> list[ForwardRef]:
    """
    Creates a patch for the Despawn ability of a line.

    :param converter_group: Group that gets the patch.
    :param line: Unit/Building line that has the ability.
    :param container_obj_ref: Reference of the raw API object the patch is nested in.
    :param diff: A diff between two ConvertObject instances.
    :returns: The forward references for the generated patches.
    """
    head_unit_id = line.get_head_unit_id()
    tech_id = converter_group.get_id()
    dataset = line.data

    patches = []

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
    tech_lookup_dict = internal_name_lookups.get_tech_lookups(dataset.game_version)

    game_entity_name = name_lookup_dict[head_unit_id][0]

    if diff:
        diff_dead_unit = diff["dead_unit_id"]
        if isinstance(diff_dead_unit, NoDiffMember):
            return patches

        diff_animation_id = dataset.genie_units[diff_dead_unit.value]["idle_graphic0"].value

        # TODO: If the head unit has an invalid -1 graphic, it doesnt get the Animated
        #       property for the ability in the ability subprocessor, so
        #       we can't patch it here.
        #
        #       We have to find a solution for this, e.g. patch in the Animated ability
        #       here or in the ability subprocessor.
        dead_unit_id = line.get_head_unit()["dead_unit_id"].value
        if dead_unit_id == -1:
            return patches

        dead_unit = dataset.genie_units[dead_unit_id]
        dead_unit_animation_id = dead_unit["idle_graphic0"].value
        if dead_unit_animation_id == -1:
            return patches

    else:
        return patches

    patch_target_ref = f"{game_entity_name}.Despawn"
    nyan_patch_name = f"Change{game_entity_name}Despawn"

    # Nyan patch
    wrapper, anim_patch_forward_ref = create_animation_patch(
        converter_group,
        line,
        patch_target_ref,
        nyan_patch_name,
        container_obj_ref,
        "Despawn",
        "despawn_",
        [diff_animation_id]
    )
    patches.append(anim_patch_forward_ref)

    if isinstance(line, GenieBuildingLineGroup):
        # Store building upgrades next to their game entity definition,
        # not in the Age up techs.
        wrapper.set_location(("data/game_entity/generic/"
                              f"{name_lookup_dict[head_unit_id][1]}/"))
        wrapper.set_filename(f"{tech_lookup_dict[tech_id][1]}_upgrade")

    return patches
