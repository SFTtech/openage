# Copyright 2019-2019 the openage authors. See copying.md for legal info.

"""
Nyan structs.

Simple implementation to store nyan objects and
members for usage in the converter.
"""

import re

from enum import Enum


class NyanObject:
    """
    Superclass for nyan objects.
    """

    def __init__(self, name, parents=None, members=None,
                 nested_objects=None):
        """
        Initializes the object and does some correctness
        checks, for your convenience.
        """
        self.name = name                        # identifier
        self._parents = parents                 # parent objects
        self._members = members                 # members unique to this object
        self._inherited_members = list()        # members inherited from parents
        self._nested_objects = nested_objects   # nested objects

    def add_nested_object(self, nested_object):
        """
        Adds a nested object to the nyan object.
        """
        if isinstance(nested_object, NyanObject):
            if nested_object not in self._nested_objects:
                self._nested_objects += nested_object

        else:
            raise Exception("nested objects must have <NyanObject> type")

    def get_name(self):
        """
        Returns the name of the object.
        """
        return self.name

    def is_abstract(self):
        """
        Returns True if any member is uninitialized.
        """
        for member in self.get_members():
            if not member.is_initialized():
                return True

        return False

    def is_patch(self):
        """
        Returns True if the object is a NyanPatch.
        """
        return False

    def get_members(self):
        """
        Returns all NyanMembers of the object, excluding members from nested objects.
        """
        return self._members + self._inherited_members

    def get_member_by_name(self, member_name):
        """
        Returns the NyanMember with the specified name or
        None if there is no member with that name.
        
        For inherited members, the notation 'origin_name.member'
        must be used.
        """
        for member in self.get_members():
            if member.get_name() == member_name:
                return member

        return None

    def dump(self):
        """
        Returns the string representation of the object.
        """
        output_str = "%s(" % (self.name)

        # TODO: Implement

        return output_str

    def __setattr__(self, name, value):
        """
        Overwrites the standard __setattr__() function to prevent direct
        access to members or parents. We do this so that we can ensure
        at least some type safety and coherent structure.
        
        NyanMembers and nested objects can still be accessed with class
        functions.
        """
        raise Exception("No direct access allowed. Use "
                        "the class functions instead")

    def __repr__(self):
        return "NyanObject<%s>" % (self.name)


class NyanPatch(NyanObject):
    """
    Superclass for nyan patches.    
    """

    def __init__(self, name, target, parents=None, members=None,
                 nested_objects=None, add_inheritance=None):

        super().__init__(name, parents, members, nested_objects)

        self._target = target                       # patch target
        self._add_inheritance = add_inheritance     # new inheritance

    def is_patch(self):
        """
        Returns True if the object is a nyan patch.
        """
        return True

    def __repr__(self):
        return "NyanPatch<%s<%s>>" % (self.name, self.target.name)


