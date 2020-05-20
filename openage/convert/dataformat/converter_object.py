# Copyright 2019-2020 the openage authors. See copying.md for legal info.

"""
Objects that represent data structures in the original game.

These are simple containers that can be processed by the converter.
"""

from openage.convert.dataformat.aoc.combined_sound import CombinedSound
from openage.convert.dataformat.aoc.combined_terrain import CombinedTerrain
from openage.convert.dataformat.value_members import NoDiffMember
from openage.nyan.nyan_structs import NyanPatch, NyanPatchMember

from ...nyan.nyan_structs import NyanObject, MemberOperator
from .aoc.combined_sprite import CombinedSprite
from .aoc.expected_pointer import ExpectedPointer
from .value_members import ValueMember


class ConverterObject:
    """
    Storage object for data objects in the to-be-converted games.
    """

    __slots__ = ('obj_id', 'members')

    def __init__(self, obj_id, members=None):
        """
        Creates a new ConverterObject.

        :param obj_id: An identifier for the object (as a string or int)
        :param members: An already existing member dict.
        """
        self.obj_id = obj_id

        self.members = {}

        if members:
            member_list = list(members.values())

            if all(isinstance(member, ValueMember) for member in member_list):
                self.members.update(members)

            else:
                raise Exception("members must be an instance of ValueMember")

    def get_id(self):
        """
        Returns the object's ID.
        """
        return self.obj_id

    def add_member(self, member):
        """
        Adds a member to the object.
        """
        key = member.get_name()
        self.members.update({key: member})

    def add_members(self, members):
        """
        Adds multiple members to the object.
        """
        for member in members:
            key = member.get_name()
            self.members.update({key: member})

    def get_member(self, name):
        """
        Returns a member of the object.
        """
        return self.members[name]

    def has_member(self, name):
        """
        Returns True if the object has a member with the specified name.
        """
        return name in self.members

    def remove_member(self, name):
        """
        Removes a member from the object.
        """
        self.members.pop(name)

    def short_diff(self, other):
        """
        Returns the obj_diff between two objects as another ConverterObject.

        The object created by short_diff() only contains members
        that are different. It does not contain NoDiffMembers.
        """
        if type(self) is not type(other):
            raise Exception("type %s cannot be diffed with type %s"
                            % (type(self), type(other)))

        obj_diff = {}

        for member_id, member in self.members.items():
            member_diff = member.diff(other.get_member(member_id))

            if not isinstance(member_diff, NoDiffMember):
                obj_diff.update({member_id: member_diff})

        return ConverterObject("%s-%s-sdiff" % (self.obj_id, other.get_id()), members=obj_diff)

    def diff(self, other):
        """
        Returns the obj_diff between two objects as another ConverterObject.
        """
        if type(self) is not type(other):
            raise Exception("type %s cannot be diffed with type %s"
                            % (type(self), type(other)))

        obj_diff = {}

        for member_id, member in self.members.items():
            obj_diff.update({member_id: member.diff(other.get_member(member_id))})

        return ConverterObject("%s-%s-diff" % (self.obj_id, other.get_id()), members=obj_diff)

    def __getitem__(self, key):
        """
        Short command for getting a member of the object.
        """
        return self.get_member(key)

    def __repr__(self):
        raise NotImplementedError(
            "return short description of the object %s" % (type(self)))


