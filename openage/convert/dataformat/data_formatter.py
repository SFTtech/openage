# Copyright 2014-2017 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R

from . import entry_parser
from . import util
from .generated_file import GeneratedFile
from .members import RefMember


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
                0: entry_parser.ParserTemplate(
                    signature    = "int %sfill(char * /*by_line*/)",
                    headers      = set(),
                    impl_headers = set(),
                    template     = "$signature {\n\treturn -1;\n}"
                ),
                None: entry_parser.ParserTemplate(
                    signature    = "int %sfill(char *by_line)",
                    headers      = set(),
                    impl_headers = util.determine_headers(
                        ("strtok_custom", "engine_error")
                    ),
                    template     = """$signature {
\tchar *buf[$member_count];
\tint count = openage::util::string_tokenize_to_buf(by_line, '$delimiter', buf, $member_count);

\tif (count != $member_count) {
\t\tthrow openage::error::Error(MSG(err) <<
\t\t\t"Tokenizing $struct_name led to " << count << " columns ("
\t\t\t"expected " << $member_count << ")!");
\t}

$parsers

\treturn -1;
}
""",
                ),
            }
        ),
        "recurse": entry_parser.ParserMemberFunction(
            func_name = "recurse",
            templates = {
                0: entry_parser.ParserTemplate(
                    signature    = "int %srecurse(openage::util::Dir /*basedir*/)",
                    headers      = util.determine_header("engine_dir"),
                    impl_headers = set(),
                    template     = "$signature {\n\treturn -1;\n}\n"
                ),
                None: entry_parser.ParserTemplate(
                    signature = "int %srecurse(openage::util::Dir basedir)",
                    headers   = util.determine_header("engine_dir"),
                    impl_headers = set(),
                    template  = "$signature {\n$parsers\n\n\treturn -1;\n}\n"
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
