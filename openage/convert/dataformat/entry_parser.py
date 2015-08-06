# Copyright 2014-2015 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R

from string import Template

from .content_snippet import ContentSnippet, SectionType


class EntryParser:
    def __init__(self, lines, headers, typerefs, destination="fill"):
        self.lines       = lines
        self.headers     = headers
        self.typerefs    = typerefs
        self.destination = destination

    def get_code(self, indentlevel=1):
        indent = "\t" * indentlevel
        return indent + ("\n" + indent).join(self.lines)


class ParserTemplate:
    def __init__(self, signature, template, impl_headers, headers):
        self.signature    = signature     # function signature, containing %s as possible namespace prefix
        self.template     = template      # template text where insertions will be made
        self.impl_headers = impl_headers  # headers for the c file
        self.headers      = headers       # headers for the header file

    def get_signature(self, class_name):
        return self.signature % (class_name)

    def get_text(self, class_name, data):
        data["signature"] = self.get_signature("%s::" % class_name)
        return Template(self.template).substitute(data)


class ParserMemberFunction:
    def __init__(self, templates, func_name):
        self.templates = templates  # dict: key=function_member_count (None=any), value=ParserTemplate
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
        data["parsers"]    = "\n".join(parser.get_code(1) for parser in parser_list)
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
