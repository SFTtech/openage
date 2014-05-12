#!/usr/bin/env python3
#
#this file contains code for generating data files
#and their corresponding structs.

from collections import OrderedDict
import pprint
import util
from util import dbg
import re
from string import Template
import struct
import os.path



#global member type modifiers
READ          = util.NamedObject("binary-read member")
READ_EXPORT   = util.NamedObject("binary-read and exported member")
NOREAD_EXPORT = util.NamedObject("only export member, don't read it")
READ_UNKNOWN  = util.NamedObject("member to read but which is unknown")

#regex for matching type array definitions like int[1337]
#group 1: type name, group 2: length
vararray_match = re.compile("([a-zA-Z0-9_ -]+)\\[(\\d+)\\]")

def encode_value(val):
    """
    encodes val to a (possibly escaped) string,
    for use in a csv column of type valtype (string)
    """

    val = str(val)
    val = val.replace("\\", "\\\\")
    val = val.replace(",", "\\,")
    val = val.replace("\n", "\\n")

    return val


def gather_data(obj, members):
    """
    queries the given object for the given member variables
    and returns that as a dict.

    key: member name
    value: obj's member value
    """
    ret = dict()

    for attr, _ in members:
        ret[attr] = getattr(obj, attr)

    return ret


class Exportable:
    """
    superclass for all exportable data members

    exportable classes inherit from this.
    """

    def __init__(self, **args):
        #store passed arguments as members
        self.__dict__.update(args)

    def dump(self, filename):
        """
        main data dumping function, the magic happens in here.

        recursively dumps all object members as DataDefinitions.

        returns [DataDefinition, ..]
        """

        ret = list()
        self_data = dict()

        members = self.get_data_format(allowed_modes=(True, READ_EXPORT, NOREAD_EXPORT), flatten_includes=True)
        for is_parent, export, member_name, member_type in members:

            if member_name:
                #gather data members of the currently queried object
                self_data[member_name] = getattr(self, member_name)
            else:
                raise Exception("invalid data member name %s" % member_name)

            if isinstance(member_type, MultisubtypeMember):
                dbg(lazymsg=lambda: "%s => entering member %s" % (filename, member_name), lvl=2)

                subdata_definitions = list()
                for subtype_name, submember_class in member_type.class_lookup.items():

                    if isinstance(member_type, SubdataMember):
                        subdata_item_iter  = self_data[member_name]
                        submember_filename = filename
                        is_single_subdata  = True
                    else:
                        dbg(lazymsg=lambda: "%s => entering multisubtype member %s" % (filename, subtype_name), lvl=2)
                        subdata_item_iter  = self_data[member_name][subtype_name]
                        submember_filename = "%s-%s" % (filename, subtype_name)
                        is_single_subdata  = False

                    submember_data = list()
                    for idx, submember_data_item in enumerate(subdata_item_iter):
                        if not isinstance(submember_data_item, Exportable):
                            raise Exception("tried to dump object not inheriting from Exportable")

                        dbg(lazymsg=lambda: "%s: submember item %d" % (filename, idx), lvl=3)

                        #generate output filename
                        member_ref   = "" #getattr(self, member_type.ref_to) if member_type.ref_to else ""
                        sub_filename = "%s-%s/%s-%04d%s" % (submember_filename, member_name, member_name, idx, member_ref)

                        #recursive call, fetches DataDefinitions and the next-level data dict
                        data_sets, data = submember_data_item.dump(sub_filename)

                        #store recursively generated DataDefinitions to the flat list
                        ret += data_sets

                        #append the next-level entry to the list
                        #that will contain the data for the current level DataDefinition
                        submember_data.append(data)

                    #create DataDefinition for the next-level data pile.
                    subdata_definitions.append(DataDefinition(
                        submember_class,
                        submember_data,
                        submember_filename,
                    ))

                    #store filename instead of data list
                    #is used to determine the file to read next.
                    if is_single_subdata:
                        self_data[member_name] = submember_filename
                    else:
                        self_data[member_name][subtype_name] = submember_filename

                        dbg(lazymsg=lambda: "%s => leaving multisubtype member %s" % (filename, subtype_name), lvl=2)

                #store all created submembers to the flat list
                ret += subdata_definitions

                dbg(lazymsg=lambda: "%s => leaving member %s" % (filename, member_name), lvl=2)

        dbg(lazymsg=lambda: "%s data:\n%s" % (filename, pprint.pformat(self_data)), lvl=4)

        return ret, self_data

    def read(self, raw, offset, cls=None):
        """
        recursively read defined binary data from raw at given offset.

        this is used to fill the python classes with data from the binary input.
        """

        dbg(lazymsg=lambda: "%s: reading binary data at 0x%08x" % (repr(self), offset), lvl=3)

        if cls:
            target_class = cls
        else:
            target_class = self

        members = target_class.get_data_format(allowed_modes=(True, READ_EXPORT, READ, READ_UNKNOWN), flatten_includes=False)
        for is_parent, export, var_name, var_type in members:

            dbg(lazymsg=lambda: "%s: reading entry %s..." % (repr(target_class), var_name), lvl=4)

            if isinstance(var_type, IncludeMembers):
                if not issubclass(var_type.cls, Exportable):
                    raise Exception("class where members should be included is not exportable: %s" % var_type.cls.__name__)

                dbg(lazymsg=lambda: "calling include class %s.read()" % (var_type.cls.__name__), lvl=4)
                offset = var_type.cls.read(self, raw, offset, cls=var_type.cls)

            elif isinstance(var_type, MultisubtypeMember):
                #subdata reference implies recursive call for reading the binary data

                #arguments passed to the next-level constructor.
                varargs = dict()

                if var_type.passed_args:
                    for passed_member_name in var_type.passed_args:
                        varargs[passed_member_name] = getattr(self, passed_member_name)

                #subdata list length has to be defined beforehand as a object member OR number.
                #it's name or count is specified at the subdata member definition by length.
                list_len = var_type.get_length(self)

                #prepare result storage lists
                if isinstance(var_type, SubdataMember):
                    #single-subtype child data list
                    setattr(self, var_name, list())
                    single_type_subdata = True
                else:
                    #multi-subtype child data list
                    setattr(self, var_name, util.gen_dict_key2lists(var_type.class_lookup.keys()))
                    single_type_subdata = False

                for i in range(list_len):
                    if single_type_subdata:
                        #append single data entry to the subdata object list
                        new_data_class = var_type.class_lookup[None]
                    else:
                        #determine subtype first, then append the data entry to the desired list
                        subtype_name = self_data[var_type.type_to]
                        new_data_class = var_type.class_lookup[subtype_name]

                    if not issubclass(new_data_class, Exportable):
                        raise Exception("dumped data is not exportable: %s" % new_data_class.__name__)

                    #create instance of submember class
                    new_data = new_data_class(**varargs)

                    #recursive call, read the subdata.
                    offset = new_data.read(raw, offset, new_data_class)

                    #append the new data to the appropriate list
                    if single_type_subdata:
                        getattr(self, var_name).append(new_data)
                    else:
                        getattr(self, var_name)[subtype_name].append(new_data)

            else:
                #reading binary data, as this member is no reference but actual content.

                data_count = 1

                if type(var_type) == str:
                    #standard type implies binary length

                    is_array = vararray_match.match(var_type)

                    if is_array:
                        struct_type = is_array.group(1)
                        data_count  = int(is_array.group(2))
                        if struct_type == "char":
                            struct_type = "char[]"
                    else:
                        struct_type = var_type
                        data_count  = 1

                elif isinstance(var_type, DataMember):
                    #special type requires having set the raw data type
                    struct_type = var_type.raw_type
                    data_count  = var_type.get_length(self)

                else:
                    raise Exception("unknown data member definition %s for member %s" % (var_type, var_name))

                if data_count <= 0:
                    raise Exception("invalid length <= 0 in %s for member %s" % (var_type, var_name))

                if struct_type not in util.struct_type_lookup:
                    raise Exception("unknown primitive struct data type %s for member %s" % (struct_type, var_name))

                if export == READ_UNKNOWN:
                    #for unknown variables, generate uid for the unknown memory location
                    var_name = "unknown-0x%08x" % offset

                #lookup c type to python struct scan type
                symbol, size = util.struct_type_lookup[struct_type]

                dbg(lazymsg=lambda: "\tdumping %s<%s> as '< %d%s' at 0x%08x" % (var_name, var_type, data_count, symbol, offset), lvl=4)
                struct_format = "< %d%s" % (data_count, symbol)
                result = struct.unpack_from(struct_format, raw, offset)
                offset += struct.calcsize(struct_format)

                if symbol == "s":
                    #stringify char array
                    result = util.zstr(result[0])
                elif len(result) == 1:
                    #store first tuple element
                    result = result[0]

                #run entry hook for non-primitive members
                if isinstance(var_type, DataMember):
                    result = var_type.entry_hook(result)

                dbg(lazymsg=lambda: "\t==> storing self.%s = %s" % (var_name, result), lvl=4)

                #store member's data value
                setattr(self, var_name, result)

        return offset

    @classmethod
    def structs(cls):
        """
        create struct definitions for this class and its subdata references.
        """

        ret = list()
        struct_parents = list()

        dbg(lazymsg=lambda: "%s: generating structs" % (repr(cls)), lvl=2)

        #acquire all struct members, including the included members
        members = cls.get_data_format(allowed_modes=(True, READ_EXPORT, NOREAD_EXPORT), flatten_includes=True)
        for is_parent, export, member_name, member_type in members:

            dbg(lazymsg=lambda: "%s: exporting %s: %s" % (repr(cls), member_name, member_type), lvl=3)

            if isinstance(member_type, MultisubtypeMember):
                for subtype_name, subtype_class in member_type.class_lookup.items():
                    if not issubclass(subtype_class, Exportable):
                        raise Exception("tried to export structs from non-exportable %s" % subtype_class)
                    ret += subtype_class.structs()
            else:
                continue

        ret.append(StructDefinition(cls))

        return ret

    @classmethod
    def get_effective_type(cls):
        return cls.name_struct

    @classmethod
    def get_data_format(cls, allowed_modes=False, flatten_includes=False, is_parent=False):
        ret = list()

        for member in cls.data_format:
            export, member_name, member_type = member

            if isinstance(member_type, IncludeMembers):
                if flatten_includes:
                    #recursive call
                    ret += member_type.cls.get_data_format(allowed_modes, flatten_includes, is_parent=True)
                    continue
                else:
                    is_parent = True

            if allowed_modes:
                if export not in allowed_modes:
                    dbg(lazymsg=lambda: "%s: skipping member %s" % (repr(cls), member_name), lvl=4)
                    continue

            member_entry = (is_parent,) + member
            dbg(lazymsg=lambda: "%s: returning member entry %s" % (repr(cls), member_entry), lvl=4)
            ret.append(member_entry)
        return ret


