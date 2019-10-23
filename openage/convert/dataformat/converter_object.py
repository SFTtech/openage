# Copyright 2019-2019 the openage authors. See copying.md for legal info.

"""
Objects that represent data structures in the original game.

These are simple containers that can be processed by the converter.
"""

from .value_members import ValueMember


class ConverterObject:
    """
    Storage object for data objects in the to-be-converted games.
    """

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
        return (name in self.members)

    def remove_member(self, name):
        """
        Removes a member from the object.
        """
        self.members.pop(name)

    def short_diff(self, other):
        """
        Returns the diff between two objects as another ConverterObject.

        The object created by short_diff() only contains members and raw_api_objects
        that are different. It does not contain NoDiffMembers.
        """
        raise NotImplementedError(
            "%s has no short_diff() implementation" % (type(self)))

    def diff(self, other):
        """
        Returns the diff between two objects as another ConverterObject.
        """
        raise NotImplementedError(
            "%s has no diff() implementation" % (type(self)))

    def __repr__(self):
        raise NotImplementedError(
            "return short description of the object %s" % (type(self)))


class ConverterObjectGroup:
    """
    A group of objects that are connected together in some way
    and need each other for conversion. ConverterObjectGroup
    instances are converted to the nyan API.
    """

    def __init__(self, group_id, raw_api_objects=None):
        """
        Creates a new ConverterObjectGroup.

        :paran group_id:  An identifier for the object group (as a string or int)
        :param raw_api_objects: A list of raw API objects. These will become
                                proper API objects during conversion.
        """
        self.group_id = group_id

        # stores the objects that will later be mapped to nyan objects
        self.raw_api_objects = {}

        if raw_api_objects:
            self._create_raw_api_object_dict(raw_api_objects)

    def add_raw_api_object(self, subobject):
        """
        Adds a subobject to the object.
        """
        key = subobject.get_id()
        self.raw_api_objects.update({key: subobject})

    def get_raw_api_object(self, obj_id):
        """
        Returns a subobject of the object.
        """
        return self.raw_api_objects[obj_id]

    def has_raw_api_object(self, obj_id):
        """
        Returns True if the object has a subobject with the specified ID.
        """
        return (obj_id in self.raw_api_objects)

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
        raise NotImplementedError(
            "return short description of the object %s" % (type(self)))


class RawAPIObject:
    """
    An object that contains all the necessary information to create
    a nyan API object.
    """

    def __init__(self, api_ref, data):

        # fqon of the API object
        self.api_ref = api_ref

        # A list of ValueMembers that are necessary to translate
        # the object to an actual API object.
        self.data = data

    def add_data(self, new_data):
        """
        Adds more data to the object.

        :param new_data: A list of new ValueMembers.
        """
        self.data.extend(new_data)

    def get_nyan_object(self):
        """
        Returns the nyan API object for the raw API object.
        """
        raise NotImplementedError(
            "returning a nyan object of the object %s not possible" (type(self)))

    def __repr__(self):
        raise NotImplementedError(
            "return short description of the object %s" % (type(self)))


class ConverterObjectContainer:
    """
    A conainer for all ConverterObject instances in a converter process.

    It is recommended to create one ConverterObjectContainer for everything
    and pass the reference around.
    """
    pass
