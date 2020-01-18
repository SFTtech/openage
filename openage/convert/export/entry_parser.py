# Copyright 2014-2017 the openage authors. See copying.md for legal info.

"""
Creates templates for parsing contents of the exported
data files.
"""


from string import Template

from .content_snippet import ContentSnippet, SectionType


class EntryParser:
    """
    An entry parser for reading a single column value from a data file.
    """

    # what, shall i just make up some more?
    # pylint: disable=too-few-public-methods

    def __init__(self, lines, headers, typerefs, destination="fill"):
        self.lines = lines
        self.headers = headers
        self.typerefs = typerefs
        self.destination = destination

    def get_code(self, indentlevel=1):
        """
        Return the parser code, indented correctly.
        """
        indent = "    " * indentlevel
        return indent + ("\n" + indent).join(self.lines)


class ParserTemplate:
    """
    Tempalte for a data parser function, its content is generated.
    """

    def __init__(self, signature, template, impl_headers, headers):
        # function signature, containing %s as possible namespace prefix
        self.signature = signature

        # template text where insertions will be made
        self.template = template

        # headers for the c file
        self.impl_headers = impl_headers

        # headers for the header file
        self.headers = headers

    def get_signature(self, class_name):
        """
        Return the function signature of this parser.
        """
        return self.signature.format(class_name)

    def get_text(self, class_name, data):
        """
        Return the template text, where variables were
        substituted already. Variables are passed in through `data`.

        That way, generated code is inserted into the template.
        """
        data["signature"] = self.get_signature("{}::".format(class_name))
        return Template(self.template).substitute(data)


class ParserMemberFunction:
    """
    Define a parsing member function for generated code files.
    The parser member function consists of one or multiple ParserTemplates.

    The templates may vary depending on the member count,
    so that for 0 members the signature can be different to omit arguments.
    """

    def __init__(self, templates, func_name):
        # dict: {function_member_count (None=any): ParserTemplate}
        self.templates = templates
        self.func_name = func_name

    def get_template(self, lookup):
        """
        get the appropritate parser member function template
        lookup = len(function members)

        -> this allows to generate stub functions without unused variables.
        """

        if lookup not in self.templates.keys():
            lookup = None
        return self.templates[lookup]

    def get_snippet(self, parser_list, file_name, class_name, data):
        """
        Return a content snippet for this parser function.
        This is then placed into the generated file.
        """
        data["parsers"] = "\n".join(parser.get_code(1) for parser in parser_list)
        data["class_name"] = class_name

        # lookup function for length of parser list.
        # if the len is 0, this should provide the stub function.
        template = self.get_template(len(parser_list))
        snippet = ContentSnippet(
            template.get_text(class_name, data),
            file_name,
            SectionType.section_body,
            orderby="%s_%s" % (class_name, self.func_name),
            reprtxt=template.get_signature(class_name + "::"),
        )

        snippet.includes |= template.impl_headers | set().union(*(p.headers for p in parser_list))
        snippet.typerefs |= {class_name}.union(*(p.typerefs for p in parser_list))

        return snippet

    def get_signature(self):
        """
        return the function signature for this member function.
        """
        return self.templates[None].get_signature("")

    def get_headers(self):
        """
        return the needed headers for the function signature of this member function.
        """
        return self.templates[None].headers
