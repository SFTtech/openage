# Copyright 2014-2014 the openage authors. See copying.md for legal info.

from .content_snippet import ContentSnippet


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

        super().__init__(data, file_name, ContentSnippet.section_header, orderby=header_name)

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
