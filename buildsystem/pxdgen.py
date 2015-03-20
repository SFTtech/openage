# Copyright 2015-2015 the openage authors. See copying.md for legal info.

"""
Auto-generates PXD files from annotated C++ headers.

Invoked via cmake during the regular build process.
"""

import re
import os

from pygments.token import Token
from pygments.lexers import get_lexer_for_filename


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

    @param infilename:
        input (C++ header) file name. is opened and read.
    @param outfilename:
        output (pxd) file name. is opened and written.
    """

    def __init__(self, infilename, outfilename):
        self.infilename = infilename
        self.outfilename = outfilename

        self.warnings = []

        # current parsing state (not valid until self.parse() is called)
        self.stack, self.lineno, self.annotations = None, None, None

    def parser_error(self, message, lineno=None):
        """
        Returns a ParserError object for this generator, at the current line.
        """
        if lineno is None:
            lineno = self.lineno

        return ParserError(self.infilename, lineno, message)

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

        with open(self.infilename) as infile:
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

        yield ("# this PXD definition file was auto-generated from "
               "the pxd annotations in")

        yield "# " + self.infilename

        # Include hacks.h, which contains various hacks and workarounds
        # that should be run on Cython-compiled files.
        # See the header file for more info.
        yield ""
        yield 'cdef extern from "cpp/pyinterface/hacks.h":'
        yield '    # this extern cdef is auto-added by pxdgen.'
        yield '    # it ensures that all of the Cython-specific hacks are'
        yield "    # included, including the replacement for Cython's own"
        yield "    # exception handler. See the header file for further docs."
        yield "    int hacks"

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
                    yield 'cdef extern from "{}" namespace "{}":'.format(
                        self.infilename, namespace)
            else:
                prefix = ""

            for annotation in annotation_lines:
                yield prefix + self.postprocess_annotation_line(annotation)

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
                "there's no need to use 'cdef' in PXD files:\n" +
                annotation)

        return annotation

    def generate(self, print_warnings=True):
        """
        reads the input file and writes the output file.
        the output file is updated only if its content will change.

        on parsing failure, raises ParserError

        returns True if the output file was actually written.
        """
        result = "\n".join(line for line in self.get_pxd_lines())

        if os.path.exists(self.outfilename):
            with open(self.outfilename) as outfile:
                if outfile.read() == result:
                    # don't write the file if the content wouldn't change
                    return False

        def create_parent_dir(filename):
            """
            creates the parent directory of 'filename'; recurses if needed.

            an empty __init__.py is created in each directory.
            """
            dirname = os.path.dirname(filename)

            if not os.path.exists(dirname):
                # recursively create the parent directories
                create_parent_dir(dirname)

                # create the directory itself
                os.mkdir(dirname)

                # create __init__.py
                init_filename = os.path.join(dirname, "__init__.py")
                if not os.path.exists(init_filename):
                    print("generating " + init_filename)
                    with open(init_filename, "w"):
                        pass

        create_parent_dir(self.outfilename)

        with open(self.outfilename, 'w') as outfile:
            outfile.write(result)

        if print_warnings and self.warnings:
            print("\x1b[33;1mWARNING\x1b[m pxdgen[" + self.infilename + "]:")
            for warning in self.warnings:
                print(warning)

        return True


def parse_args():
    """
    pxdgen command-line interface.

    designed to allow both manual and automatic (via CMake) usage.
    """
    import argparse

    cli = argparse.ArgumentParser()
    cli.add_argument('files', nargs='*', metavar='PXDSOURCE',
                     help="input files (usually cpp .h files).")
    cli.add_argument('--file-list',
                     help="semicolon-separated list of input files.")
    cli.add_argument('--ignore-timestamps', action='store_false',
                     help="force generating even if the output file is already"
                          "up to date")
    cli.add_argument('--target-dir',
                     help="output files are created in this directory.")

    args = cli.parse_args()

    if args.file_list:
        file_list = open(args.file_list).read().strip().split(';')
    else:
        file_list = []

    from itertools import chain
    args.all_files = chain(args.files, file_list)

    return args


def main():
    """ main function """
    args = parse_args()
    cwd = os.getcwd()
    cppdir = cwd + "/cpp"

    for inputfile in args.all_files:
        if not (os.path.isabs(inputfile) and inputfile.startswith(cppdir)):
            print("pxdgen source file is not an absolute path in " + cppdir)
            exit(1)

        outputfile = os.path.relpath(inputfile, cppdir)
        outputfile = os.path.splitext(outputfile)[0] + '.pxd'

        if args.target_dir:
            outputfile = os.path.join(args.target_dir, outputfile)

        if not args.ignore_timestamps and os.path.exists(outputfile):
            if os.path.getmtime(inputfile) <= os.path.getmtime(outputfile):
                continue

        if PXDGenerator(inputfile, outputfile).generate():
            print("generating " + outputfile)


if __name__ == '__main__':
    main()
