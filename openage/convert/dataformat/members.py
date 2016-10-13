# Copyright 2014-2016 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R,abstract-method

import types
from enum import Enum

from .content_snippet import ContentSnippet, SectionType
from .entry_parser import EntryParser
from .generated_file import GeneratedFile
from .struct_snippet import StructSnippet
from .util import determine_headers, determine_header


class DataMember:
    """
    member variable of data files and generated structs.

    equals:
    * one column in a csv file.
    * member in the C struct
    * data field in the .dat file
    """
    def __init__(self):
        self.length = 1
        self.raw_type = None
        self.do_raw_read = True

    def get_parsers(self, idx, member):
        raise NotImplementedError("implement the parser generation for the member type %s" % type(self))

    def get_headers(self, output_target):
        raise NotImplementedError("return needed headers for %s for a given output target" % type(self))

    def get_typerefs(self):
        """
        this member entry references these types.
        this is most likely the type name of the corresponding struct entry.
        """
        return set()

    def entry_hook(self, data):
        """
        allows the data member class to modify the input data

        is used e.g. for the number => enum lookup
        """

        return data

    def get_effective_type(self):
        raise NotImplementedError("return the effective (struct) type of member %s" % type(self))

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

    def get_struct_entries(self, member_name):
        """
        return the lines to put inside the C struct.
        """

        return ["%s %s;" % (self.get_effective_type(), member_name)]

    def format_hash(self, hasher):
        """
        hash these member's settings.

        used to determine data format changes.
        """
        raise NotImplementedError("return the hasher updated with member settings")

    def __repr__(self):
        raise NotImplementedError("return short description of the member type %s" % (type(self)))


class GroupMember(DataMember):
    """
    member that references to another class, pretty much like the SubdataMember,
    but with a fixed length of 1.

    this is just a reference to a single struct instance.
    """

    def __init__(self, cls):
        super().__init__()
        self.cls = cls

    def get_headers(self, output_target):
        return {self.cls.name_struct_file}

    def get_typerefs(self):
        return {self.get_effective_type()}

    def get_effective_type(self):
        return self.cls.get_effective_type()

    def get_parsers(self, idx, member):
        # TODO: new type of csv file, probably go for yaml...
        return [
            EntryParser(
                ["this->%s.fill(buf[%d]);" % (member, idx)],
                headers     = set(),
                typerefs    = set(),
                destination = "fill",
            )
        ]

    def format_hash(self, hasher):
        return self.cls.format_hash(hasher)

    def __repr__(self):
        return "GroupMember<%s>" % repr(self.cls)


class IncludeMembers(GroupMember):
    """
    a member that requires evaluating the given class
    as a include first.

    example:
    the unit class "building" and "movable" will both have
    common members that have to be read first.
    """

    def __init__(self, cls):
        super().__init__(cls)

    def get_parsers(self, idx, member):
        raise Exception("this should never be called!")

    def __repr__(self):
        return "IncludeMember<%s>" % repr(self.cls)


class DynLengthMember(DataMember):
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
            raise Exception("invalid length type passed to %s: %s<%s>" % (type(self), length, type(length)))

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
                # self.length specifies the attribute name where the length is stored
                length_def = self.length

            # look up the given member name and return the value.
            if not isinstance(length_def, str):
                raise Exception("length lookup definition is not str: %s<%s>" % (length_def, type(length_def)))

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
            raise Exception("unknown length definition supplied: %s" % target)

    def format_hash(self, hasher):
        if isinstance(self.length, types.LambdaType):
            # update hash with the lambda code
            # pylint: disable=no-member
            hasher.update(self.length.__code__.co_code)
        else:
            hasher.update(str(self.length).encode())

        return hasher


class RefMember(DataMember):
    """
    a struct member that can be referenced/references another struct.
    """

    def __init__(self, type_name, file_name):
        DataMember.__init__(self)
        self.type_name = type_name
        self.file_name = file_name

        # xrefs not supported yet.
        # would allow reusing a struct definition that lies in another file
        self.resolved  = False

    def format_hash(self, hasher):
        # the file_name is irrelevant for the format hash
        # engine-internal relevance only.

        # type name is none for subdata members, hash is determined
        # by recursing into the subdata member itself.
        if self.type_name:
            hasher.update(self.type_name.encode())

        return hasher


