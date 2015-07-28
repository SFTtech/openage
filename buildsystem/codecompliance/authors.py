# Copyright 2014-2015 the openage authors. See copying.md for legal info.

"""
Checks whether all authors are properly listed in copying.md.
"""

import re


def get_author_emails_copying_md():
    """
    yields all emails from the author table in copying.md

    they must be part of a line like

    |     name     |    nick    |    email    |
    """
    with open("copying.md") as fobj:
        for line in fobj:
            match = re.match("^.*\\|[^|]*\\|[^|]*\\|([^|]*)\\|.*$", line)
            if not match:
                continue

            email = match.group(1).strip()
            if '@' not in email:
                continue

            yield email


def get_author_emails_git_shortlog(exts):
    """
    yields emails of all authors that have authored any of the files ending
    in exts (plus their templates)

    parses the output of git shortlog -sne
    """
    from subprocess import Popen, PIPE

    invocation = ['git', 'shortlog', '-sne', '--']
    for ext in exts:
        invocation.append("*{}".format(ext))
        invocation.append("*{}.in".format(ext))
        invocation.append("*{}.template".format(ext))

    output = Popen(invocation, stdout=PIPE).communicate()[0]

    for line in output.decode('utf-8', errors='replace').split('\n'):
        match = re.match("^ +[0-9]+\t[^<]*\\<(.*)\\>$", line)
        if match:
            yield match.group(1).lower()


def find_issues():
    """
    compares the output of git shortlog -sne to the authors table in copying.md

    prints all discrepancies, and returns False if one is detected.
    """
    relevant_exts = ('.cpp', '.h', '.py', '.pyi', '.pyx', '.cmake')

    copying_md_emails = set(get_author_emails_copying_md())
    git_shortlog_emails = set(get_author_emails_git_shortlog(relevant_exts))

    # look for git emails that are unlisted in copying.md
    for email in git_shortlog_emails - copying_md_emails:
        if email in {'coop@sft.mx', '?'}:
            continue

        yield (
            "author inconsistency",
            ("{}\n"
             "\temail appears in git log, "
             "but not in copying.md or .mailmap".format(email)))
