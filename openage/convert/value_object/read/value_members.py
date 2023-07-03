# Copyright 2019-2023 the openage authors. See copying.md for legal info.
# TODO pylint: disable=C,R,abstract-method

"""
Storage format for values from data file entries.
Data from ReadMembers is supposed to be transferred
to these objects for easier handling during the conversion
process and advanced features like creating diffs.

Quick usage guide on when to use which ValueMember:
    - IntMember, FloatMember, BooleanMember and StringMember: should
      be self explanatory.
    - IDMember: References to other structures in form of identifiers.
                Also useful for flags with more than two options.
    - BitfieldMember: Value is used as a bitfield.
    - ContainerMember: For modelling specific substructures. ContainerMembers
                       can store members with different types. However, the
                       member names must be unique.
                       (e.g. a unit object)
    - ArrayMember: Stores a list of members with uniform type. Can be used
                   when repeating substructures appear in a data file.
                   (e.g. multiple unit objects, list of coordinates)
"""
from __future__ import annotations
import typing


from enum import Enum
from math import isclose
from abc import ABC, abstractmethod

from .dynamic_loader import DynamicLoader


class ValueMember(ABC):
    """
    Stores a value member from a data file.
    """

    __slots__ = ('_name', '_value')

    def __init__(self, name: str):
        self._name = name
        self._value = None

    @property
    def name(self) -> str:
        """
        Returns the name of the member.
        """
        return self._name

    @property
    def value(self) -> typing.Any:
        """
        Returns the value of a member.
        """
        return self._value

    @abstractmethod
    def get_type(self) -> StorageType:
        """
        Returns the type of a member.
        """

    @abstractmethod
    def diff(self, other: ValueMember) -> ValueMember:
        """
        Returns a new member object that contains the diff between
        self's and other's values.

        If they are equal, return a NoDiffMember.
        """

    def __repr__(self):
        return f"{self.__class__.__name__}<{self.name}>"


class IntMember(ValueMember):
    """
    Stores numeric integer values.
    """

    def __init__(self, name: str, value: typing.Union[int, float]):
        super().__init__(name)

        self._value = int(value)

    def get_type(self) -> StorageType:
        return StorageType.INT_MEMBER

    def diff(
        self,
        other: IntMember
    ) -> typing.Union[NoDiffMember, IntMember]:
        if self.get_type() is other.get_type():

            if self.value == other.value:
                return NoDiffMember(self.name, self)

            else:
                diff_value = other.value - self.value

                return IntMember(self.name, diff_value)

        else:
            raise TypeError(
                f"type {type(self)} member cannot be diffed with type {type(other)}")


class FloatMember(ValueMember):
    """
    Stores numeric floating point values.
    """

    def __init__(self, name: str, value: typing.Union[int, float]):
        super().__init__(name)

        self._value = float(value)

    def get_type(self) -> StorageType:
        return StorageType.FLOAT_MEMBER

    def diff(
        self,
        other: FloatMember
    ) -> typing.Union[NoDiffMember, FloatMember]:
        if self.get_type() is other.get_type():
            # Float must have the last 6 digits in common
            if isclose(self.value, other.value, rel_tol=1e-7):
                return NoDiffMember(self.name, self)

            else:
                diff_value = other.value - self.value

                return FloatMember(self.name, diff_value)

        else:
            raise TypeError(
                f"type {type(self)} member cannot be diffed with type {type(other)}")


class BooleanMember(ValueMember):
    """
    Stores boolean values.
    """

    def __init__(self, name: str, value: bool):
        super().__init__(name)

        self._value = bool(value)

    def get_type(self) -> StorageType:
        return StorageType.BOOLEAN_MEMBER

    def diff(
        self,
        other: BooleanMember
    ) -> typing.Union[NoDiffMember, BooleanMember]:
        if self.get_type() is other.get_type():
            if self.value == other.value:
                return NoDiffMember(self.name, self)

            else:
                return BooleanMember(self.name, other.value)

        else:
            raise TypeError(
                f"type {type(self)} member cannot be diffed with type {type(other)}")


