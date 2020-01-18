# Copyright 2014-2019 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R

from string import Template

from .content_snippet import ContentSnippet, SectionType
from .header_snippet import HeaderSnippet


class GeneratedFile:
    """
    represents a writable file that was generated automatically.

    it's filled by many ContentSnippets before its contents are generated.
    """

    namespaces = ("openage", "gamedata")

    @classmethod
    def namespacify(cls, var_type):
        return "%s::%s" % ("::".join(cls.namespaces), var_type)

    # default preferences for output modes
    default_preferences = {
        "folder":         "",
        "file_suffix":    "",
        "process":        True,
        "content_prefix": Template(""),
        "content_suffix": Template(""),
    }

    # override the default preferences with the
    # configuration for all the output formats
    output_preferences = {
        "csv": {
            "folder":      "",
            "file_suffix": ".docx",
            "process":     False,
        },
        # main file structure for generated headers
        "struct": {
            "file_suffix": ".gen.h",
            "content_prefix": Template(
                "#pragma once\n"
                "\n"
                "${headers}\n"
                "\n\n"
                "${opennamespaces}\n"
                "\n"
            ),
            "content_suffix": Template(
                "\n"
                "${closenamespaces}\n"
            ),
        },
        # file structure for generated c++ implementation files
        "structimpl": {
            "file_suffix":    ".gen.cpp",
            "content_prefix": Template(
                "\n"
                "${headers}\n"
                "\n"
                "${opennamespaces}\n"
                "\n"
            ),
            "content_suffix": Template(
                "\n"
                "${closenamespaces}\n"
            ),
        }
    }

    def __init__(self, file_name, format_):
        self.snippets  = set()
        self.typedefs  = set()
        self.typerefs  = set()
        self.file_name = file_name
        self.format_   = format_
        self.included_typedefs = set()

    def add_snippet(self, snippet, inherit_typedefs=True):
        if not isinstance(snippet, ContentSnippet):
            raise Exception("only ContentSnippets can be added to generated files, "
                            "tried %s" % type(snippet))

        if not snippet.file_name == self.file_name and not snippet.file_name:
            raise Exception("only snippets with the same target file_name "
                            "can be put into the same generated file.")

        if snippet not in (self.snippets):
            self.snippets.add(snippet)

            if inherit_typedefs:
                self.typedefs |= snippet.typedefs
                self.typerefs |= snippet.typerefs
            else:
                self.included_typedefs |= snippet.typedefs

            # add all included snippets, namely HeaderSnippets for #include lol.h
            for s in snippet.includes:
                self.add_snippet(s, inherit_typedefs=False)

    def get_include_snippet(self, file_name=True):
        """
        return a snippet with a header entry for this file to be able to include it.
        """

        ret = HeaderSnippet(
            self.file_name + self.output_preferences[self.format_]["file_suffix"],
            file_name=file_name,
            is_global=False,
        )

        ret.typedefs |= self.typedefs
        return ret

    def create_xref_headers(self, file_pool):
        """
        discover and add needed header snippets for type references accross files.
        """

        new_resolves = set()
        for include_candidate in file_pool:
            candidate_resolves = include_candidate.typedefs & (self.typerefs - self.typedefs)

            if len(candidate_resolves) > 0:
                new_header = include_candidate.get_include_snippet()

                self.add_snippet(new_header, inherit_typedefs=False)

                new_resolves |= candidate_resolves

        still_missing = ((self.typerefs - self.typedefs) - self.included_typedefs) - new_resolves
        if len(still_missing) > 0:
            raise Exception("still missing types for %s:\n%s" % (self, still_missing))

    def create_forward_declarations(self, file_pool):
        """
        create forward declarations for this generated file.

        a forward declatation is needed when a referenced type is defined
        in an included header, that includes a header that includes the first one.
        """

    def generate(self):
        """
        actually generate the content for this file.
        """

        # TODO: create new snippets for resolving cyclic dependencies (forward declarations)

        # apply preference overrides
        prefs = self.default_preferences.copy()
        prefs.update(self.output_preferences[self.format_])

        # if there is no need for sorting and resolving dependencies, just return the snippets
        # as they have been added
        if not prefs["process"]:
            file_data = "\n".join(snippet.get_data() for snippet in self.snippets)
            return prefs["folder"] + '/' + self.file_name + prefs["file_suffix"], file_data

        snippets_header = {s for s in self.snippets if s.section == SectionType.section_header}
        snippets_body   = self.snippets - snippets_header

        if len(snippets_body) == 0:
            raise Exception("generated file %s has no body snippets!" % (repr(self)))

        # type references in this file that could not be resolved
        missing_types = set()

        # put snippets into list in correct order
        # snippets will be written according to this [(snippet, prio), ...] list
        snippets_priorized = list()

        # determine each snippet's priority by number of type references and definitions
        # smaller prio means written earlier in the file.
        # also, find snippet dependencies
        for s in sorted(snippets_body):
            snippet_prio = len(s.typerefs) - len(s.typedefs)
            snippets_priorized.append((s, snippet_prio))

            # let each snippet find others as dependencies
            missing_types |= s.add_required_snippets(self.snippets)

        if len(missing_types) > 0:
            import pprint
            raise Exception("missing types for %s:\n%s" % (repr(self), pprint.pformat(missing_types)))

        # sort snippets according to their priority determined above
        snippets_priorized_sorted = sorted(snippets_priorized, key=lambda s: s[1])

        # create list of snippets to be put in the generated file.
        # [(snippet, prio)]
        snippets_body_sorted = list()
        snippets_body_set = set()

        # fetch list of all required snippets for all snippets to put in the file
        for snippet, _ in snippets_priorized_sorted:
            snippet_candidates = snippet.get_required_snippets()

            for s in snippet_candidates:
                if s.section == SectionType.section_header:
                    if s not in snippets_header:
                        snippets_header.add(s)
                        continue

                elif s.section == SectionType.section_body:
                    if s not in snippets_body_set:
                        snippets_body_sorted.append(s)
                        snippets_body_set.add(s)
                        continue

        # these snippets will be written outside the namespace
        # in the #include section
        snippets_header_sorted = sorted(snippets_header, key=lambda h: (not h.is_global, h.name))

        # merge file contents
        header_data = "".join(header.get_data() for header in snippets_header_sorted)
        file_data   = "\n".join(snippet.get_data() for snippet in snippets_body_sorted)

        # fill file header and footer with the generated file_name
        content_prefix = prefs["content_prefix"].substitute(
            headers=header_data,
            opennamespaces="\n".join(["namespace %s {" % spc
                                      for spc in self.namespaces]),
        )

        content_suffix = prefs["content_suffix"].substitute(
            closenamespaces="\n".join(["} // %s" % spc
                                       for spc in reversed(self.namespaces)]),
        )

        # this is the final file content
        file_data = "".join((content_prefix, file_data, content_suffix))

        # whee, return (file_name, content)
        return prefs["folder"] + '/' + self.file_name + prefs["file_suffix"], file_data

    def __repr__(self):
        return "GeneratedFile<%s>(file_name=%s)" % (self.format_, self.file_name)
