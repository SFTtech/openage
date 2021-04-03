# Copyright 2014-2021 the openage authors. See copying.md for legal info.
#
# TODO pylint: disable=C,R,abstract-method

from enum import Enum


class ReadMember:
    """
    member variable of data files and generated structs.

    equals:
    * data field in the .dat file
    """

    def __init__(self):
        self.length = 1
        self.raw_type = None
        self.do_raw_read = True

    def entry_hook(self, data):
        """
        allows the data member class to modify the input data

        is used e.g. for the number => enum lookup
        """
        return data

    def get_empty_value(self):
        """
        when this data field is not filled, use the returned value instead.
        """
        return 0

    def get_length(self, obj=None):
        del obj  # unused
        return self.length

    def verify_read_data(self, obj, data):
        """
        gets called for each entry. used to check for storage validity (e.g. 0 expected)
        """
        del obj, data  # unused
        return True

    def __repr__(self):
        raise NotImplementedError(
            f"return short description of the member type {type(self)}")


class GroupMember(ReadMember):
    """
    member that references to another class, pretty much like the SubdataMember,
    but with a fixed length of 1.

    this is just a reference to a single struct instance.
    """

    def __init__(self, cls):
        super().__init__()
        self.cls = cls

    def __repr__(self):
        return f"GroupMember<{repr(self.cls)}>"


class IncludeMembers(GroupMember):
    """
    a member that requires evaluating the given class
    as a include first.

    example:
    the unit class "building" and "movable" will both have
    common members that have to be read first.
    """

    def __repr__(self):
        return f"IncludeMember<{repr(self.cls)}>"


class DynLengthMember(ReadMember):
    """
    a member that can have a dynamic length.
    """

    any_length = "any_length"

    def __init__(self, length):
        super().__init__()

        type_ok = False

        if isinstance(length, int) or isinstance(length, str) or (length is self.any_length):
            type_ok = True

        if callable(length):
            type_ok = True

        if not type_ok:
            raise Exception("invalid length type passed to %s: %s<%s>" % (
                type(self), length, type(length)))

        self.length = length

    def get_length(self, obj=None):
        if self.is_dynamic_length():
            if self.length is self.any_length:
                return self.any_length

            if not obj:
                raise Exception("dynamic length query requires source object")

            if callable(self.length):
                # length is a lambda that determines the length by some fancy manner
                # pass the target object as lambda parameter.
                length_def = self.length(obj)

                # if the lambda returns a non-dynamic length (aka a number)
                # return it directly. otherwise, the returned variable name
                # has to be looked up again.
                if not self.is_dynamic_length(target=length_def):
                    return length_def

            else:
                # self.length specifies the attribute name where the length is
                # stored
                length_def = self.length

            # look up the given member name and return the value.
            if not isinstance(length_def, str):
                raise Exception("length lookup definition is not str: %s<%s>" % (
                    length_def, type(length_def)))

            return getattr(obj, length_def)

        else:
            # non-dynamic length (aka plain number) gets returned directly
            return self.length

    def is_dynamic_length(self, target=None):
        if target is None:
            target = self.length

        if target is self.any_length:
            return True
        elif isinstance(target, str):
            return True
        elif isinstance(target, int):
            return False
        elif callable(target):
            return True
        else:
            raise Exception(f"unknown length definition supplied: {target}")


class RefMember(ReadMember):
    """
    a struct member that can be referenced/references another struct.
    """

    def __init__(self, type_name, file_name):
        ReadMember.__init__(self)
        self.type_name = type_name
        self.file_name = file_name

        # xrefs not supported yet.
        # would allow reusing a struct definition that lies in another file
        self.resolved = False


class NumberMember(ReadMember):
    """
    this struct member/data column contains simple numbers
    """

    # primitive types, directly parsable by sscanf
    type_scan_lookup = {
        "char":          "hhd",
        "int8_t":        "hhd",
        "uint8_t":       "hhu",
        "int16_t":       "hd",
        "uint16_t":      "hu",
        "int":           "d",
        "int32_t":       "d",
        "uint":          "u",
        "uint32_t":      "u",
        "float":         "f",
    }

    def __init__(self, number_def):
        super().__init__()
        if number_def not in self.type_scan_lookup:
            raise Exception(
                f"created number column from unknown type {number_def}")

        # type used for the output struct
        self.number_type = number_def
        self.raw_type = number_def

    def __repr__(self):
        return self.number_type


