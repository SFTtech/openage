# Copyright 2019-2019 the openage authors. See copying.md for legal info.
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
    - ContainerMember: For modelling specific substructures. ContainerMembers
                       can store members with different types. However, the
                       member names must be unique.
                       (e.g. a unit object)
    - ArrayMember: Stores a list of members with uniform type. Can be used
                   when repeating substructures appear in a data file.
                   (e.g. multiple unit objects, list of coordinates)
"""

from enum import Enum


class ValueMember:
    """
    Stores a value member from a data file.
    """

    def __init__(self, name):
        self.name = name
        self.member_type = None
        self.value = None

    def get_name(self):
        """
        Returns the name of the member.
        """
        return self.name

    def get_value(self):
        """
        Returns the value of a member.
        """
        raise NotImplementedError(
            "%s cannot have values" % (type(self)))

    def get_type(self):
        """
        Returns the type of a member.
        """
        raise NotImplementedError(
            "%s cannot have a type" % (type(self)))

    def diff(self, other):
        """
        Returns a new member object that contains the diff between
        self's and other's values.

        If they are equal, return a NoDiffMember.
        """
        raise NotImplementedError(
            "%s has no diff implemented" % (type(self)))

    def __repr__(self):
        raise NotImplementedError(
            "return short description of the member type %s" % (type(self)))


class IntMember(ValueMember):
    """
    Stores numeric integer values.
    """

    def __init__(self, name, value):
        super().__init__(name)

        self.value = int(value)
        self.member_type = MemberTypes.INT_MEMBER

    def get_value(self):
        return self.value

    def get_type(self):
        return self.member_type

    def diff(self, other):
        if self.get_type() is other.get_type():

            if self.get_value() == other.get_value():
                return NoDiffMember(self.name)

            else:
                diff_value = self.get_value() - other.get_value()

                return IntMember(self.name, diff_value)

        else:
            raise Exception(
                "type %s member cannot be diffed with type %s" % (type(self), type(other)))

    def __repr__(self):
        return "IntMember<%s>" % (type(self))


class FloatMember(ValueMember):
    """
    Stores numeric floating point values.
    """

    def __init__(self, name, value):
        super().__init__(name)

        self.value = float(value)
        self.member_type = MemberTypes.FLOAT_MEMBER

    def get_value(self):
        return self.value

    def get_type(self):
        return self.member_type

    def diff(self, other):
        if self.get_type() is other.get_type():
            if self.get_value() == other.get_value():
                return NoDiffMember(self.name)

            else:
                diff_value = self.get_value() - other.get_value()

                return FloatMember(self.name, diff_value)

        else:
            raise Exception(
                "type %s member cannot be diffed with type %s" % (type(self), type(other)))

    def __repr__(self):
        return "FloatMember<%s>" % (type(self))


class BooleanMember(ValueMember):
    """
    Stores boolean values.
    """

    def __init__(self, name, value):
        super().__init__(name)

        self.value = bool(value)
        self.member_type = MemberTypes.BOOLEAN_MEMBER

    def get_value(self):
        return self.value

    def get_type(self):
        return self.member_type

    def diff(self, other):
        if self.get_type() is other.get_type():
            if self.get_value() == other.get_value():
                return NoDiffMember(self.name)

            else:
                return BooleanMember(self.name, other.get_value())

        else:
            raise Exception(
                "type %s member cannot be diffed with type %s" % (type(self), type(other)))

    def __repr__(self):
        return "BooleanMember<%s>" % (type(self))


class IDMember(IntMember):
    """
    Stores references to media/resource IDs.
    """

    def __init__(self, name, value):
        super().__init__(name, value)

        self.member_type = MemberTypes.ID_MEMBER

    def diff(self, other):
        if self.get_type() is other.get_type():
            if self.get_value() == other.get_value():
                return NoDiffMember(self.name)

            else:
                return IDMember(self.name, other.get_value())

        else:
            raise Exception(
                "type %s member cannot be diffed with type %s" % (type(self), type(other)))

    def __repr__(self):
        return "IDMember<%s>" % (type(self))


class StringMember(ValueMember):
    """
    Stores string values.
    """

    def __init__(self, name, value):
        super().__init__(name)

        self.value = str(value)
        self.member_type = MemberTypes.STRING_MEMBER

    def get_value(self):
        return self.value

    def get_type(self):
        return self.member_type

    def diff(self, other):
        if self.get_type() is other.get_type():
            if self.get_value() == other.get_value():
                return NoDiffMember(self.name)

            else:
                return StringMember(self.name, other.get_value())

        else:
            raise Exception(
                "type %s member cannot be diffed with type %s" % (type(self), type(other)))

    def __len__(self):
        return len(self.value)

    def __repr__(self):
        return "StringMember<%s>" % (type(self))


class ContainerMember(ValueMember):
    """
    Stores multiple members as key-value pairs.

    The name of the members are the keys, the member objects
    are the value of the dict.
    """

    def __init__(self, name, submembers):
        super().__init__(name)

        self.value = {}
        self.member_type = MemberTypes.CONTAINER_MEMBER

        # submembers is a list of members
        self._create_dict(submembers)

    def get_value(self):
        return self.value

    def get_type(self):
        return self.member_type

    def diff(self, other):
        if self.get_type() is other.get_type():
            if len(self) == len(other):
                diff_list = list()

                # optimization to avoid constant calls to other
                other_dict = other.get_value()

                for key in self.value.keys():
                    diff_value = self.value[key].diff(other_dict[key])

                    diff_list.append(diff_value)

                return ContainerMember(self.name, diff_list)

            else:
                raise Exception(
                    "ContainerMembers must have same length for diff")

        else:
            raise Exception(
                "type %s member cannot be diffed with type %s" % (type(self), type(other)))

    def _create_dict(self, member_list):
        """
        Creates the dict from the member list passed to __init__.
        """
        for member in member_list:
            key = member.get_name()

            self.value.update({key: member})

    def __len__(self):
        return len(self.value)

    def __repr__(self):
        return "ContainerMember<%s>" % (type(self))


class ArrayMember(ValueMember):
    """
    Stores an ordered list of members with the same type.
    """

    def __init__(self, name, allowed_member_type, members):
        super().__init__(name)

        # Check if members have correct type
        for member in members:
            if member.get_type() is not allowed_member_type:
                raise Exception("%s has type %s, but this ArrayMember only allows %s"
                                % (member, member.get_type(), allowed_member_type))

        self.value = members

        if allowed_member_type is MemberTypes.INT_MEMBER:
            self.member_type = MemberTypes.ARRAY_INT
        elif allowed_member_type is MemberTypes.FLOAT_MEMBER:
            self.member_type = MemberTypes.ARRAY_FLOAT
        elif allowed_member_type is MemberTypes.BOOLEAN_MEMBER:
            self.member_type = MemberTypes.ARRAY_BOOL
        elif allowed_member_type is MemberTypes.ID_MEMBER:
            self.member_type = MemberTypes.ARRAY_ID
        elif allowed_member_type is MemberTypes.STRING_MEMBER:
            self.member_type = MemberTypes.ARRAY_STRING
        elif allowed_member_type is MemberTypes.CONTAINER_MEMBER:
            self.member_type = MemberTypes.ARRAY_CONTAINER

    def get_value(self):
        return self.value

    def get_type(self):
        return self.member_type

    def diff(self, other):
        if self.get_type() == other.get_type():
            if len(self) == len(other):

                diff_list = []
                other_list = other.get_value()

                for index in range(len(self)):
                    diff_value = self.value[index].diff(other_list[index])

                    diff_list.append(diff_value)

                return ArrayMember(self.name, diff_list)

            else:
                raise Exception(
                    "ArrayMembers must have same length for diff")

        else:
            raise Exception(
                "type %s member cannot be diffed with type %s" % (type(self), type(other)))

    def __len__(self):
        return len(self.value)

    def __repr__(self):
        return "ArrayMember<%s>" % (type(self))


class NoDiffMember(ValueMember):
    """
    Is returned when no difference between two members is found.
    """

    def __init__(self, name):
        super().__init__(name)

    def __repr__(self):
        return "NoDiffMember<%s>" % (type(self))


class MemberTypes(Enum):
    """
    Types for values members.
    """

    INT_MEMBER       = "int"
    FLOAT_MEMBER     = "float"
    BOOLEAN_MEMBER   = "boolean"
    ID_MEMBER        = "id"
    STRING_MEMBER    = "string"
    CONTAINER_MEMBER = "container"

    # Array types                       # array of:
    ARRAY_INT        = "intarray"       # IntegerMembers
    ARRAY_FLOAT      = "floatarray"     # FloatMembers
    ARRAY_BOOL       = "boolarray"      # BooleanMembers
    ARRAY_ID         = "idarray"        # IDMembers
    ARRAY_STRING     = "stringarray"    # StringMembers
    ARRAY_CONTAINER  = "contarray"      # ContainerMembers
