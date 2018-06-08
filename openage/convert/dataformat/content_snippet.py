# Copyright 2014-2018 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R

from enum import Enum

from ...log import spam


class SectionType(Enum):
    section_header = "header"
    section_body = "body"


class ContentSnippet:
    """
    one part of text for generated files to be saved in "file_name"

    before whole source files can be written, it's content snippets
    have to be ordered according to their dependency chain.

    also, each snipped can have import requirements that have to be
    included in top of the source.
    """

    def __init__(self, data, file_name, section, orderby=None, reprtxt=None):
        self.data      = data       # snippet content
        self.file_name = file_name  # snippet wants to be saved in this file
        self.typerefs  = set()      # these types are referenced
        self.typedefs  = set()      # these types are defined
        self.includes  = set()      # needed snippets, e.g. headers
        self.section   = section    # place the snippet in this file section
        self.orderby   = orderby    # use this value for ordering snippets
        self.reprtxt   = reprtxt    # representation text

        # snippets to be positioned before this one
        self.required_snippets = set()

        # snippet content is ready by default.
        # subclasses may require generation.
        self.data_ready = True

    def get_data(self):
        if not self.data_ready:
            self.generate_content()
        return self.data

    def generate_content(self):
        # no generation needed by default
        pass

    def add_required_snippets(self, snippet_list):
        """
        save required snippets for this one by looking at wanted type
        references.

        the available candidates have to be passed as argument
        """

        self.required_snippets |= {
            s for s in snippet_list if len(self.typerefs & s.typedefs) > 0
        }

        spam("snippet %s requires %s",
             repr(self), repr(self.required_snippets))

        resolved_types = set()
        for s in self.required_snippets:
            resolved_types |= (self.typerefs & s.typedefs)

        missing_types  = self.typerefs - resolved_types

        return missing_types

    def get_required_snippets(self):
        """
        return all referenced and the snippet itself in the order they
        need to be put in the file.
        """

        # TODO: loop detection
        ret = list()

        # sort snippets deterministically by __lt__ function
        for s in sorted(self.required_snippets):
            ret += s.get_required_snippets()

        ret.append(self)
        return ret

    def __hash__(self):
        """
        hash all relevant snippet properties
        """

        return hash((
            self.data,
            self.file_name,
            self.section,
            frozenset(self.typedefs),
            frozenset(self.typerefs),
        ))

    def __lt__(self, other):
        """
        comparison of two snippets for their ordering
        """

        if isinstance(other, type(self)) or isinstance(self, type(other)):
            if not (other.orderby and self.orderby):
                faild = self if other.orderby else other
                raise Exception("%s doesn't have orderby member set" % (
                    repr(faild)))
            else:
                ret = self.orderby < other.orderby
                return ret
        else:
            raise TypeError("unorderable types: %s < %s" % (
                type(self), type(other)
            ))

    def __eq__(self, other):
        """
        equality check for text snippets
        """

        if type(other) is not type(self):
            return False

        return (
            self.file_name == other.file_name and
            self.data      == other.data and
            self.section   == other.section and
            self.typedefs  == other.typedefs and
            self.typerefs  == other.typerefs
        )

    def __repr__(self):
        if self.reprtxt:
            data = " = %s" % self.reprtxt
        elif self.data:
            data = " = %s..." % repr(self.data[:25])
        else:
            data = ""

        return "%s(file=%s)%s" % (self.__class__.__name__,
                                  self.file_name, data)

    def __str__(self):
        if self.data_ready:
            return "".join((
                repr(self), ", "
                "data = '", str(self.data), "'"
            ))
        else:
            return "".join((repr(self), ": lazy generation pending"))