class NyanMember:
    """
    Superclass for all nyan members.
    """

    def __init__(self, name, member_type, member_complex_type=None,
                 value=None, operator=None, override_depth=0, optional=False):
        """
        Initializes the member and does some correctness
        checks, for your convenience.
        """
        self.name = name                                    # identifier

        self._member_type = MemberType(member_type)         # type
        self._set_type = MemberType(member_complex_type)    # set/orderedset type
        self._optional = optional                           # whether the value is allowed
                                                            # to be NYAN_NONE

        self._operator = MemberOperator(operator)           # operator type
        self._override_depth = override_depth               # override depth
        self.value = value                                  # value

        # check for errors in the initilization
        self._sanity_check()

        # Explicit type conversions for values
        if self.value and not self._optional:
            self._type_conversion()

    def get_name(self):
        """
        Returns the name of the object.
        """
        return self.name

    def is_complex(self):
        """
        Returns True if the member is a set or orderedset.
        """
        return self._member_type in (MemberType.SET, MemberType.ORDEREDSET)

    def is_initialized(self):
        """
        Returns True if the member has a value.
        """
        return self.value != None

    def is_inherited(self):
        """
        Returns True if the member is inherited from another object.
        """
        return False

    def dump(self):
        """
        Returns the nyan string representation of the member.
        """
        output_str = "%s" % (self.name)

        if self._optional:
            output_str += " : optional(%s)" % (self._member_type)

        if self.is_initialized():
            output_str += " %s%s %s" % ("@" * self._override_depth,
                                        self._operator.value, self.__str__())

        return output_str

    def _sanity_check(self):
        """
        Check if the member conforms to nyan grammar rules. Also does
        a bunch of type checks because nyan requires type safety.
        """
        # self.name must be a string
        if not isinstance(self.name, str):
            raise Exception("%s: 'name' must be a string" % (self.__repr__()))

        # self.name must conform to nyan grammar rules
        if not (re.fullmatch(r"[a-zA-Z_]", self.name[0]) and
                re.fullmatch(r"[a-zA-Z0-9_]*", self.name[1:])):
            raise Exception("%s: 'name' is not well-formed" % (self.__repr__()))

        if self.is_complex():
            # if the member type is complex, then the set type needs
            # to be initialized
            if not self._set_type:
                raise Exception("%s: '_set_type' is required for complex types"
                                % (self.__repr__()))

            # set types cannot be sets
            if self._set_type in (MemberType.SET, MemberType.ORDEREDSET):
                raise Exception("%s: '_set_type' cannot be complex but is %s"
                                % (self.__repr__(), self._set_type))

        else:
            # if the member is not complex, the set type should be None
            if self._set_type:
                raise Exception("%s: member has '_set_type' but is not complex"
                                % (self.__repr__()))

        # Check if operator type matches with member type
        if self._member_type in (MemberType.INT, MemberType.FLOAT)\
                and not self._operator in (MemberOperator.ASSIGN,
                                           MemberOperator.ADD,
                                           MemberOperator.SUBTRACT,
                                           MemberOperator.MULTIPLY,
                                           MemberOperator.DIVIDE):
            raise Exception("%s: %s is not a valid operator for %s member type"
                            % (self.__repr__(), self._operator.value,
                               self._member_type.value))

        elif self._member_type is MemberType.TEXT\
                and not self._operator in (MemberOperator.ASSIGN,
                                           MemberOperator.ADD):
            raise Exception("%s: %s is not a valid operator for %s member type"
                            % (self.__repr__(), self._operator.value,
                               self._member_type.value))

        elif self._member_type is MemberType.FILE\
                and not self._operator is MemberOperator.ASSIGN:
            raise Exception("%s: %s is not a valid operator for %s member type"
                            % (self.__repr__(), self._operator.value,
                               self._member_type.value))

        elif self._member_type is MemberType.BOOLEAN\
                and not self._operator in (MemberOperator.ASSIGN,
                                           MemberOperator.AND,
                                           MemberOperator.OR):
            raise Exception("%s: %s is not a valid operator for %s member type"
                            % (self.__repr__(), self._operator.value,
                               self._member_type.value))

        elif self._member_type is MemberType.SET\
                and not self._operator in (MemberOperator.ASSIGN,
                                           MemberOperator.ADD,
                                           MemberOperator.SUBTRACT,
                                           MemberOperator.AND,
                                           MemberOperator.OR):
            raise Exception("%s: %s is not a valid operator for %s member type"
                            % (self.__repr__(), self._operator.value,
                               self._member_type.value))

        elif self._member_type is MemberType.ORDEREDSET\
                and not self._operator in (MemberOperator.ASSIGN,
                                           MemberOperator.ADD,
                                           MemberOperator.SUBTRACT,
                                           MemberOperator.AND):
            raise Exception("%s: %s is not a valid operator for %s member type"
                            % (self.__repr__(), self._operator.value,
                               self._member_type.value))

        # override depth must be a non-negative integer
        if not (isinstance(self._override_depth, int) and
                self._override_depth >= 0):
            raise Exception("%s: '_override_depth' must be a non-negative integer"
                            % (self.__repr__()))

        # Member values can only be NYAN_NONE if the member is optional
        if self.value is MemberSpecialValue.NYAN_NONE and not\
                self._optional:
            raise Exception("%s: 'value' is NYAN_NONE but member is not optional"
                            % (self.__repr__()))

        # NYAN_NONE values can only be assigned
        if self.value is MemberSpecialValue.NYAN_NONE and not\
                self._operator is MemberOperator.ASSIGN:
            raise Exception(("%s: 'value' with NYAN_NONE can only have operator type "
                             "MemberOperator.ASSIGN") % (self.__repr__()))

    def _type_conversion(self):
        """
        Explicit type conversion of the member value.

        This lets us convert data fields without worrying about the
        correct types too much, e.g. if a boolean is stored as uint8.
        """
        if self._member_type is MemberType.INT:
            self.value = int(self.value)

        elif self._member_type is MemberType.FLOAT:
            self.value = float(self.value)

        elif self._member_type is MemberType.TEXT:
            self.value = str(self.value)

        elif self._member_type is MemberType.FILE:
            self.value = str(self.value)

        elif self._member_type is MemberType.BOOLEAN:
            self.value = bool(self.value)

        elif self._member_type is MemberType.SET:
            self.value = set(self.value)

        elif self._member_type is MemberType.ORDEREDSET:
            # TODO: Implement proper Orderedset()
            self.value = list(self.value)

    def __setattr__(self, name, value):
        """
        Overwrites the standard __setattr__() function to prevent other values
        than self.value to be changed. Right now, inherited members are copied
        from parents, so changes of all other parameters are NOT reported
        downstream.

        TODO: Implement listeners to do this properly.
        """
        if name == "value":
            self.value = value

            self._type_conversion()

    def __str__(self):
        """
        Returns the nyan string representation of the value.
        """
        if not self.is_initialized():
            print("UNINITIALIZED VALUE %s" % self.__repr__())

        if self._optional and self.value is MemberSpecialValue.NYAN_NONE:
                return MemberSpecialValue.NYAN_NONE.value

        if self._member_type in (MemberType.INT, MemberType.FLOAT,
                                 MemberType.TEXT, MemberType.FILE,
                                 MemberType.BOOLEAN):
            if self._member_type is MemberType.FLOAT:
                return "%sf" % self.value

            return "%s" % self.value

        elif self._member_type in (MemberType.SET, MemberType.ORDEREDSET):
            output_str = ""

            if self._member_type is MemberType.ORDEREDSET:
                output_str += "o"

            output_str += "{"

            for val in self.value:
                output_str += "%s, " % val

            return output_str[:-1] + "}"

        elif self._member_type is MemberType.OBJECT:
            return self.value.name

        else:
            raise Exception("%s has no valid type" % self.__repr__())

    def __repr__(self):
        return "NyanMember<%s: %s>" % (self.name, self._member_type)