class NumberMember(DataMember):
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
            raise Exception("created number column from unknown type %s" % number_def)

        self.number_type = number_def
        self.raw_type    = number_def

    def get_parsers(self, idx, member):
        scan_symbol = self.type_scan_lookup[self.number_type]

        return [
            EntryParser(
                ["if (sscanf(buf[%d], \"%%%s\", &this->%s) != 1) { return %d; }" % (idx, scan_symbol, member, idx)],
                headers     = determine_header("sscanf"),
                typerefs    = set(),
                destination = "fill",
            )
        ]

    def get_headers(self, output_target):
        if "struct" == output_target:
            return determine_header(self.number_type)
        else:
            return set()

    def get_effective_type(self):
        return self.number_type

    def format_hash(self, hasher):
        hasher.update(self.number_type.encode())

        return hasher

    def __repr__(self):
        return self.number_type


# TODO: convert to KnownValueMember
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
    ABORT    = "data_absent"
    CONTINUE = "data_exists"

    def __str__(self):
        return self.value


class ContinueReadMember(NumberMember):
    """
    data field that aborts reading further members of the class
    when its value == 0.
    """

    Result = ContinueReadMemberResult

    def __init__(self, raw_type):
        super().__init__(raw_type)

    def entry_hook(self, data):
        if data == 0:
            return self.Result.ABORT
        else:
            return self.Result.CONTINUE

    def get_empty_value(self):
        return 0

    def get_parsers(self, idx, member):
        entry_parser_txt = (
            "//remember if the following members are undefined",
            "if (0 == strcmp(buf[%d], \"%s\")) {" % (idx, self.Result.ABORT.value),
            "\tthis->%s = 0;" % (member),
            "} else if (0 == strcmp(buf[%d], \"%s\")) {" % (idx, self.Result.CONTINUE.value),
            "\tthis->%s = 1;" % (member),
            "} else {",
            "\tthrow openage::error::Error(MSG(err) << \"unexpected value '\" << buf[%d] << \"' for %s\");" % (idx, self.__class__.__name__),
            "}",
        )

        return [
            EntryParser(
                entry_parser_txt,
                headers     = determine_headers(("strcmp", "engine_error")),
                typerefs    = set(),
                destination = "fill",
            )
        ]


class EnumMember(RefMember):
    """
    this struct member/data column is a enum.
    """

    def __init__(self, type_name, values, file_name=None):
        super().__init__(type_name, file_name)
        self.values    = values
        self.resolved  = True    # TODO, xrefs not supported yet.

    def get_parsers(self, idx, member):
        enum_parse_else = ""
        enum_parser = list()
        enum_parser.append("// parse enum %s" % (self.type_name))
        for enum_value in self.values:
            enum_parser.extend([
                "%sif (0 == strcmp(buf[%d], \"%s\")) {" % (enum_parse_else, idx, enum_value),
                "\tthis->%s = %s::%s;"                  % (member, self.type_name, enum_value),
                "}",
            ])
            enum_parse_else = "else "

        enum_parser.extend([
            "else {",
            "\tthrow openage::error::Error(MSG(err) << \"unknown enum value '\" << buf[%d] << \"' encountered. valid are: %s\\n---\\nIf this is an inconsistency due to updates in the media converter, try re-converting the media\\n---\");" % (idx, ",".join(self.values)),
            "}",
        ])

        return [
            EntryParser(
                enum_parser,
                headers     = determine_headers(("strcmp", "engine_error")),
                typerefs    = set(),
                destination = "fill",
            )
        ]

    def get_headers(self, output_target):
        return set()

    def get_typerefs(self):
        return {self.get_effective_type()}

    def get_effective_type(self):
        return self.type_name

    def validate_value(self, value):
        return value in self.values

    def get_snippets(self, file_name, format_):
        """
        generate enum snippets from given data

        input: EnumMember
        output: ContentSnippet
        """

        if format_ == "struct":
            snippet_file_name = self.file_name or file_name

            txt = list()

            # create enum definition
            txt.extend([
                "enum class %s {\n\t" % self.type_name,
                ",\n\t".join(self.values),
                "\n};\n\n",
            ])

            snippet = ContentSnippet(
                "".join(txt),
                snippet_file_name,
                SectionType.section_body,
                orderby=self.type_name,
                reprtxt="enum class %s" % self.type_name,
            )
            snippet.typedefs |= {self.type_name}

            return [snippet]
        else:
            return list()

    def format_hash(self, hasher):
        hasher = super().format_hash(hasher)

        for v in sorted(self.values):
            hasher.update(v.encode())

        return hasher

    def __repr__(self):
        return "enum %s" % self.type_name


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
                h = " = %s" % hex(data)
            except TypeError:
                h = ""
            raise Exception("failed to find %s%s in lookup dict %s!" % (str(data), h, self.type_name)) from None