class ConverterObjectGroup:
    """
    A group of objects that are connected together in some way
    and need each other for conversion. ConverterObjectGroup
    instances are converted to the nyan API.
    """

    __slots__ = ('group_id', 'raw_api_objects')

    def __init__(self, group_id, raw_api_objects=None):
        """
        Creates a new ConverterObjectGroup.

        :paran group_id:  An identifier for the object group (as a string or int)
        :param raw_api_objects: A list of raw API objects. These will become
                                proper API objects during conversion.
        """
        self.group_id = group_id

        # Stores the objects that will later be converted to nyan objects
        # This uses the RawAPIObject's ids as keys.
        self.raw_api_objects = {}

        # Stores push values to members of other converter object groups
        self.raw_member_pushs = []

        if raw_api_objects:
            self._create_raw_api_object_dict(raw_api_objects)

    def get_id(self):
        """
        Returns the object group's ID.
        """
        return self.group_id

    def add_raw_api_object(self, subobject):
        """
        Adds a subobject to the object.
        """
        key = subobject.get_id()
        self.raw_api_objects.update({key: subobject})

    def add_raw_api_objects(self, subobjects):
        """
        Adds several subobject to the object.
        """
        for subobject in subobjects:
            self.add_raw_api_object(subobject)

    def add_raw_member_push(self, push_object):
        """
        Adds a RawPushMember to the object.
        """
        self.raw_member_pushs.append(push_object)

    def create_nyan_objects(self):
        """
        Creates nyan objects from the existing raw API objects.
        """
        patch_objects = []
        for raw_api_object in self.raw_api_objects.values():
            raw_api_object.create_nyan_object()

            if raw_api_object.is_patch():
                patch_objects.append(raw_api_object)

        for patch_object in patch_objects:
            patch_object.link_patch_target()

    def create_nyan_members(self):
        """
        Fill nyan members of all raw API objects.
        """
        for raw_api_object in self.raw_api_objects.values():
            raw_api_object.create_nyan_members()

            if not raw_api_object.is_ready():
                raise Exception("%s: object is not ready for export. "
                                "Member or object not initialized." % (raw_api_object))

    def execute_raw_member_pushs(self, push_object):
        """
        Extend raw members of referenced raw API objects.
        """
        for push in self.raw_member_pushs:
            expected_pointer = push.get_object_target()
            raw_api_object = expected_pointer.resolve_raw()

    def get_raw_api_object(self, obj_id):
        """
        Returns a subobject of the object.
        """
        try:
            return self.raw_api_objects[obj_id]

        except KeyError:
            raise Exception("%s: Could not find raw API object with obj_id %s"
                            % (self, obj_id))

    def get_raw_api_objects(self):
        """
        Returns all raw API objects.
        """
        return self.raw_api_objects

    def has_raw_api_object(self, obj_id):
        """
        Returns True if the object has a subobject with the specified ID.
        """
        return obj_id in self.raw_api_objects

    def remove_raw_api_object(self, obj_id):
        """
        Removes a subobject from the object.
        """
        self.raw_api_objects.pop(obj_id)

    def _create_raw_api_object_dict(self, subobject_list):
        """
        Creates the dict from the subobject list passed to __init__.
        """
        for subobject in subobject_list:
            self.add_raw_api_object(subobject)

    def __repr__(self):
        return "ConverterObjectGroup<%s>" % (self.group_id)