class InheritedNyanMember(NyanMember):
    """
    Superclass for all nyan members inherited from other objects.
    
    TODO: Check if value was reassigned/changed
    """

    def __init__(self, name, member_type, origin, set_type=None,
                 value=None, operator=None, override_depth=None, optional=False):

        super().__init__(name, member_type, set_type,
                         value, operator, override_depth, optional)

        self._origin = origin                       # nyan object which
                                                    # defines the member

    def get_name(self):
        """
        Returns the name of the member.
        """
        return "%s.%s" % (self._origin.name, self.name)

    def is_inherited(self):
        """
        Returns True if the member is inherited from another object.
        """
        return True

    def dump(self):
        """
        Returns the string representation of the member.
        """
        return "%s %s%s %s" % (self.name, "@" * self._override_depth,
                                   self._operator.value, self.__str__())

    def __repr__(self):
        return "InheritedNyanMember<%s: %s>" % (self.name, self._member_type)


class MemberType(Enum):
    """
    Symbols for nyan member types.
    """

    # Primitive types
    INT = "int"
    FLOAT = "float"
    TEXT = "text"
    FILE = "file"
    BOOLEAN = "bool"
    OBJECT = "object"

    # Complex types
    SET = "set"
    ORDEREDSET = "orderedset"


class MemberSpecialValue(Enum):
    """
    Symbols for special nyan values.
    """
    # nyan none type
    NYAN_NONE = "None"


class MemberOperator(Enum):
    """
    Symbols for nyan member operators.
    """

    ASSIGN = "="  # assignment
    ADD = "+="  # addition, append, insertion, union
    SUBTRACT = "-="  # subtraction, remove
    MULTIPLY = "*="  # multiplication
    DIVIDE = "/="  # division
    AND = "&="  # logical AND, intersect
    OR = "|="  # logical OR, union
