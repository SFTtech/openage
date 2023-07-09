# Copyright 2019-2023 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-lines,too-many-arguments,too-many-return-statements,too-many-locals

"""
Nyan structs.

Simple implementation to store nyan objects and
members for usage in the converter. This is not
a real nyan^TM implementation, but rather a "dumb"
storage format.

Python does not enforce static types, so be careful
 and only use the provided functions, please. :)
"""
from __future__ import annotations
import typing


from enum import Enum
import re

from ..util.ordered_set import OrderedSet

if typing.TYPE_CHECKING:
    from openage.nyan.import_tree import ImportTree


INDENT = "    "
MAX_LINE_WIDTH = 130


class NyanObject:
    """
    Superclass for nyan objects.
    """

    __slots__ = ('name', '_fqon', '_parents', '_inherited_members', '_members',
                 '_nested_objects', '_children')

    def __init__(
        self,
        name: str,
        parents: OrderedSet[NyanObject] = None,
        members: OrderedSet[NyanMember] = None,
        nested_objects: OrderedSet[NyanObject] = None
    ):
        """
        Initializes the object and does some correctness
        checks, for your convenience.
        """
        self.name = name

        # unique identifier (in modpack)
        self._fqon: tuple[str] = (self.name,)

        # parent objects
        self._parents: OrderedSet[NyanObject] = OrderedSet()
        # members inherited from parents
        self._inherited_members: OrderedSet[InheritedNyanMember] = OrderedSet()
        if parents:
            self._parents.update(parents)

        # members unique to this object
        self._members: OrderedSet[NyanMember] = OrderedSet()
        if members:
            self._members.update(members)

        # nested objects
        self._nested_objects: OrderedSet[NyanObject]  = OrderedSet()
        if nested_objects:
            self._nested_objects.update(nested_objects)

            for nested_object in self._nested_objects:
                nested_object.set_fqon(f"{self._fqon}.{nested_object.get_name()}")

        # Set of children
        self._children: OrderedSet[NyanObject] = OrderedSet()

        self._sanity_check()

        if len(self._parents) > 0:
            self._process_inheritance()

    def add_nested_object(self, new_nested_object: NyanObject) -> None:
        """
        Adds a nested object to the nyan object.
        """
        if not isinstance(new_nested_object, NyanObject):
            raise TypeError("nested object must have <NyanObject> type")

        if new_nested_object is self:
            raise ValueError(
                "nyan object must not contain itself as nested object")

        self._nested_objects.add(new_nested_object)

        new_nested_object.set_fqon((*self._fqon,
                                    new_nested_object.get_name()))

    def add_member(self, new_member: NyanMember) -> None:
        """
        Adds a member to the nyan object.
        """
        if new_member.is_inherited():
            raise TypeError("added member cannot be inherited")

        if not isinstance(new_member, NyanMember):
            raise TypeError("added member must have <NyanMember> type")

        self._members.add(new_member)

        # Update child objects
        for child in self._children:
            # Create a new member for every child with self as parent and origin
            inherited_member = InheritedNyanMember(
                new_member.get_name(),
                new_member.get_member_type(),
                self,
                self,
                None,
                None,
                0
            )
            child.update_inheritance(inherited_member)

    def add_child(self, new_child: NyanObject) -> None:
        """
        Registers another object as a child.
        """
        if not isinstance(new_child, NyanObject):
            raise TypeError("children must have <NyanObject> type")

        self._children.add(new_child)

        # Pass members and inherited members to the child object
        for member in self._members:
            # Create a new member with self as parent and origin
            inherited_member = InheritedNyanMember(
                member.get_name(),
                member.get_member_type(),
                self,
                self,
                None,
                None,
                0
            )
            new_child.update_inheritance(inherited_member)

        for inherited in self._inherited_members:
            # Create a new member with self as parent
            inherited_member = InheritedNyanMember(
                inherited.get_name(),
                inherited.get_member_type(),
                self,
                inherited.get_origin(),
                None,
                None,
                0
            )
            new_child.update_inheritance(inherited_member)

    def has_member(self, member_name: str, origin: NyanObject = None) -> bool:
        """
        Returns True if the NyanMember with the specified name exists.
        """
        if origin and origin is not self:
            for inherited_member in self._inherited_members:
                if origin == inherited_member.get_origin():
                    if inherited_member.get_name() == member_name:
                        return True

        else:
            for member in self._members:
                if member.get_name() == member_name:
                    return True

        return False

    def get_fqon(self) -> tuple[str]:
        """
        Returns the fqon of the nyan object.
        """
        return self._fqon

    def get_members(self) -> OrderedSet[NyanMember]:
        """
        Returns all NyanMembers of the object, including inherited members.
        """
        return self._members.union(self._inherited_members)

    def get_member_by_name(self, member_name: str, origin: NyanObject = None) -> NyanMember:
        """
        Returns the NyanMember with the specified name.
        """
        if origin and origin is not self:
            # Inherited member: Search in the inheritance tree
            for inherited_member in self._inherited_members:
                if origin == inherited_member.get_origin():
                    if inherited_member.get_name() == member_name:
                        return inherited_member

            raise ValueError(f"{repr(self)} has no member '{member_name}' with origin '{origin}'")

        # Else: Member should be a direct member of this nyan object
        for member in self._members:
            if member.get_name() == member_name:
                return member

        raise ValueError(f"{self} has no member '{member_name}'")

    def get_uninitialized_members(self) -> list:
        """
        Returns all uninitialized NyanMembers of the object.
        """
        uninit_members = []
        for member in self.get_members():
            if not member.is_initialized():
                uninit_members.append(member)

        return uninit_members

    def get_name(self) -> str:
        """
        Returns the name of the object.
        """
        return self.name

    def get_nested_objects(self) -> OrderedSet[NyanObject]:
        """
        Returns all nested NyanObjects of this object.
        """
        return self._nested_objects

    def get_parents(self) -> OrderedSet[NyanObject]:
        """
        Returns all nested parents of this object.
        """
        return self._parents

    def has_ancestor(self, nyan_object: NyanObject) -> bool:
        """
        Returns True if the given nyan object is an ancestor
        of this nyan object.
        """
        for parent in self._parents:
            if parent is nyan_object:
                return True

        for parent in self._parents:
            if parent.has_ancestor(nyan_object):
                return True

        return False

    def is_abstract(self) -> bool:
        """
        Returns True if any unique or inherited members are uninitialized.
        """
        return len(self.get_uninitialized_members()) > 0

    @staticmethod
    def is_patch() -> bool:
        """
        Returns True if the object is a NyanPatch.
        """
        return False

    def set_fqon(self, new_fqon: tuple[str]):
        """
        Set a new value for the fqon.
        """
        if isinstance(new_fqon, str):
            self._fqon = new_fqon.split(".")

        elif isinstance(new_fqon, tuple):
            self._fqon = new_fqon

        else:
            raise TypeError(f"{self}: Fqon must be a tuple(str) not {type(new_fqon)}")

        # Recursively set fqon for nested objects
        for nested_object in self._nested_objects:
            nested_fqon = (*new_fqon, nested_object.get_name())
            nested_object.set_fqon(nested_fqon)

    def update_inheritance(self, new_inherited_member: InheritedNyanMember) -> None:
        """
        Add an inherited member to the object. Should only be used by
        parent objects.
        """
        if not self.has_ancestor(new_inherited_member.get_origin()):
            raise ValueError(f"{repr(self)}: cannot add inherited member "
                             f"{new_inherited_member} because "
                             f"{new_inherited_member.get_origin()} is not "
                             f"an ancestor of {repr(self)}")

        if not isinstance(new_inherited_member, InheritedNyanMember):
            raise TypeError("added member must have <InheritedNyanMember> type")

        # Only add it, if it was not inherited before
        if not self.has_member(new_inherited_member.get_name(),
                               new_inherited_member.get_origin()):
            self._inherited_members.add(new_inherited_member)

        # Update child objects
        for child in self._children:
            # Create a new member for every child with self as parent
            inherited_member = InheritedNyanMember(
                new_inherited_member.get_name(),
                new_inherited_member.get_member_type(),
                self,
                new_inherited_member.get_origin(),
                None,
                None,
                0
            )
            child.update_inheritance(inherited_member)

    def dump(self, indent_depth: int = 0, import_tree: ImportTree = None) -> str:
        """
        Returns the string representation of the object.
        """
        # Header
        output_str = f"{self.get_name()}"

        output_str += self._prepare_inheritance_content(import_tree=import_tree)

        # Members
        output_str += self._prepare_object_content(indent_depth, import_tree=import_tree)

        return output_str

    def _prepare_object_content(self, indent_depth: int, import_tree: ImportTree = None) -> None:
        """
        Returns a string containing the nyan object's content
        (members, nested objects).

        Subroutine of dump().
        """
        output_str = ""
        empty = True

        if len(self._inherited_members) > 0:
            for inherited_member in self._inherited_members:
                if inherited_member.has_value():
                    empty = False
                    member_str = inherited_member.dump(
                        indent_depth + 1,
                        import_tree=import_tree,
                        namespace=self.get_fqon()
                    )
                    output_str += f"{(indent_depth + 1) * INDENT}{member_str}\n"

            if not empty:
                output_str += "\n"

        if len(self._members) > 0:
            empty = False
            for member in self._members:
                if self.is_patch():
                    # Patches do not need the type definition
                    member_str = member.dump_short(
                        indent_depth + 1,
                        import_tree=import_tree,
                        namespace=self.get_fqon()
                    )

                else:
                    member_str = member.dump(
                        indent_depth + 1,
                        import_tree=import_tree,
                        namespace=self.get_fqon()
                    )

                output_str += f"{(indent_depth + 1) * INDENT}{member_str}\n"

            output_str += "\n"

        # Nested objects
        if len(self._nested_objects) > 0:
            empty = False
            for nested_object in self._nested_objects:
                nested_str = nested_object.dump(
                    indent_depth + 1,
                    import_tree=import_tree
                )
                output_str += f"{(indent_depth + 1) * INDENT}{nested_str}\n"

            output_str = output_str[:-1]

        # Empty objects need a 'pass' line
        if empty:
            output_str += f"{(indent_depth + 1) * INDENT}pass\n\n"

        return output_str

    def _prepare_inheritance_content(self, import_tree: ImportTree = None) -> None:
        """
        Returns a string containing the nyan object's inheritance set
        in the header.

        Subroutine of dump().
        """
        output_str = "("

        if len(self._parents) > 0:
            for parent in self._parents:
                if import_tree:
                    sfqon = ".".join(import_tree.get_alias_fqon(
                        parent.get_fqon(),
                        namespace=self.get_fqon()
                    ))

                else:
                    sfqon = ".".join(parent.get_fqon())

                output_str += f"{sfqon}, "

            output_str = output_str[:-2]

        output_str += "):\n"

        return output_str

    def _process_inheritance(self) -> None:
        """
        Notify parents of the object.
        """
        for parent in self._parents:
            parent.add_child(self)

    def _sanity_check(self) -> None:
        """
        Check if the object conforms to nyan grammar rules. Also does
        a bunch of type checks.
        """
        # self.name must be a string
        if not isinstance(self.name, str):
            raise TypeError(f"{repr(self)}: 'name' must be a string")

        # self.name must conform to nyan grammar rules
        if not re.fullmatch(r"[a-zA-Z_][a-zA-Z0-9_]*", self.name):
            raise SyntaxError(f"{repr(self)}: 'name' is not well-formed")

        # self._parents must be NyanObjects
        for parent in self._parents:
            if not isinstance(parent, NyanObject):
                raise TypeError(f"{repr(self)}: {repr(parent)} must have NyanObject type")

        # self._members must be NyanMembers
        for member in self._members:
            if not isinstance(member, NyanMember):
                raise TypeError(f"{repr(self)}: {repr(member)} must have NyanMember type")

            # a member in self._members must also not be inherited
            if isinstance(member, InheritedNyanMember):
                raise TypeError(f"{repr(self)}: regular member {repr(member)} must not "
                                "have InheritedNyanMember type")

        # self._nested_objects must be NyanObjects
        for nested_object in self._nested_objects:
            if not isinstance(nested_object, NyanObject):
                raise TypeError(f"{repr(self)}: {repr(nested_object)} must have NyanObject type")

            if nested_object is self:
                raise ValueError(f"{repr(self)}: must not contain itself as nested object")

    def __repr__(self):
        return f"NyanObject<{self.name}>"


