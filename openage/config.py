# Copyright 2014-2016 the openage authors. See copying.md for legal info.

"""
Project configuration
"""

from os.path import normpath
from site import getsitepackages


DEVMODE = not any([normpath(__file__).startswith(normpath(path)) for path in getsitepackages()])


def get_build_config():
    """Returns the build configuration of openage."""
    cached_config = getattr(get_build_config, '_cached_config', None)
    if cached_config:
        return cached_config

    from .build import get_build_configuration
    build_config = get_build_configuration()

    setattr(get_build_config, '_cached_config', build_config)

    return build_config
