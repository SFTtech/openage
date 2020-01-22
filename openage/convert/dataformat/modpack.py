# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Defines a modpack that can be exported.
"""


class Modpack:

    def __init__(self, name, exportdir):

        # Mandatory
        self.name = name

        # Data export
        self.exportdir = exportdir
        self.export_files = []
