# Copyright 2026-2026 the openage authors. See copying.md for legal info.
"""
Tests for the nyan structures of the converter.
"""

from openage.testing.testing import assert_raises, assert_value, result

from .nyan_structs import MemberOperator, NyanMember, NyanMemberType


def number_typing():
    """
    int and float operands are strictly separated, like in nyan.
    """
    int_type = NyanMemberType("int")
    float_type = NyanMemberType("float")

    value = NyanMember("a", int_type, 5.0, MemberOperator.ASSIGN).get_value()
    assert_value(type(value), int)
    assert_value(value, 5)
    assert_value(NyanMember("a", int_type, 2.0, MemberOperator.MULTIPLY).get_value(), 2)

    with assert_raises(TypeError):
        result(NyanMember("a", int_type, 1.1, MemberOperator.MULTIPLY))

    with assert_raises(TypeError):
        result(NyanMember("a", int_type, 2, MemberOperator.DIVIDE))

    value = NyanMember("a", float_type, 5, MemberOperator.ASSIGN).get_value()
    assert_value(type(value), float)
    assert_value(NyanMember("a", float_type, 2, MemberOperator.DIVIDE).get_value(), 2.0)

    with assert_raises(ValueError):
        result(NyanMember("a", float_type, 0, MemberOperator.DIVIDE))
