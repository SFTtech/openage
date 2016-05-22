# Copyright 2015-2016 the openage authors. See copying.md for legal info.

"""
Auto-generates PXD files from annotated C++ headers.

Invoked via cmake during the regular build process.
"""

import re
import os

from pygments.token import Token
from pygments.lexers import get_lexer_for_filename


CWD = os.getcwd()


class ParserError(Exception):
    """
    Represents a fatal parsing error in PXDGenerator.
    """
    def __init__(self, filename, lineno, message):
        super().__init__("{}:{} {}".format(filename, lineno, message))


class PXDGenerator:
    """
    Represents, and performs, a single conversion of a C++ header file to a
    PXD file.

    @param filename:
        input (C++ header) file name. is opened and read.
        the output filename is the same, but with .pxd instead of .h.
    """

    def __init__(self, filename):
        self.filename = filename

        self.warnings = []

        # current parsing state (not valid until self.parse() is called)
        self.stack, self.lineno, self.annotations = None, None, None

    def parser_error(self, message, lineno=None):
        """
        Returns a ParserError object for this generator, at the current line.
        """
        if lineno is None:
            lineno = self.lineno

        return ParserError(self.filename, lineno, message)

    def tokenize(self):
        """
        Tokenizes the input file.

        Yields (tokentype, val) pairs, where val is a string.

        The concatenation of all val strings is equal to the input file's
        content.
        """
        # contains all namespaces and other '{' tokens
        self.stack = []
        # current line number
        self.lineno = 1

        # we're using the pygments lexer (mainly because that was the first
        # google hit for 'python c++ lexer', and it's fairly awesome to use)

        lexer = get_lexer_for_filename('.cpp')

        with open(self.filename) as infile:
            code = infile.read()

        for token, val in lexer.get_tokens(code):
            # ignore whitespaces
            yield token, val
            self.lineno += val.count('\n')

    def handle_singleline_comment(self, val):
        """
        Breaks down a '//'-style single-line comment, and passes the result
        to handle_comment()

        @param val:
            the comment text, as string, including the '//'
        """
        try:
            val = re.match('^// (.*)$', val).group(1)
        except AttributeError as ex:
            raise self.parser_error("invalid single-line comment") from ex

        self.handle_comment(val)

    def handle_multiline_comment(self, val):
        """
        Breaks down a '/* */'-style multi-line comment, and passes the result
        to handle_comment()

        @param val:
            the comment text, as string, including the '/*' and '*/'
        """
        try:
            val = re.match('^/\\*(.*)\\*/$', val, re.DOTALL).group(1)
        except AttributeError as ex:
            raise self.parser_error("invalid multi-line comment") from ex

        # for a comment '/* foo\n * bar\n */', val is now 'foo\n * bar\n '
        # however, we'd prefer ' * foo\n * bar'
        val = ' * ' + val.rstrip()
        # actually, we'd prefer [' * foo', ' * bar'].
        lines = val.split('\n')

        comment_lines = []
        for idx, line in enumerate(lines):
            try:
                line = re.match('^ \\*( (.*))?$', line).group(2) or ""
            except AttributeError as ex:
                raise self.parser_error("invalid multi-line comment line",
                                        idx + self.lineno) from ex

            # if comment is still empty, don't append anything
            if comment_lines or line.strip() != "":
                comment_lines.append(line)

        self.handle_comment('\n'.join(comment_lines).rstrip())

    def handle_comment(self, val):
        """
        Handles any comment, with its format characters removed,
        extracting the pxd annotation
        """
        annotations = re.findall('pxd:\\s(.*?)(:pxd|$)', val, re.DOTALL)
        annotations = [annotation[0] for annotation in annotations]

        if not annotations:
            raise self.parser_error("comment contains no valid pxd annotation")

        for annotation in annotations:
            # remove empty lines at end
            annotation = annotation.rstrip()

            annotation_lines = annotation.split('\n')
            for idx, line in enumerate(annotation_lines):
                if line.strip() != "":
                    # we've found the first non-empty annotation line
                    self.add_annotation(annotation_lines[idx:])
                    break
            else:
                raise self.parser_error("pxd annotation is empty:\n" + val)

    def add_annotation(self, annotation_lines):
        """
        Adds a (current namespace, pxd annotation) tuple to self.annotations.
        """
        if "{" in self.stack:
            raise self.parser_error("PXD annotation is brace-enclosed")
        elif not self.stack:
            namespace = None
        else:
            namespace = "::".join(self.stack)

        self.annotations.append((namespace, annotation_lines))

    def handle_token(self, token, val):
        """
        Handles one token while the parser is in its regular state.

        Returns the new state integer.
        """
        # accept any token here
        if token == Token.Keyword and val == 'namespace':
            # advance to next state on 'namespace'
            return 1

        elif (token, val) == (Token.Punctuation, '{'):
            self.stack.append('{')

        elif (token, val) == (Token.Punctuation, '}'):
            try:
                self.stack.pop()
            except IndexError as ex:
                raise self.parser_error("unmatched '}'") from ex

        elif token == Token.Comment.Single and 'pxd:' in val:
            self.handle_singleline_comment(val)

        elif token == Token.Comment.Multiline and 'pxd:' in val:
            self.handle_multiline_comment(val)

        else:
            # we don't care about all those other tokens
            pass

        return 0

    def parse(self):
        """
        Parses the input file.

        Internally calls self.tokenize().

        Adds all found PXD annotations to self.annotations,
        together with info about the namespace in which they were encountered.
        """

        self.annotations = []
        state = 0

        for token, val in self.tokenize():
            # ignore whitespaces
            if token == Token.Text and not val.strip():
                continue

            if state == 0:
                state = self.handle_token(token, val)

            elif state == 1:
                # we're inside a namespace definition; expect Token.Name
                if token != Token.Name:
                    raise self.parser_error(
                        "expected identifier after 'namespace'")
                state = 2
                self.stack.append(val)

            elif state == 2:
                # expect {
                if (token, val) != (Token.Punctuation, '{'):
                    raise self.parser_error("expected '{' after 'namespace " +
                                            self.stack[-1] + "'")
                state = 0

        if self.stack:
            raise self.parser_error("expected '}', but found EOF")

    def get_pxd_lines(self):
        """
        calls self.parse() and processes the pxd annotations to pxd code lines.
        """

        from datetime import datetime
        year = datetime.now().year
        yield ("# Copyright 2013-{} the openage authors. "
               "See copying.md for legal info.".format(year))

        yield ""

        yield ("# Auto-generated from annotations in " +
               os.path.split(self.filename)[1])

        yield "# " + self.filename

        self.parse()

        # namespace of the previous pxd annotation
        previous_namespace = None

        for namespace, annotation_lines in self.annotations:
            yield ""

            if namespace != previous_namespace:
                yield ""

            if namespace:
                prefix = "    "

                if namespace != previous_namespace:
                    yield (
                        "cdef extern "
                        "from \"" + self.filename + "\" "
                        "namespace \"" + namespace + "\" "
                        "nogil"
                        ":"
                    )
            else:
                prefix = ""

            for annotation in annotation_lines:
                annotation = self.postprocess_annotation_line(annotation)
                if annotation:
                    yield prefix + annotation
                else:
                    # don't emit a line that consists of just the prefix
                    # (avoids trailing whitespace)
                    yield ""

            previous_namespace = namespace

        yield ""

    def postprocess_annotation_line(self, annotation):
        """
        Post-processes each individual annotation line, applying hacks and
        testing it, etc.

        See openage/pyinterface/hacks.h for documentation on the individual
        hacks.
        """
        annotation = annotation.rstrip()
        if annotation.endswith(')'):
            self.warnings.append(
                "line ends with ')'; mark the function as 'except +' or "
                "'noexcept':\n" + annotation)
        elif annotation.endswith('noexcept'):
            annotation = annotation[:-8].rstrip()

        if 'cdef ' in annotation:
            self.warnings.append(
                "there's no need to use 'cdef' in PXD annotations:\n" +
                annotation)

        return annotation

    def generate(self, pxdfile, print_warnings=True):
        """
        reads the input file and writes the output file.
        the output file is updated only if its content will change.

        on parsing failure, raises ParserError.
        """

        # create empty __init__.py in all parent directories.
        # CMake requires this; else it won't find the .pxd files.
        dirname = os.path.abspath(os.path.dirname(pxdfile))
        while dirname.startswith(CWD + os.path.sep):
            initfile = os.path.join(dirname, "__init__.py")
            if not os.path.isfile(initfile):
                with open(initfile, "w"):
                    pass

            # parent dir
            dirname = os.path.dirname(dirname)

        result = "\n".join(self.get_pxd_lines())

        with open(pxdfile, 'w') as outfile:
            outfile.write(result)

        if print_warnings and self.warnings:
            print("\x1b[33;1mWARNING\x1b[m pxdgen[" + self.filename + "]:")
            for warning in self.warnings:
                print(warning)


def parse_args():
    """
    pxdgen command-line interface.

    designed to allow both manual and automatic (via CMake) usage.
    """
    import argparse

    cli = argparse.ArgumentParser()
    cli.add_argument("input", help=("input file (usually cpp .h files)."))
    cli.add_argument("output", help=("The output is stored in this file."))

    return cli.parse_args()


def main():
    """ CLI entry point """
    args = parse_args()
    cppdir = CWD + "/libopenage"

    filename = os.path.abspath(args.input)
    if not filename.startswith(cppdir):
        print("pxdgen source file is not in " + cppdir + ": " + filename)
        exit(1)

    PXDGenerator(filename).generate(args.output, print_warnings=True)

if __name__ == '__main__':
    main()
