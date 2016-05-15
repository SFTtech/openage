# Copyright 2014-2016 the openage authors. See copying.md for legal info.

"""
Project configuration
"""

import importlib


try:
    importlib.import_module("openage.devmode")
    DEVMODE = True
except ImportError:
    DEVMODE = False


def get_build_config():
    """Returns the build configuration of openage."""
    cached_config = getattr(get_build_config, '_cached_config', None)
    if cached_config:
        return cached_config

    from .build import get_build_configuration
    build_config = get_build_configuration()

    setattr(get_build_config, '_cached_config', build_config)

    return build_config