class CharArrayMember(DynLengthMember):
    """
    struct member/column type that allows to store equal-length char[n].
    """

    def __init__(self, length):
        super().__init__(length)
        self.raw_type = "char[]"

    def get_parsers(self, idx, member):
        headers = set()

        if self.is_dynamic_length():
            lines = ["this->%s = buf[%d];" % (member, idx)]
        else:
            data_length = self.get_length()
            lines = [
                "strncpy(this->%s, buf[%d], %d); this->%s[%d] = '\\0';" % (
                    member, idx, data_length, member, data_length - 1
                )
            ]
            headers |= determine_header("strncpy")

        return [
            EntryParser(
                lines,
                headers     = headers,
                typerefs    = set(),
                destination = "fill",
            )
        ]

    def get_headers(self, output_target):
        ret = set()

        if "struct" == output_target:
            if self.is_dynamic_length():
                ret |= determine_header("std::string")

        return ret

    def get_effective_type(self):
        if self.is_dynamic_length():
            return "std::string"
        else:
            return "char"

    def get_empty_value(self):
        return ""

    def __repr__(self):
        return "%s[%s]" % (self.get_effective_type(), self.length)


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

    def __init__(self, type_name, subtype_definition, class_lookup, length, passed_args=None, ref_to=None, offset_to=None, file_name=None, ref_type_params=None):
        RefMember.__init__(self, type_name, file_name)
        DynLengthMember.__init__(self, length)

        self.subtype_definition = subtype_definition  # to determine the subtype for each entry, read this value to do the class_lookup
        self.class_lookup       = class_lookup        # dict to look up type_name => exportable class
        self.passed_args        = passed_args         # list of member names whose values will be passed to the new class
        self.ref_to             = ref_to              # add this member name's value to the filename
        self.offset_to          = offset_to           # link to member name which is a list of binary file offsets
        self.ref_type_params    = ref_type_params     # dict to specify type_name => constructor arguments

        # no xrefs supported yet..
        self.resolved          = True

    def get_headers(self, output_target):
        if "struct" == output_target:
            return determine_header("std::vector")
        elif "structimpl" == output_target:
            return determine_header("read_csv_file")
        else:
            return set()

    def get_effective_type(self):
        return self.type_name

    def get_empty_value(self):
        return list()

    def get_contained_types(self):
        return {
            contained_type.get_effective_type()
            for contained_type in self.class_lookup.values()
        }

    def get_parsers(self, idx, member):
        return [
            EntryParser(
                ["this->%s.index_file.filename = buf[%d];" % (member, idx)],
                headers     = set(),
                typerefs    = set(),
                destination = "fill",
            ),
            EntryParser(
                ["this->%s.recurse(basedir);" % (member)],
                headers     = set(),
                typerefs    = set(),
                destination = "recurse",
            )

        ]

    def get_typerefs(self):
        return {self.type_name}

    def get_snippets(self, file_name, format_):
        """
        return struct definitions for this type
        """

        from .multisubtype_base import MultisubtypeBaseFile

        snippet_file_name = self.file_name or file_name

        if format_ == "struct":
            snippet = StructSnippet(snippet_file_name, self.type_name)

            for (entry_name, entry_type) in sorted(self.class_lookup.items()):
                entry_type = entry_type.get_effective_type()
                snippet.add_member(
                    "struct openage::util::subdata<%s> %s;" % (
                        GeneratedFile.namespacify(entry_type), entry_name
                    )
                )
                snippet.typerefs |= {entry_type}

            snippet.includes |= determine_header("subdata")
            snippet.typerefs |= {MultisubtypeBaseFile.name_struct}
            snippet.add_member("struct openage::util::subdata<%s> index_file;\n" % (MultisubtypeBaseFile.name_struct))

            from .data_formatter import DataFormatter
            snippet.add_members((
                "%s;" % m.get_signature()
                for _, m in sorted(DataFormatter.member_methods.items())
            ))

            return [snippet]

        elif format_ == "structimpl":
            # TODO: generalize this member function generation...

            txt = list()
            txt.extend((
                "int %s::fill(char * /*line*/) {\n" % (self.type_name),
                "\treturn -1;\n",
                "}\n",
            ))

            # function to recursively read the referenced files
            txt.extend((
                "int %s::recurse(openage::util::Dir basedir) {\n" % (self.type_name),
                "\tthis->index_file.read(basedir); //read ref-file entries\n",
                "\tint subtype_count = this->index_file.data.size();\n"
                "\tif (subtype_count != %s) {\n" % len(self.class_lookup),
                "\t\tthrow openage::error::Error(MSG(err) << \"multisubtype index file entry count mismatched! \" << subtype_count << \" != %d\");\n" % (len(self.class_lookup)),
                "\t}\n\n",
                "\topenage::util::Dir new_basedir = basedir.append(openage::util::dirname(this->index_file.filename));\n",
                "\tint idx = -1, idxtry;\n\n",
                "\t//yes, the following code can be heavily optimized and converted to member methods..\n",
            ))

            for (entry_name, entry_type) in sorted(self.class_lookup.items()):
                # get the index_file data index of the current entry first
                txt.extend((
                    "\tidxtry = 0;\n",
                    "\tfor (auto &file_reference : this->index_file.data) {\n",
                    "\t\tif (file_reference.subtype == \"%s\") {\n" % (entry_name),
                    "\t\t\t\tidx = idxtry;\n",
                    "\t\t\tbreak;\n",
                    "\t\t}\n",
                    "\t\tidxtry += 1;\n",
                    "\t}\n",
                    "\tif (idx == -1) {\n",
                    "\t\tthrow openage::error::Error(MSG(err) << \"multisubtype index file contains no entry for %s!\");\n" % (entry_name),
                    "\t}\n",
                    "\tthis->%s.filename = this->index_file.data[idx].filename;\n" % (entry_name),
                    "\tthis->%s.read(new_basedir);\n" % (entry_name),
                    "\tidx = -1;\n\n"
                ))
            txt.append("\treturn -1;\n}\n")

            snippet = ContentSnippet(
                "".join(txt),
                snippet_file_name,
                SectionType.section_body,
                orderby=self.type_name,
                reprtxt="multisubtype %s container fill function" % self.type_name,
            )
            snippet.typerefs |= self.get_contained_types() | {self.type_name, MultisubtypeBaseFile.name_struct}
            snippet.includes |= determine_headers(("engine_dir", "engine_error"))

            return [snippet]

        else:
            return list()

    def format_hash(self, hasher):
        hasher = RefMember.format_hash(self, hasher)
        hasher = DynLengthMember.format_hash(self, hasher)

        for _, subtype_class in sorted(self.class_lookup.items()):
            hasher = subtype_class.format_hash(hasher)

        return hasher

    def __repr__(self):
        return "MultisubtypeMember<%s:len=%s>" % (self.type_name, self.length)


