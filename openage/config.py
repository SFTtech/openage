# Copyright 2015-2016 the openage authors. See copying.md for legal info.

# pylint: disable=unused-import

"""
Project configuration
"""

import importlib
import os

try:
    importlib.import_module("openage.devmode")
    DEVMODE = True
except ImportError:
    DEVMODE = False

try:
    # pylint: disable=wrong-import-position
    from .config_generated import (
        GLOBAL_ASSET_DIR, BUILD_SRC_DIR, BUILD_BIN_DIR,
        VERSION, CONFIG_OPTIONS,
        COMPILER, COMPILERFLAGS, CYTHONVERSION
    )

except ImportError:
    # Dummy values
    VERSION = "< unknown version >"
    LONGVERSION = VERSION

    GLOBAL_ASSET_DIR = "/usr/local/share/openage"
    BUILD_SRC_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    BUILD_BIN_DIR = os.path.abspath(os.path.join(BUILD_SRC_DIR, "bin"))

else:
    LONGVERSION = (
        "openage {version}{devmode}\n"
        "{config_options}\n"
        "{compiler} [{compilerflags}]\n"
        "Cython {cython}"
    ).format(
        version=VERSION,
        devmode=(" [devmode]" if DEVMODE else ""),
        config_options=CONFIG_OPTIONS,
        compiler=COMPILER,
        compilerflags=COMPILERFLAGS,
        cython=CYTHONVERSION
    )