class RawAPIObject:
    """
    An object that contains all the necessary information to create
    a nyan API object. Members are stored as (membername, value) pairs.
    Values refer either to primitive values (int, float, str),
    expected pointers to objects or expected media files.
    The 'expected' values two have to be resolved in an additional step.
    """

    __slots__ = ('obj_id', 'name', 'api_ref', 'raw_members', 'raw_parents',
                 '_location', '_filename', 'nyan_object', '_patch_target')

    def __init__(self, obj_id, name, api_ref, location=""):
        """
        Creates a raw API object.

        :param obj_id: Unique identifier for the raw API object.
        :type obj_id: str
        :param name: Name of the nyan object created from the raw API object.
        :type name: str
        :param api_ref: The openage API objects used as reference for creating the nyan object.
        :type api_ref: dict
        :param location: Relative path of the nyan file in the modpack or another raw API object.
        :type location: str, .expected_pointer.ExpectedPointer
        """

        self.obj_id = obj_id
        self.name = name

        self.api_ref = api_ref

        self.raw_members = []
        self.raw_parents = []

        self._location = location
        self._filename = None

        self.nyan_object = None
        self._patch_target = None

    def add_raw_member(self, name, value, origin):
        """
        Adds a raw member to the object.

        :param name: Name of the member (has to be a valid inherited member name).
        :type name: str
        :param value: Value of the member.
        :type value: int, float, bool, str, list
        :param origin: from which parent the member was inherited.
        :type origin: str
        """
        self.raw_members.append((name, value, origin))

    def add_raw_patch_member(self, name, value, origin, operator):
        """
        Adds a raw member to the object.

        :param name: Name of the member (has to be a valid target member name).
        :type name: str
        :param value: Value of the member.
        :type value: int, float, bool, str, list
        :param origin: from which parent the target's member was inherited.
        :type origin: str
        :param operator: the operator for the patched member
        :type operator: MemberOperator
        """
        self.raw_members.append((name, value, origin, operator))

    def add_raw_parent(self, parent_id):
        """
        Adds a raw parent to the object.

        :param parent_id: fqon of the parent in the API object dictionary
        :type parent_id: str
        """
        self.raw_parents.append(parent_id)

    def extend_raw_member(self, name, push_value, origin):
        """
        Extends a raw member value.

        :param name: Name of the member (has to be a valid inherited member name).
        :type name: str
        :param push_value: Extended value of the member.
        :type push_value: list
        :param origin: from which parent the member was inherited.
        :type origin: str
        """
        for raw_member in self.raw_members:
            member_name = raw_member[0]
            member_value = raw_member[1]
            member_origin = raw_member[2]

            if name == member_name and member_origin == member_origin:
                member_value = member_value.extend(push_value)
                break

        else:
            raise Exception("%s: Cannot extend raw member %s with origin %s: member not found"
                            % (self, name, origin))

    def create_nyan_object(self):
        """
        Create the nyan object for this raw API object. Members have to be created separately.
        """
        parents = []
        for raw_parent in self.raw_parents:
            parents.append(self.api_ref[raw_parent])

        if self.is_patch():
            self.nyan_object = NyanPatch(self.name, parents)

        else:
            self.nyan_object = NyanObject(self.name, parents)

    def create_nyan_members(self):
        """
        Fills the nyan object members with values from the raw members.
        References to nyan objects or media files with be resolved.
        The nyan object has to be created before this function can be called.
        """
        if self.nyan_object is None:
            raise Exception("%s: nyan object needs to be created before"
                            "member values can be assigned" % (self))

        for raw_member in self.raw_members:
            member_name = raw_member[0]
            member_value = raw_member[1]
            member_origin = self.api_ref[raw_member[2]]
            member_operator = None
            if self.is_patch():
                member_operator = raw_member[3]

            if isinstance(member_value, ExpectedPointer):
                member_value = member_value.resolve()

            elif isinstance(member_value, CombinedSprite):
                member_value = member_value.get_relative_sprite_location()

            elif isinstance(member_value, CombinedTerrain):
                member_value = member_value.get_relative_terrain_location()

            elif isinstance(member_value, CombinedSound):
                member_value = member_value.get_relative_file_location()

            elif isinstance(member_value, list):
                # Resolve elements in the list, if it's not empty
                if member_value:
                    temp_values = []

                    for temp_value in member_value:
                        if isinstance(temp_value, ExpectedPointer):
                            temp_values.append(temp_value.resolve())

                        elif isinstance(temp_value, CombinedSprite):
                            temp_values.append(temp_value.get_relative_sprite_location())

                        elif isinstance(member_value, CombinedTerrain):
                            member_value = member_value.get_relative_terrain_location()

                        elif isinstance(temp_value, CombinedSound):
                            temp_values.append(temp_value.get_relative_file_location())

                        else:
                            temp_values.append(temp_value)

                    member_value = temp_values

            elif isinstance(member_value, float):
                # Round floats to 6 decimal places for increased readability
                # should have no effect on balance, hopefully
                member_value = round(member_value, ndigits=6)

            if self.is_patch():
                nyan_member = NyanPatchMember(member_name, self.nyan_object.get_target(), member_origin,
                                              member_value, member_operator)
                self.nyan_object.add_member(nyan_member)

            else:
                nyan_member = self.nyan_object.get_member_by_name(member_name, member_origin)
                nyan_member.set_value(member_value, MemberOperator.ASSIGN)

    def link_patch_target(self):
        """
        Set the target NyanObject for a patch.
        """
        if not self.is_patch():
            raise Exception("Cannot link patch target: %s is not a patch"
                            % (self))

        if isinstance(self._patch_target, ExpectedPointer):
            target = self._patch_target.resolve()

        else:
            target = self._patch_target

        self.nyan_object.set_target(target)

    def get_filename(self):
        """
        Returns the filename of the raw API object.
        """
        return self._filename

    def get_file_location(self):
        """
        Returns a tuple with 
            1. the relative path to the directory
            2. the filename
        where the nyan object will be stored.

        This method can be called instead of get_location() when
        you are unsure whether the nyan object will be nested.
        """
        if isinstance(self._location, ExpectedPointer):
            # Work upwards until we find the root object
            nesting_raw_api_object = self._location.resolve_raw()
            nesting_location = nesting_raw_api_object.get_location()

            while isinstance(nesting_location, ExpectedPointer):
                nesting_raw_api_object = nesting_location.resolve_raw()
                nesting_location = nesting_raw_api_object.get_location()

            return (nesting_location, nesting_raw_api_object.get_filename())

        return (self._location, self._filename)

    def get_id(self):
        """
        Returns the ID of the raw API object.
        """
        return self.obj_id

    def get_location(self):
        """
        Returns the relative path to a directory or an ExpectedPointer
        to another RawAPIObject.
        """
        return self._location

    def get_nyan_object(self):
        """
        Returns the nyan API object for the raw API object.
        """
        if self.nyan_object:
            return self.nyan_object

        raise Exception("nyan object for %s has not been created yet" % (self))

    def is_ready(self):
        """
        Returns whether the object is ready to be exported.
        """
        return self.nyan_object is not None and not self.nyan_object.is_abstract()

    def is_patch(self):
        """
        Returns True if the object is a patch.
        """
        return self._patch_target is not None

    def set_filename(self, filename, suffix="nyan"):
        """
        Set the filename of the resulting nyan file.

        :param filename: File name prefix (without extension).
        :type filename: str
        :param suffix: File extension (defaults to "nyan")
        :type suffix: str
        """
        self._filename = "%s.%s" % (filename, suffix)

    def set_location(self, location):
        """
        Set the relative location of the object in a modpack. This must
        be a path to a nyan file or an ExpectedPointer to a nyan object.

        :param location: Relative path of the nyan file in the modpack or
                         an expected pointer toanother raw API object.
        :type location: str, .expected_pointer.ExpectedPointer
        """
        self._location = location

    def set_patch_target(self, target):
        """
        Set an ExpectedPointer as a target for this object. If this
        is done, the RawAPIObject will be converted to a patch.

        :param target: An expected pointer to another raw API object or a nyan object.
        :type target: .expected_pointer.ExpectedPointer, ..nyan.nyan_structs.NyanObject
        """
        self._patch_target = target

    def __repr__(self):
        return "RawAPIObject<%s>" % (self.obj_id)