class SubdataMember(MultisubtypeMember):
    """
    struct member/data column that references to one another data set.
    """

    def __init__(self, ref_type, length, offset_to=None, ref_to=None, ref_type_params=None, passed_args=None):
        super().__init__(
            type_name          = None,
            subtype_definition = None,
            class_lookup       = {None: ref_type},
            length             = length,
            offset_to          = offset_to,
            ref_to             = ref_to,
            ref_type_params    = {None: ref_type_params},
            passed_args        = passed_args,
        )

    def get_headers(self, output_target):
        if "struct" == output_target:
            return determine_header("subdata")
        else:
            return set()

    def get_subtype(self):
        return GeneratedFile.namespacify(tuple(self.get_contained_types())[0])

    def get_effective_type(self):
        return "openage::util::subdata<%s>" % (self.get_subtype())

    def get_parsers(self, idx, member):
        return [
            EntryParser(
                ["this->%s.filename = buf[%d];" % (member, idx)],
                headers     = set(),
                typerefs    = set(),
                destination = "fill",
            ),
            EntryParser(
                ["this->%s.read(basedir);" % (member)],
                headers     = set(),
                typerefs    = set(),
                destination = "recurse",
            ),
        ]

    def get_snippets(self, file_name, format_):
        del file_name, format_  # unused
        return list()

    def get_typerefs(self):
        return self.get_contained_types()

    def get_subdata_type_name(self):
        return self.class_lookup[None].__name__

    def __repr__(self):
        return "SubdataMember<%s:len=%s>" % (self.get_subdata_type_name(), self.length)


class ArrayMember(SubdataMember):
    """
    autogenerated subdata member for arrays like float[8].
    """

    def __init__(self, ref_type, length, ref_type_params=None):
        super().__init__(ref_type, length)

    def __repr__(self):
        return "ArrayMember<%s:len=%s>" % (self.get_subdata_type_name(), self.length)