class IDMember(ValueMember):
    """
    Stores references to media/resource IDs.
    """

    def __init__(self, name: str, value: int):
        super().__init__(name)

        self._value = int(value)

    def get_type(self) -> StorageType:
        return StorageType.ID_MEMBER

    def diff(
        self,
        other: IDMember
    ) -> typing.Union[NoDiffMember, IDMember]:
        if self.get_type() is other.get_type():
            if self.value == other.value:
                return NoDiffMember(self.name, self)

            else:
                return IDMember(self.name, other.value)

        else:
            raise TypeError(
                f"type {type(self)} member cannot be diffed with type {type(other)}")


class BitfieldMember(ValueMember):
    """
    Stores bit field members.
    """

    def __init__(self, name: str, value: int):
        super().__init__(name)

        self._value = value

    def get_value_at_pos(self, pos: int) -> bool:
        """
        Return the boolean value stored at a specific position
        in the bitfield.

        :param pos: Position in the bitfield, starting with the least significant bit.
        :type pos: int
        """
        return bool(self.value & (2 ** pos))

    def get_type(self) -> StorageType:
        return StorageType.BITFIELD_MEMBER

    def diff(
        self,
        other: BitfieldMember
    ) -> typing.Union[NoDiffMember, BitfieldMember]:
        """
        Uses XOR to determine which bits are different in 'other'.
        """
        if self.get_type() is other.get_type():
            if self.value == other.value:
                return NoDiffMember(self.name, self)

            else:
                difference = self.value ^ other.value
                return BitfieldMember(self.name, difference)

        else:
            raise TypeError(
                f"type {type(self)} member cannot be diffed with type {type(other)}")

    def __len__(self):
        return len(self.value)


class StringMember(ValueMember):
    """
    Stores string values.
    """

    def __init__(self, name: str, value: StringMember):
        super().__init__(name)

        self._value = str(value)

    def get_type(self) -> StorageType:
        return StorageType.STRING_MEMBER

    def diff(
        self,
        other: StringMember
    ) -> typing.Union[NoDiffMember, StringMember]:
        if self.get_type() is other.get_type():
            if self.value == other.value:
                return NoDiffMember(self.name, self)

            else:
                return StringMember(self.name, other.value)

        else:
            raise TypeError(
                f"type {type(self)} member cannot be diffed with type {type(other)}")

    def __len__(self):
        return len(self.value)


class ContainerMember(ValueMember):
    """
    Stores multiple members as key-value pairs.

    The name of the members are the keys, the member objects
    are the value of the dict.
    """

    def __init__(
        self,
        name: str,
        submembers: list[typing.Union[IntMember,
                                      FloatMember,
                                      BooleanMember,
                                      IDMember,
                                      BitfieldMember,
                                      StringMember,
                                      ArrayMember,
                                      ContainerMember]]
    ):
        """
        :param submembers: Stored members as a list or dict
        :type submembers: list, dict
        """
        super().__init__(name)

        self._value = {}

        if isinstance(submembers, (dict, DynamicLoader)):
            # submembers is a list or loads dynamically
            self._value = submembers

        else:
            # submembers is a list of members
            self._create_dict(submembers)

    def get_type(self) -> StorageType:
        return StorageType.CONTAINER_MEMBER

    def diff(
        self,
        other: ContainerMember
    ) -> typing.Union[NoDiffMember, ContainerMember]:
        if self.get_type() is other.get_type():
            diff_dict = {}

            other_dict = other.value

            for key in self.value.keys():
                if key in other.value.keys():
                    diff_value = self.value[key].diff(other_dict[key])

                else:
                    # Key is missing in other dict
                    diff_value = RightMissingMember(key, self.value[key])

                diff_dict.update({key: diff_value})

            for key in other.value.keys():
                if key not in self.value.keys():
                    # Key is missing in this dict
                    diff_value = LeftMissingMember(key, other_dict[key])
                    diff_dict.update({key: diff_value})

            if all(isinstance(member, NoDiffMember) for member in diff_dict.values()):
                return NoDiffMember(self.name, self)

            return ContainerMember(self.name, diff_dict)

        else:
            raise TypeError(
                f"type {type(self)} member cannot be diffed with type {type(other)}")

    def _create_dict(self, member_list: list[typing.Union[IntMember,
                                                          FloatMember,
                                                          BooleanMember,
                                                          IDMember,
                                                          BitfieldMember,
                                                          StringMember,
                                                          ArrayMember,
                                                          ContainerMember]]) -> None:
        """
        Creates the dict from the member list passed to __init__.
        """
        for member in member_list:
            self._value.update({member.name: member})

    def __getitem__(self, key):
        """
        Short command for getting a member in the container.
        """
        return self.value[key]

    def __len__(self):
        return len(self.value)


