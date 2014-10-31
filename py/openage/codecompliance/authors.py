# Copyright 2014-2014 the openage authors. See copying.md for legal info.

import re
import os
from subprocess import Popen, PIPE

from . import util


def find_issues():
    """
    compares the output of git shortlog -sne to the authors table in copying.md

    prints all discrepancies, and returns False if one is detected.
    """
    known_emails = set()

    # look for known emails in copying.md
    # (they must be part of a line like
    # .* <garbage, othergarbage, email, garbage, email> .*

    expr = re.compile('^.*\|[^|]*\|[^|]*\|([^|]*)\|.*$')
    for line in open('copying.md').read().split('\n'):
        match = expr.match(line)
        if not match:
            continue

        for email in match.group(1).split(', '):
            if '@' not in email:
                continue

            email = email.lower().strip()
            known_emails.add(email)

    output = Popen(['git', 'shortlog', '-sne'], stdout=PIPE).communicate()[0]
    expr = re.compile("^ +[0-9]+\t[^<]*\<(.*)\>$")
    for line in output.decode('utf-8', errors='replace').split('\n'):
        match = expr.match(line)
        if not match:
            continue

        email = match.group(1).lower()

        if email not in known_emails:
            if email in {'coop@sft.mx', '?'}:
                continue

            yield util.Issue(
                "unknown commit email in `git shortlog -e`: {}".format(email),
                "\n\tadd the email address to `copying.md` or `.mailmap`")
