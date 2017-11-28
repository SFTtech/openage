# Copyright 2015-2016 the openage authors. See copying.md for legal info.

"""
Provides some classes designed to expand the functionality of struct.struct
"""


from collections import OrderedDict
from struct import Struct

from ..util.files import read_guaranteed


class NamedStructMeta(type):
    """
    Metaclass for NamedStruct.

    Not unlike the meta-class for Enum, processes all the member attributes
    at class-creation time.
    """
    @classmethod
    def __prepare__(mcs, name, bases, **kwds):
        del mcs, name, bases, kwds  # unused variables

        return OrderedDict()

    def __new__(mcs, name, bases, classdict, **kwds):
        del kwds  # unused variable

        specstr = None
        attributes = []
        postprocessors = {}

        for membername, value in classdict.items():
            # ignore hidden and None members
            if membername.startswith('_') or value is None:
                continue

            valuehasspecstr = hasattr(value, "specstr")

            # ignore member methods
            if not valuehasspecstr:
                if callable(value) or isinstance(value, classmethod):
                    continue

            if membername == 'endianness':
                if specstr is not None:
                    raise Exception("endianness has been given multiple times")

                if value not in "@=<>!":
                    raise ValueError("endianess: expected one of @=<>!")

                specstr = value
                continue

            if specstr is None:
                raise Exception("NamedStruct: endianness expected before "
                                "attribute " + membername)

            if valuehasspecstr:
                postprocessors[membername], value = value, value.specstr
            elif isinstance(value, str):
                pass
            else:
                raise TypeError(
                    "NamedStruct member %s: expected str, but got %s"
                    % (membername, repr(value)))

            specstr += value

            attributes.append(membername)

        classdict["_attributes"] = attributes
        classdict["_postprocessors"] = postprocessors
        if specstr:
            classdict["_struct"] = Struct(specstr)

        return type.__new__(mcs, name, bases, dict(classdict))


class NamedStruct(metaclass=NamedStructMeta):
    """
    Designed to be inherited from, similar to Enum.

    Specify all fields of the struct, as 'membername = specstr',
    where specstr is a string describing the field, as in struct.Struct.
    NamedStructMeta translates those individual specstr fragments to
    a complete specstr.

    Alternatively to a specstr, a callable object with a specstr member
    may be passed; the specstr is used as usual, but afterwards the
    callable is invoked to post-process the extracted data.
    One example for such a callable is the Flags class.

    Alternatively, attributes may be set to None; those are ignored,
    and may be set manually at some later point.

    The first member must be 'endianess'.

    Example:

    class MyStruct(NamedStruct):
        endianness = "<"

        mgck = "4s"
        test = "I"
        rofl = "H"
        flag = MyFlagType

    The constructor takes a bytes object of the appropriate length, and fills
    in all the members with the struct's actual values.
    """

    # those values are set by the metaclass.
    _postprocessors = None
    _struct = None
    _attributes = None

    def __init__(self, data):
        if not self._struct:
            raise NotImplementedError(
                "Abstract NamedStruct can not be instantiated")

        values = self._struct.unpack(data)

        if len(self._attributes) != len(values):
            raise Exception("internal error: "
                            "number of attributes differs from number of "
                            "struct fields")

        for name, value in zip(self._attributes, values):
            # pylint: disable=unsupported-membership-test
            if name in self._postprocessors:
                # pylint: disable=unsubscriptable-object
                value = self._postprocessors[name](value)

            setattr(self, name, value)

    @classmethod
    def unpack(cls, data):
        """
        Unpacks data and returns a NamedStruct object that holds the fields.
        """
        return cls(data)

    @classmethod
    def size(cls):
        """
        Returns the size of the struct, in bytes.
        """
        return cls._struct.size

    @classmethod
    def read(cls, fileobj):
        """
        Reads the appropriate amount of data from fileobj, and unpacks it.
        """
        data = read_guaranteed(fileobj, cls._struct.size)
        return cls.unpack(data)

    @classmethod
    def from_nullbytes(cls):
        """
        Decodes nullbytes (sort of a 'default' value).
        """
        data = b"\x00" * cls._struct.size
        return cls.unpack(data)

    # nobody has needed .pack() and .write() functions this far; implement
    # them if you need them.

    def __len__(self):
        """
        Returns the number of fields.
        """
        return len(self._attributes)

    def __getitem__(self, index):
        """
        Returns the n-th field, or raises IndexError.
        """
        # pylint: disable=unsubscriptable-object
        return getattr(self, self._attributes[index])

    def as_dict(self):
        """
        Returns a key-value dict for all attributes.
        """
        # pylint: disable=not-an-iterable
        return {attr: getattr(self, attr) for attr in self._attributes}

    def __iter__(self):
        return iter(self)

    def __repr__(self):
        return str(type(self)) + ": " + repr(self.as_dict())

    def __str__(self):
        return type(self).__name__ + ":\n\t" + "\n\t".join(
            str(key).ljust(20) + " = " + str(value)
            for key, value in sorted(self.as_dict().items())
        )


class FlagsMeta(type):
    """
    Metaclass for Flags. Compare to NamedStructMeta.
    """
    def __new__(mcs, name, bases, classdict, **kwds):
        del kwds  # unused variable

        # we don't need to know the order of the flags, so we don't need
        # to do the whole 'OrderedDict' dance.

        # stores a mapping of flag value <-> flag name
        flags = {}
        specstr_found = False

        for membername, value in classdict.items():
            if membername.startswith('_'):
                continue

            if membername == "specstr":
                specstr_found = True

                if not isinstance(value, str):
                    raise TypeError(
                        "expected str as value for specstr, "
                        "but got " + repr(value))

                continue

            if callable(value) or isinstance(value, classmethod):
                continue

            if not isinstance(value, int):
                raise TypeError(
                    "expected int as value for flag " + membername + ", "
                    "but got " + repr(value))

            flagvalue = 1 << value
            flags[flagvalue] = membername

        if flags and not specstr_found:
            raise Exception("expected a 'specstr' attribute")

        classdict["_flags"] = flags

        return type.__new__(mcs, name, bases, classdict)


class Flags(metaclass=FlagsMeta):
    """
    Designed to be inherited from, similar to Enum.

    Used to generate flag parsers (for boolean flags that
    are stored in an integer value).

    Specify the bit numbers of all possible flags as attributes,
    e.g.:

    class MyFlags(Flags):
        thisflag = 0
        thatflag = 1

    The constructor of the class takes an integer argument,
    which is parsed; all the boolean values are stored in the
    attributes.
    If any unknown bits are set, self.unknown() is called.
    """

    # set by the metaclass
    _flags = None

    def __init__(self, val):
        for flagvalue, flagname in self._flags.items():
            if val & flagvalue:
                setattr(self, flagname, True)
                val &= ~flagvalue
            else:
                setattr(self, flagname, False)

        if val:
            self.unknown(val)

    def unknown(self, unknownflags):
        """
        Default handler for any unknown bits. Overload if needed.
        """
        raise ValueError(
            "unknown flag values: " + bin(unknownflags) + " "
            "in addition to existing flags: " + str(self.as_dict()))

    def as_dict(self):
        """
        Returns a key-value dict for all flags.
        """
        return {flagname: getattr(self, flagname)
                for flagname in self._flags.values()}

    def __repr__(self):
        return repr(type(self)) + ": " + repr(self.as_dict())