class ContentSnippet:
    """
    one part of text for generated files to be saved in "file_name"

    before whole source files can be written, it's content snippets
    have to be ordered according to their dependency chain.

    also, each snipped can have import requirements that have to be
    included in top of the source.
    """

    section_header   = util.NamedObject("header")
    section_body     = util.NamedObject("body")

    def __init__(self, data, file_name, section):
        self.data      = data       #snippet content
        self.file_name = file_name  #snippet wants to be saved in this file
        self.typerefs  = set()      #these types are referenced
        self.typedefs  = set()      #these types are defined
        self.headers   = set()      #required headers
        self.section   = section

    def get_data(self):
        return self.data

    def __hash__(self):
        return hash((self.data, self.file_name, self.section))

    def __eq__(self, other):
        return (
            self.data == other.data
            and self.file_name == other.file_name
            and self.section   == other.section
        )

    def __repr__(self):
        return "ContentSnippet"

    def __str__(self):
        return "".join([
            repr(self), " [%s], " % (self.file_name),
            "data = '", self.data, "'"
        ])


class CHeader:
    """
    represents an includable C header
    """

    def __init__(self, name, is_global=False):
        self.name = name
        self.is_global = is_global

        if self.is_global:
            self.data = "#include <%s>\n" % self.name
        else:
            self.data = "#include \"%s\"\n" % self.name

    def get_file_name(self):
        return self.name

    def get_snippets(self, file_name):
        return [ ContentSnippet(self.data, file_name, ContentSnippet.section_header) ]

    def __hash__(self):
        return hash((self.name, self.is_global))

    def __eq__(self, other):
        return (
            self.name == other.name
            and self.is_global == other.is_global
        )

    def __repr__(self):
        sym0 = "<" if self.is_global else "\""
        sym1 = ">" if self.is_global else "\""
        return "CHeader %s%s%s" % (sym0, self.name, sym1)