class ArrayMember(ValueMember):
    """
    Stores an ordered list of members with the same type.
    """

    __slots__ = ('_allowed_member_type')

    def __init__(
        self,
        name: str,
        allowed_member_type: StorageType,
        members: list[typing.Union[IntMember,
                                   FloatMember,
                                   BooleanMember,
                                   IDMember,
                                   BitfieldMember,
                                   StringMember,
                                   ArrayMember,
                                   ContainerMember]]
    ):
        super().__init__(name)

        self._value = members

        self._allowed_member_type = allowed_member_type

        # Check if members have correct type
        for member in members:
            if not isinstance(member, (NoDiffMember, LeftMissingMember, RightMissingMember)):
                if member.get_type() is not self._allowed_member_type:
                    raise TypeError("%s has type %s, but this ArrayMember only allows %s"
                                    % (member, member.get_type(), allowed_member_type))

    def get_type(self) -> StorageType:
        if self._allowed_member_type is StorageType.INT_MEMBER:
            return StorageType.ARRAY_INT

        elif self._allowed_member_type is StorageType.FLOAT_MEMBER:
            return StorageType.ARRAY_FLOAT

        elif self._allowed_member_type is StorageType.BOOLEAN_MEMBER:
            return StorageType.ARRAY_BOOL

        elif self._allowed_member_type is StorageType.ID_MEMBER:
            return StorageType.ARRAY_ID

        elif self._allowed_member_type is StorageType.BITFIELD_MEMBER:
            return StorageType.ARRAY_BITFIELD

        elif self._allowed_member_type is StorageType.STRING_MEMBER:
            return StorageType.ARRAY_STRING

        elif self._allowed_member_type is StorageType.CONTAINER_MEMBER:
            return StorageType.ARRAY_CONTAINER

        raise TypeError(f"{self} has no valid member type")

    def get_container(
        self,
        key_member_name: str,
        force_not_found: bool = False,
        force_duplicate: bool = False
    ) -> ContainerMember:
        """
        Returns a ContainerMember generated from an array with type ARRAY_CONTAINER.
        It uses the values of the members with the specified name as keys.
        By default, this method raises an exception if a member with this
        name does not exist or the same key is used twice.

        :param key_member_name: A member in the containers whos value is used as the key.
        :type key_member_name: str
        :param force_not_found: Do not raise an exception if the member is not found.
        :type force_not_found: bool
        :param force_duplicate: Do not raise an exception if the same key value is used twice.
        :type force_duplicate: bool
        """
        if self.get_type() is not StorageType.ARRAY_CONTAINER:
            raise TypeError("%s: Container can only be generated from arrays with"
                            " type 'contarray', not %s"
                            % (self, self.get_type()))

        member_dict = {}
        for container in self.value:
            if key_member_name not in container.value.keys():
                if force_not_found:
                    continue

                raise KeyError("%s: Container %s has no member called %s"
                               % (self, container, key_member_name))

            key_member_value = container[key_member_name].value

            if key_member_value in member_dict.keys():
                if force_duplicate:
                    continue

                raise KeyError("%s: Duplicate key %s for container member %s"
                               % (self, key_member_value, key_member_name))

            member_dict.update({key_member_value: container})

        return ContainerMember(self.name, member_dict)

    def diff(
        self,
        other: ArrayMember
    ) -> typing.Union[NoDiffMember, ArrayMember]:
        if self.get_type() == other.get_type():
            diff_list = []
            other_list = other.value

            index = 0
            if len(self) <= len(other):
                while index < len(self):
                    diff_value = self.value[index].diff(other_list[index])
                    diff_list.append(diff_value)
                    index += 1

                while index < len(other):
                    diff_value = other_list[index]
                    diff_list.append(LeftMissingMember(diff_value.name, diff_value))
                    index += 1

            else:
                while index < len(other):
                    diff_value = self.value[index].diff(other_list[index])
                    diff_list.append(diff_value)
                    index += 1

                while index < len(self):
                    diff_value = self.value[index]
                    diff_list.append(RightMissingMember(diff_value.name, diff_value))
                    index += 1

            if all(isinstance(member, NoDiffMember) for member in diff_list):
                return NoDiffMember(self.name, self)

            return ArrayMember(self.name, self._allowed_member_type, diff_list)

        else:
            raise TypeError(
                f"type {type(self)} member cannot be diffed with type {type(other)}")

    def __getitem__(self, key):
        """
        Short command for getting a member in the array.
        """
        return self.value[key]

    def __len__(self):
        return len(self.value)


