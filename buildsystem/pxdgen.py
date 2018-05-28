# Copyright 2015-2018 the openage authors. See copying.md for legal info.

"""
Auto-generates PXD files from annotated C++ headers.

Invoked via cmake during the regular build process.
"""

import argparse
import os
import re
import sys

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
            # pylint: disable=no-member
            val = re.match(r"^/\*(.*)\*/$", val, re.DOTALL).group(1)
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
                line = re.match(r'^ \*( (.*))?$', line).group(2) or ""
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

        annotations = re.findall(r"pxd:\s(.*?)(:pxd|$)",
                                 val, re.DOTALL)   # pylint: disable=no-member
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
                        "from r\"" + self.filename.replace('\\', '/') + "\" "
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
        if annotation.endswith(';'):
            self.warnings.append(
                "cython declaration ends in ';', "
                "what have you done?")

        if annotation.endswith(')'):
            self.warnings.append(
                "mark the function as 'except +' or "
                "'noexcept':\n" + annotation)

        elif annotation.endswith('noexcept'):
            annotation = annotation[:-8].rstrip()

        if 'cdef ' in annotation:
            self.warnings.append(
                "there's no need to use 'cdef' in PXD annotations:\n" +
                annotation)

        return annotation

    def generate(self, ignore_timestamps=False, print_warnings=True):
        """
        reads the input file and writes the output file.
        the output file is updated only if its content will change.

        on parsing failure, raises ParserError.
        """
        pxdfile = os.path.splitext(self.filename)[0] + '.pxd'

        # create empty __init__.py in all parent directories.
        # Cython requires this; else it won't find the .pxd files.
        dirname = os.path.abspath(os.path.dirname(self.filename))
        while dirname.startswith(CWD + os.path.sep):
            initfile = os.path.join(dirname, "__init__.py")
            if not os.path.isfile(initfile):
                print("\x1b[36mpxdgen: initfile %s\x1b[0m" % os.path.relpath(initfile, CWD))
                with open(initfile, "w"):
                    pass

            # parent dir
            dirname = os.path.dirname(dirname)

        if not ignore_timestamps and os.path.exists(pxdfile):
            # skip the file if the timestamp is up to date
            if os.path.getmtime(self.filename) <= os.path.getmtime(pxdfile):
                return False

        result = "\n".join(self.get_pxd_lines())

        if os.path.exists(pxdfile):
            with open(pxdfile) as outfile:
                if outfile.read() == result:
                    # don't write the file if the content is up to date
                    return False

        with open(pxdfile, 'w') as outfile:
            print("\x1b[36mpxdgen: generate %s\x1b[0m" % os.path.relpath(pxdfile, CWD))
            outfile.write(result)

        if print_warnings and self.warnings:
            print("\x1b[33;1mWARNING\x1b[m pxdgen[" + self.filename + "]:")
            for warning in self.warnings:
                print(warning)

        return True


def parse_args():
    """
    pxdgen command-line interface.

    designed to allow both manual and automatic (via CMake) usage.
    """
    cli = argparse.ArgumentParser()

    cli.add_argument('files', nargs='*', metavar='HEADERFILE',
                     help="input files (usually cpp .h files).")
    cli.add_argument('--file-list',
                     help="semicolon-separated list of input files.")
    cli.add_argument('--ignore-timestamps', action='store_true',
                     help="force generating even if the output file is already"
                          "up to date")
    cli.add_argument('-v', '--verbose', action="store_true",
                     help="increase logging verbosity")

    args = cli.parse_args()

    if args.file_list:
        file_list = open(args.file_list).read().strip().split(';')
    else:
        file_list = []

    from itertools import chain
    args.all_files = list(chain(args.files, file_list))

    return args


def main():
    """ CLI entry point """
    args = parse_args()
    cppdir = os.path.join(CWD, "libopenage")

    if args.verbose:
        hdr_count = len(args.all_files)
        plural = "s" if hdr_count > 1 else ""

        print("extracting pxd information "
              "from {} header{}...".format(hdr_count, plural))

    for filename in args.all_files:
        filename = os.path.realpath(os.path.abspath(filename))
        if not filename.startswith(cppdir):
            print("pxdgen source file is not in " + cppdir + ": " + filename)
            sys.exit(1)

        if args.verbose:
            print("creating .pxd for '{}':".format(filename))

        generator = PXDGenerator(filename)

        result = generator.generate(
            ignore_timestamps=args.ignore_timestamps,
            print_warnings=True
        )

        if args.verbose and not result:
            print("nothing done.")


if __name__ == '__main__':
    main()
