# Copyright 2019-2023 the openage authors. See copying.md for legal info.

"""
Forward references point to an object that is not created yet.
This can be utilized to avoid cyclic dependencies like A->B
while B->A during conversion. The pointer can be resolved
once the object has been created.
"""

from __future__ import annotations
import typing


if typing.TYPE_CHECKING:
    from openage.convert.entity_object.conversion.converter_object import ConverterObjectGroup, \
        RawAPIObject
    from openage.nyan.nyan_structs import NyanObject


class ForwardRef:
    """
    Declares a forward reference to a RawAPIObject.
    """

    __slots__ = ('group_object', 'raw_api_object_ref')

    def __init__(self, converter_object_group: ConverterObjectGroup, raw_api_object_ref: str):
        """
        Creates a forward reference to a RawAPIObject that will be created
        by a converter object group.

        :param converter_object_group: ConverterObjectGroup where the RawAPIObject
                                       will be created.
        :type converter_object_group: ConverterObjectGroup
        :param raw_api_object_ref: Reference of the RawAPIObject in the group.
        :type raw_api_object_ref: str
        """

        self.group_object = converter_object_group
        self.raw_api_object_ref = raw_api_object_ref

    def resolve(self) -> NyanObject:
        """
        Returns the nyan object reference for the pointer.
        """
        raw_api_obj = self.group_object.get_raw_api_object(self.raw_api_object_ref)

        return raw_api_obj.get_nyan_object()

    def resolve_raw(self) -> RawAPIObject:
        """
        Returns the raw API object reference for the pointer.
        """
        return self.group_object.get_raw_api_object(self.raw_api_object_ref)

    def __repr__(self):
        return f"ForwardRef<{self.raw_api_object_ref}>"
