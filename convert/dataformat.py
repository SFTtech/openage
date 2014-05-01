#!/usr/bin/env python3
#
#this file contains code for generating data files
#and their corresponding structs.

from collections import OrderedDict
import util
from util import dbg
import re
from string import Template
import os.path


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

    def __init__(self):
        pass

    def dump(self, filename):
        """
        main data dumping function, the magic happens in here.

        recursively dumps all object members as DataDefinitions.

        returns [DataDefinition, ..]
        """

        ret = list()

        self_data = dict()

        for member_name, member_type in self.data_format:
            #gather data members of the currently queried object
            self_data[member_name] = getattr(self, member_name)

            if isinstance(member_type, MultisubtypeMember):
                dbg(lazymsg=lambda: "%s => entering multisubtype member %s" % (filename, member_name), lvl=2)

                subdata_definitions = list()
                for subtype_name, submember_class in member_type.class_lookup.items():
                    dbg(lazymsg=lambda: "%s => entering submember %s" % (filename, subtype_name), lvl=2)

                    if isinstance(member_type, SubdataMember):
                        subdata_item_iter  = self_data[member_name]
                        submember_filename = filename
                        is_single_subdata  = True
                    else:
                        subdata_item_iter  = self_data[member_name][subtype_name]
                        submember_filename = "%s-%s" % (filename, subtype_name)
                        is_single_subdata  = False

                    submember_data = list()
                    for idx, submember_data_item in enumerate(subdata_item_iter):
                        if not isinstance(submember_data_item, Exportable):
                            raise Exception("tried to dump object not inheriting from Exportable")

                        dbg(lazymsg=lambda: "submember item %d" % idx, lvl=3)

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

                    dbg(lazymsg=lambda: "%s => leaving submember %s" % (filename, subtype_name), lvl=2)

                ret += subdata_definitions

                dbg(lazymsg=lambda: "%s => leaving multisubtype member %s" % (filename, member_name), lvl=2)

        dbg(lazymsg=lambda: str(self_data), lvl=3)

        return ret, self_data

    @classmethod
    def structs(cls):
        """
        create struct definitions for this class and its subdata references.
        """

        ret = list()

        for member_name, member_type in cls.data_format:
            if isinstance(member_type, MultisubtypeMember):
                for subtype_name, subtype_class in member_type.class_lookup:
                    ret += member_type.subtype_class.structs()

        ret.append(StructDefinition(cls))

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

    def get_snippet(self, file_name):
        return ContentSnippet(self.data, file_name, ContentSnippet.section_header)

    def __hash__(self):
        return hash((self.name, self.is_global))

    def __eq__(self, other):
        return (
            self.name == other.name
            and self.is_global == other.is_global
        )


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
            header_snippets.append(header.get_snippet(self.file_name))

        #merge file contents
        file_data = ""
        subst_headers = ""

        for header in header_snippets:
            subst_headers += header.get_data()

        for snippet in sorted_snippets:
            file_data += snippet.get_data()

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

    def get_parser(self, idx, member):
        raise NotImplementedError("implement the parser generation for each member type")

    def get_headers(self):
        raise NotImplementedError("return needed headers for the parser")

    def get_typerefs(self):
        return set()

    def get_typedefs(self):
        return set()

    def get_type(self):
        raise NotImplementedError("return the type to be put into a struct")

    def __repr__(self):
        raise NotImplementedError("return short description of the member type")


class RefMember(DataMember):
    """
    a struct member that can be referenced/references another struct.
    """

    def __init__(self):
        super().__init__()
        self.resolved  = False
        self.type_name = None

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

    def get_parser(self, idx, member):
        scan_symbol = self.type_scan_lookup[self.number_type]
        return [ "if (sscanf(buf[%d], \"%%%s\", &this->%s) != 1) { return false; }" % (idx, scan_symbol, member) ]

    def get_headers(self):
        return get_headers("sscanf") | get_headers(self.number_type)

    def get_type(self):
        return self.number_type

    def __repr__(self):
        return self.number_type


