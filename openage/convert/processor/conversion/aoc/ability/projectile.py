# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for the Projectile ability.
"""
from __future__ import annotations
import typing

from math import degrees

from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup
    from .....value_object.conversion.forward_ref import ForwardRef


def projectile_ability(line: GenieGameEntityGroup, position: int = 0) -> ForwardRef:
    """
    Adds a Projectile ability to projectiles in a line. Which projectile should
    be added is determined by the 'position' argument.

    :param line: Unit/Building line that gets the ability.
    :param position: Index of the projectile to add (0 or 1 for AoC).
    :returns: The forward reference for the ability.
    """
    current_unit = line.get_head_unit()
    current_unit_id = line.get_head_unit_id()
    dataset = line.data

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

    game_entity_name = name_lookup_dict[current_unit_id][0]

    # First projectile is mandatory
    obj_ref = f"{game_entity_name}.ShootProjectile.Projectile{str(position)}"
    ability_ref = f"{game_entity_name}.ShootProjectile.Projectile{position}.Projectile"
    ability_raw_api_object = RawAPIObject(ability_ref,
                                          "Projectile",
                                          dataset.nyan_api_objects)
    ability_raw_api_object.add_raw_parent("engine.ability.type.Projectile")
    ability_location = ForwardRef(line, obj_ref)
    ability_raw_api_object.set_location(ability_location)

    # Arc
    if position == 0:
        projectile_id = current_unit["projectile_id0"].value

    elif position == 1:
        projectile_id = current_unit["projectile_id1"].value

    else:
        raise ValueError(f"Invalid projectile position {position}")

    projectile = dataset.genie_units[projectile_id]
    arc = degrees(projectile["projectile_arc"].value)
    ability_raw_api_object.add_raw_member("arc",
                                          arc,
                                          "engine.ability.type.Projectile")

    # Accuracy
    accuracy_name = (f"{game_entity_name}.ShootProjectile."
                     f"Projectile{position}.Projectile.Accuracy")
    accuracy_raw_api_object = RawAPIObject(accuracy_name,
                                           "Accuracy",
                                           dataset.nyan_api_objects)
    accuracy_raw_api_object.add_raw_parent("engine.util.accuracy.Accuracy")
    accuracy_location = ForwardRef(line, ability_ref)
    accuracy_raw_api_object.set_location(accuracy_location)

    accuracy_value = current_unit["accuracy"].value
    accuracy_raw_api_object.add_raw_member("accuracy",
                                           accuracy_value,
                                           "engine.util.accuracy.Accuracy")

    accuracy_dispersion = current_unit["accuracy_dispersion"].value
    accuracy_raw_api_object.add_raw_member("accuracy_dispersion",
                                           accuracy_dispersion,
                                           "engine.util.accuracy.Accuracy")
    dropoff_type = dataset.nyan_api_objects["engine.util.dropoff_type.type.InverseLinear"]
    accuracy_raw_api_object.add_raw_member("dispersion_dropoff",
                                           dropoff_type,
                                           "engine.util.accuracy.Accuracy")

    allowed_types = [
        dataset.pregen_nyan_objects["util.game_entity_type.types.Building"].get_nyan_object(),
        dataset.pregen_nyan_objects["util.game_entity_type.types.Unit"].get_nyan_object()
    ]
    accuracy_raw_api_object.add_raw_member("target_types",
                                           allowed_types,
                                           "engine.util.accuracy.Accuracy")
    accuracy_raw_api_object.add_raw_member("blacklisted_entities",
                                           [],
                                           "engine.util.accuracy.Accuracy")

    line.add_raw_api_object(accuracy_raw_api_object)
    accuracy_forward_ref = ForwardRef(line, accuracy_name)
    ability_raw_api_object.add_raw_member("accuracy",
                                          [accuracy_forward_ref],
                                          "engine.ability.type.Projectile")

    # Target mode
    target_mode = dataset.nyan_api_objects["engine.util.target_mode.type.CurrentPosition"]
    ability_raw_api_object.add_raw_member("target_mode",
                                          target_mode,
                                          "engine.ability.type.Projectile")

    # Ingore types; buildings are ignored unless targeted
    ignore_forward_refs = [
        dataset.pregen_nyan_objects["util.game_entity_type.types.Building"].get_nyan_object()
    ]
    ability_raw_api_object.add_raw_member("ignored_types",
                                          ignore_forward_refs,
                                          "engine.ability.type.Projectile")
    ability_raw_api_object.add_raw_member("unignored_entities",
                                          [],
                                          "engine.ability.type.Projectile")

    line.add_raw_api_object(ability_raw_api_object)

    ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

    return ability_forward_ref
