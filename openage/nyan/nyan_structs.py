# Copyright 2019-2019 the openage authors. See copying.md for legal info.

"""
Nyan structs.

Simple implementation to store nyan objects and
members for usage in the converter. This is not
a real nyan^TM implementation, but rather a "dumb"
storage format.

Python does not enforce static types, so be careful
 and only use the provided functions, please. :)
"""

import re

from enum import Enum
from pygments.lexer import inherit

INDENT = "    "


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
        self.name = name                        # object name
        self._fqon = self.name                  # unique identifier (in modpack)

        self._parents = set()                   # parent objects
        self._inherited_members = set()         # members inherited from parents
        if parents:
            self._parents.update(parents)
            self._process_inheritance()

        self._members = set()                   # members unique to this object
        if members:
            self._members.update(members)

        self._nested_objects = set()            # nested objects
        if nested_objects:
            self._nested_objects.update(nested_objects)

        self._sanity_check()

    def add_nested_object(self, nested_object):
        """
        Adds a nested object to the nyan object.
        """
        if not isinstance(nested_object, NyanObject):
            raise Exception("nested object must have <NyanObject> type")

        if nested_object is self:
            raise Exception("nyan object must not contain itself as nested object")

        self._nested_objects.add(nested_object)

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

    def get_fqon(self):
        """
        Returns the fqon of the nyan object.
        """
        return self._fqon

    def get_members(self):
        """
        Returns all NyanMembers of the object, excluding members from nested objects.
        """
        return self._members | self._inherited_members

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

    def get_name(self):
        """
        Returns the name of the object.
        """
        return self.name

    def set_fqon(self, new_fqon):
        """
        Set a new value for the fqon.
        """
        if not isinstance(self.name, str):
            raise Exception("%s: 'new_fqon' must be a string"
                            % (self.__repr__()))

        elif not re.fullmatch(r"[a-zA-Z_][a-zA-Z_]*('.'[a-zA-Z_][a-zA-Z_]*)*",
                              self.name):
            raise Exception("%s: 'new_fqon' must be match [a-zA-Z_.]* grammar"
                            % (self.__repr__()))

        else:
            self._fqon = new_fqon

    def dump(self, indent_depth=0):
        """
        Returns the string representation of the object.
        """
        # Header
        output_str = "%s%s(" % (indent_depth * INDENT,
                                self.get_name())

        if len(self._parents) > 0:
            for parent in self._parents:
                output_str += "%s, " % (parent.get_name())

            output_str = output_str[:-2]

        output_str += "):\n"

        empty = True

        # Members
        if len(self._inherited_members) > 0:
            for inherited_member in self._inherited_members:
                if inherited_member.is_initialized():
                    empty = False
                    output_str += "%s%s\n" % ((indent_depth + 1) * INDENT,
                                              inherited_member.dump())
            if empty == False:
                output_str += "\n"

        if len(self._members) > 0:
            empty = False
            for member in self._members:
                output_str += "%s%s\n" % ((indent_depth + 1) * INDENT,
                                          member.dump())

            output_str += "\n"

        # Nested objects
        if len(self._nested_objects) > 0:
            empty = False
            for nested_object in self._nested_objects:
                output_str += "%s%s" % (indent_depth * INDENT,
                                          nested_object.dump(
                                              indent_depth + 1
                                              ))

            output_str += "\n"

        # Epmty objects need a 'pass' line
        if empty:
            output_str += "%spass" % ((indent_depth + 1) * INDENT)

        return output_str

    def _process_inheritance(self):
        """
        Creates inherited members from parent members.
        """
        for parent in self._parents:
            parent_members = parent.get_members()

            for parent_member in parent_members:
                # Copy members without initializing them
                if parent_member.is_inherited():
                    member_name = parent_member.get_name().split(".")[-1]
                    inherited_member = InheritedNyanMember(
                                            member_name,
                                            parent_member.get_member_type(),
                                            parent,
                                            parent_member.get_origin(),
                                            None,
                                            parent_member.get_set_type(),
                                            None,
                                            None,
                                            parent_member.is_optional()
                                            )

                else:
                    inherited_member = InheritedNyanMember(
                                            parent_member.get_name(),
                                            parent_member.get_member_type(),
                                            parent,
                                            parent,
                                            None,
                                            parent_member.get_set_type(),
                                            None,
                                            None,
                                            parent_member.is_optional()
                                            )

                self._inherited_members.add(inherited_member)

    def _sanity_check(self):
        """
        Check if the object conforms to nyan grammar rules. Also does
        a bunch of type checks.
        """
        # self.name must be a string
        if not isinstance(self.name, str):
            raise Exception("%s: 'name' must be a string" % (self.__repr__()))

        # self.name must conform to nyan grammar rules
        if not re.fullmatch(r"[a-zA-Z_][a-zA-Z_]*", self.name):
            raise Exception("%s: 'name' is not well-formed" % (self.__repr__()))

        # self._parents must be NyanObjects
        for parent in self._parents:
            if not isinstance(parent, NyanObject):
                raise Exception("%s: %s must have NyanObject type"
                                % (self.__repr__(), parent.__repr__()))

        # self._members must be NyanMembers
        for member in self._members:
            if not isinstance(member, NyanMember):
                raise Exception("%s: %s must have NyanMember type"
                                % (self.__repr__(), member.__repr__()))

            # a member in self._members must also not be inherited
            if isinstance(member, InheritedNyanMember):
                raise Exception("%s: %s must not have InheritedNyanMember type"
                                % (self.__repr__(), member.__repr__()))

        # self._nested_objects must be NyanObjects
        for nested_object in self._nested_objects:
            if not isinstance(nested_object, NyanObject):
                raise Exception("%s: %s must have NyanObject type"
                                % (self.__repr__(),
                                   nested_object.__repr__()))

            if nested_object is self:
                raise Exception("%s: must not contain itself as nested object"
                                % (self.__repr__()))

    def __iter__(self):
        return self

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

    def get_target(self):
        """
        Returns the target of the patch.
        """
        return self._target

    def is_patch(self):
        """
        Returns True if the object is a nyan patch.
        """
        return True

    def dump(self, indent_depth):
        """
        Returns the string representation of the object.
        """
        # Header
        output_str = "%s%s<%s>" % (indent_depth * INDENT,
                                   self.get_name(),
                                   self.get_target())

        if len(self._add_inheritance) != 0:
            output_str += "["

            for new_inheritance in self._add_inheritance:
                if new_inheritance[0] == "FRONT":
                    output_str += "+%s, " % (new_inheritance.get_name())
                elif new_inheritance[0] == "BACK":
                    output_str += "%s+, " % (new_inheritance.get_name())

            output_str = output_str[:-2] + "]"

        output_str += "("

        for parent in self._parents:
            output_str += "%s, " % (parent.get_name())

        output_str = output_str[:-2] + "):\n"

        # Members
        for inherited_member in self._inherited_members:
            output_str += "%s%s\n" % ((indent_depth + 1) * INDENT,
                                      inherited_member.dump())

        output_str += "\n"

        for member in self._members:
            output_str += "%s%s\n" % ((indent_depth + 1) * INDENT,
                                      member.dump())

        # Nested objects
        for nested_object in self._nested_objects:
            output_str += "%s%s\n" % ((indent_depth + 1) * INDENT,
                                      nested_object.dump())

        output_str += "\n"

        return output_str

    def _sanity_check(self):
        """
        Check if the object conforms to nyan grammar rules. Also does
        a bunch of type checks.
        """
        # Target must be a nyan object
        if not isinstance(self._target, NyanObject):
            raise Exception("%s: '_target' must have NyanObject type"
                            % (self.__repr__()))

        # Added inheritance must be tuples of "FRONT"/"BACK"
        # and a nyan object
        for inherit in self._add_inheritance:
            if not isinstance(inherit, tuple):
                raise Exception("%s: '_add_inheritance' must be a tuple"
                                % (self.__repr__()))

            if len(inherit) != 2:
                raise Exception("%s: '_add_inheritance' must have length 2"
                                % (self.__repr__()))

            if inherit[0] not in ("FRONT", "BACK"):
                raise Exception("%s: added inheritance must be FRONT or BACK mode"
                                % (self.__repr__()))

            if not isinstance(inherit[1], NyanObject):
                raise Exception("%s: added inheritance must contain NyanObject"
                                % (self.__repr__()))

    def __repr__(self):
        return "NyanPatch<%s<%s>>" % (self.name, self.target.name)