class GeneratedFile:
    """
    represents a writable file that was generated automatically.
    """

    #inserted warning message for generated files
    dontedit = """
//do not edit this file, it's autogenerated. all changes will be undone!
//make changes in the convert script and regenerate the files."""

    #default preferences for output modes
    default_preferences = {
        "folder":         "",
        "file_suffix":    "",
        "content_prefix": "",
        "content_suffix": "",
        "presuffix_func": lambda x: x
    }


    #override the default preferences with the
    #configuration for all the output formats
    output_preferences = {
        "csv": {
            "folder":      "",
            "file_suffix": ".docx",
        },
        "struct": {
            "file_suffix": ".h",
            "content_prefix": """#ifndef _${file_name}_H_
#define _${file_name}_H_

${headers}
%s

namespace engine {

""" % dontedit,
            "content_suffix": """
} //namespace engine

#endif // _${file_name}_H_
""",
            "presuffix_func": lambda x: x.upper()
        },
        "structimpl": {
            "file_suffix":    ".cpp",
            "content_prefix": """#include "${file_name}.h"

${headers}
%s

namespace engine {\n\n""" % dontedit,
            "content_suffix": "} //namespace engine\n",
        }
    }


    def __init__(self, file_name):
        self.snippets  = set()
        self.headers   = set()
        self.typedefs  = set()
        self.typerefs  = set()
        self.content   = None
        self.file_name = file_name

    def add_snippet(self, snippet):
        if not isinstance(snippet, ContentSnippet):
            raise Exception("only ContentSnippets can be added to generated files.")

        if not snippet.file_name == self.file_name:
            raise Exception("only snippets with the same target file_name can be put into the same generated file.")

        self.snippets.add(snippet)
        self.headers  |= snippet.headers
        self.typedefs |= snippet.typedefs
        self.typerefs |= snippet.typerefs

        dbg("\nsnippet for %s" % snippet.file_name, lvl=2)
        dbg("headers: %s" % snippet.headers, lvl=2)
        dbg("typedefs: %s" % snippet.typedefs, lvl=2)
        dbg("typerefs: %s" % snippet.typerefs, lvl=2)

    def generate(self, output_format):
        """
        actually generate the content for this file.
        """

        #apply preference overrides
        prefs = self.default_preferences.copy()
        prefs.update(self.output_preferences[output_format])

        #TODO: create new snippets for resolving cyclic dependencies (forward declarations)
        #TODO: references to other files don't matter as they are #included anyway
        #TODO: create pending typedefs, these types are requested but not part of this file.
        #they need to be referenced via header include then.

        #put snippets into list in correct order
        #snippets will be written according to this [(snippet, prio)] list
        snippets_priorized = list()

        #determine each snippet's priority by number of type references and definitions
        #smaller prio means written earlier in the file.
        for s in self.snippets:
            snippet_prio = len(s.typerefs) - len(s.typedefs)
            snippets_priorized.append((s, snippet_prio))

        sorted_snippets = (snippet for (snippet, _) in sorted(snippets_priorized, key=lambda s: s[1]))

        #these snippets will be written outside the namespace
        #in the #include section
        header_snippets = list()
        for header in sorted(self.headers, key=lambda h: (not h.is_global, h.name)):
            header_snippets.extend(header.get_snippets(self.file_name))

        #merge file contents
        file_data = ""
        subst_headers = ""

        for header in header_snippets:
            subst_headers += header.get_data()

        for snippet in sorted_snippets:
            file_data += "\n%s" % snippet.get_data()

        #create content, with prefix and suffix (actually header guards)
        subst_file_name = prefs["presuffix_func"](self.file_name)

        #fill file header and footer with the generated file_name
        content_prefix = Template(prefs["content_prefix"]).substitute(file_name=subst_file_name, headers=subst_headers)
        content_suffix = Template(prefs["content_suffix"]).substitute(file_name=subst_file_name)

        #this is the final file content
        file_data = content_prefix + file_data + content_suffix

        #determine output file name
        output_file_name_parts = [
            prefs["folder"],
            "%s%s" % (self.file_name, prefs["file_suffix"])
        ]

        #whee, store the content
        self.content = file_data
        self.file_name = os.path.join(*output_file_name_parts)