class NyanPatch(NyanObject):
    """
    Superclass for nyan patches.
    """

    __slots__ = ('_target', '_add_inheritance')

    def __init__(
        self,
        name: str,
        parents: OrderedSet[NyanObject] = None,
        members: OrderedSet[NyanObject] = None,
        nested_objects: OrderedSet[NyanObject] = None,
        target: NyanObject = None,
        add_inheritance: OrderedSet[NyanObject] = None
    ):

        self._target = target                  # patch target (can be added later)
        self._add_inheritance = OrderedSet()   # new inheritance
        if add_inheritance:
            self._add_inheritance.update(add_inheritance)

        super().__init__(name, parents, members, nested_objects)

    def get_target(self) -> NyanObject:
        """
        Returns the target of the patch.
        """
        return self._target

    def is_abstract(self) -> bool:
        """
        Returns True if unique or inherited members were
        not initialized or the patch target is not set.
        """
        return super().is_abstract() or not self._target

    @staticmethod
    def is_patch() -> bool:
        """
        Returns True if the object is a nyan patch.
        """
        return True

    def set_target(self, target: NyanObject) -> NyanObject:
        """
        Set the target of the patch.
        """
        self._target = target

        if not isinstance(self._target, NyanObject):
            raise TypeError(f"{repr(self)}: '_target' must have NyanObject type")

    def dump(self, indent_depth: int = 0, import_tree: ImportTree = None) -> str:
        """
        Returns the string representation of the object.
        """
        # Header
        output_str = f"{self.get_name()}"

        if import_tree:
            sfqon = ".".join(import_tree.get_alias_fqon(self._target.get_fqon()))

        else:
            sfqon = ".".join(self._target.get_fqon())

        output_str += f"<{sfqon}>"

        if len(self._add_inheritance) > 0:
            output_str += "["

            for new_inheritance in self._add_inheritance:
                if import_tree:
                    sfqon = ".".join(import_tree.get_alias_fqon(new_inheritance.get_fqon()))

                else:
                    sfqon = ".".join(new_inheritance.get_fqon())

                if new_inheritance[0] == "FRONT":
                    output_str += f"+{sfqon}, "
                elif new_inheritance[0] == "BACK":
                    output_str += f"{sfqon}+, "

            output_str = output_str[:-2] + "]"

        output_str += super()._prepare_inheritance_content(import_tree = import_tree)

        # Members
        output_str += super()._prepare_object_content(indent_depth = indent_depth,
                                                      import_tree = import_tree)

        return output_str

    def _sanity_check(self) -> None:
        """
        Check if the object conforms to nyan grammar rules. Also does
        a bunch of type checks.
        """
        super()._sanity_check()

        # Target must be a nyan object
        if self._target:
            if not isinstance(self._target, NyanObject):
                raise TypeError(f"{repr(self)}: '_target' must have NyanObject type")

        # Added inheritance must be tuples of "FRONT"/"BACK"
        # and a nyan object
        if len(self._add_inheritance) > 0:
            for inherit in self._add_inheritance:
                if not isinstance(inherit, tuple):
                    raise TypeError(f"{repr(self)}: '_add_inheritance' must be a tuple")

                if len(inherit) != 2:
                    raise SyntaxError(f"{repr(self)}: '_add_inheritance' tuples must have length 2")

                if inherit[0] not in ("FRONT", "BACK"):
                    raise ValueError(f"{repr(self)}: added inheritance must be FRONT or BACK mode")

                if not isinstance(inherit[1], NyanObject):
                    raise ValueError(f"{repr(self)}: added inheritance must contain NyanObject")

    def __repr__(self):
        return f"NyanPatch<{self.name}<{self._target.name}>>"


