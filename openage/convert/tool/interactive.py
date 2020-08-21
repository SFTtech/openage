# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Interactive browser for game asset files.
"""

import os
import readline  # pylint: disable=unused-import

from ...log import warn, info
from ...util.fslike.directory import Directory
from ..service.mount.mount_asset_dirs import mount_asset_dirs
from .subtool.version_select import get_game_version


def interactive_browser(srcdir=None):
    """
    launch an interactive view for browsing the original
    archives.

    TODO: Enhance functionality and fix SLP conversion.
    """

    info("launching interactive data browser...")

    # the variables are actually used, in the interactive prompt.
    # pylint: disable=possibly-unused-variable
    game_version = get_game_version(srcdir)
    data = mount_asset_dirs(srcdir, game_version)

    if not data:
        warn("cannot launch browser as no valid input assets were found.")
        return

    def save(path, target):
        """
        save a path to a custom target
        """
        with path.open("rb") as infile:
            with open(target, "rb") as outfile:
                outfile.write(infile.read())

    def save_slp(path, target, palette=None):
        """
        save a slp as png.
        """
        from ..entity_object.export.texture import Texture
        from ..value_object.media.slp import SLP
        from ..service.read.palette import get_palettes

        if not palette:
            palette = get_palettes(data)

        with path.open("rb") as slpfile:
            tex = Texture(SLP(slpfile.read()), palette)

            out_path, filename = os.path.split(target)
            tex.save(Directory(out_path).root, filename)

    import code
    from pprint import pprint

    import rlcompleter

    completer = rlcompleter.Completer(locals())
    readline.parse_and_bind("tab: complete")
    readline.parse_and_bind("set show-all-if-ambiguous on")
    readline.set_completer(completer.complete)

    code.interact(
        banner=("\nuse `pprint` for beautiful output!\n"
                "you can access stuff by the `data` variable!\n"
                "`data` is an openage.util.fslike.path.Path!\n\n"
                "* version detection:   pprint(game_versions)\n"
                "* list contents:       pprint(list(data['graphics'].list()))\n"
                "* dump data:           save(data['file/path'], '/tmp/outputfile')\n"
                "* save a slp as png:   save_slp(data['dir/123.slp'], '/tmp/pic.png')\n"),
        local=locals()
    )