class DataMember:
    """
    one struct member, which equals one column in a csv file.
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
        return set()

    def get_typedefs(self):
        return set()

    def entry_hook(self, data):
        """
        allows the data member class to modify the input data

        is used e.g. for the number->enum lookup
        """

        return data

    def get_effective_type(self):
        raise NotImplementedError("return the effective type of member type %s" % type(self))

    def get_length(self):
        return self.length

    def get_struct_entries(self, member_name):
        """
        return the lines to put inside the C struct.
        """

        return [ "%s %s;" % (self.get_effective_type(), member_name) ]

    def __repr__(self):
        raise NotImplementedError("return short description of the member type %s" % (type(self)))


class IncludeMembers(DataMember):
    """
    a member that requires evaluating the given class
    as a include first.

    example:
    the unit class "building" and "movable" will both have
    common members that have to be read first.
    """

    def __init__(self, cls):
        super().__init__()
        self.cls = cls

    def get_headers(self, output_target):
        return { self.cls.name_struct_file }

    def get_typedefs(self):
        return { self.cls.get_effective_type() }

    def __repr__(self):
        return "IncludeMember<%s>" % repr(self.cls)


class DynLengthMember(DataMember):
    """
    a member that can have a dynamic length.
    """

    any_length = util.NamedObject("unspecified length")

    def __init__(self, length=None):
        super(DynLengthMember).__init__()
        self.length = length

    def get_length(self, obj=None):
        if self.is_dynamic_length():
            if self.length is self.any_length:
                return -1

            if not obj:
                raise Exception("dynamic length query requires source object")

            #self.length specifies the attribute name where the length is stored
            return getattr(obj, self.length)

        else:
            return self.length

    def is_dynamic_length(self):
        if self.length is self.any_length:
            return True
        elif isinstance(self.length, str):
            return True
        elif isinstance(self.length, int):
            return False
        else:
            raise Exception("unknown length definition supplied: %s" % self.length)

    def store_length(self, obj):
        self.length = self.get_length(obj)


class RefMember(DataMember):
    """
    a struct member that can be referenced/references another struct.
    """

    def __init__(self, type_name, file_name):
        super().__init__()
        self.resolved  = False
        self.type_name = type_name
        self.file_name = file_name


class NumberMember(DataMember):
    """
    this struct member/data column contains simple numbers
    """

    #primitive types, directly parsable by sscanf
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
        return [ "if (sscanf(buf[%d], \"%%%s\", &this->%s) != 1) { return false; }" % (idx, scan_symbol, member) ]

    def get_headers(self, output_target):
        if "structimpl" == output_target:
            return determine_headers("sscanf") | determine_headers(self.number_type)
        else:
            return set()

    def get_effective_type(self):
        return self.number_type

    def __repr__(self):
        return self.number_type


class EnumMember(RefMember):
    """
    this struct member/data column is a enum.
    """

    def __init__(self, type_name, file_name=None, values=None):
        super().__init__(type_name, file_name)
        self.values    = values
        self.resolved  = True    #TODO..

    def get_parsers(self, idx, member):
        enum_parse_else = ""
        enum_parser = list()
        enum_parser.append("//parse enum %s" % (self.type_name))
        for enum_value in self.values:
            enum_parser.extend([
                "%sif (0 == strcmp(buf[%d], \"%s\")) {" % (enum_parse_else, idx, enum_value),
                "\tthis->%s = %s::%s;"                  % (member, self.type_name, enum_value),
                "}",
            ])
            enum_parse_else = "else "

        enum_parser.extend([
            "else {",
            "\treturn false;",
            "}",
        ])

        return enum_parser

    def get_headers(self, output_target):
        if "structimpl" == output_target:
            return determine_headers("strcmp")
        else:
            return set()

    def get_typedefs(self):
        return { self.type_name }

    def get_effective_type(self):
        return self.type_name

    def validate_value(self, value):
        return value in self.values

    def get_snippets(self, file_name):
        """
        generate enum snippets from given data

        input: EnumMember
        output: ContentSnippet
        """

        snippet_file_name = self.file_name or file_name

        txt = list()

        #create enum definition
        txt.extend([
            "enum class %s {\n\t" % self.type_name,
            ",\n\t".join(self.values),
            "\n};\n\n",
        ])

        snippet = ContentSnippet("".join(txt), snippet_file_name, ContentSnippet.section_body)
        snippet.typedefs |= self.get_typedefs()

        return [ snippet ]

    def __repr__(self):
        return "enum %s" % self.type_name


class EnumLookupMember(EnumMember):
    def __init__(self, type_name, lookup_dict, raw_type=None, file_name=None):
        super().__init__(type_name, file_name, lookup_dict.values())
        self.lookup_dict = lookup_dict
        self.raw_type = raw_type

    def data_hook(self, data):
        return self.lookup_dict[data]


class CharArrayMember(DynLengthMember):
    """
    struct member/column type that allows to store equal-length char[n].
    """

    def __init__(self, length):
        super().__init__(length)
        self.raw_type = "char"

    def get_parsers(self, idx, member):
        if self.is_dynamic_length():
            return [ "this->%s = buf[%d];" % (member, idx) ]
        else:
            return [ "strncpy(this->%s, buf[%d], %d); this->%s[%d-1] = '\\0';" % (member, idx, self.get_length(), member, self.get_length()) ]

    def get_headers(self, output_target):
        ret = set()

        if "structimpl" == output_target:
            if not self.is_dynamic_length():
                ret |= determine_headers("strncpy")
        elif "struct" == output_target:
            if self.is_dynamic_length():
                ret |= determine_headers("std::string")

        return ret

    def get_effective_type(self):
        if self.is_dynamic_length():
            return "std::string"
        else:
            return "char";

    def get_length(self):
        if self.is_dynamic_length():
            return 1
        else:
            return super().get_length()

    def __repr__(self):
        return "".join(self.get_effective_type())


class StringMember(CharArrayMember):
    """
    member with unspecified string length, aka std::string
    """

    def __init__(self):
        super().__init__(DynLengthMember.any_length)


class MultisubtypeMember(DynLengthMember, RefMember):
    """
    struct member/data column that groups multiple references to
    multiple other data sets.
    """

    def __init__(self, type_name, class_lookup, type_to, length=None, passed_args=None, ref_to=None, offset_to=None, file_name=None):
        DynLengthMember.__init__(self, length)
        RefMember.__init__(self, type_name, file_name)
        self.class_lookup      = class_lookup        #!< dict to look up type_name => class
        self.type_to           = type_to             #!< member name whose value specifies the subdata type for each entry
        self.passed_args       = passed_args         #!< list of member names whose values will be passed to the new class
        self.ref_to            = ref_to              #!< add this member name's value to the filename
        self.offset_to         = offset_to           #!< link to member name which is a list of binary file offsets

        #no xrefs supported yet..
        self.resolved          = True

    def get_headers(self, output_target):
        if "struct" == output_target:
            return determine_headers(self.get_effective_type())
        else:
            return set()

    def get_effective_type(self):
        return "%s" % (self.type_name)

    def get_contained_types(self):
        return [ contained_type.get_effective_type() for contained_type in self.class_lookup.values() ]

    def get_parsers(self, idx, member):
        #TODO
        #0. read file where entry type files are defined.
        #1. read each of the entry type files and fill member.`entry_type`
        return [ "this->%s.filename = buf[%d];" % (member, idx) ]

    def get_typedefs(self):
        return { self.type_name }

    def get_typerefs(self):
        return set(self.get_contained_types())

    def get_snippets(self, file_name):

        snippet_file_name = self.file_name or file_name

        txt = list()

        txt.append("struct %s {\n" % (self.type_name))
        txt.extend(["\tstd::vector<%s> %s;\n" % (entry_type.get_effective_type(), entry_name) for (entry_name, entry_type) in self.class_lookup.items()])
        txt.append("};\n")

        snippet = ContentSnippet("".join(txt), snippet_file_name, ContentSnippet.section_body)
        snippet.typedefs |= self.get_typedefs()
        snippet.typerefs |= self.get_typerefs()
        snippet.headers  |= determine_headers("std::vector")

        return [ snippet ]

    def __repr__(self):
        return "MultisubtypeMember<%s>" % str(self.class_lookup)


class SubdataMember(MultisubtypeMember):
    """
    struct member/data column that references to one another data set.
    """

    def __init__(self, ref_type, ref_to=None, length=None, offset_to=None):
        super().__init__(type_name=None, class_lookup={None: ref_type}, type_to=ref_to, length=length, offset_to=None)

    def get_effective_type(self):
        return "std::vector<%s>" % (self.get_contained_types()[0])

    def get_parsers(self, idx, member):
        return [ "this->%s = util::read_csv_file<%s>(buf[%d]);" % (member, self.get_contained_types()[0], idx) ]

    def get_snippets(self, file_name):
        return list()

    def get_typedefs(self):
        return set()

    def get_typerefs(self):
        return set(self.get_contained_types())

    def __repr__(self):
        return "SubdataMember<%s>" % self.class_lookup[None].__name__


class StructDefinition:
    """
    input data read from the data files.

    one data set roughly represents one struct in the gamedata dat file.
    it consists of multiple DataMembers, they define the struct members.
    """

    def __init__(self, target):

        dbg("generating struct definition from %s" % (repr(target)), lvl=2)

        self.name_struct_file   = target.name_struct_file    #!< name of file where generated struct will be placed
        self.name_struct        = target.name_struct         #!< name of generated C struct
        self.struct_description = target.struct_description  #!< comment placed above generated C struct

        #create ordered dict of member type objects from structure definition
        self.members = OrderedDict()
        self.parent_members = list()
        self.parent_classes = list()

        members = target.get_data_format(allowed_modes=(True, READ_EXPORT, NOREAD_EXPORT), flatten_includes=True)
        for is_parent, export, member_name, member_type in members:

            if isinstance(member_type, IncludeMembers):
                raise Exception("something went very wrong.")

            #select member type class according to the defined member type
            if type(member_type) == str:
                array_match = vararray_match.match(member_type)
                if array_match:
                    array_type   = array_match.group(1).strip()
                    array_length = int(array_match.group(2))
                    if array_length > 0 and array_type == "char":
                        member = CharArrayMember(array_length)
                    else:
                        raise Exception("unknown type or array length <= 0")
                elif member_type == "std::string":
                    member = StringMember()
                else:
                    member = NumberMember(member_type)

            elif isinstance(member_type, DataMember):
                member = member_type

            else:
                raise Exception("unknown member type specification!")

            if member is None:
                raise Exception("member %s of struct %s is None" % (member_name, self.name_struct))

            self.members[member_name] = member
            self.parent_members.append(member_name)

        members = target.get_data_format(flatten_includes=False)
        for is_parent, _, _, member_type in members:
            if is_parent and isinstance(member_type, IncludeMembers):
                self.parent_classes.append(member_type.cls)

    def dynamic_ref_update(self, lookup_ref_data):
        """
        update ourself the with the given reference data.

        data members can be cross references to definitions somewhere else.
        this symbol resolution is done here by replacing the references.
        """

        for member_name, member_type in self.members.items():
            if not isinstance(member_type, RefMember):
                continue

            #this member of self is already resolved
            if member_type.resolved:
                continue

            type_name = member_type.get_effective_type()

            #replace the xref with the real definition
            self.members[type_name] = lookup_ref_data[type_name]

    def __str__(self):
        ret = [
            repr(self),
            "\n\tstruct file name: ", self.name_struct_file,
            "\n\tstruct name: ", self.name_struct,
            "\n\tstruct description: ", self.struct_description,
        ]
        return "".join(ret)

    def __repr__(self):
        return "StructDefinition<%s>" % self.name_struct


class DataDefinition(StructDefinition):
    """
    data structure definition by given object including data.
    """

    def __init__(self, target, data, name_data_file):
        super().__init__(
            target,
        )

        self.data               = data                #!< list of dicts, member_name=>member_value
        self.name_data_file     = name_data_file      #!< name of file where data will be placed in

    def __str__(self):
        ret = [
            "\n\tdata file name: ", str(self.name_data_file),
            "\n\tdata: ", str(self.data),
        ]
        return "%s%s" % (super().__str__(), "".join(ret))

    def __repr__(self):
        return "DataDefinition<%s>" % self.name_struct


class DataFormatter:
    """
    transforms and merges data structures
    the input data also specifies the output structure.

    this class generates the plaintext being stored in the data files
    it is the central part of the data exporting functionality.
    """

    #csv column delimiter:
    DELIMITER = ","

    #method signature for fill function
    fill_csignature = "bool %sfill(char *by_line)"

    def __init__(self):
        #list of all dumpable data sets
        self.data = list()

        #collection of all type definitions
        self.typedefs = dict()

    def add_data(self, data_set_pile, prefix=None):
        """
        add a given StructDefinition to the storage, so it can be exported later.

        other exported data structures are collected from the given input.
        """

        if type(data_set_pile) is not list:
            data_set_pile = [ data_set_pile ]

        #add all data sets
        for data_set in data_set_pile:

            if prefix:
                data_set.name_data_file = "%s%s" % (prefix, data_set.name_data_file)

            #collect column type specifications
            for member_name, member_type in data_set.members.items():

                #store resolved (first-time definitions) members in a symbol list
                if isinstance(member_type, RefMember):
                    if member_type.resolved:
                        if member_type.get_effective_type() in self.typedefs:
                            if data_set.members[member_name] is not self.typedefs[member_type.get_effective_type()]:
                                raise Exception("different redefinition of type %s" % member_type.get_effective_type())
                        else:
                            ref_member = data_set.members[member_name]

                            #if not overridden, use name of struct file to store
                            if ref_member.file_name == None:
                                ref_member.file_name = data_set.name_struct_file

                            self.typedefs[member_type.get_effective_type()] = ref_member

            self.data.append(data_set)

    def export(self, requested_formats):
        """
        generate content snippets that will be saved to generated files

        output: {file_name: content}
        """

        #returned file_name=>content mapping
        ret = dict()

        #storage of all needed content snippets
        files = dict()

        #create empty dicts for each format
        #each of these dicts has "file_name" => GeneratedFile
        for format in requested_formats:
            files[format] = dict()

            snippets = list()

            #iterate over all stored data sets and
            #generate all data snippets for the requested output formats.
            for data_set in self.data:

                #resolve data xrefs for this data_set
                data_set.dynamic_ref_update(self.typedefs)

                #generate one output chunk list for each requested format
                if format == "csv":
                    snippet = self.generate_csv(data_set)

                elif format == "struct":
                    snippet = self.generate_struct(data_set)

                elif format == "structimpl":
                    snippet = self.generate_struct_implementation(data_set)

                else:
                    raise Exception("unknown export format %s requested" % format)

                snippets.append(snippet)

            #create snippets for the encountered type definitions
            for type_name, type_definition in self.typedefs.items():
                if format == "struct":
                    type_snippets = type_definition.get_snippets(type_definition.file_name)
                else:
                    continue

                snippets.extend(type_snippets)

            for snippet in snippets:
                #if this file was not yet created, do it nao

                if snippet.file_name not in files[format]:
                    files[format][snippet.file_name] = GeneratedFile(snippet.file_name)

                files[format][snippet.file_name].add_snippet(snippet)


        #files is currently:
        #{format: {file_name: GeneratedFile}}

        #we now invoke the content generation for each generated file
        for format, gen_files in files.items():
            for file_name, gen_file in gen_files.items():
                gen_file.generate(format)

                ret[gen_file.file_name] = gen_file.content

        return ret

    def generate_csv(self, dataset):
        """
        generate a csv data representation
        """

        if not isinstance(dataset, DataDefinition):
            raise Exception("can only generate a csv from a DataDefinition")

        member_types = dataset.members.values()
        csv_column_types = list()

        #create column types line entries as comment in the csv file
        for c_type in member_types:
            csv_column_types.append(repr(c_type))

        #the resulting csv content
        #begin with the csv information comment header
        txt = [
            "#struct ", dataset.name_struct, "\n",
            "".join("#%s\n" % line for line in dataset.struct_description.split("\n")),
            "#", self.DELIMITER.join(csv_column_types), "\n",
            "#", self.DELIMITER.join(dataset.members.keys()), "\n",
        ]

        #create csv data lines:
        for idx, data_line in enumerate(dataset.data):
            row_entries = list()
            for member_name, member_type in dataset.members.items():
                entry = data_line[member_name]

                #check if enum data value is valid
                if isinstance(member_type, EnumMember):
                    if not member_type.validate_value(entry):
                        raise Exception("data entry %d '%s' not a valid %s value" % (idx, entry, repr(member_type)))

                elif isinstance(member_type, SubdataMember):
                    entry = "%s%s" % (entry, GeneratedFile.output_preferences["csv"]["file_suffix"])

                #encode each data field, to escape newlines and commas
                row_entries.append(encode_value(entry))

            txt.extend((self.DELIMITER.join(row_entries), "\n"))

        return ContentSnippet("".join(txt), dataset.name_data_file, ContentSnippet.section_body)

    def generate_struct(self, dataset):
        """
        generate C structs (that should be placed in a header)
        """

        #the resulting header definition snippet
        txt = list()

        #this c snippet needs these headers
        headers = set()

        #these data type references are needed for this snippet
        needed_types = set()

        #optional struct description
        #prepend * before every comment line
        if dataset.struct_description != None:
            txt.extend([
                "/**\n * ",
                "\n * ".join(dataset.struct_description.split("\n")),
                "\n */\n",
            ])

        struct_entries = list()

        #create struct members and inheritance parents
        for member_name, member_type in dataset.members.items():
            if member_name in dataset.parent_members:
                continue

            headers  |= member_type.get_headers("struct")
            needed_types |= member_type.get_typedefs()

            struct_entries.extend(["\t%s\n" % entry for entry in member_type.get_struct_entries(member_name)])

        dbg(lazymsg=lambda: "%s needed_types = %s" % (dataset.name_struct, needed_types), lvl=4)

        parents = [parent_class.get_effective_type() for parent_class in dataset.parent_classes]

        #struct definition
        txt.append("struct %s%s {\n" % (dataset.name_struct, ", ".join(parents)))

        #struct member entries
        txt.extend(struct_entries)

        #append member count variable
        txt.append("\n\tstatic constexpr size_t member_count = %d;\n" % len(dataset.members))

        #add filling function prototype
        txt.append("\n\t%s;\n" % (self.fill_csignature % ""))

        #struct ends
        txt.append("};\n")

        snippet = ContentSnippet("".join(txt), dataset.name_struct_file, ContentSnippet.section_body)
        snippet.headers    |= headers
        snippet.typerefs   |= needed_types
        snippet.typedefs.add(dataset.name_struct)

        return snippet

    def generate_struct_implementation(self, dataset):
        """
        create C code for the implementation of the struct functions
        it is used to fill a struct instance with data of a csv data line
        """

        #required headers for this C snippet
        headers = set()

        #referenced types in this C snippet
        typerefs = set()

        #list of lines for each parsers for a single field of the csv
        parsers = []

        for idx, (member_name, member_type) in enumerate(dataset.members.items()):
            parsers.append(member_type.get_parsers(idx, member_name))
            headers  |= member_type.get_headers("structimpl")
            #the code references to the definition of the members
            typerefs |= member_type.get_typedefs()

        #indent/newline the token parsers
        parser_code = "\n\n\t".join("\n\t".join(p) for p in parsers)

        #prepend struct name to fill function signature
        fill_signature = self.fill_csignature % ("%s::" % dataset.name_struct)

        member_count = dataset.name_struct + "::member_count"

        #definition of filling function
        txt = Template("""
