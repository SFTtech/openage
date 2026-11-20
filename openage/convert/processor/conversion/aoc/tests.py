# Copyright 2026-2026 the openage authors. See copying.md for legal info.
"""
Tests for the AoC conversion processors.
"""

from openage.testing.testing import assert_value

from .....nyan.nyan_structs import MemberOperator
from ....entity_object.conversion.aoc.genie_object_container import GenieObjectContainer
from ....entity_object.conversion.aoc.genie_unit import GenieUnitLineGroup, GenieUnitObject
from ....entity_object.conversion.converter_object import ConverterObjectGroup, RawAPIObject
from ....entity_object.conversion.stringresource import StringResource
from ....service.read.nyan_api_loader import load_api
from ....tool.singlefile import AOC_GAME_VERSION
from ....value_object.read.value_members import IntMember
from .upgrade_attribute_subprocessor import AoCUpgradeAttributeSubprocessor


def hp_float_patches() -> None:
    """
    HP upgrades patch the float attribute settings without rounding.
    """
    api = load_api()
    dataset = GenieObjectContainer(AOC_GAME_VERSION, api, StringResource())

    # civs 1 and 2 are Britons and Franks, entity 4 the Archer
    britons = ConverterObjectGroup(1)
    franks = ConverterObjectGroup(2)
    line = GenieUnitLineGroup(4, dataset)
    line.add_unit(GenieUnitObject(4, dataset, {"id0": IntMember("id0", 4)}))
    for obj_id, name, parent in (
        ("Archer", "Archer", "engine.util.game_entity.GameEntity"),
        ("Archer.Live.Health", "Health", "engine.util.attribute.AttributeSetting"),
    ):
        raw_api_object = RawAPIObject(obj_id, name, api)
        raw_api_object.add_raw_parent(parent)
        line.add_raw_api_object(raw_api_object)

    # one civ per patch because their names only depend on the civ and entity
    multiply = AoCUpgradeAttributeSubprocessor.hp_upgrade(britons, line, 1.1, MemberOperator.MULTIPLY)
    add = AoCUpgradeAttributeSubprocessor.hp_upgrade(franks, line, 10, MemberOperator.ADD)

    for group in (line, britons, franks):
        group.create_nyan_objects()

    for group in (line, britons, franks):
        group.create_nyan_members()

    for patches, operator, value in (
        (multiply, MemberOperator.MULTIPLY, 1.1),
        (add, MemberOperator.ADD, 10.0),
    ):
        (wrapper,) = patches
        patch = wrapper.resolve().get_member_by_name("patch", api["engine.util.patch.Patch"]).get_value()
        for member_name in ("max_value", "starting_value"):
            member = patch.get_member_by_name(member_name)
            assert_value(member.get_operator(), operator)
            assert_value(type(member.get_value()), float)
            assert_value(member.get_value(), value)
