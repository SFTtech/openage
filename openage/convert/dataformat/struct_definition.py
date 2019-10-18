# Copyright 2014-2017 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R

from collections import OrderedDict
import re

from openage.convert.dataformat.read_members import IncludeMembers, StringMember, CharArrayMember, NumberMember, ReadMember, RefMember
from .member_access import READ_EXPORT, NOREAD_EXPORT
from .content_snippet import ContentSnippet, SectionType
from .struct_snippet import StructSnippet
from .util import determine_header


# regex for matching type array definitions like int[1337]
# group 1: type name, group 2: length
vararray_match = re.compile("([{0}]+) *\\[([{0}]+)\\] *;?".format("a-zA-Z0-9_"))

# match a simple number
integer_match = re.compile("\\d+")


class StructDefinition:
    """
    input data read from the data files.

    one data set roughly represents one struct in the gamedata dat file.
    it consists of multiple DataMembers, they define the struct members.
    """

    def __init__(self, target):
        """
        create a struct definition from an Exportable
        """

        # name of file where generated struct will be placed
        self.name_struct_file   = target.name_struct_file

        # name of generated C struct
        self.name_struct        = target.name_struct

        # comment placed above generated C struct
        self.struct_description = target.struct_description

        # if not None, a prefix will be added to the final path
        self.prefix             = None

        # if not None, will be packed with other files into a single one
        self.single_output      = None

        # target Exportable class that defines the data format
        self.target             = target

        # create ordered dict of member type objects from structure definition
        self.members = OrderedDict()
        self.inherited_members = list()
        self.parent_classes = list()

        target_members = target.get_data_format(
            allowed_modes=(True, READ_EXPORT, NOREAD_EXPORT),
            flatten_includes=True
        )

        for is_parent, _, member_name, _, member_type in target_members:

            if isinstance(member_type, IncludeMembers):
                raise Exception("something went very wrong, "
                                "inheritance should be flattened at this point.")

            if not isinstance(member_name, str):
                raise Exception("member name has to be a string, "
                                "currently: %s<%s>" % (str(member_name),
                                                       type(member_name)))

            # create member type class according to the defined member type
            if isinstance(member_type, str):
                array_match = vararray_match.match(member_type)
                if array_match:
                    array_type   = array_match.group(1)
                    array_length = array_match.group(2)

                    if array_type == "char":
                        member = CharArrayMember(array_length)

                    elif array_type in NumberMember.type_scan_lookup:
                        # member = ArrayMember(ref_type=NumberMember,
                        #                      length=array_length,
                        #                      ref_type_params=[array_type])
                        # BIG BIG TODO
                        raise NotImplementedError("implement exporting arrays!")

                    else:
                        raise Exception("member %s has unknown array type %s" % (member_name, member_type))

                elif member_type == "std::string":
                    member = StringMember()
                else:
                    member = NumberMember(member_type)

            elif isinstance(member_type, ReadMember):
                member = member_type

            else:
                raise Exception("unknown member type specification!")

            if member is None:
                raise Exception("member %s of struct %s is None" % (member_name,
                                                                    self.name_struct))

            self.members[member_name] = member

            if is_parent:
                self.inherited_members.append(member_name)

        members = target.get_data_format(flatten_includes=False)
        for _, _, _, _, member_type in members:
            if isinstance(member_type, IncludeMembers):
                self.parent_classes.append(member_type.cls)

    def dynamic_ref_update(self, lookup_ref_data):
        """
        update ourself the with the given reference data.

        data members can be cross references to definitions somewhere else.
        this symbol resolution is done here by replacing the references.
        """

        for _, member_type in self.members.items():
            if not isinstance(member_type, RefMember):
                continue

            # this member of self is already resolved
            if member_type.resolved:
                continue

            type_name = member_type.get_effective_type()

            # replace the xref with the real definition
            self.members[type_name] = lookup_ref_data[type_name]

    def generate_struct(self, genfile):
        """
        generate C struct snippet (that should be placed in a header).
        it represents the struct definition in C-code.
        """

        parents = [parent_class.get_effective_type()
                   for parent_class in self.parent_classes]

        snippet = StructSnippet(self.name_struct_file,
                                self.name_struct,
                                self.struct_description,
                                parents)

        snippet.typedefs.add(self.name_struct)

        # add struct members and inheritance parents
        for member_name, member_type in self.members.items():
            if member_name in self.inherited_members:
                # inherited members don't need to be added as they're stored in the superclass
                continue

            snippet.includes |= member_type.get_headers("struct")
            snippet.typerefs |= member_type.get_typerefs()

            snippet.add_members(member_type.get_struct_entries(member_name))

        # append member count variable
        snippet.add_member("static constexpr size_t member_count = %d;" % len(self.members))
        snippet.includes |= determine_header("size_t")

        # add filling function prototypes
        for _, member in sorted(genfile.member_methods.items()):
            snippet.add_member("%s;" % member.get_signature())
            snippet.includes |= member.get_headers()

        return [snippet]

    def generate_struct_implementation(self, genfile):
        """
        create C code for the implementation of the struct functions.
        it is used to fill a struct instance with data of a csv data line.
        """

        # returned snippets
        ret = list()

        # constexpr member count definition
        ret.append(ContentSnippet(
            data="constexpr size_t %s::member_count;" % self.name_struct,
            file_name=self.name_struct_file,
            section=SectionType.section_body,
            orderby=self.name_struct,
        ))

        # variables to be replaced in the function template
        template_args = {
            "member_count":  self.name_struct + "::member_count",
            "delimiter":     genfile.DELIMITER,
            "struct_name":   self.name_struct,
        }

        # create a list of lines for each parser
        # a parser converts one csv line to struct entries
        parsers = {key: [] for key in genfile.member_methods}

        # place all needed parsers into their requested member function destination
        for idx, (member_name, member_type) in enumerate(self.members.items()):
            for parser in member_type.get_parsers(idx, member_name):
                parsers[parser.destination].append(parser)

        # create parser snippets and return them
        for parser_type, parser_list in parsers.items():
            ret.append(
                genfile.member_methods[parser_type].get_snippet(
                    parser_list,
                    file_name  = self.name_struct_file,
                    class_name = self.name_struct,
                    data       = template_args,
                )
            )

        return ret

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
