# Copyright 2020-2021 the openage authors. See copying.md for legal info.
"""
Initial version detection based on user input.

TODO: Version selection.
"""
from ....log import warn, info
from ...service.init.version_detect import iterate_game_versions
from ...value_object.init.game_version import Support


def get_game_version(srcdir, avail_game_eds, avail_game_exps):
    """
    Mount the input folders for conversion.
    """
    game_version = iterate_game_versions(srcdir, avail_game_eds, avail_game_exps)

    no_support = False
    if not game_version[0]:
        warn("No valid game version(s) could not be detected in %s", srcdir)

        # no supported version was found
        no_support = True

    else:
        # Check for broken edition
        broken_edition = game_version[0].support == Support.breaks

        # a broken edition is installed
        if broken_edition:
            warn("You have installed an incompatible game edition:")
            warn(" * \x1b[31;1m%s\x1b[m", game_version[0])
            no_support = True

        broken_expansions = []
        for expansion in game_version[1]:
            if expansion.support == Support.breaks:
                broken_expansions.append(expansion)

        # a broken expansion is installed
        if broken_expansions:
            warn("You have installed incompatible game expansions:")
            for expansion in broken_expansions:
                warn(" * \x1b[31;1m%s\x1b[m", expansion)

    # inform about supported versions
    if no_support:
        warn("You need at least one of:")
        for edition in avail_game_eds:
            if edition.support == Support.yes:
                warn(" * \x1b[34m%s\x1b[m", edition)

        return (False, set())

    info("Compatible game edition detected:")
    info(" * %s", game_version[0].edition_name)
    if game_version[1]:
        info("Compatible expansions detected:")
        for expansion in game_version[1]:
            info(" * %s", expansion.expansion_name)

    return game_version
