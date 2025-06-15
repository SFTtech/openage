# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create conditions for AoC.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject, ConverterObjectGroup
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_object_container import GenieObjectContainer

LOGIC_PARENT = "engine.util.logic.LogicElement"
LITERAL_PARENT = "engine.util.logic.literal.Literal"
INTERVAL_PARENT = "engine.util.logic.literal.type.AttributeBelowValue"

SCOPE_PARENT = "engine.util.logic.literal_scope.LiteralScope"
SELF_SCOPE_PARENT = "engine.util.logic.literal_scope.type.Self"


def generate_death_condition(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate DeathCondition objects for unit and building deaths in AoC.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    pregen_nyan_objects = full_data_set.pregen_nyan_objects
    api_objects = full_data_set.nyan_api_objects

    literal_location = "data/util/logic/death/"

    death_ref_in_modpack = "util.logic.literal.death.StandardHealthDeathLiteral"
    literal_raw_api_object = RawAPIObject(death_ref_in_modpack,
                                          "StandardHealthDeathLiteral",
                                          api_objects,
                                          literal_location)
    literal_raw_api_object.set_filename("death")
    literal_raw_api_object.add_raw_parent(INTERVAL_PARENT)

    # Literal will not default to 'True' when it was fulfilled once
    literal_raw_api_object.add_raw_member("only_once", False, LOGIC_PARENT)

    # Scope
    scope_forward_ref = ForwardRef(pregen_converter_group,
                                   "util.logic.literal_scope.death.StandardHealthDeathScope")
    literal_raw_api_object.add_raw_member("scope",
                                          scope_forward_ref,
                                          LITERAL_PARENT)

    # Attribute
    health_forward_ref = ForwardRef(pregen_converter_group,
                                    "util.attribute.types.Health")
    literal_raw_api_object.add_raw_member("attribute",
                                          health_forward_ref,
                                          INTERVAL_PARENT)

    # sidenote: Apparently this is actually HP<1 in Genie
    # (https://youtu.be/FdBk8zGbE7U?t=7m16s)
    literal_raw_api_object.add_raw_member("threshold",
                                          1,
                                          INTERVAL_PARENT)

    pregen_converter_group.add_raw_api_object(literal_raw_api_object)
    pregen_nyan_objects.update({death_ref_in_modpack: literal_raw_api_object})

    # LiteralScope
    death_scope_ref_in_modpack = "util.logic.literal_scope.death.StandardHealthDeathScope"
    scope_raw_api_object = RawAPIObject(death_scope_ref_in_modpack,
                                        "StandardHealthDeathScope",
                                        api_objects)
    scope_location = ForwardRef(pregen_converter_group, death_ref_in_modpack)
    scope_raw_api_object.set_location(scope_location)
    scope_raw_api_object.add_raw_parent(SELF_SCOPE_PARENT)

    scope_diplomatic_stances = [api_objects["engine.util.diplomatic_stance.type.Self"]]
    scope_raw_api_object.add_raw_member("stances",
                                        scope_diplomatic_stances,
                                        SCOPE_PARENT)

    pregen_converter_group.add_raw_api_object(scope_raw_api_object)
    pregen_nyan_objects.update({death_scope_ref_in_modpack: scope_raw_api_object})


def generate_garrison_empty_condition(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate condition objects for emptying garrisoned buildings when they have
    low HP in AoC.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    pregen_nyan_objects = full_data_set.pregen_nyan_objects
    api_objects = full_data_set.nyan_api_objects

    literal_location = "data/util/logic/garrison_empty/"

    garrison_literal_ref_in_modpack = "util.logic.literal.garrison.BuildingDamageEmpty"
    literal_raw_api_object = RawAPIObject(garrison_literal_ref_in_modpack,
                                          "BuildingDamageEmptyLiteral",
                                          api_objects,
                                          literal_location)
    literal_raw_api_object.set_filename("garrison_empty")
    literal_raw_api_object.add_raw_parent(INTERVAL_PARENT)

    # Literal will not default to 'True' when it was fulfilled once
    literal_raw_api_object.add_raw_member("only_once", False, LOGIC_PARENT)

    # Scope
    scope_forward_ref = ForwardRef(pregen_converter_group,
                                   "util.logic.literal_scope.garrison.BuildingDamageEmptyScope")
    literal_raw_api_object.add_raw_member("scope",
                                          scope_forward_ref,
                                          LITERAL_PARENT)

    # Attribute
    health_forward_ref = ForwardRef(pregen_converter_group,
                                    "util.attribute.types.Health")
    literal_raw_api_object.add_raw_member("attribute",
                                          health_forward_ref,
                                          INTERVAL_PARENT)

    # Threshhold
    literal_raw_api_object.add_raw_member("threshold",
                                          0.2,
                                          INTERVAL_PARENT)

    pregen_converter_group.add_raw_api_object(literal_raw_api_object)
    pregen_nyan_objects.update({garrison_literal_ref_in_modpack: literal_raw_api_object})

    # LiteralScope
    garrison_scope_ref_in_modpack = "util.logic.literal_scope.garrison.BuildingDamageEmptyScope"
    scope_raw_api_object = RawAPIObject(garrison_scope_ref_in_modpack,
                                        "BuildingDamageEmptyScope",
                                        api_objects)
    scope_location = ForwardRef(pregen_converter_group, garrison_literal_ref_in_modpack)
    scope_raw_api_object.set_location(scope_location)
    scope_raw_api_object.add_raw_parent(SELF_SCOPE_PARENT)

    scope_diplomatic_stances = [api_objects["engine.util.diplomatic_stance.type.Self"]]
    scope_raw_api_object.add_raw_member("stances",
                                        scope_diplomatic_stances,
                                        SCOPE_PARENT)

    pregen_converter_group.add_raw_api_object(scope_raw_api_object)
    pregen_nyan_objects.update({garrison_scope_ref_in_modpack: scope_raw_api_object})