class NyanMemberType:
    """
    Superclass for nyan member types.
    """

    __slots__ = ('_member_type', '_element_types')

    def __init__(
        self,
        member_type: typing.Union[str, MemberType, NyanObject],
        element_types: typing.Collection[NyanMemberType] = None
    ):
        """
        Initializes the member type and does some correctness
        checks, for your convenience.
        """
        if isinstance(member_type, NyanObject):
            self._member_type = member_type

        else:
            self._member_type = MemberType(member_type)

        self._element_types = None
        if element_types:
            self._element_types = tuple(element_types)

        # check for errors in the initilization
        self._sanity_check()

    def get_type(self) -> MemberType:
        """
        Returns the member type.
        """
        return self._member_type

    def get_real_type(self) -> MemberType:
        """
        Returns the member type without wrapping modifiers.
        """
        if self.is_modifier():
            return self._element_types[0].get_real_type()

        return self._member_type

    def get_element_types(self) -> tuple[NyanMemberType, ...]:
        """
        Returns the element types.
        """
        return self._element_types

    def get_real_element_types(self) -> tuple[NyanMemberType, ...]:
        """
        Returns the element types without wrapping modifiers.
        """
        if self.is_modifier():
            return self._element_types[0].get_real_element_types()

        return self._element_types

    def is_primitive(self) -> bool:
        """
        Returns True if the member type is a single value.
        """
        return self._member_type in (MemberType.INT,
                                     MemberType.FLOAT,
                                     MemberType.TEXT,
                                     MemberType.FILE,
                                     MemberType.BOOLEAN)

    def is_real_primitive(self) -> bool:
        """
        Returns True if the member type is a primitive wrapped in a modifier.
        """
        if self.is_modifier():
            return self._element_types[0].is_real_primitive()

        return self.is_primitive()

    def is_complex(self) -> bool:
        """
        Returns True if the member type is a collection.
        """
        return self._member_type in (MemberType.SET,
                                     MemberType.ORDEREDSET,
                                     MemberType.DICT)

    def is_real_complex(self) -> bool:
        """
        Returns True if the member type is a collection wrapped in a modifier.
        """
        if self.is_modifier():
            return self._element_types[0].is_real_complex()

        return self.is_complex()

    def is_object(self) -> bool:
        """
        Returns True if the member type is an object.
        """
        return isinstance(self._member_type, NyanObject)

    def is_real_object(self) -> bool:
        """
        Returns True if the member type is an object wrapped in a modifier.
        """
        if self.is_modifier():
            return self._element_types[0].is_real_object()

        return self.is_object()

    def is_modifier(self) -> bool:
        """
        Returns True if the member type is a modifier.
        """
        return self._member_type in (MemberType.ABSTRACT,
                                     MemberType.CHILDREN,
                                     MemberType.OPTIONAL)

    def is_composite(self) -> bool:
        """
        Returns True if the member is a composite type with at least one element type.
        """
        return self.is_complex() or self.is_modifier()

    def accepts_op(self, operator: MemberOperator) -> bool:
        """
        Check if an operator is compatible with the member type.
        """
        if self.is_modifier():
            return self._element_types[0].accepts_op(operator)

        if self._member_type in (MemberType.INT, MemberType.FLOAT)\
            and operator not in (MemberOperator.ASSIGN,
                                 MemberOperator.ADD,
                                 MemberOperator.SUBTRACT,
                                 MemberOperator.MULTIPLY,
                                 MemberOperator.DIVIDE):
            return False

        if self._member_type is MemberType.TEXT\
                and operator not in (MemberOperator.ASSIGN,
                                     MemberOperator.ADD):
            return False

        if self._member_type is MemberType.FILE\
                and operator is not MemberOperator.ASSIGN:
            return False

        if self._member_type is MemberType.BOOLEAN\
                and operator not in (MemberOperator.ASSIGN,
                                     MemberOperator.AND,
                                     MemberOperator.OR):
            return False

        if self._member_type is MemberType.SET\
                and operator not in (MemberOperator.ASSIGN,
                                     MemberOperator.ADD,
                                     MemberOperator.SUBTRACT,
                                     MemberOperator.AND,
                                     MemberOperator.OR):
            return False

        if self._member_type is MemberType.ORDEREDSET\
                and operator not in (MemberOperator.ASSIGN,
                                     MemberOperator.ADD,
                                     MemberOperator.SUBTRACT,
                                     MemberOperator.AND,
                                     MemberOperator.OR):
            return False

        if self._member_type is MemberType.DICT\
                and operator not in (MemberOperator.ASSIGN,
                                     MemberOperator.ADD,
                                     MemberOperator.SUBTRACT,
                                     MemberOperator.AND,
                                     MemberOperator.OR):
            return False

        return True

    def accepts_value(self, value) -> bool:
        """
        Check if a value is compatible with the member type.
        """
        # Member values can only be NYAN_NONE if the member is optional
        if value is MemberSpecialValue.NYAN_NONE:
            return self._member_type is MemberType.OPTIONAL

        if self.is_modifier():
            return self._element_types[0].accepts_value(value)

        # inf is only used for ints and floats
        if value is MemberSpecialValue.NYAN_INF and\
                self._member_type not in (MemberType.INT, MemberType.FLOAT):
            return False

        # Values that are nyan objects must be the member type
        # or the children of the member type
        if self.is_object():
            if not (value is self._member_type or
                    value.has_ancestor(self._member_type)):
                return False

        return True

    def _sanity_check(self) -> None:
        """
        Check if the member type and element types are compatiable.
        """
        if self.is_composite():
            # if the member type is a composite, then the element types need
            # to be initialized
            if not self._element_types:
                raise TypeError(f"{repr(self)}: element types are required for composite types")

            if self.is_complex():
                # element types of complex types cannot be complex
                for elem_type in self._element_types:
                    if elem_type.is_real_complex():
                        raise TypeError(
                            f"{repr(self)}: element types cannot be complex "
                            f"but contains {elem_type}")

        else:
            # if the member is not a composite, the element types should be None
            if self._element_types:
                raise TypeError(
                    f"{repr(self)}: member type has element types "
                    "but is not a composite")

    def dump(self, import_tree: ImportTree = None, namespace: tuple[str] = None) -> str:
        """
        Returns the nyan string representation of the member type.
        """
        if self.is_primitive():
            return self._member_type.value

        if self.is_object():
            if import_tree:
                sfqon = ".".join(import_tree.get_alias_fqon(
                    self._member_type.get_fqon(),
                    namespace
                ))

            else:
                sfqon = ".".join(self._member_type.get_fqon())

            return sfqon

        # Composite types
        return (
            f"{self._member_type.value}("
            f"{', '.join(elem_type.dump(import_tree) for elem_type in self._element_types)})"
        )

    def __repr__(self):
        return f"NyanMemberType<{self.dump()}>"