class ZeroMember(NumberMember):
    """
    data field that is known to always needs to be zero.
    neat for finding offset errors.
    """

    def __init__(self, raw_type, length=1):
        super().__init__(raw_type)
        self.length = length

    def verify_read_data(self, obj, data):
        # fail if a single value of data != 0
        if any(False if v == 0 else True for v in data):
            return False
        else:
            return True


class ContinueReadMemberResult(Enum):
    ABORT = "data_absent"
    CONTINUE = "data_exists"

    def __str__(self):
        return str(self.value)


class ContinueReadMember(NumberMember):
    """
    data field that aborts reading further members of the class
    when its value == 0.
    """

    Result = ContinueReadMemberResult

    def entry_hook(self, data):
        if data == 0:
            return self.Result.ABORT
        else:
            return self.Result.CONTINUE

    def get_empty_value(self):
        return 0


class EnumMember(RefMember):
    """
    this struct member/data column is a enum.
    """

    def __init__(self, type_name, values, file_name=None):
        super().__init__(type_name, file_name)
        self.values = values
        self.resolved = True    # TODO, xrefs not supported yet.

    def validate_value(self, value):
        return value in self.values

    def __repr__(self):
        return f"enum {self.type_name}"


class EnumLookupMember(EnumMember):
    """
    enum definition, does lookup of raw datfile data => enum value
    """

    def __init__(self, type_name, lookup_dict, raw_type, file_name=None):
        super().__init__(
            type_name,
            [v for k, v in sorted(lookup_dict.items())],
            file_name
        )
        self.lookup_dict = lookup_dict
        self.raw_type = raw_type

    def entry_hook(self, data):
        """
        perform lookup of raw data -> enum member name
        """

        try:
            return self.lookup_dict[data]
        except KeyError:
            try:
                h = f" = {hex(data)}"
            except TypeError:
                h = ""
            raise Exception("failed to find %s%s in lookup dict %s!" %
                            (str(data), h, self.type_name)) from None


class CharArrayMember(DynLengthMember):
    """
    struct member/column type that allows to store equal-length char[n].
    """

    def __init__(self, length):
        super().__init__(length)
        self.raw_type = "char[]"

    def get_empty_value(self):
        return ""

    def __repr__(self):
        return f"{self.raw_type}[{self.length}]"


class StringMember(CharArrayMember):
    """
    member with unspecified string length, aka std::string
    """

    def __init__(self):
        super().__init__(DynLengthMember.any_length)


class MultisubtypeMember(RefMember, DynLengthMember):
    """
    struct member/data column that groups multiple references to
    multiple other data sets.
    """

    def __init__(self, type_name, subtype_definition, class_lookup, length,
                 passed_args=None, ref_to=None,
                 offset_to=None, file_name=None,
                 ref_type_params=None):

        RefMember.__init__(self, type_name, file_name)
        DynLengthMember.__init__(self, length)

        # to determine the subtype for each entry,
        # read this value to do the class_lookup
        self.subtype_definition = subtype_definition

        # dict to look up type_name => exportable class
        self.class_lookup = class_lookup

        # list of member names whose values will be passed to the new class
        self.passed_args = passed_args

        # add this member name's value to the filename
        self.ref_to = ref_to

        # link to member name which is a list of binary file offsets
        self.offset_to = offset_to

        # dict to specify type_name => constructor arguments
        self.ref_type_params = ref_type_params

        # no xrefs supported yet.. just set to true as if they were resolved.
        self.resolved = True

    def get_empty_value(self):
        return list()

    def get_contained_types(self):
        return {
            contained_type.get_effective_type()
            for contained_type in self.class_lookup.values()
        }

    def __repr__(self):
        return f"MultisubtypeMember<{self.type_name}:len={self.length}>"


class SubdataMember(MultisubtypeMember):
    """
    Struct member/data column that references to just one another data set.
    It's a special case of the multisubtypemember with one subtype.
    """

    def __init__(self, ref_type, length, offset_to=None,
                 ref_to=None, ref_type_params=None, passed_args=None):
        super().__init__(
            type_name=None,
            subtype_definition=None,
            class_lookup={None: ref_type},
            length=length,
            offset_to=offset_to,
            ref_to=ref_to,
            ref_type_params={None: ref_type_params},
            passed_args=passed_args,
        )

    def get_subdata_type_name(self):
        return self.class_lookup[None].__name__

    def __repr__(self):
        return f"SubdataMember<{self.get_subdata_type_name()}:len={self.length}>"


class ArrayMember(DynLengthMember):
    """
    subdata member for C-type arrays like float[8].
    """

    def __init__(self, raw_type, length):
        super().__init__(length)
        self.raw_type = raw_type

    def __repr__(self):
        return f"ArrayMember<{self.raw_type}:len={self.length}>"