class NyanMember:
    """
    Superclass for all nyan members.
    """

    def __init__(self, name, member_type, value=None, set_type=None,
                 operator=None, override_depth=0, optional=False):
        """
        Initializes the member and does some correctness
        checks, for your convenience.
        """
        self.name = name                                # identifier

        if isinstance(member_type, NyanObject):         # type
            self._member_type = member_type
        else:
            self._member_type = MemberType(member_type)

        self._set_type = None                           # set/orderedset type
        if set_type:
            if isinstance(set_type, NyanObject):
                self._set_type = set_type
            else:
                self._set_type = MemberType(set_type)

        self._optional = optional                       # whether the value is allowed
                                                        # to be NYAN_NONE

        self._operator = None
        if operator:
            self._operator = MemberOperator(operator)   # operator type
        self._override_depth = override_depth           # override depth
        self.value = value                              # value

        # check for errors in the initilization
        self._sanity_check()

        # Explicit type conversions for values
        if self.value and not self._optional:
            self._type_conversion()

    def get_name(self):
        """
        Returns the name of the member.
        """
        return self.name

    def get_member_type(self):
        """
        Returns the type of the member.
        """
        return self._member_type

    def get_set_type(self):
        """
        Returns the set type of the member.
        """
        return self._set_type

    def get_operator(self):
        """
        Returns the operator of the member.
        """
        return self._operator

    def get_override_depth(self):
        """
        Returns the override depth of the member.
        """
        return self._override_depth

    def get_value(self):
        """
        Returns the value of the member.
        """
        return self.value

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

    def is_optional(self):
        """
        Returns True if the member is optional.
        """
        return self._optional

    def set_value(self, value):
        """
        Set the value of the nyan member to the specified value.
        """
        self.value = value

        self._type_conversion()

    def dump(self):
        """
        Returns the nyan string representation of the member.
        """
        output_str = "%s" % (self.name)

        if self._optional:
            output_str += " : optional(%s)" % (self._member_type.value)

        else:
            output_str += " : %s" % (self._member_type.value)

        if self.is_complex():
            if isinstance(self._set_type, NyanObject):
                output_str += "(%s)" % (self._set_type.get_name())

            else:
                output_str += "(%s)" % (self._set_type.value)

        if self.is_initialized():
            output_str += " %s%s %s" % ("@" * self._override_depth,
                                        self._operator.value, self.__str__())

        return output_str

    def _sanity_check(self):
        """
        Check if the member conforms to nyan grammar rules. Also does
        a bunch of type checks.
        """
        # self.name must be a string
        if not isinstance(self.name, str):
            raise Exception("%s: 'name' must be a string"
                            % (self.__repr__()))

        # self.name must conform to nyan grammar rules
        if not re.fullmatch(r"[a-zA-Z_][a-zA-Z_]*", self.name[0]):
            raise Exception("%s: 'name' is not well-formed"
                            % (self.__repr__()))

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

        if self.is_initialized():
            # Check if operator type matches with member type
            if self._member_type in (MemberType.INT, MemberType.FLOAT)\
                    and not self._operator in (MemberOperator.ASSIGN,
                                               MemberOperator.ADD,
                                               MemberOperator.SUBTRACT,
                                               MemberOperator.MULTIPLY,
                                               MemberOperator.DIVIDE):
                raise Exception("%s: %s is not a valid operator for %s member type"
                                % (self.__repr__(), self._operator,
                                   self._member_type))

            elif self._member_type is MemberType.TEXT\
                    and not self._operator in (MemberOperator.ASSIGN,
                                               MemberOperator.ADD):
                raise Exception("%s: %s is not a valid operator for %s member type"
                                % (self.__repr__(), self._operator,
                                   self._member_type))

            elif self._member_type is MemberType.FILE\
                    and not self._operator is MemberOperator.ASSIGN:
                raise Exception("%s: %s is not a valid operator for %s member type"
                                % (self.__repr__(), self._operator,
                                   self._member_type))

            elif self._member_type is MemberType.BOOLEAN\
                    and not self._operator in (MemberOperator.ASSIGN,
                                               MemberOperator.AND,
                                               MemberOperator.OR):
                raise Exception("%s: %s is not a valid operator for %s member type"
                                % (self.__repr__(), self._operator,
                                   self._member_type))

            elif self._member_type is MemberType.SET\
                    and not self._operator in (MemberOperator.ASSIGN,
                                               MemberOperator.ADD,
                                               MemberOperator.SUBTRACT,
                                               MemberOperator.AND,
                                               MemberOperator.OR):
                raise Exception("%s: %s is not a valid operator for %s member type"
                                % (self.__repr__(), self._operator,
                                   self._member_type))

            elif self._member_type is MemberType.ORDEREDSET\
                    and not self._operator in (MemberOperator.ASSIGN,
                                               MemberOperator.ADD,
                                               MemberOperator.SUBTRACT,
                                               MemberOperator.AND):
                raise Exception("%s: %s is not a valid operator for %s member type"
                                % (self.__repr__(), self._operator,
                                   self._member_type))

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
            # TODO: Implement Orderedset()
            self.value = list(self.value)

    def _get_primitive_value_str(self, member_type, value):
        """
        Returns the nyan string representation of primitive values.
        
        Subroutine of __str__
        """
        if member_type is MemberType.FLOAT:
            return "%sf" % value

        if isinstance(member_type, NyanObject):
            return value.get_name()

        return "%s" % value

    def __str__(self):
        """
        Returns the nyan string representation of the value.
        """
        if not self.is_initialized():
            return "UNINITIALIZED VALUE %s" % self.__repr__()

        if self._optional and self.value is MemberSpecialValue.NYAN_NONE:
                return MemberSpecialValue.NYAN_NONE.value

        if self._member_type in (MemberType.INT, MemberType.FLOAT,
                                 MemberType.TEXT, MemberType.FILE,
                                 MemberType.BOOLEAN):
            return self._get_primitive_value_str(self._member_type,
                                                 self.value)

        elif self._member_type in (MemberType.SET, MemberType.ORDEREDSET):
            output_str = ""

            if self._member_type is MemberType.ORDEREDSET:
                output_str += "o"

            output_str += "{"

            if len(self.value) > 0:
                for val in self.value:
                    output_str += "%s, " % self._get_primitive_value_str(
                                                        self._set_type,
                                                        val
                                                        )

                return output_str[:-2] + "}"

        elif isinstance(self._member_type, NyanObject):
            return self.value.get_name()

        else:
            raise Exception("%s has no valid type" % self.__repr__())

    def __repr__(self):
        return "NyanMember<%s: %s>" % (self.name, self._member_type)


