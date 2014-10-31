# Copyright 2014-2014 the openage authors. See copying.md for legal info.

import os


class Issue:
    def __init__(self, title, text):
        self.title = title
        self.text = text

    def print(self):
        if os.isatty(1):
            print("\x1b[33;1mWARNING\x1b[m {}: {}".format(
                self.title,
                self.text))
        else:
            print("WARNING {}: {}".format(self.title, self.text))