class NyanMember:
    """
    Superclass for all nyan members.
    """

    __slots__ = ('name', '_member_type', 'value', '_operator', '_override_depth')

    def __init__(
        self,
        name: str,
        member_type: NyanMemberType,
        value = None,
        operator: MemberOperator = None,
        override_depth: int = 0
    ):
        """
        Initializes the member and does some correctness
        checks, for your convenience.
        """
        self.name = name                                # identifier

        if isinstance(member_type, NyanMemberType):     # type
            self._member_type = member_type

        else:
            raise TypeError(f"NyanMember<{self.name}>: Expected NyanMemberType for member_type "
                            f"but got {type(member_type)}")

        self._override_depth = override_depth           # override depth

        self._operator: MemberOperator = None
        if operator:
            operator = MemberOperator(operator)   # operator type

        self.value = None                               # value
        if value is not None:
            # Needs to check for None because 0 is also False
            self.set_value(value, operator)

        # check for errors in the initilization
        self._sanity_check()

    def get_name(self) -> str:
        """
        Returns the name of the member.
        """
        return self.name

    def get_member_type(self) -> NyanMemberType:
        """
        Returns the type of the member.
        """
        return self._member_type

    def get_operator(self) -> MemberOperator:
        """
        Returns the operator of the member.
        """
        return self._operator

    def get_override_depth(self) -> int:
        """
        Returns the override depth of the member.
        """
        return self._override_depth

    def get_value(self):
        """
        Returns the value of the member.
        """
        return self.value

    def is_primitive(self) -> bool:
        """
        Returns True if the member is a single value.
        """
        return self._member_type.is_real_primitive()

    def is_complex(self) -> bool:
        """
        Returns True if the member is a collection.
        """
        return self._member_type.is_real_complex()

    def is_object(self) -> bool:
        """
        Returns True if the member is an object.
        """
        return self._member_type.is_real_object()

    def is_initialized(self) -> bool:
        """
        Returns True if the member has a value.
        """
        return self.value is not None

    @ staticmethod
    def is_inherited() -> bool:
        """
        Returns True if the member is inherited from another object.
        """
        return False

    def has_value(self) -> bool:
        """
        Returns True if the member has a value.
        """
        return self.value is not None

    def set_value(self, value, operator: MemberOperator = None) -> None:
        """
        Set the value of the nyan member to the specified value and
        optionally, the operator.
        """
        if not self.value and not operator:
            raise ValueError(f"Setting a value for an uninitialized member {repr(self)} "
                             "requires also setting the operator")

        self.value = value
        self._operator = operator

        if self.value not in (MemberSpecialValue.NYAN_INF, MemberSpecialValue.NYAN_NONE):
            self._type_conversion()

        self._sanity_check()

    def dump(
        self,
        indent_depth: int,
        import_tree: ImportTree = None,
        namespace: tuple[str] = None
    ) -> str:
        """
        Returns the nyan string representation of the member.
        """
        output_str = f"{self.name} : {self._member_type.dump(import_tree=import_tree)}"

        if self.is_initialized():
            value_str = self._get_value_str(
                indent_depth,
                import_tree=import_tree,
                namespace=namespace
            )
            output_str += (f" {'@' * self._override_depth}{self._operator.value} {value_str}")

        return output_str

    def dump_short(
        self,
        indent_depth: int,
        import_tree: ImportTree = None,
        namespace: tuple[str] = None
    ) -> str:
        """
        Returns the nyan string representation of the member, but
        without the type definition.
        """
        value_str = self._get_value_str(
            indent_depth,
            import_tree=import_tree,
            namespace=namespace
        )
        return f"{self.get_name()} {'@' * self._override_depth}{self._operator.value} {value_str}"

    def _sanity_check(self) -> None:
        """
        Check if the member conforms to nyan grammar rules. Also does
        a bunch of type checks.
        """
        # self.name must be a string
        if not isinstance(self.name, str):
            raise TypeError(f"{repr(self)}: 'name' must be a string")

        # self.name must conform to nyan grammar rules
        if not re.fullmatch(r"[a-zA-Z_][a-zA-Z0-9_]*", self.name[0]):
            raise SyntaxError(f"{repr(self)}: 'name' is not well-formed")

        if (self.is_initialized() and not self.is_inherited()) or\
                (self.is_inherited() and self.has_value()):
            # override depth must be a non-negative integer
            if not (isinstance(self._override_depth, int) and
                    self._override_depth >= 0):
                raise ValueError(f"{repr(self)}: override depth must be a non-negative integer")

            # Check if operator type matches with member type
            if not self._member_type.accepts_op(self._operator):
                raise TypeError((
                    f"{repr(self)}: {self._operator} is not a valid"
                    f"operator for member type {self._member_type}"
                ))

            # Check if value is compatible with member type
            if not self._member_type.accepts_value(self.value):
                raise TypeError(f"{repr(self)}: value '{self.value}' is not compatible "
                                f"with type '{self._member_type}'")

    def _type_conversion(self) -> None:
        """
        Explicit type conversion of the member value.

        This lets us convert data fields without worrying about the
        correct types too much, e.g. if a boolean is stored as uint8.
        """
        if self._member_type.get_real_type() is MemberType.INT and\
                self._operator not in (MemberOperator.DIVIDE, MemberOperator.MULTIPLY):
            self.value = int(self.value)

        elif self._member_type.get_real_type() is MemberType.FLOAT:
            self.value = float(self.value)

        elif self._member_type.get_real_type() is MemberType.TEXT:
            self.value = str(self.value)

        elif self._member_type.get_real_type() is MemberType.FILE:
            self.value = str(self.value)

        elif self._member_type.get_real_type() is MemberType.BOOLEAN:
            self.value = bool(self.value)

        elif self._member_type.get_real_type() is MemberType.SET:
            self.value = OrderedSet(self.value)

        elif self._member_type.get_real_type() is MemberType.ORDEREDSET:
            self.value = OrderedSet(self.value)

        elif self._member_type.get_real_type() is MemberType.DICT:
            self.value = dict(self.value)

    @staticmethod
    def _get_primitive_value_str(
        member_type: NyanMemberType,
        value,
        import_tree: ImportTree = None,
        namespace: tuple[str] = None
    ) -> str:
        """
        Returns the nyan string representation of primitive values.

        Subroutine of _get_value_str(..)
        """
        if member_type.get_real_type() in (MemberType.TEXT, MemberType.FILE):
            return f"\"{value}\""

        if member_type.is_real_object():
            if import_tree:
                sfqon = ".".join(import_tree.get_alias_fqon(
                    value.get_fqon(),
                    namespace
                ))

            else:
                sfqon = ".".join(value.get_fqon())

            return sfqon

        return f"{value}"

    def _get_complex_value_str(
        self,
        indent_depth: int,
        member_type: NyanMemberType,
        value,
        import_tree: ImportTree = None,
        namespace: tuple[str] = None
    ) -> str:
        """
        Returns the nyan string representation of complex values.

        Subroutine of _get_value_str()
        """
        output_str = ""

        if member_type.get_real_type() is MemberType.ORDEREDSET:
            output_str += "o"

        output_str += "{"

        # Store the values for formatting
        # TODO: Dicts
        stored_values = []

        if member_type.get_real_type() is MemberType.DICT:
            for key, val in value.items():
                subtype = member_type.get_real_element_types()[0]
                key_str = self._get_primitive_value_str(
                    subtype,
                    key,
                    import_tree=import_tree,
                    namespace=namespace
                )

                subtype = member_type.get_real_element_types()[1]
                val_str = self._get_primitive_value_str(
                    subtype,
                    val,
                    import_tree=import_tree,
                    namespace=namespace
                )

                stored_values.append(f"{key_str}: {val_str}")

        else:
            for val in value:
                subtype = member_type.get_real_element_types()[0]
                stored_values.append(self._get_primitive_value_str(
                    subtype,
                    val,
                    import_tree=import_tree,
                    namespace=namespace
                ))

        # Check if the line gets too long
        # TODO: this does not account for a type definition
        concat_values = ", ".join(stored_values)
        line_length = len(indent_depth * INDENT) + len((
            f"{self.name} "
            f"{'@' * self._override_depth}"
            f"{self._operator.value} "
            f"{concat_values}"
        ))

        if line_length < MAX_LINE_WIDTH:
            output_str += concat_values

        elif stored_values:
            output_str += "\n"

            # How much space is left per formatted line
            space_left = MAX_LINE_WIDTH - len((indent_depth + 1) * INDENT)

            # Find the longest value's length
            longest_len = len(max(stored_values, key=len))

            # How man values of that length fit in one line
            values_per_line = space_left // longest_len
            values_per_line = max(values_per_line, 1)

            output_str += (indent_depth + 1) * INDENT

            val_index = 0
            end_index = len(stored_values)
            for val in stored_values:
                val_index += 1
                output_str += val

                if val_index % values_per_line == 0:
                    output_str += ",\n"

                    if val_index != end_index:
                        output_str += ((indent_depth + 1) * INDENT)

                else:
                    output_str += ", "

            output_str = output_str[:-2] + "\n"
            output_str += (indent_depth * INDENT)

        output_str = output_str + "}"

        return output_str

    def _get_value_str(
        self,
        indent_depth: int,
        import_tree: ImportTree = None,
        namespace: tuple[str] = None
    ) -> str:
        """
        Returns the nyan string representation of the value.
        """
        if not self.is_initialized():
            return f"UNINITIALIZED VALUE {repr(self)}"

        if self.value is MemberSpecialValue.NYAN_NONE:
            return MemberSpecialValue.NYAN_NONE.value

        if self.value is MemberSpecialValue.NYAN_INF:
            return MemberSpecialValue.NYAN_INF.value

        if self.is_primitive() or self.is_object():
            return self._get_primitive_value_str(
                self._member_type,
                self.value,
                import_tree=import_tree,
                namespace=namespace
            )

        if self.is_complex():
            return self._get_complex_value_str(
                indent_depth,
                self._member_type,
                self.value,
                import_tree=import_tree,
                namespace=namespace
            )

        raise TypeError(f"{repr(self)} has no valid type")

    def __str__(self):
        return self._get_value_str(indent_depth=0)

    def __repr__(self):
        return f"NyanMember<{self.name}: {self._member_type}>"


