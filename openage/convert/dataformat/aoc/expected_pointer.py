# Copyright 2019-2020 the openage authors. See copying.md for legal info.

"""
Expected pointers reference an object that is not created yet.
This can be utilized to avoid cyclic dependencies like A->B
while B->A during conversion. The pointer can be resolved
once the object has been created.
"""


class ExpectedPointer:

    def __init__(self, converter_object_group_ref, raw_api_object_ref):
        """
        Creates an expected pointer to a RawAPIObject that will be created
        by a converter object group.

        :param converter_object_group_ref: ConverterObjectGroup where the nyan object will be created.
        :type converter_object_group_ref: ConverterObjectGroup
        :param raw_api_object_ref: Name of the raw API object.
        :type raw_api_object_ref: str
        """

        self.group_object = converter_object_group_ref
        self.raw_api_object_name = raw_api_object_ref

    def resolve(self):
        """
        Returns the nyan object reference for the pointer.
        """
        raw_api_obj = self.group_object.get_raw_api_object(self.raw_api_object_name)

        return raw_api_obj.get_nyan_object()

    def resolve_raw(self):
        """
        Returns the raw API object reference for the pointer.
        """
        return self.group_object.get_raw_api_object(self.raw_api_object_name)

    def __repr__(self):
        return "ExpectedPointer<%s>" % (self.raw_api_object_name)