class EnumMember(RefMember):
    """
    this struct member/data column is a C enum.
    """

    def __init__(self, type_name, file_name=None, values=None):
        super().__init__()
        self.type_name = type_name
        self.file_name = file_name
        self.values    = values
        self.resolved  = True    #TODO..

    def get_parser(self, idx, member):
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

    def get_headers(self):
        return get_headers("strcmp")

    def get_typedefs(self):
        return { self.type_name }

    def get_type(self):
        return self.type_name

    def validate_value(self, value):
        return value in self.values

    def get_snippet(self):
        """
        generate enum snippets from given data

        input: EnumMember
        output: ContentSnippet
        """

        txt = list()

        #create enum definition
        txt.extend([
            "enum class %s {\n\t" % self.type_name,
            ",\n\t".join(self.values),
            "\n};\n\n",
        ])

        snippet = ContentSnippet("".join(txt), self.file_name, ContentSnippet.section_body)
        snippet.typedefs |= self.get_typedefs()

        return snippet

    def __repr__(self):
        return "enum %s" % self.type_name


class CharArrayMember(DataMember):
    """
    struct member/column type that allows to store equal-length char[n].
    """

    def __init__(self, length):
        super().__init__()
        self.length = length

    def get_parser(self, idx, member):
        return [ "strncpy(this->%s, buf[%d], %d); this->%s[%d-1] = '\\0';" % (member, idx, self.length, member, self.length) ]

    def get_headers(self):
        return get_headers("strncpy")

    def get_type(self):
        return "char"

    def __repr__(self):
        return "char[%d]" % (self.length)


class StringMember(DataMember):
    """
    struct member/column type to store std::strings
    """

    def __init__(self):
        super().__init__()
        self.type_name = "std::string"

    def get_parser(self, idx, member):
        return [ "this->%s = buf[%d];" % (member, idx) ]

    def get_headers(self):
        return get_headers(self.type_name)

    def get_type(self):
        return self.type_name

    def __repr__(self):
        return self.type_name


class MultisubtypeMember(DataMember):
    """
    struct member/data column that groups multiple references to
    multiple other data sets.
    """

    def __init__(self, class_lookup, ref_to=None):
        super().__init__()
        self.class_lookup = class_lookup
        self.ref_to = ref_to

    def get_headers(self):
        return set()

    def get_type(self):
        raise NotImplementedError()

    def get_parser(self):
        raise NotImplementedError()

    def __repr__(self):
        return "MultisubtypeMember<%s>" % str(self.class_lookup)


class SubdataMember(MultisubtypeMember):
    """
    struct member/data column that references to one another data set.
    """

    def __init__(self, ref_type, ref_to=None):
        super().__init__({ref_to: ref_type}, ref_to)

    def __repr__(self):
        return "SubdataMember<%s>" % self.ref_type.__name__

    def get_headers(self):
        return get_headers(self.get_type())

    def get_type(self):
        return "std::string"

    def get_parser(self, idx, member):
        return [ "this->%s = buf[%d];" % (member, idx) ]


class DataSet:
    """
    input data read from the data files.

    one data set roughly represents one struct in the gamedata dat file.
    it consists of multiple DataMembers, they define the struct members.
    """

    #regex for matching char array definitions like char[1337]
    chararray_match = re.compile("char *\\[(\\d+)\\]")

    def __init__(self, name_struct_file, name_struct, struct_description, data_format, data=None, name_data_file=None):

        self.name_struct_file   = name_struct_file
        self.name_struct        = name_struct
        self.struct_description = struct_description
        self.data               = data
        self.name_data_file     = name_data_file

        #create ordered dict of member type objects from structure definition
        self.members = OrderedDict()

        for member_name, member_type in data_format:
            #select member type class according to the defined member type
            if type(member_type) == str:
                chararray = self.chararray_match.match(member_type)
                if chararray:
                    array_length = int(chararray.group(1))
                    if array_length > 0:
                        member = CharArrayMember(array_length)
                    else:
                        raise Exception("you defined an array with length <= 0")
                elif member_type == "std::string":
                    member = StringMember()
                else:
                    member = NumberMember(member_type)

            elif isinstance(member_type, DataMember):
                member = member_type

            else:
                raise Exception("unknown member type specification!")

            self.members[member_name] = member

    def gather_subdata(self):
        """
        returns a list of DataSets recursively by querying members
        """
        raise NotImplementedError("subdata gathering not implemented yet")

    def dynamic_ref_update(self, lookup_ref_data):
        """
        update ourself the with the given reference data.

        data members can be cross references to definitions somewhere else.
        this symbol resolution is done here by replacing the references.
        """

        for member_name, member_type in self.members.items():
            if not isinstance(member_type, RefMember):
                continue

            #this member is already resolved
            if member_type.resolved:
                continue

            if member_type.name not in lookup_ref_data:
                raise Exception("cant resolve data references of type %s" % member_type.name)

            #replace the xref with the real definition
            self.members[member_name] = lookup_ref_data[member_name]

    def __str__(self):
        ret = list()
        ret.extend([
            repr(self),
            "\n\tstruct file name: ", self.name_struct_file,
            "\n\tstruct name: ", self.name_struct,
            "\n\tstruct description: ", self.struct_description,
            "\n\tdata file name: ", str(self.name_data_file),
            "\n\tdata: ", str(self.data), "\n",
        ])
        return "".join(ret)

    def __repr__(self):
        return "DataSet<%s>" % self.name_struct


