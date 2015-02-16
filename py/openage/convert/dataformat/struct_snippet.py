# Copyright 2014-2015 the openage authors. See copying.md for legal info.

from string import Template

from .content_snippet import ContentSnippet


class StructSnippet(ContentSnippet):
    """
    text snippet for generating C++ structs.

    it can generate all struct members and inheritance annotations.
    """

    struct_base = Template("""${comment}struct ${struct_name}${inheritance} {
$members
};
""")

    def __init__(self, file_name, struct_name, comment=None, parents=None):
        super().__init__(None, file_name, ContentSnippet.section_body, orderby=struct_name)
        self.data_ready = False

        self.struct_name = struct_name
        self.member_list = list()

        self.set_comment(comment)
        self.set_parents(parents)

        self.reprtxt = "struct %s" % (self.struct_name)
        self.typedefs |= {self.struct_name}

    def set_comment(self, comment):
        if comment:
            self.comment = "".join((
                "/**\n * ",
                "\n * ".join(comment.split("\n")),
                "\n */\n",
            ))
        else:
            self.comment = ""

    def set_parents(self, parent_types):
        if parent_types and len(parent_types) > 0:
            self.typerefs |= set(parent_types)
            self.inheritance = " : %s" % (", ".join(parent_types))
        else:
            self.inheritance = ""

    def add_member(self, member):
        self.member_list.append(member)

    def add_members(self, members):
        self.member_list.extend(members)

    def generate_content(self):
        """
        generate C struct snippet (that should be placed in a header).
        it represents the struct definition in C-code.
        """

        self.members = "".join("\t%s\n" % m for m in self.member_list)

        self.data = self.struct_base.substitute(self.__dict__)
        self.data_ready = True

    def __hash__(self):
        return hash((
            self.struct_name,
            frozenset(self.member_list),
            self.data,
            self.file_name,
            self.section,
            frozenset(self.typedefs),
            frozenset(self.typerefs),
        ))

    def __eq__(self, other):
        if type(other) != type(self):
            return False

        return (
            self.struct_name == other.struct_name and
            self.file_name   == other.file_name and
            self.data        == other.data and
            self.member_list == other.member_list and
            self.section     == other.section and
            self.typedefs    == other.typedefs and
            self.typerefs    == other.typerefs
        )

    def __str__(self):
        if self.data_ready:
            return super().__str__(self)
        else:
            return "".join((
                repr(self), " => members = \n",
                pprint.pformat(self.member_list),
            ))