class RawMemberPush:
    """
    An object that contains additional values for complex members
    in raw API objects (lists or sets). Pushing these values to the
    raw API object will extennd the list or set. The values should be
    pushed to the raw API objects before their nyan members are created.
    """

    def __init__(self, expected_pointer, member_name, member_origin, push_value):
        """
        Creates a new member push.

        :param expected_pointer: Expected pointer of the RawAPIObject.
        :type expected_pointer: ExpectedPointer
        :param member_name: Name of the member that is extended.
        :type member_name: str
        :param member_origin: Fqon of the object the member was inherited from.
        :type member_origin: str
        :param push_value: Value that extends the existing member value.
        :type push_value: list
        """
        self.expected_pointer = expected_pointer
        self.member_name = member_name
        self.member_origin = member_origin
        self.push_value = push_value

    def get_object_target(self):
        """
        Returns the expected pointer for the push target.
        """
        return self.expected_pointer

    def get_member_name(self):
        """
        Returns the name of the member that is extended.
        """
        return self.member_name

    def get_member_origin(self):
        """
        Returns the fqon of the member's origin.
        """
        return self.member_origin

    def get_push_value(self):
        """
        Returns the value that extends the member's existing value.
        """
        return self.push_value


class ConverterObjectContainer:
    """
    A conainer for all ConverterObject instances in a converter process.

    It is recommended to create one ConverterObjectContainer for everything
    and pass the reference around.
    """

    def __repr__(self):
        return "ConverterObjectContainer"