class StructDefinition(DataSet):
    """
    data structure definition by given class.
    """

    def __init__(self, target_class):
        super().__init__(
            target_class.name_struct_file,
            target_class.name_struct,
            target_class.struct_description,
            target_class.data_format,
        )


class DataDefinition(DataSet):
    """
    data structure definition by given object, including data.
    """

    def __init__(self, target_obj, data, data_name):
        super().__init__(
            target_obj.name_struct_file,
            target_obj.name_struct,
            target_obj.struct_description,
            target_obj.data_format,
            data,
            data_name,
        )


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
        add a given DataSet to the storage, so it can be exported later.

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
                        if member_type.type_name in self.typedefs:
                            raise Exception("redefinition of type %s" % member_type.name)
                        else:
                            self.typedefs[member_type.type_name] = data_set.members[member_name]

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
                    snippet = type_definition.get_snippet()
                else:
                    continue

                snippets.append(snippet)

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

        if not isinstance(dataset, DataSet):
            raise Exception("can only generate a csv from a DataSet")

        if dataset.data is None:
            raise Exception("cannot generate csv from dataless %s" % str(dataset))

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
        txt = ["\n"]

        #this c snipped need these headers
        headers = set()

        #these data type references are needed for this snippet
        typerefs = set()

        for idx, (member_name, member_type) in enumerate(dataset.members.items()):
            headers  |= member_type.get_headers()
            #in a struct there are references to member type definitions -> sic.
            typerefs |= member_type.get_typedefs()

        #optional struct description
        #prepend * before every comment line
        if dataset.struct_description != None:
            txt.extend([
                "/**\n * ",
                "\n * ".join(dataset.struct_description.split("\n")),
                "\n */\n",
            ])

        #struct definition
        txt.append("struct %s {\n" % (dataset.name_struct))

        #create struct members
        for member_name, member_type in dataset.members.items():

            if member_type.length > 1:
                dlength = "[%d]" % member_type.length
            else:
                dlength = ""

            txt.append("\t%s %s%s;\n" % (member_type.get_type(), member_name, dlength))

        #append member count variable
        txt.append("\n\tstatic constexpr size_t member_count = %d;\n" % len(dataset.members))

        #add filling function prototype
        struct_fill_signature  = self.fill_csignature % ""
        txt.append("\n\t%s;\n" % struct_fill_signature)

        #struct ends
        txt.append("};\n")

        snippet = ContentSnippet("".join(txt), dataset.name_struct_file, ContentSnippet.section_body)
        snippet.headers    |= headers
        snippet.typerefs   |= typerefs
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

        #creating a parser for a single field of the csv
        parsers = []

        for idx, (member_name, member_type) in enumerate(dataset.members.items()):
            parsers.append(member_type.get_parser(idx, member_name))
            headers  |= member_type.get_headers()
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
        snippet.headers  |= headers | get_headers("strtok_custom")
        snippet.typerefs |= typerefs
        return snippet


def get_headers(for_type):
    """
    returns the includable headers for using the given C type.
    """

    #these headers are needed for the type
    ret = set()

    cstdinth  = CHeader("cstdint", is_global=True)
    stringh   = CHeader("string",  is_global=True)
    cstringh  = CHeader("cstring", is_global=True)
    cstdioh   = CHeader("cstdio",  is_global=True)
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
        "strcmp":          { cstringh },
        "strncpy":         { cstringh },
        "strtok_custom":   { engine_util_strings_h },
        "sscanf":          { cstdioh  },
    }

    if for_type in type_map:
        ret |= type_map[for_type]

    return ret