class InheritedNyanMember(NyanMember):
    """
    Superclass for all nyan members inherited from other objects.
    
    TODO: sanity check
    """

    def __init__(self, name, member_type, parent, origin, value=None,
                set_type=None, operator=None, override_depth=None, optional=False):
        """
        Initializes the member and does some correctness
        checks, for your convenience.
        """

        super().__init__(name, member_type, value,
                         set_type, operator, override_depth, optional)

        self._parent = parent               # the direct parent of the
                                            # object which contains the
                                            # member

        self._origin = origin               # nyan object which originally
                                            # defines the member

    def get_name(self):
        """
        Returns the name of the member.
        """
        return "%s.%s" % (self._origin.name, self.name)

    def get_origin(self):
        """
        Returns the origin of the member.
        """
        return self._origin

    def get_parent(self):
        """
        Returns the direct parent of the member.
        """
        return self._parent

    def is_inherited(self):
        """
        Returns True if the member is inherited from another object.
        """
        return True

    def dump(self):
        """
        Returns the string representation of the member.
        """
        return "%s %s%s %s" % (self.get_name(), "@" * self._override_depth,
                                   self._operator.value, self.__str__())

    def _sanity_check(self):
        """
        Check if the member conforms to nyan grammar rules. Also does
        a bunch of type checks.
        """
        # parent must be a nyan object
        if not isinstance(self._parent, NyanObject):
            raise Exception("%s: '_parent' must have NyanObject type"
                            % (self.__repr__()))

        # origin must be a nyan object
        if not isinstance(self._origin, NyanObject):
            raise Exception("%s: '_origin' must have NyanObject type"
                            % (self.__repr__()))

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
