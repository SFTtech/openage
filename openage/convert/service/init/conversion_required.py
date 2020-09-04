# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Test whether there already are converted modpacks present.
"""
from . import changelog
from ....log import info, dbg


def conversion_required(asset_dir, args):
    """
    Returns true if an asset conversion is required to run the game.

    Sets options in args according to what sorts of conversion are required.

    TODO: Reimplement change detection for new converter.
    """
    version_path = asset_dir / 'converted' / changelog.ASSET_VERSION_FILENAME
    # determine the version of assets
    try:
        with version_path.open() as fileobj:
            asset_version = fileobj.read().strip()

        try:
            asset_version = int(asset_version)
        except ValueError:
            info("Converted asset version has improper format; "
                 "expected integer number")
            asset_version = -1

    except FileNotFoundError:
        # assets have not been converted yet
        info("No converted assets have been found")
        asset_version = -1

    changes = changelog.changes(asset_version,)

    if not changes:
        dbg("Converted assets are up to date")
        return False

    if asset_version >= 0 and asset_version != changelog.ASSET_VERSION:
        info("Found converted assets with version %d, "
             "but need version %d", asset_version, changelog.ASSET_VERSION)

    info("Converting %s", ", ".join(sorted(changes)))

    # try to resolve resolve the output path
    target_path = asset_dir.resolve_native_path_w()
    if not target_path:
        raise OSError("could not resolve a writable asset path "
                      "in {}".format(asset_dir))

    info("Will save to '%s'", target_path.decode(errors="replace"))

    for component in changelog.COMPONENTS:
        if component not in changes:
            # don't reconvert this component:
            setattr(args, "no_{}".format(component), True)

    if "metadata" in changes:
        args.no_pickle_cache = True

    return True
