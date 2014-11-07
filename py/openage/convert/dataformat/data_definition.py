# Copyright 2014-2014 the openage authors. See copying.md for legal info.

import os.path

from .content_snippet import ContentSnippet
from .generated_file import GeneratedFile
from .members import EnumMember, MultisubtypeMember
from .util import encode_value
from .struct_definition import StructDefinition

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

    def generate_csv(self, genfile):
        """
        create a text snippet to represent the csv data
        """

        member_types = self.members.values()
        csv_column_types = list()

        #create column types line entries as comment in the csv file
        for c_type in member_types:
            csv_column_types.append(repr(c_type))

        #the resulting csv content
        #begin with the csv information comment header
        txt = [
            "#struct ", self.name_struct, "\n",
            "".join("#%s\n" % line for line in self.struct_description.split("\n")),
            "#", genfile.DELIMITER.join(csv_column_types), "\n",
            "#", genfile.DELIMITER.join(self.members.keys()), "\n",
        ]

        from .multisubtype_base import MultisubtypeBaseFile

        #create csv data lines:
        for idx, data_line in enumerate(self.data):
            row_entries = list()
            for member_name, member_type in self.members.items():
                entry = data_line[member_name]

                make_relpath = False

                #check if enum data value is valid
                if isinstance(member_type, EnumMember):
                    if not member_type.validate_value(entry):
                        raise Exception("data entry %d '%s' not a valid %s value" % (idx, entry, repr(member_type)))

                #insert filename to read this field
                if isinstance(member_type, MultisubtypeMember):
                    #subdata member stores the follow-up filename
                    entry = "%s%s" % (entry, GeneratedFile.output_preferences["csv"]["file_suffix"])
                    make_relpath = True

                if self.target == MultisubtypeBaseFile:
                    #if the struct definition target is the multisubtype base file,
                    #it already created the filename entry.
                    #it needs to be made relative as well.
                    if member_name == MultisubtypeBaseFile.data_format[1][1]:
                        #only make the filename entry relative
                        make_relpath = True

                if make_relpath:
                    #filename to reference to, make it relative to the current file name
                    entry = os.path.relpath(entry, os.path.dirname(self.name_data_file))

                #encode each data field, to escape newlines and commas
                row_entries.append(encode_value(entry))

            #create one csv line, separated by DELIMITER (probably a ,)
            txt.extend((genfile.DELIMITER.join(row_entries), "\n"))

        if self.prefix:
            snippet_file_name = self.prefix + self.name_data_file
        else:
            snippet_file_name = self.name_data_file

        return [ContentSnippet(
            "".join(txt),
            snippet_file_name,
            ContentSnippet.section_body,
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
