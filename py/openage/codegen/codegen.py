# Copyright 2014-2014 the openage authors. See copying.md for legal info.

import datetime
import os
import sys

from openage.log import dbg

contenttemplate = """\
{prefix} {copyrightline}

{prefix} Warning: this file was auto-generated; manual changes are futile.
{prefix} For details, see buildsystem/codegen.cmake and py/openage/codegen.

{code}"""


def generate_all_raw(cpp_src_dir):
    # add invocations to all individual generators here.
    from ..convert import datafile
    for filename, content in datafile.generate_gamedata_structs(cpp_src_dir):
        yield "gamedata/%s" % filename, content

    from . import cpp_tests
    yield cpp_tests.generate_testregistration(cpp_src_dir)


def generate_all(cpp_src_dir):
    for filename, content in generate_all_raw(cpp_src_dir):
        # test whether filename matches the pattern *.gen.*
        basename = os.path.basename(filename)
        try:
            # this will raise a ValueError if basename doesn't have exactly
            # three dot-separated components
            basename, marker, suffix = basename.split('.')

            # if the middle component isn't "gen", raise a ValueError
            # manually
            if not (basename and marker and suffix) or marker != 'gen':
                raise ValueError()
        except ValueError:
            dbg("error in codegen: required filename format is " +
                "[base].gen.[suffix], but filename is %s" % filename, 0)
            exit(1)

        # get absolute filename
        absfilename = "".join((cpp_src_dir, "/", filename))

        # post-process content
        extension = os.path.splitext(filename)[1].lower().strip()
        if extension in {'.h', '.hpp', '.c', '.cpp'}:
            comment_prefix = '//'
        else:
            comment_prefix = '#'

        splitcontent = content.split('\n')
        if "copyright" in splitcontent[0].lower():
            # the content already contains a copyright line
            if splitcontent[1].strip() in {'', comment_prefix}:
                content = '\n'.join(splitcontent[2:])
            else:
                content = '\n'.join(splitcontent[1:])

            copyright = splitcontent[0]
            if copyright.startswith(comment_prefix):
                copyright = copyright[len(comment_prefix):]
            copyright = copyright.strip()

        else:
            year = datetime.datetime.now().year
            copyright = ("Copyright 2013-{} the openage authors. "
                         "See copying.md for legal info.").format(year)

        content = contenttemplate.format(copyrightline=copyright,
                                         prefix=comment_prefix,
                                         code=content)

        yield absfilename, filename, content


manual_depends = set()


def add_manual_depend(path):
    manual_depends.add(os.path.abspath(path))


def get_depends():
    """
    yields all python module files that have been imported so far,
    plus all manually-added depends.
    """
    for m in sys.modules.values():
        filename = getattr(m, '__file__', None)
        if not filename:
            continue

        yield os.path.abspath(filename)

    for manual_depend in manual_depends:
        yield manual_depend
