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

ENDIANNESS = "<"

#global member type modifiers
READ          = util.NamedObject("binary-read_member")
READ_EXPORT   = util.NamedObject("binary-read-export_member")
NOREAD_EXPORT = util.NamedObject("noread_export_member")
READ_UNKNOWN  = util.NamedObject("read_unknown_member")

#regex for matching type array definitions like int[1337]
#group 1: type name, group 2: length
vararray_match = re.compile("([{0}]+) *\\[([{0}]+)\\] *;?".format("a-zA-Z0-9_"))

#match a simple number
integer_match = re.compile("\\d+")


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

        ret = list()        #returned list of data definitions
        self_data = dict()  #data of the current object

        members = self.get_data_format(allowed_modes=(True, READ_EXPORT, NOREAD_EXPORT), flatten_includes=True)
        for is_parent, export, member_name, member_type in members:

            #gather data members of the currently queried object
            self_data[member_name] = getattr(self, member_name)

            if isinstance(member_type, MultisubtypeMember):
                dbg(lazymsg=lambda: "%s => entering member %s" % (filename, member_name), lvl=3)

                current_member_filename = "%s-%s" % (filename, member_name)

                if isinstance(member_type, SubdataMember):
                    is_single_subdata  = True
                    subdata_item_iter  = self_data[member_name]

                    #filename for the file containing the single subdata type entries:
                    submember_filename = current_member_filename

                else:
                    is_single_subdata  = False

                multisubtype_ref_file_data = list()  #file names for ref types
                subdata_definitions = list()         #subdata member DataDefitions
                for subtype_name, submember_class in member_type.class_lookup.items():
                    #if we are in a subdata member, this for loop will only run through once.
                    #else, do the actions for each subtype

                    if not is_single_subdata:
                        dbg(lazymsg=lambda: "%s => entering multisubtype member %s" % (filename, subtype_name), lvl=3)

                        #iterate over the data for the current subtype
                        subdata_item_iter  = self_data[member_name][subtype_name]

                        #filename for the file containing one of the subtype data entries:
                        submember_filename = "%s/%s-%s" % (filename, member_name, subtype_name)

                    submember_data = list()
                    for idx, submember_data_item in enumerate(subdata_item_iter):
                        if not isinstance(submember_data_item, Exportable):
                            raise Exception("tried to dump object not inheriting from Exportable")

                        #generate output filename for next-level files
                        nextlevel_filename = "%s/%s-%04d" % (submember_filename, member_name, idx)

                        #recursive call, fetches DataDefinitions and the next-level data dict
                        data_sets, data = submember_data_item.dump(nextlevel_filename)

                        #store recursively generated DataDefinitions to the flat list
                        ret += data_sets

                        #append the next-level entry to the list
                        #that will contain the data for the current level DataDefinition
                        if len(data.keys()) > 0:
                            submember_data.append(data)

                    #create file only if it has at least one entry
                    if len(submember_data) > 0:
                        #create DataDefinition for the next-level data pile.
                        subdata_definition = DataDefinition(
                            submember_class,
                            submember_data,
                            submember_filename,
                        )

                        if not is_single_subdata:
                            #create entry for type file index.
                            #for each subtype, create entry in the subtype data file lookup file
                            #sync this with MultisubtypeBaseFile!
                            multisubtype_ref_file_data.append({
                                MultisubtypeMember.MultisubtypeBaseFile.data_format[0][1]: subtype_name,
                                MultisubtypeMember.MultisubtypeBaseFile.data_format[1][1]: "%s%s" % (
                                    subdata_definition.name_data_file, GeneratedFile.output_preferences["csv"]["file_suffix"]
                                ),
                            })

                        subdata_definitions.append(subdata_definition)
                    else:
                        pass

                    if not is_single_subdata:
                        dbg(lazymsg=lambda: "%s => leaving multisubtype member %s" % (filename, subtype_name), lvl=3)

                #store filename instead of data list
                #is used to determine the file to read next.
                # -> multisubtype members: type file index
                # -> subdata members:      filename of subdata
                #TODO: save relative path?
                self_data[member_name] = current_member_filename

                #for multisubtype members, append data definition for storing references to all the subtype files
                if not is_single_subdata and len(multisubtype_ref_file_data) > 0:

                    #this is the type file index.
                    multisubtype_ref_file = DataDefinition(
                        MultisubtypeMember.MultisubtypeBaseFile,
                        multisubtype_ref_file_data,
                        self_data[member_name],                          #create file to contain refs to subtype files
                    )

                    subdata_definitions.append(multisubtype_ref_file)

                #store all created submembers to the flat list
                ret += subdata_definitions

                dbg(lazymsg=lambda: "%s => leaving member %s" % (filename, member_name), lvl=3)


        #return flat list of DataDefinitions and dict of {member_name: member_value, ...}
        return ret, self_data

    def read(self, raw, offset, cls=None, members=None):
        """
        recursively read defined binary data from raw at given offset.

        this is used to fill the python classes with data from the binary input.
        """

        dbg(lazymsg=lambda: "-> 0x%08x => reading %s" % (offset, repr(self)), lvl=3)

        if cls:
            target_class = cls
        else:
            target_class = self

        #break out of the current reading loop when members don't exist in source data file
        stop_reading_members = False

        if not members:
            members = target_class.get_data_format(allowed_modes=(True, READ_EXPORT, READ, READ_UNKNOWN), flatten_includes=False)

        for is_parent, export, var_name, var_type in members:

            if stop_reading_members:
                if isinstance(var_type, DataMember):
                    replacement_value = var_type.get_empty_value()
                else:
                    replacement_value = 0

                setattr(self, var_name, replacement_value)
                continue

            if isinstance(var_type, GroupMember):
                if not issubclass(var_type.cls, Exportable):
                    raise Exception("class where members should be included is not exportable: %s" % var_type.cls.__name__)

                if isinstance(var_type, IncludeMembers):
                    #call the read function of the referenced class (cls),
                    #but store the data to the current object (self).
                    offset = var_type.cls.read(self, raw, offset, cls=var_type.cls)
                else:
                    #create new instance of referenced class (cls),
                    #use its read method to store data to itself,
                    #then save the result as a reference named `var_name`
                    #TODO: constructor argument passing may be required here.
                    grouped_data = var_type.cls()
                    offset = grouped_data.read(raw, offset)

                    setattr(self, var_name, grouped_data)

            elif isinstance(var_type, MultisubtypeMember):
                #subdata reference implies recursive call for reading the binary data

                #arguments passed to the next-level constructor.
                varargs = dict()

                if var_type.passed_args:
                    if type(var_type.passed_args) == str:
                        var_type.passed_args = set(var_type.passed_args)
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

                #check if entries need offset checking
                if var_type.offset_to:
                    offset_lookup = getattr(self, var_type.offset_to[0])
                else:
                    offset_lookup = None

                for i in range(list_len):

                    #if datfile offset == 0, entry has to be skipped.
                    if offset_lookup:
                        if not var_type.offset_to[1](offset_lookup[i]):
                            continue
                        #TODO: don't read sequentially, use the lookup as new offset?

                    if single_type_subdata:
                        #append single data entry to the subdata object list
                        new_data_class = var_type.class_lookup[None]
                    else:
                        #to determine the subtype class, read the binary definition
                        #this utilizes an on-the-fly definition of the data to be read.
                        offset = self.read(
                            raw, offset, cls=target_class,
                            members=(((False,) + var_type.subtype_definition),)
                        )

                        #read the variable set by the above read call to
                        #use the read data to determine the denominaton of the member type
                        subtype_name = getattr(self, var_type.subtype_definition[1])

                        #look up the type name to get the subtype class
                        new_data_class = var_type.class_lookup[subtype_name]

                    if not issubclass(new_data_class, Exportable):
                        raise Exception("dumped data is not exportable: %s" % new_data_class.__name__)

                    #create instance of submember class
                    new_data = new_data_class(**varargs)

                    #dbg(lazymsg=lambda: "%s: calling read of %s..." % (repr(self), repr(new_data)), lvl=4)

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
                is_custom_member = False

                if type(var_type) == str:
                    #TODO: generate and save member type on the fly
                    #instead of just reading
                    is_array = vararray_match.match(var_type)

                    if is_array:
                        struct_type = is_array.group(1)
                        data_count  = is_array.group(2)
                        if struct_type == "char":
                            struct_type = "char[]"

                        if integer_match.match(data_count):
                            #integer length
                            data_count = int(data_count)
                        else:
                            #dynamic length specified by member name
                            data_count = getattr(self, data_count)

                    else:
                        struct_type = var_type
                        data_count  = 1

                elif isinstance(var_type, DataMember):
                    #special type requires having set the raw data type
                    struct_type = var_type.raw_type
                    data_count  = var_type.get_length(self)
                    is_custom_member = True

                else:
                    raise Exception("unknown data member definition %s for member '%s'" % (var_type, var_name))

                if data_count < 0:
                    raise Exception("invalid length %d < 0 in %s for member '%s'" % (data_count, var_type, var_name))

                if struct_type not in util.struct_type_lookup:
                    raise Exception("%s: member %s requests unknown data type %s" % (repr(self), var_name, struct_type))

                if export == READ_UNKNOWN:
                    #for unknown variables, generate uid for the unknown memory location
                    var_name = "unknown-0x%08x" % offset

                #lookup c type to python struct scan type
                symbol = util.struct_type_lookup[struct_type]

                #read that stuff!!11
                dbg(lazymsg=lambda: "        @0x%08x: reading %s<%s> as '< %d%s'" % (offset, var_name, var_type, data_count, symbol), lvl=4)

                struct_format = "%s %d%s" % (ENDIANNESS, data_count, symbol)
                result        = struct.unpack_from(struct_format, raw, offset)

                dbg(lazymsg=lambda: "                \_ = %s" % (result, ), lvl=4)

                if is_custom_member:
                    if not var_type.verify_read_data(self, result):
                        raise Exception("invalid data when reading %s at offset %#08x" % (var_name, offset))

                #TODO: move these into a read entry hook/verification method
                if symbol == "s":
                    #stringify char array
                    result = util.zstr(result[0])
                elif data_count == 1:
                    #store first tuple element
                    result = result[0]

                    if symbol == "f":
                        import math
                        if not math.isfinite(result):
                            raise Exception("invalid float when reading %s at offset %#08x" % (var_name, offset))

                #increase the current file position by the size we just read
                offset += struct.calcsize(struct_format)

                #run entry hook for non-primitive members
                if is_custom_member:
                    result = var_type.entry_hook(result)

                    if result == ContinueReadMember.ABORT:
                        #don't go through all other members of this class!
                        stop_reading_members = True


                #store member's data value
                setattr(self, var_name, result)

        dbg(lazymsg=lambda: "<- 0x%08x <= finished %s" % (offset, repr(self)), lvl=3)
        return offset

    @classmethod
    def structs(cls):
        """
        create struct definitions for this class and its subdata references.
        """

        ret = list()

        dbg(lazymsg=lambda: "%s: generating structs" % (repr(cls)), lvl=2)

        #acquire all struct members, including the included members
        members = cls.get_data_format(allowed_modes=(True, READ_EXPORT, NOREAD_EXPORT), flatten_includes=False)
        for is_parent, export, member_name, member_type in members:

            dbg(lazymsg=lambda: "%s: exporting member %s<%s>" % (repr(cls), member_name, member_type), lvl=3)

            if isinstance(member_type, MultisubtypeMember):
                for subtype_name, subtype_class in member_type.class_lookup.items():
                    if not issubclass(subtype_class, Exportable):
                        raise Exception("tried to export structs from non-exportable %s" % subtype_class)
                    ret += subtype_class.structs()

            elif isinstance(member_type, GroupMember):
                dbg("entering group/include member %s of %s" % (member_name, cls), lvl=3)
                if not issubclass(member_type.cls, Exportable):
                    raise Exception("tried to export structs from non-exportable member included class %s" % repr(member_type.cls))
                ret += member_type.cls.structs()

            else:
                continue

        new_def = StructDefinition(cls)
        dbg(lazymsg=lambda: "=> %s: created new struct definition: %s" % (repr(cls), str(new_def)), lvl=3)
        ret.append(new_def)

        return ret

    @classmethod
    def get_effective_type(cls):
        return cls.name_struct

    @classmethod
    def get_data_format(cls, allowed_modes=False, flatten_includes=False, is_parent=False):
        for member in cls.data_format:
            export, member_name, member_type = member

            definitively_return_member = False

            if isinstance(member_type, IncludeMembers):
                if flatten_includes:
                    #recursive call
                    for m in member_type.cls.get_data_format(allowed_modes, flatten_includes, is_parent=True):
                        yield m
                    continue

            elif isinstance(member_type, ContinueReadMember):
                definitively_return_member = True

            if allowed_modes:
                if export not in allowed_modes:
                    if not definitively_return_member:
                        continue

            member_entry = (is_parent,) + member
            yield member_entry


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

    def __init__(self, data, file_name, section, reprtxt=None):
        self.data      = data       #snippet content
        self.file_name = file_name  #snippet wants to be saved in this file
        self.typerefs  = set()      #these types are referenced
        self.typedefs  = set()      #these types are defined
        self.includes  = set()      #needed snippets, e.g. headers
        self.section   = section
        self.reprtxt   = reprtxt    #representation text

        self.required_snippets = set() #snippets to be positioned before this one

    def get_data(self):
        return self.data

    def add_required_snippets(self, snippet_list):
        """
        save required snippets for this one by looking at wanted type references
        """

        self.required_snippets |= {s for s in snippet_list if len(self.typerefs & s.typedefs) > 0}

        dbg(lazymsg=lambda: "snippet %s requires %s" % (repr(self), repr(self.required_snippets)), lvl=4)

        resolved_types = set()
        for s in self.required_snippets:
            resolved_types |= (self.typerefs & s.typedefs)

        missing_types  = self.typerefs - resolved_types

        return missing_types

    def get_required_snippets(self, defined=None):
        """
        return all referenced and the snippet itself in the order they
        need to be put in the file.
        """

        #TODO: loop detection
        ret = list()

        dbg(lazymsg=lambda: "required snippets for %s {" % (repr(self)), push=True, lvl=4)
        for s in self.required_snippets:
            ret += s.get_required_snippets()

        dbg(pop=True, lvl=4)
        dbg(lazymsg=lambda: "}", lvl=4)

        ret.append(self)
        return ret

    def __hash__(self):
        return hash((self.data, self.file_name, self.section))

    def __eq__(self, other):
        if not other or type(other) != type(self):
            return False

        return (
            self.file_name   == other.file_name
            and self.data    == other.data
            and self.section == other.section
        )

    def __repr__(self):
        if self.reprtxt:
            data = self.reprtxt
        else:
            data = "%s..." % repr(self.data[:25])
        return "ContentSnippet[file=%s](%s)" % (self.file_name, data)

    def __str__(self):
        return "".join((
            repr(self), ", "
            "data = '", self.data, "'"
        ))