$funcsignature {
\t//tokenize
\tchar *buf[$member_count];
\tint count = engine::util::string_tokenize_to_buf(by_line, '$delimiter', buf, $member_count);

\t//check tokenization result
\tif (count != $member_count) {
\t\treturn false;
\t}

\t//now store each of the tokens/struct members
\t$parsers

\treturn true;
}
""").substitute(funcsignature=fill_signature, delimiter=self.DELIMITER, parsers=parser_code, member_count=member_count)

        snippet = ContentSnippet(txt, dataset.name_struct_file, ContentSnippet.section_body)
        snippet.headers  |= headers | determine_headers("strtok_custom")
        snippet.typerefs |= typerefs
        return snippet


def determine_headers(for_type):
    """
    returns the includable headers for using the given C type.
    """

    #these headers are needed for the type
    ret = set()

    cstdinth  = CHeader("cstdint", is_global=True)
    stringh   = CHeader("string",  is_global=True)
    cstringh  = CHeader("cstring", is_global=True)
    cstdioh   = CHeader("cstdio",  is_global=True)
    vectorh   = CHeader("vector",  is_global=True)
    engine_util_strings_h = CHeader("../engine/util/strings.h", False)

    #lookup for type->{header}
    type_map = {
        "int8_t":          { cstdinth },
        "uint8_t":         { cstdinth },
        "int16_t":         { cstdinth },
        "uint16_t":        { cstdinth },
        "int32_t":         { cstdinth },
        "uint32_t":        { cstdinth },
        "int64_t":         { cstdinth },
        "uint64_t":        { cstdinth },
        "std::string":     { stringh  },
        "std::vector":     { vectorh  },
        "strcmp":          { cstringh },
        "strncpy":         { cstringh },
        "strtok_custom":   { engine_util_strings_h },
        "sscanf":          { cstdioh  },
    }

    if for_type in type_map:
        ret |= type_map[for_type]
    else:
        dbg("could not determine header for %s" % for_type, lvl=2)

    return ret
