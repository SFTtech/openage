# Copyright 2014-2019 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R,abstract-method

import types
from enum import Enum

from .content_snippet import ContentSnippet, SectionType
from .entry_parser import EntryParser
from .generated_file import GeneratedFile
from .struct_snippet import StructSnippet
from .util import determine_headers, determine_header


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

    def get_parsers(self, idx, member):
        raise NotImplementedError(
            "implement the parser generation for the member type %s" % type(self))

    def get_headers(self, output_target):
        raise NotImplementedError(
            "return needed headers for %s for a given output target" % type(self))

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
        raise NotImplementedError(
            "return the effective (struct) type of member %s" % type(self))

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
        raise NotImplementedError(
            "return the hasher updated with member settings")

    def __repr__(self):
        raise NotImplementedError(
            "return short description of the member type %s" % (type(self)))


class GroupMember(ReadMember):
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
                headers=set(),
                typerefs=set(),
                destination="fill",
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

    def get_parsers(self, idx, member):
        raise Exception("this should never be called!")

    def __repr__(self):
        return "IncludeMember<%s>" % repr(self.cls)


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
            raise Exception("unknown length definition supplied: %s" % target)

    def format_hash(self, hasher):
        if isinstance(self.length, types.LambdaType):
            # update hash with the lambda code
            # pylint: disable=no-member
            hasher.update(self.length.__code__.co_code)
        else:
            hasher.update(str(self.length).encode())

        return hasher


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

    def format_hash(self, hasher):
        # the file_name is irrelevant for the format hash
        # engine-internal relevance only.

        # type name is none for subdata members, hash is determined
        # by recursing into the subdata member itself.
        if self.type_name:
            hasher.update(self.type_name.encode())

        return hasher


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
                "created number column from unknown type %s" % number_def)

        # type used for the output struct
        self.number_type = number_def
        self.raw_type = number_def

    def get_parsers(self, idx, member):
        scan_symbol = self.type_scan_lookup[self.number_type]

        return [
            EntryParser(
                ["if (sscanf(buf[%d].c_str(), \"%%%s\", &this->%s) != 1) "
                 "{ return %d; }" % (idx, scan_symbol, member, idx)],
                headers=determine_header("sscanf"),
                typerefs=set(),
                destination="fill",
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
    ABORT = "data_absent"
    CONTINUE = "data_exists"

    def __str__(self):
        return self.value


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

    def get_parsers(self, idx, member):
        entry_parser_txt = (
            "// remember if the following members are undefined",
            'if (buf[%d] == "%s") {' % (idx, self.Result.ABORT.value),
            "    this->%s = 0;" % (member),
            '} else if (buf[%d] == "%s") {' % (
                idx, self.Result.CONTINUE.value),
            "    this->%s = 1;" % (member),
            "} else {",
            ('    throw openage::error::Error(ERR << "unexpected value \'"'
             '<< buf[%d] << "\' for %s");' % (idx, self.__class__.__name__)),
            "}",
        )

        return [
            EntryParser(
                entry_parser_txt,
                headers=determine_headers(("engine_error",)),
                typerefs=set(),
                destination="fill",
            )
        ]


class EnumMember(RefMember):
    """
    this struct member/data column is a enum.
    """

    def __init__(self, type_name, values, file_name=None):
        super().__init__(type_name, file_name)
        self.values = values
        self.resolved = True    # TODO, xrefs not supported yet.

    def get_parsers(self, idx, member):
        enum_parse_else = ""
        enum_parser = list()
        enum_parser.append("// parse enum %s" % (self.type_name))
        for enum_value in self.values:
            enum_parser.extend([
                '%sif (buf[%d] == "%s") {' % (
                    enum_parse_else, idx, enum_value),
                "    this->%s = %s::%s;" % (member,
                                            self.type_name, enum_value),
                "}",
            ])
            enum_parse_else = "else "

        enum_parser.append(
            "else {{\n"
            "    throw openage::error::Error(\n"
            "        MSG(err)\n"
            '        << "unknown enum value \'" << buf[{idx}]\n'
            '        << "\' encountered. valid are: "\n'
            '           "{valids}\\n---\\n"\n'
            '           "If this is an inconsistency due to updates in the "\n'
            '           "media converter, try re-converting the assets\\n---\"\n'
            '    );'
            "}}".format(idx=idx, valids=",".join(self.values)),
        )

        return [
            EntryParser(
                enum_parser,
                headers=determine_headers(("engine_error")),
                typerefs=set(),
                destination="fill",
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
                "enum class %s {\n    " % self.type_name,
                ",\n    ".join(self.values),
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
            raise Exception("failed to find %s%s in lookup dict %s!" %
                            (str(data), h, self.type_name)) from None


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
            # copy to std::string
            lines = ["this->%s = buf[%d];" % (member, idx)]

        else:
            # copy to char[n]
            data_length = self.get_length()
            lines = [
                "strncpy(this->%s, buf[%d].c_str(), %d);" % (member,
                                                             idx, data_length),
                "this->%s[%d] = '\\0';" % (member, data_length - 1),
            ]
            headers |= determine_header("strncpy")

        return [
            EntryParser(
                lines,
                headers=headers,
                typerefs=set(),
                destination="fill",
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
            # first, the parser to just read the index file name
            EntryParser(
                ["this->%s.subdata_meta.filename = buf[%d];" % (member, idx)],
                headers=set(),
                typerefs=set(),
                destination="fill",
            ),
            # then the parser that uses the index file to recurse over
            # the "real" data entries.
            # the above parsed filename is searched in this basedir.
            EntryParser(
                ["this->%s.recurse(storage, basedir);" % (member)],
                headers=set(),
                typerefs=set(),
                destination="recurse",
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
            # all the struct info is packed in one text snippet.
            snippet = StructSnippet(snippet_file_name, self.type_name)

            # for each subdata type, add a container struct,
            # which basically stores the list of entries of that subdata.
            for (entry_name, entry_type) in sorted(self.class_lookup.items()):
                entry_type = entry_type.get_effective_type()

                snippet.add_member(
                    "struct openage::util::csv_subdata<%s> %s;" % (
                        GeneratedFile.namespacify(entry_type), entry_name
                    )
                )
                snippet.typerefs |= {entry_type}

            snippet.includes |= determine_header("csv_subdata")
            snippet.typerefs |= {MultisubtypeBaseFile.name_struct}

            # metainformation about locations and types of subdata to recurse
            # basically maps subdata type to a filename where this subdata is
            # stored
            snippet.add_member("struct openage::util::csv_subdata<%s> subdata_meta;\n" % (
                MultisubtypeBaseFile.name_struct))

            # add member methods to the struct
            from .data_formatter import DataFormatter
            snippet.add_members((
                "%s;" % member.get_signature()
                for _, member in sorted(DataFormatter.member_methods.items())
            ))

            return [snippet]

        elif format_ == "structimpl":
            # TODO: generalize this member function generation...

            txt = list()

            # function to fill up the struct contents, does nothing here.
            txt.append(
                "int {type_name}::fill(const std::string & /*line*/) {{\n"
                "    return -1;\n"
                "}}\n".format(type_name=self.type_name)
            )

            # function to recursively read the referenced files
            # this reads the metainformation for the subtypes to be read.
            #
            # this is invoked in util/csv.h:
            # CSVCollection::read
            txt.append(
                "bool {type_name}::recurse(const openage::util::CSVCollection &storage,\n"
                "{type_leng}               const std::string &basedir) {{\n"
                "\n"
                "    // the .filename was set by the previous entry parser already\n"
                "    // so now read the index-file entries\n"
                "    this->subdata_meta.read(storage, basedir);\n"
                "\n"
                "    int subtype_count = this->subdata_meta.data.size();\n"
                "    if (subtype_count != {subtype_count}) {{\n"
                "        throw openage::error::Error(\n"
                '            ERR << "multisubtype index file entry count mismatched!"\n'
                '            << subtype_count << " != {subtype_count}"\n'
                "        );\n"
                "    }}\n"
                "\n"
                "    // the recursed data files are relative to the subdata_meta filename\n"
                "    std::string metadata_dir = basedir + openage::util::fslike::PATHSEP + openage::util::dirname(this->subdata_meta.filename);\n"
                "    int idx;\n"
                "    int idxtry;\n"
                "\n".format(type_name=self.type_name,
                            type_leng=" " * len(self.type_name),
                            subtype_count=len(self.class_lookup))
            )

            for (entry_name, entry_type) in sorted(self.class_lookup.items()):
                # for each type in a multisubtype member:
                # * try to find the type name index in the metadatafile
                # * fetch the filename for that type from the metadata (by the index)
                # * fill that subdata type with information
                txt.append(
                    "    // read subtype '{entry_name}'\n"
                    "    idx = -1;\n"
                    "    idxtry = 0;\n"
                    "    // find the index of the subdata in the metadata\n"
                    "    for (auto &file_reference : this->subdata_meta.data) {{\n"
                    '        if (file_reference.subtype == "{entry_name}") {{\n'
                    "            idx = idxtry;\n"
                    "            break;\n"
                    "        }}\n"
                    "        idxtry += 1;\n"
                    "    }}\n"
                    "    if (idx == -1) {{\n"
                    "        throw openage::error::Error(\n"
                    '            ERR << "multisubtype index file contains no entry for {entry_name}!"\n'
                    "        );\n"
                    "    }}\n"
                    "\n"
                    "    // the filename is relative to the metadata file!\n"
                    "    this->{entry_name}.filename = this->subdata_meta.data[idx].filename;\n"
                    "    this->{entry_name}.read(storage, metadata_dir);\n"
                    "\n".format(entry_name=entry_name)
                )

            txt.append(
                "    return true;\n"
                "}\n"
            )

            snippet = ContentSnippet(
                "".join(txt),
                snippet_file_name,
                SectionType.section_body,
                orderby=self.type_name,
                reprtxt="multisubtype %s container fill function" % self.type_name,
            )
            snippet.typerefs |= (self.get_contained_types() |
                                 {self.type_name, MultisubtypeBaseFile.name_struct})
            snippet.includes |= determine_headers(
                ("util::Path", "engine_error", "csv_collection", "std::string")
            )

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

    def get_headers(self, output_target):
        if "struct" == output_target:
            return determine_header("csv_subdata")
        else:
            return set()

    def get_subtype(self):
        return GeneratedFile.namespacify(tuple(self.get_contained_types())[0])

    def get_effective_type(self):
        return "openage::util::csv_subdata<%s>" % (self.get_subtype())

    def get_parsers(self, idx, member):
        return [
            # to read subdata, first fetch the filename to read
            EntryParser(
                ["this->%s.filename = buf[%d];" % (member, idx)],
                headers=set(),
                typerefs=set(),
                destination="fill",
            ),
            # then read the subdata content from the storage,
            # searching for the filename relative to basedir.
            EntryParser(
                ["this->%s.read(storage, basedir);" % (member)],
                headers=set(),
                typerefs=set(),
                destination="recurse",
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


class ArrayMember(DynLengthMember):
    """
    subdata member for C-type arrays like float[8].
    """

    def __init__(self, raw_type, length):
        super().__init__(length)
        self.raw_type = raw_type

    def __repr__(self):
        return "ArrayMember<%s:len=%s>" % (self.raw_type, self.length)