class NyanPatchMember(NyanMember):
    """
    Nyan members for patches.
    """

    __slots__ = ('_patch_target', '_member_origin')

    def __init__(
        self,
        name: str,
        patch_target: NyanObject,
        member_origin: NyanObject,
        value,
        operator: MemberOperator,
        override_depth: int = 0
    ):
        """
        Initializes the member and does some correctness checks,
        for your convenience. Other than the normal members,
        patch members must initialize all values in the constructor
        """
        # the target object of the patch
        self._patch_target = patch_target

        # the origin of the patched member from the patch target
        self._member_origin = member_origin

        target_member_type = self._get_target_member_type(name, member_origin)

        super().__init__(name, target_member_type, value, operator, override_depth)

    def get_name_with_origin(self) -> str:
        """
        Returns the name of the member in <member_origin>.<name> form.
        """
        return f"{self._member_origin.name}.{self.name}"

    def dump(
        self,
        indent_depth: int,
        import_tree: ImportTree = None,
        namespace: tuple[str] = None
    ) -> str:
        """
        Returns the string representation of the member.
        """
        return self.dump_short(indent_depth, import_tree=import_tree, namespace=namespace)

    def dump_short(
        self,
        indent_depth: int,
        import_tree: ImportTree = None,
        namespace: tuple[str] = None
    ) -> str:
        """
        Returns the nyan string representation of the member, but
        without the type definition.
        """
        value_str = self._get_value_str(
            indent_depth,
            import_tree=import_tree,
            namespace=namespace
        )
        return (f"{self.get_name_with_origin()} {'@' * self._override_depth}"
                f"{self._operator.value} {value_str}")

    def _sanity_check(self) -> None:
        """
        Check if the member conforms to nyan grammar rules. Also does
        a bunch of type checks.
        """
        super()._sanity_check()

        # patch target must be a nyan object
        if not isinstance(self._patch_target, NyanObject):
            raise TypeError(f"{self}: '_patch_target' must have NyanObject type")

        # member origin must be a nyan object
        if not isinstance(self._member_origin, NyanObject):
            raise TypeError(f"{self}: '_member_origin' must have NyanObject type")

    def _get_target_member_type(self, name: str, origin: NyanObject):
        """
        Retrieves the type of the patched member.
        """
        target_member = self._member_origin.get_member_by_name(name, origin)

        return target_member.get_member_type()

    def __repr__(self):
        return f"NyanPatchMember<{self.name}: {self._member_type}>"


