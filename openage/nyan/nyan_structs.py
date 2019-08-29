# Copyright 2019-2019 the openage authors. See copying.md for legal info.

"""
Nyan structs.

Simple implementation to store nyan objects and
members for usage in the converter.
"""

from enum import Enum

class NyanObject:
    """
    Superclass for nyan objects.
    """
    
    def __init__(self, name, parents=None, members=None,
                 nested_objects=None):
        
        self.name = name                            # identifier
        self.parents = parents                      # inheritance
        self.members = members                      # members unique to this object
        self.inherited_members = None               # members inherited from parents
        self.nested_objects = nested_objects        # nested objects


    def get_name(self):
        """
        Returns the name of the nyan object.
        """
        return self.name

    def get_members(self):
        """
        Returns unique and inherited members of the nyan object.
        """
        return self.members + self.inherited_members

    def get_unique_members(self):
        """
        Returns unique and inherited members of the nyan object.
        """
        return self.members

    def get_inherited_members(self):
        """
        Returns unique and inherited members of the nyan object.
        """
        return self.inherited_members


class NyanPatch(NyanObject):
    """
    Superclass for nyan patches.    
    """

    def __init__(self, name, target, parents=None, members=None,
                 nested_objects=None, add_inheritance=None):

        super().__init__(name, parents, members, nested_objects)
        self.target = target                    # patch target
        self.add_inheritance = add_inheritance  # new inheritance


    def get_target(self):
        """
        Returns the target of the nyan patch.
        """
        return self.target


class NyanMember:
    """
    Superclass for all nyan members.
    """
    
    def __init__(self, name, member_type, value=None, operator=None,
                 override_depth=0, optional=False):
        
        self.name = name                            # identifier
        self.member_type = MemberType(member_type)  # type
        self.optional = optional                    # whether the value is allowed
                                                    # to be NYAN_NONE

        self.operator = MemberOperator(operator)    # operator type
        self.override_depth = override_depth        # override depth
        
        self.value = value                          # value
    
    def get_name(self):
        """
        Returns the name of the nyan member.
        """
        return self.name


class InheritedNyanMember(NyanMember):
    """
    Superclass for all inherited nyan members.
    """
    
    def __init__(self, name, member_type, origin, value=None,
                 operator=None, override_depth=None):
        
        super().__init__(name, member_type, value, operator, override_depth)
        self.origin = origin
        
class MemberType(Enum):
    """
    Symbols for nyan member types.
    """    
    
    # Primitive types
    INT     = "int"
    FLOAT   = "float"
    TEXT    = "text"
    FILE    = "file"
    BOOLEAN = "bool"
    
    # Complex types
    OBJECT     = "object"
    SET        = "set"
    ORDEREDSET = "orderedset"
    
class MemberSpecialValue(Enum):
    """
    Symbols for special nyan values.
    """

    # The nyan none type
    NYAN_NONE = "None"

class MemberOperator(Enum):
    """
    Symbols for nyan member operators.
    """
    
    ASSIGN   = "="
    ADD      = "+="
    SUBTRACT = "-="
    MULTIPLY = "*="
    DIVIDE   = "/="
    AND      = "&="
    OR       = "|="
    