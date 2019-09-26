# Copyright 2014-2017 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R

from . import entry_parser
from . import util
from .generated_file import GeneratedFile
from openage.convert.dataformat.read_members import RefMember


class DataFormatter:
    """
    transforms and merges data structures
    the input data also specifies the output structure.

    this class generates the plaintext being stored in the data files
    it is the central part of the data exporting functionality.
    """

    # csv column delimiter:
    DELIMITER = ","

    member_methods = {
        "fill": entry_parser.ParserMemberFunction(
            func_name = "fill",
            templates = {
                # used as dummy when there is no field to parse
                0: entry_parser.ParserTemplate(
                    signature    = "int {}fill(const std::string & /*line*/)",
                    headers      = {"std::string"},
                    impl_headers = set(),
                    template     = (
                        "$signature {\n"
                        "    return -1;\n"
                        "}\n"
                    )
                ),
                # used to parse at least one member field of the struct
                None: entry_parser.ParserTemplate(
                    signature    = "int {}fill(const std::string &line)",
                    headers      = util.determine_headers(
                        ("std::string",)
                    ),
                    impl_headers = util.determine_headers(
                        ("strtok_custom", "engine_error", "std::vector")
                    ),
                    template     = (
                        "$signature {\n"
                        "    std::vector<std::string> buf = openage::util::split_escape(\n"
                        "        line, '$delimiter', $member_count\n"
                        "    );\n"
                        "\n"
                        "    if (buf.size() != $member_count) {\n"
                        "        throw openage::error::Error(\n"
                        "            ERR\n"
                        '            << "Tokenizing $struct_name led to "\n'
                        '            << buf.size()\n'
                        '            << " columns (expected "\n'
                        '            << $member_count\n'
                        '            << ")!"\n'
                        "        );\n"
                        "    }\n"
                        "\n"
                        "$parsers\n"
                        "\n"
                        "    return -1;\n"
                        "}\n"
                    ),
                )
            }
        ),
        "recurse": entry_parser.ParserMemberFunction(
            func_name = "recurse",
            templates = {
                # used when this struct requires no further recursion
                0: entry_parser.ParserTemplate(
                    signature    = (
                        "bool {}recurse(const openage::util::CSVCollection & /*storage*/, "
                        "const std::string & /*basedir*/)"
                    ),
                    headers      = util.determine_headers(
                        ("std::string", "csv_collection")
                    ),
                    impl_headers = set(),
                    template     = (
                        "$signature {\n"
                        "    return true;\n"
                        "}\n"
                    )
                ),
                # used when we have to recurse deeper
                None: entry_parser.ParserTemplate(
                    signature = (
                        "bool {}recurse(const openage::util::CSVCollection &storage, "
                        "const std::string &basedir)"
                    ),
                    headers   = util.determine_headers(
                        ("std::string", "csv_collection")
                    ),
                    impl_headers = set(),
                    template  = (
                        "$signature {\n"
                        "$parsers\n"
                        "\n"
                        "    return true;\n"
                        "}\n"
                    )
                ),
            }
        ),
    }

    def __init__(self):
        # list of all dumpable data sets
        self.data = list()

        # collection of all type definitions
        self.typedefs = dict()

    def add_data(self, data_set_pile, prefix=None, single_output=None):
        """
        add a given StructDefinition to the storage, so it can be exported later.

        other exported data structures are collected from the given input.
        """

        if not isinstance(data_set_pile, list):
            data_set_pile = [data_set_pile]

        # add all data sets
        for data_set in data_set_pile:

            # TODO: allow prefixes for all file types
            # (missing: struct, structimpl)
            if prefix:
                data_set.prefix = prefix

            if single_output:
                data_set.single_output = single_output

            # collect column type specifications
            for member_name, member_type in data_set.members.items():

                # store resolved (first-time definitions) members in a
                # symbol list
                if isinstance(member_type, RefMember):
                    if member_type.resolved:
                        if member_type.get_effective_type() in self.typedefs:
                            if data_set.members[member_name] is not self.typedefs[member_type.get_effective_type()]:
                                raise Exception("different redefinition of type %s" % member_type.get_effective_type())
                        else:
                            ref_member = data_set.members[member_name]

                            # if not overridden,
                            # use name of struct file to store
                            if ref_member.file_name is None:
                                ref_member.file_name = data_set.name_struct_file

                            self.typedefs[member_type.get_effective_type()] = ref_member

            self.data.append(data_set)

    def export(self, projectdir, requested_formats):
        """
        Generates files in the requested formats to projectdir.

        projectdir is a util.fslike.path.Path.
        """
        # storage of all needed content snippets
        generate_files = list()

        for format_ in requested_formats:
            files = dict()

            snippets = list()

            # iterate over all stored data sets and
            # generate all data snippets for the requested output formats.
            for data_set in self.data:

                # resolve data xrefs for this data_set
                data_set.dynamic_ref_update(self.typedefs)

                # generate one output chunk list for each requested format
                if format_ == "csv":
                    new_snippets = data_set.generate_csv(self)

                elif format_ == "struct":
                    new_snippets = data_set.generate_struct(self)

                elif format_ == "structimpl":
                    new_snippets = data_set.generate_struct_implementation(self)

                else:
                    raise Exception("unknown export format %s requested" % format_)

                snippets.extend(new_snippets)

            # create snippets for the encountered type definitions
            for _, type_definition in sorted(self.typedefs.items()):
                type_snippets = type_definition.get_snippets(type_definition.file_name, format_)
                snippets.extend(type_snippets)

            # assign all snippets to generated files
            for snippet in snippets:

                # if this file was not yet created, do it nao
                if snippet.file_name not in files:
                    files[snippet.file_name] = GeneratedFile(snippet.file_name, format_)

                files[snippet.file_name].add_snippet(snippet)

            generate_files.extend(files.values())

        # files is currently a list of GeneratedFile objects.

        # find xref header includes
        for gen_file in generate_files:
            # only create headers for non-data files
            if gen_file.format_ not in {"csv"}:
                gen_file.create_xref_headers(generate_files)
                gen_file.create_forward_declarations(generate_files)

        # we now invoke the content generation for each generated file
        for gen_file in generate_files:
            file_name, content = gen_file.generate()
            with projectdir[file_name].open('wb') as outfile:
                outfile.write(content.encode('utf-8'))