class HeaderSnippet(ContentSnippet):
    """
    represents an includable C header
    """

    def __init__(self, header_name, file_name=True, is_global=False):
        self.is_global = is_global
        self.name = header_name

        if self.is_global:
            data = "#include <%s>\n" % self.name
        else:
            data = "#include \"%s\"\n" % self.name

        super().__init__(data, file_name, ContentSnippet.section_header)

    def get_file_name(self):
        return self.name

    def __hash__(self):
        return hash((self.name, self.is_global))

    def __eq__(self, other):
        return (
            type(self) == type(other)
            and self.name == other.name
            and self.is_global == other.is_global
        )

    def __repr__(self):
        sym0 = "<" if self.is_global else "\""
        sym1 = ">" if self.is_global else "\""
        return "HeaderSnippet(%s%s%s)" % (sym0, self.name, sym1)


class GeneratedFile:
    """
    represents a writable file that was generated automatically.

    it's filled by many ContentSnippets before its contents are generated.
    """

    namespace = "gamedata"

    @classmethod
    def namespacify(cls, var_type):
        return "%s::%s" % (cls.namespace, var_type)

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
            "content_prefix": """#ifndef _${header_guard}_H_
#define _${header_guard}_H_

${headers}
%s

namespace ${namespace} {

""" % dontedit,
            "content_suffix": """
} //namespace ${namespace}

#endif // _${header_guard}_H_
""",
        },
        "structimpl": {
            "file_suffix":    ".cpp",
            "content_prefix": """
${headers}
%s

namespace ${namespace} {\n\n""" % dontedit,
            "content_suffix": "} //namespace ${namespace}\n",
        }
    }


    def __init__(self, file_name, format):
        self.snippets  = set()
        self.typedefs  = set()
        self.typerefs  = set()
        self.file_name = file_name
        self.format    = format

    def add_snippet(self, snippet):
        if not isinstance(snippet, ContentSnippet):
            raise Exception("only ContentSnippets can be added to generated files, tried %s" % type(snippet))

        if not snippet.file_name == self.file_name and snippet.file_name != True:
            raise Exception("only snippets with the same target file_name can be put into the same generated file.")

        self.snippets.add(snippet)
        self.typedefs |= snippet.typedefs
        self.typerefs |= snippet.typerefs

        dbg(lazymsg=lambda: "adding snippet to %s:" % (repr(self)), lvl=2)
        dbg(lazymsg=lambda: " %s"                   % repr(snippet), lvl=2)
        dbg(lazymsg=lambda: " typedefs:  %s"        % snippet.typedefs, lvl=3)
        dbg(lazymsg=lambda: " typerefs:  %s"        % snippet.typerefs, lvl=3)
        dbg(lazymsg=lambda: " includes:  %s {"      % repr(snippet.includes), lvl=3)

        #recursively add all included snippets
        for s in snippet.includes:
            dbg(push="snippet_add")
            self.add_snippet(s)
            dbg(pop="snippet_add")

        dbg(lazymsg=lambda: "}", lvl=3)

    def get_include_snippet(self, file_name=True):
        """
        return a snippet with a header entry for this file to be able to include it.
        """

        ret = HeaderSnippet(
            "".join((self.file_name, self.output_preferences[self.format]["file_suffix"])),
            file_name=file_name,
            is_global=False,
        )

        ret.typedefs |= self.typedefs
        return ret

    def create_xref_headers(self, file_pool):
        """
        discover and add needed header snippets for type references accross files.
        """

        if self.format in ("csv",):
            return

        dbg("%s typerefs %s" % (repr(self), repr(self.typerefs)), lvl=3)
        dbg("%s typedefs %s" % (repr(self), repr(self.typedefs)), lvl=3)

        new_resolves = set()
        for include_candidate in file_pool:
            candidate_resolves = include_candidate.typedefs & (self.typerefs - self.typedefs)
            if len(candidate_resolves) > 0:
                new_header = include_candidate.get_include_snippet()

                dbg(lazymsg=lambda: "%s adding header %s" % (repr(self), repr(new_header)), push="add_header", lvl=3)
                self.add_snippet(new_header)
                dbg(pop="add_header")

                new_resolves |= candidate_resolves

        still_missing = (self.typerefs - self.typedefs) - new_resolves
        if len(still_missing) > 0:
            raise Exception("still missing types for %s:\n%s" % (self, still_missing))

    def generate(self):
        """
        actually generate the content for this file.
        """

        #TODO: create new snippets for resolving cyclic dependencies (forward declarations)

        dbg(push="generation", lvl=2)

        dbg(lazymsg=lambda: "".join((
            "\n===========\n",
            "snippets stored for %s:\n" % (repr(self)),
            pprint.pformat(self.snippets),
            "\n-----------",
        )), lvl=3)

        #apply preference overrides
        prefs = self.default_preferences.copy()
        prefs.update(self.output_preferences[self.format])

        snippets_header = {s for s in self.snippets if s.section == ContentSnippet.section_header}
        snippets_body   = self.snippets - snippets_header

        #type references in this file that could not be resolved
        missing_types = set()

        #put snippets into list in correct order
        #snippets will be written according to this [(snippet, prio), ...] list
        snippets_priorized = list()

        #determine each snippet's priority by number of type references and definitions
        #smaller prio means written earlier in the file.
        dbg("assigning snippet priorities:", push="snippetprio", lvl=4)
        for s in snippets_body:
            snippet_prio = len(s.typerefs) - len(s.typedefs)
            snippets_priorized.append((s, snippet_prio))

            #let each snippet find others as dependencies
            missing_types |= s.add_required_snippets(self.snippets)

            dbg(lazymsg=lambda: "prio %3.d => %s" % (snippet_prio, repr(s)), lvl=4)
        dbg(pop="snippetprio")

        if len(missing_types) > 0:
            raise Exception("missing types for %s:\n%s" % (repr(self), pprint.pformat(missing_types)))

        #sort snippets according to their priority determined above
        snippets_priorized_sorted = (snippet for (snippet, _) in sorted(snippets_priorized, key=lambda s: s[1]))

        #create list of snippets to be put in the generated file.
        snippets_body_ordered = list()

        #fetch list of all required snippets for all snippets to put in the file
        for snippet in snippets_priorized_sorted:
            snippet_candidates = snippet.get_required_snippets()
            dbg(lazymsg=lambda: "required dependency snippet candidates: %s" % (pprint.pformat(snippet_candidates)), lvl=3)
            for s in snippet_candidates:
                if s.section == ContentSnippet.section_header and s not in snippets_header:
                    dbg(lazymsg=lambda: " `-> ADD  snippet %s" % (repr(s)), lvl=4)
                    snippets_ordered.append(s)

                elif s.section == ContentSnippet.section_body and s not in snippets_body_ordered:
                    snippets_body_ordered.append(s)
                    dbg(lazymsg=lambda: " `-> ADD  snippet %s" % (repr(s)), lvl=4)

                else:
                    dbg(lazymsg=lambda: " `-> SKIP snippet %s" % (repr(s)), lvl=4)


        #these snippets will be written outside the namespace
        #in the #include section
        snippets_header_sorted = sorted(snippets_header, key=lambda h: (not h.is_global, h.name))

        dbg(lazymsg=lambda: "".join((
            "\n-----------\n",
            "snippets after ordering for %s:\n" % (repr(self)),
            pprint.pformat(snippets_header_sorted + snippets_body_ordered),
            "\n===========",
        )), lvl=3)

        #merge file contents
        header_data = "".join(header.get_data() for header in snippets_header_sorted)
        file_data   = "\n".join(snippet.get_data() for snippet in snippets_body_ordered)

        namespace    = self.namespace
        header_guard = "".join((namespace.upper(), "_", self.file_name.replace("/", "_").upper()))

        #fill file header and footer with the generated file_name
        content_prefix = Template(prefs["content_prefix"]).substitute(header_guard=header_guard, namespace=namespace, headers=header_data)
        content_suffix = Template(prefs["content_suffix"]).substitute(header_guard=header_guard, namespace=namespace)

        #this is the final file content
        file_data = "".join((content_prefix, file_data, content_suffix))

        #determine output file name
        output_file_name_parts = [
            prefs["folder"],
            "%s%s" % (self.file_name, prefs["file_suffix"])
        ]

        dbg(pop="generation")

        #whee, return the (file_name, content)
        return (os.path.join(*output_file_name_parts), file_data)

    def __repr__(self):
        return "GeneratedFile<%s>(file_name=%s)" % (self.format, self.file_name)


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
        when this data field is not filled, use the retured value instead.
        """
        return 0

    def get_length(self, obj=None):
        return self.length

    def verify_read_data(self, obj, data):
        """
        gets called for each entry. used to check for storage validity (e.g. 0 expected)
        """
        return True

    def get_struct_entries(self, member_name):
        """
        return the lines to put inside the C struct.
        """

        return [ "%s %s;" % (self.get_effective_type(), member_name) ]

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
        return { self.cls.name_struct_file }

    def get_typerefs(self):
        return { self.get_effective_type() }

    def get_effective_type(self):
        return self.cls.get_effective_type()

    def get_parsers(self, idx, member):
        #TODO: new type of csv file!
        return [ "this->%s.fill(buf[%d]);" % (member, idx) ]

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

    def get_parsers(self):
        raise Exception("this should never be called!")

    def __repr__(self):
        return "IncludeMember<%s>" % repr(self.cls)


class DynLengthMember(DataMember):
    """
    a member that can have a dynamic length.
    """

    any_length = util.NamedObject("any_length")

    def __init__(self, length):

        type_ok = False

        if (type(length) in (int, str)) or (length is self.any_length):
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
                #length is a lambda that determines the length by some fancy manner
                #pass the target object as lambda parameter.
                length_def = self.length(obj)

                #if the lambda returns a non-dynamic length (aka a number)
                #return it directly. otherwise, the returned variable name
                #has to be looked up again.
                if not self.is_dynamic_length(target=length_def):
                    return length_def

            else:
                #self.length specifies the attribute name where the length is stored
                length_def = self.length

            #look up the given member name and return the value.
            if not isinstance(length_def, str):
                raise Exception("length lookup definition is not str: %s<%s>" % (length_def, type(length_def)))

            return getattr(obj, length_def)

        else:
            #non-dynamic length (aka plain number) gets returned directly
            return self.length

    def is_dynamic_length(self, target=None):
        if target == None:
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


class RefMember(DataMember):
    """
    a struct member that can be referenced/references another struct.
    """

    def __init__(self, type_name, file_name):
        DataMember.__init__(self)
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
            return determine_headers(self.number_type)

    def get_effective_type(self):
        return self.number_type

    def __repr__(self):
        return self.number_type


#TODO: convert to KnownValueMember
class ZeroMember(NumberMember):
    """
    data field that is known to always needs to be zero.
    neat for finding offset errors.
    """

    def __init__(self, raw_type, length=1):
        super().__init__(raw_type)
        self.length = length

    def verify_read_data(self, obj, data):
        #fail if a single value of data != 0
        if any(False if v == 0 else True for v in data):
            return False
        else:
            return True


class ContinueReadMember(NumberMember):
    """
    data field that aborts reading further members of the class
    when its value == 0.
    """

    ABORT    = util.NamedObject("member_reading_abort")
    CONTINUE = util.NamedObject("member_reading_continue")

    def __init__(self, raw_type):
        super().__init__(raw_type)

    def entry_hook(self, data):
        if data == 0:
            return self.ABORT
        else:
            return self.CONTINUE

    def get_empty_value(self):
        return 0


class EnumMember(RefMember):
    """
    this struct member/data column is a enum.
    """

    def __init__(self, type_name, file_name=None, values=None):
        super().__init__(type_name, file_name)
        self.values    = values
        self.resolved  = True    #TODO, xrefs not supported yet.

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

    def get_typerefs(self):
        return { self.get_effective_type() }

    def get_effective_type(self):
        return self.type_name

    def validate_value(self, value):
        return value in self.values

    def get_snippets(self, file_name, format):
        """
        generate enum snippets from given data

        input: EnumMember
        output: ContentSnippet
        """

        if format == "struct":
            snippet_file_name = self.file_name or file_name

            txt = list()

            #create enum definition
            txt.extend([
                "enum class %s {\n\t" % self.type_name,
                ",\n\t".join(self.values),
                "\n};\n\n",
            ])

            snippet = ContentSnippet(
                "".join(txt),
                snippet_file_name,
                ContentSnippet.section_body,
                reprtxt="enum class %s" % self.type_name,
            )
            snippet.typedefs |= { self.type_name }

            return [ snippet ]
        else:
            return list()

    def __repr__(self):
        return "enum %s" % self.type_name


class EnumLookupMember(EnumMember):
    def __init__(self, type_name, lookup_dict, raw_type=None, file_name=None):
        super().__init__(type_name, file_name, values=lookup_dict.values())
        self.lookup_dict = lookup_dict
        self.raw_type = raw_type

    def entry_hook(self, data):
        return self.lookup_dict[data]


class CharArrayMember(DynLengthMember):
    """
    struct member/column type that allows to store equal-length char[n].
    """

    def __init__(self, length):
        super().__init__(length)
        self.raw_type = "char[]"

    def get_parsers(self, idx, member):
        if self.is_dynamic_length():
            return [ "this->%s = buf[%d];" % (member, idx) ]
        else:
            data_length = self.get_length()
            return [
                "strncpy(this->%s, buf[%d], %d); this->%s[%d] = '\\0';" % (
                    member, idx, data_length, member, data_length-1
                )
            ]

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

    class MultisubtypeBaseFile(Exportable):
        """
        class that describes the format
        for the base-file pointing to the per-subtype files.
        """

        name_struct_file   = "util"
        name_struct        = "multisubtype_ref"
        struct_description = "format for multi-subtype references"

        data_format = (
            (NOREAD_EXPORT, "subtype", "std::string"),
            (NOREAD_EXPORT, "filename", "std::string"),
        )


    def __init__(self, type_name, subtype_definition, class_lookup, length, passed_args=None, ref_to=None, offset_to=None, file_name=None, ref_type_params=None):
        RefMember.__init__(self, type_name, file_name)
        DynLengthMember.__init__(self, length)

        self.subtype_definition = subtype_definition  #!< to determine the subtype for each entry, read this value to do the class_lookup
        self.class_lookup       = class_lookup        #!< dict to look up type_name => exportable class
        self.passed_args        = passed_args         #!< list of member names whose values will be passed to the new class
        self.ref_to             = ref_to              #!< add this member name's value to the filename
        self.offset_to          = offset_to           #!< link to member name which is a list of binary file offsets
        self.ref_type_params    = ref_type_params     #!< dict to specify type_name => constructor arguments

        #no xrefs supported yet..
        self.resolved          = True

    def get_headers(self, output_target):
        if "struct" == output_target:
            return determine_headers("std::vector")
        elif "structimpl" == output_target:
            return determine_headers("read_csv_file")
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
        return [ "this->%s.fill(buf[%d]);" % (member, idx) ]

    def get_typerefs(self):
        return { self.type_name }

    def get_snippets(self, file_name, format):

        snippet_file_name = self.file_name or file_name

        if format == "struct":

            txt = list()

            txt.append("struct %s {\n" % (self.type_name))
            txt.extend([
                "\tstd::vector<%s> %s;\n" % (
                    GeneratedFile.namespacify(entry_type.get_effective_type()), entry_name
                ) for (entry_name, entry_type) in self.class_lookup.items()
            ])
            txt.append("\n\tbool fill(const char *filename);\n")
            txt.append("};\n")

            snippet = ContentSnippet(
                "".join(txt),
                snippet_file_name,
                ContentSnippet.section_body,
                reprtxt="multisubtype container struct %s" % self.type_name,
            )
            snippet.typedefs |= { self.type_name }
            snippet.typerefs |= self.get_contained_types()
            snippet.includes |= determine_headers("std::vector")

            return [ snippet ]

        elif format == "structimpl":
            txt = list()

            txt.append("bool %s::fill(const char *filename) {\n" % (self.type_name))

            txt.extend([
                "\t//read filenames storing data for all subtypes\n",
                "\tauto subtype_files = engine::util::read_csv_file<%s>(filename);\n\n" % (self.MultisubtypeBaseFile.name_struct),
                "\tif (subtype_files.size() != %d) {\n" % (len(self.class_lookup)),
                "\t\treturn false;\n",
                "\t}\n\n",
            ])

            entry_len_max = max(len(k) for k in self.class_lookup.keys())

            for (idx, (entry_name, entry_type)) in enumerate(self.class_lookup.items()):
                #TODO: automatic recursive file reading
                #0. read file where entry type files are defined.
                #1. read each of the entry type files and fill member.`entry_type`

                entry_spacing = " " * (entry_len_max - len(entry_name))

                txt.append(
                    "\tthis->%s%s = engine::util::read_csv_file<%s>(subtype_files[%d].%s.c_str());\n" % (
                        entry_name, entry_spacing, GeneratedFile.namespacify(entry_type.get_effective_type()), idx, self.MultisubtypeBaseFile.data_format[1][1]
                    )
                )
            txt.append("\n\treturn true;\n")
            txt.append("}\n")

            snippet = ContentSnippet(
                "".join(txt),
                snippet_file_name,
                ContentSnippet.section_body,
                reprtxt="multisubtype %s container fill function" % self.type_name,
            )
            snippet.typedefs |= { self.type_name }
            snippet.typerefs |= self.get_contained_types() | { self.MultisubtypeBaseFile.name_struct }
            snippet.includes |= determine_headers("read_csv_file") | determine_headers("std::vector")

            return [ snippet ]

        else:
            return list()

    def __repr__(self):
        return "MultisubtypeMember<%s,len=%s>" % (self.type_name, self.length)


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
            return determine_headers("std::vector")
        elif "structimpl" == output_target:
            return determine_headers("read_csv_file")
        else:
            return set()

    def get_subtype(self):
        return GeneratedFile.namespacify(tuple(self.get_contained_types())[0])

    def get_effective_type(self):
        return "std::vector<%s>" % (self.get_subtype())

    def get_parsers(self, idx, member):
        return [ "this->%s = engine::util::read_csv_file<%s>(buf[%d]);" % (member, self.get_subtype(), idx) ]

    def get_snippets(self, file_name, format):
        return list()

    def get_typerefs(self):
        return self.get_contained_types()

    def get_subdata_type_name(self):
        return self.class_lookup[None].__name__

    def __repr__(self):
        return "SubdataMember<%s,len=%s>" % (self.get_subdata_type_name(), self.length)


class ArrayMember(SubdataMember):
    """
    autogenerated subdata member for arrays like float[8].
    """

    def __init__(self, ref_type, length, ref_type_params=None):
        super().__init__(ref_type, length)

    def __repr__(self):
        return "ArrayMember<%s,len=%s>" % (self.get_subdata_type_name(), self.length)


class StructDefinition:
    """
    input data read from the data files.

    one data set roughly represents one struct in the gamedata dat file.
    it consists of multiple DataMembers, they define the struct members.
    """

    def __init__(self, target):

        dbg("generating struct definition from %s" % (repr(target)), lvl=3)

        self.name_struct_file   = target.name_struct_file    #!< name of file where generated struct will be placed
        self.name_struct        = target.name_struct         #!< name of generated C struct
        self.struct_description = target.struct_description  #!< comment placed above generated C struct

        #create ordered dict of member type objects from structure definition
        self.members = OrderedDict()
        self.inherited_members = list()
        self.parent_classes = list()

        target_members = target.get_data_format(allowed_modes=(True, READ_EXPORT, NOREAD_EXPORT), flatten_includes=True)
        for is_parent, export, member_name, member_type in target_members:

            if isinstance(member_type, IncludeMembers):
                raise Exception("something went very wrong, inheritance should be flattened at this point.")

            if type(member_name) is not str:
                raise Exception("member name has to be a string, currently: %s<%s>" % (str(member_name), type(member_name)))

            #create member type class according to the defined member type
            if type(member_type) == str:
                array_match = vararray_match.match(member_type)
                if array_match:
                    array_type   = array_match.group(1)
                    array_length = array_match.group(2)

                    if array_type == "char":
                        member = CharArrayMember(array_length)
                    elif array_type in NumberMember.type_scan_lookup:
                        #member = ArrayMember(ref_type=NumberMember, length=array_length, ref_type_params=[array_type])
                        #BIG BIG TODO
                        raise Exception("TODO: implement exporting arrays!")
                    else:
                        raise Exception("member %s has unknown array type %s" % (member_name, member_type))

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

            if is_parent:
                self.inherited_members.append(member_name)

        members = target.get_data_format(flatten_includes=False)
        for _, _, _, member_type in members:
            if isinstance(member_type, IncludeMembers):
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

            #TODO: allow prefixes for all file types (missing: struct, structimpl)
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

        output: {file_name: GeneratedFile, ...}
        """

        #storage of all needed content snippets
        generate_files = list()

        dbg("######################\ngenerating content snippets...", push="datagen", lvl=2)
        for format in requested_formats:
            files = dict()

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
                type_snippets = type_definition.get_snippets(type_definition.file_name, format)

                snippets.extend(type_snippets)

            #assign all snippets to generated files
            for snippet in snippets:

                #if this file was not yet created, do it nao
                if snippet.file_name not in files:
                    files[snippet.file_name] = GeneratedFile(snippet.file_name, format)

                files[snippet.file_name].add_snippet(snippet)

            generate_files.extend(files.values())
        dbg(pop="datagen")

        #files is currently:
        #[GeneratedFile, ...]

        #find xref header includes
        dbg("######################\ngenerating needed cross reference includes now...", push="includegen", lvl=2)
        for gen_file in generate_files:
            gen_file.create_xref_headers(generate_files)
        dbg(pop="includegen")

        dbg("######################\ngenerating real file contents now...", lvl=2)

        #actually generate the files
        ret = dict()

        #we now invoke the content generation for each generated file
        for gen_file in generate_files:
            file_name, content = gen_file.generate()
            ret[file_name] = content

        #return {file_name: content, ...}
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

                #insert filename to read this field
                if isinstance(member_type, MultisubtypeMember):
                    #subdata member stores the follow-up filename
                    entry = "%s%s" % (entry, GeneratedFile.output_preferences["csv"]["file_suffix"])

                #encode each data field, to escape newlines and commas
                row_entries.append(encode_value(entry))

            txt.extend((self.DELIMITER.join(row_entries), "\n"))

        return ContentSnippet(
            "".join(txt),
            dataset.name_data_file,
            ContentSnippet.section_body,
            reprtxt="csv for %s" % dataset.name_struct,
        )

    def generate_struct(self, dataset):
        """
        generate C struct snippet (that should be placed in a header)
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
            if member_name in dataset.inherited_members:
                #inherited members don't need to be added as they're stored in the superclass
                continue

            headers      |= member_type.get_headers("struct")
            needed_types |= member_type.get_typerefs() #TODO

            struct_entries.extend(["\t%s\n" % entry for entry in member_type.get_struct_entries(member_name)])

        #dbg(lazymsg=lambda: "%s needed_types = %s" % (dataset.name_struct, needed_types), lvl=4)

        parents = [parent_class.get_effective_type() for parent_class in dataset.parent_classes]

        needed_types |= set(parents)

        if len(parents) > 0:
            inheritance_txt = " : %s" % (", ".join(parents))
        else:
            inheritance_txt = ""

        #struct definition
        txt.append("struct %s%s {\n" % (dataset.name_struct, inheritance_txt))

        #struct member entries
        txt.extend(struct_entries)

        #append member count variable
        txt.append("\n\tstatic constexpr size_t member_count = %d;\n" % len(dataset.members))
        headers |= determine_headers("size_t")

        #add filling function prototype
        txt.append("\n\t%s;\n" % (self.fill_csignature % ""))

        #struct ends
        txt.append("};\n")

        snippet = ContentSnippet(
            "".join(txt),
            dataset.name_struct_file,
            ContentSnippet.section_body,
            reprtxt="struct %s%s" % (dataset.name_struct, inheritance_txt),
        )
        snippet.includes   |= headers
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
            #add lines for each parser to the list: [[parser line, ...], ...]
            parsers.append(member_type.get_parsers(idx, member_name))
            headers  |= member_type.get_headers("structimpl")

        #indent/newline the token parsers. inner loop = lines for one column
        parser_code = "\n\n\t".join("\n\t".join(p) for p in parsers)

        #prepend struct name to fill function signature
        fill_signature = self.fill_csignature % ("%s::" % dataset.name_struct)

        member_count = dataset.name_struct + "::member_count"

        #definition of filling function
        txt = Template("""$funcsignature {
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

        snippet = ContentSnippet(
            txt,
            dataset.name_struct_file,
            ContentSnippet.section_body,
            reprtxt=fill_signature,
        )
        snippet.includes |= headers | determine_headers("strtok_custom")
        snippet.typerefs |= typerefs | { dataset.name_struct }
        return snippet


def determine_headers(for_type):
    """
    returns the includable headers for using the given C type.
    """

    #these headers are needed for the type
    ret = set()

    cstdinth              = HeaderSnippet("stdint.h", is_global=True)
    stringh               = HeaderSnippet("string",   is_global=True)
    cstringh              = HeaderSnippet("cstring",  is_global=True)
    cstdioh               = HeaderSnippet("cstdio",   is_global=True)
    vectorh               = HeaderSnippet("vector",   is_global=True)
    cstddefh              = HeaderSnippet("stddef.h", is_global=True)
    engine_util_strings_h = HeaderSnippet("../engine/util/strings.h", is_global=False)
    engine_util_file_h    = HeaderSnippet("../engine/util/file.h", is_global=False)

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
        "size_t":          { cstddefh },
        "float":           set(),
        "int":             set(),
        "read_csv_file":   { engine_util_file_h },
    }

    if for_type in type_map:
        ret |= type_map[for_type]
    else:
        dbg("could not determine header for %s" % for_type, lvl=2)

    return ret