class NoDiffMember(ValueMember):
    """
    Is returned when no difference between two members is found.
    """

    def __init__(self, name: str, value: ValueMember):
        """
        :param value: Reference to the one of the diffed members.
        :type value: ValueMember
        """
        super().__init__(name)

        self._value = value

    @property
    def ref(self) -> ValueMember:
        """
        Returns the reference to the diffed object.
        """
        return self._value

    @property
    def value(self) -> typing.Any:
        """
        Returns the value of a member.
        """
        raise NotImplementedError(
            f"{type(self)} cannot have values; use 'ref' instead")

    def get_type(self) -> typing.NoReturn:
        """
        Returns the type of a member.
        """
        raise NotImplementedError(
            f"{type(self)} cannot have a type")

    def diff(self, other: typing.Any) -> typing.NoReturn:
        """
        Returns a new member object that contains the diff between
        self's and other's values.

        If they are equal, return a NoDiffMember.
        """
        raise NotImplementedError(
            f"{type(self)} cannot be diffed")


class LeftMissingMember(ValueMember):
    """
    Is returned when an array or container on the left side of
    the comparison has no member to compare. It stores the right
    side member as value.
    """

    def __init__(self, name: str, value: ValueMember):
        """
        :param value: Reference to the right member's object.
        :type value: ValueMember
        """
        super().__init__(name)

        self._value = value

    @property
    def ref(self) -> ValueMember:
        """
        Returns the reference to the diffed object.
        """
        return self._value

    @property
    def value(self) -> typing.Any:
        """
        Returns the value of a member.
        """
        raise NotImplementedError(
            f"{type(self)} cannot have values; use 'ref' instead")

    def get_type(self) -> typing.NoReturn:
        """
        Returns the type of a member.
        """
        raise NotImplementedError(
            f"{type(self)} cannot have a type")

    def diff(self, other: typing.Any) -> typing.NoReturn:
        """
        Returns a new member object that contains the diff between
        self's and other's values.

        If they are equal, return a NoDiffMember.
        """
        raise NotImplementedError(
            f"{type(self)} cannot be diffed")


class RightMissingMember(ValueMember):
    """
    Is returned when an array or container on the right side of
    the comparison has no member to compare. It stores the left
    side member as value.
    """

    def __init__(self, name: str, value: ValueMember):
        """
        :param value: Reference to the left member's object.
        :type value: ValueMember
        """
        super().__init__(name)

        self._value = value

    @property
    def ref(self) -> ValueMember:
        """
        Returns the reference to the diffed object.
        """
        return self._value

    @property
    def value(self) -> typing.Any:
        """
        Returns the value of a member.
        """
        raise NotImplementedError(
            f"{type(self)} cannot have values; use 'ref' instead")

    def get_type(self) -> typing.NoReturn:
        """
        Returns the type of a member.
        """
        raise NotImplementedError(
            f"{type(self)} cannot have a type")

    def diff(self, other: typing.Any) -> typing.NoReturn:
        """
        Returns a new member object that contains the diff between
        self's and other's values.

        If they are equal, return a NoDiffMember.
        """
        raise NotImplementedError(
            f"{type(self)} cannot be diffed")


class StorageType(Enum):
    """
    Types for values members.
    """

    INT_MEMBER       = "int"
    FLOAT_MEMBER     = "float"
    BOOLEAN_MEMBER   = "boolean"
    ID_MEMBER        = "id"
    BITFIELD_MEMBER  = "bitfield"
    STRING_MEMBER    = "string"
    CONTAINER_MEMBER = "container"

    # Array types                       # array of:
    ARRAY_INT        = "intarray"       # IntegerMembers
    ARRAY_FLOAT      = "floatarray"     # FloatMembers
    ARRAY_BOOL       = "boolarray"      # BooleanMembers
    ARRAY_ID         = "idarray"        # IDMembers
    ARRAY_BITFIELD   = "bitfieldarray"  # BitfieldMembers
    ARRAY_STRING     = "stringarray"    # StringMembers
    ARRAY_CONTAINER  = "contarray"      # ContainerMembers
