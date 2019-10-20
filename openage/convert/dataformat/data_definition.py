# Copyright 2014-2019 the openage authors. See copying.md for legal info.

"""
Output format specification for data to write.
"""

import os.path

from .content_snippet import ContentSnippet, SectionType
from .generated_file import GeneratedFile
from openage.convert.dataformat.read_members import EnumMember, MultisubtypeMember
from .util import encode_value, commentify_lines
from .struct_definition import StructDefinition


class DataDefinition(StructDefinition):
    """
    Contains a data definition, which is a list of dicts
    [{member_name: value}, ...]
    this can then be formatted to an arbitrary output file.
    """

    def __init__(self, target, data, name_data_file):
        super().__init__(target)

        # list of dicts, member_name=>member_value
        self.data = data

        # name of file where data will be placed in
        self.name_data_file = name_data_file

    def generate_csv(self, genfile):
        """
        create a text snippet to represent the csv data
        """

        # pylint: disable=too-many-locals

        # TODO: This method is in SERIOUS need of some refactoring, e.g. extracting methods.
        #       However, this will become obsolete with the upcoming sprite metadata files,
        #       so we hope to get away with not touchin it. If you need to make modifications
        #       here, you are going to fix this pile of crap first.

        member_types = self.members.values()
        csv_column_types = list()

        # create column types line entries as comment in the csv file
        for c_type in member_types:
            csv_column_types.append(repr(c_type))

        # the resulting csv content
        txt = []

        # create the file meta comment for sigle file packed csv files
        if self.single_output:
            txt.append("### %s.docx\n" % (self.name_data_file))

        # create the csv information comment header
        txt.extend([
            "# struct %s\n" % self.name_struct,
            commentify_lines("# ", self.struct_description),
            "# ", genfile.DELIMITER.join(csv_column_types), "\n",
            "# ", genfile.DELIMITER.join(self.members.keys()), "\n",
        ])

        # create csv data lines:
        for idx, data_line in enumerate(self.data):
            row_entries = list()
            for member_name, member_type in self.members.items():
                entry = data_line[member_name]

                make_relpath = False

                # check if enum data value is valid
                if isinstance(member_type, EnumMember) and\
                   not member_type.validate_value(entry):

                    raise Exception("data entry %d '%s'"
                                    " not a valid %s value" %
                                    (idx, entry, repr(member_type)))

                # insert filename to read this field
                if isinstance(member_type, MultisubtypeMember):
                    # subdata member stores the follow-up filename
                    entry += GeneratedFile.output_preferences["csv"]["file_suffix"]
                    make_relpath = True

                from .multisubtype_base import MultisubtypeBaseFile
                if self.target == MultisubtypeBaseFile:
                    # if the struct definition target is the multisubtype
                    # base file, it already created the filename entry.
                    # it needs to be made relative as well.
                    if member_name == MultisubtypeBaseFile.data_format[1][1]:
                        # only make the filename entry relative
                        make_relpath = True

                if make_relpath:
                    # filename to reference to, make it relative to the
                    # current file name
                    entry = os.path.relpath(
                        entry,
                        os.path.dirname(self.name_data_file)
                    ).replace(os.path.sep, '/')  # HACK: Change to better path handling

                # encode each data field, to escape newlines and commas
                row_entries.append(encode_value(entry))

            # create one csv line, separated by DELIMITER (probably a ,)
            txt.extend((genfile.DELIMITER.join(row_entries), "\n"))

        if self.single_output:
            snippet_file_name = self.single_output
        else:
            snippet_file_name = self.name_data_file

        if self.prefix:
            snippet_file_name = self.prefix + snippet_file_name

        return [ContentSnippet(
            "".join(txt),
            snippet_file_name,
            SectionType.section_body,
            orderby=self.name_struct,
            reprtxt="csv for %s" % self.name_struct,
        )]

    def __str__(self):
        ret = [
            "\n\tdata file name: ", str(self.name_data_file),
            "\n\tdata: ", str(self.data),
        ]
        return "%s%s" % (super().__str__(), "".join(ret))

    def __repr__(self):
        return "DataDefinition<%s>" % self.name_struct