class InheritedNyanMember(NyanMember):
    """
    Nyan members inherited from other objects.
    """

    __slots__ = ('_parent', '_origin')

    def __init__(
        self,
        name: str,
        member_type: NyanMemberType,
        parent: NyanObject,
        origin: NyanObject,
        value=None,
        operator: MemberOperator = None,
        override_depth: int = 0
    ):
        """
        Initializes the member and does some correctness
        checks, for your convenience.
        """
        self._parent = parent   # the direct parent of the object which contains the member
        self._origin = origin   # nyan object which originally defined the member

        super().__init__(name, member_type, value, operator, override_depth)

    def get_name_with_origin(self) -> str:
        """
        Returns the name of the member in <origin>.<name> form.
        """
        return f"{self._origin.name}.{self.name}"

    def get_origin(self) -> NyanObject:
        """
        Returns the origin of the member.
        """
        return self._origin

    def get_parent(self) -> NyanObject:
        """
        Returns the direct parent of the member.
        """
        return self._parent

    @staticmethod
    def is_inherited() -> bool:
        """
        Returns True if the member is inherited from another object.
        """
        return True

    def is_initialized(self) -> bool:
        """
        Returns True if self or the parent is initialized.
        """
        return super().is_initialized() or\
            self._parent.get_member_by_name(self.name, self._origin).is_initialized()

    def dump(
        self,
        indent_depth: int,
        import_tree: ImportTree = None,
        namespace: tuple[str] = None
    ) -> str:
        """
        Returns the string representation of the member.
        """
        return self.dump_short(indent_depth, import_tree=import_tree, namespace=namespace)

    def dump_short(
        self,
        indent_depth: int,
        import_tree: ImportTree = None,
        namespace: tuple[str] = None
    ) -> str:
        """
        Returns the nyan string representation of the member, but
        without the type definition.
        """
        value_str = self._get_value_str(
            indent_depth,
            import_tree=import_tree,
            namespace=namespace
        )
        return (f"{self.get_name_with_origin()} {'@' * self._override_depth}"
                f"{self._operator.value} {value_str}")

    def _sanity_check(self) -> None:
        """
        Check if the member conforms to nyan grammar rules. Also does
        a bunch of type checks.
        """
        super()._sanity_check()

        # parent must be a nyan object
        if not isinstance(self._parent, NyanObject):
            raise TypeError(f"{repr(self)}: '_parent' must have NyanObject type")

        # origin must be a nyan object
        if not isinstance(self._origin, NyanObject):
            raise TypeError(f"{repr(self)}: '_origin' must have NyanObject type")

    def __repr__(self):
        return f"InheritedNyanMember<{self.name}: {self._member_type}>"


class MemberType(Enum):
    """
    Symbols for nyan member types.
    """

    # Primitive types
    INT        = "int"
    FLOAT      = "float"
    TEXT       = "text"
    FILE       = "file"
    BOOLEAN    = "bool"

    # Complex types
    SET        = "set"
    ORDEREDSET = "orderedset"
    DICT       = "dict"

    # Modifier types
    ABSTRACT   = "abstract"
    CHILDREN   = "children"
    OPTIONAL   = "optional"


class MemberSpecialValue(Enum):
    """
    Symbols for special nyan values.
    """
    # nyan none type
    NYAN_NONE = "None"

    # infinite value for float and int
    NYAN_INF  = "inf"


class MemberOperator(Enum):
    """
    Symbols for nyan member operators.
    """

    ASSIGN    = "="      # assignment
    ADD       = "+="     # addition, append, insertion, union
    SUBTRACT  = "-="     # subtraction, remove
    MULTIPLY  = "*="     # multiplication
    DIVIDE    = "/="     # division
    AND       = "&="     # logical AND, intersect
    OR        = "|="     # logical OR, union
