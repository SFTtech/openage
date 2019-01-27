# Copyright 2013-2019 the openage authors. See copying.md for legal info.

"""
The Python part of openage, a free engine re-write of
Age of Empires II.

See https://openage.sft.mx and http://github.com/sfttech/openage.

Requires Python 3.4.
"""

from sys import version_info as py_version

from .log import setup_logging

if py_version < (3, 4):
    raise Exception("openage requires python 3.4 or higher.")


try:
    # TODO pylint: disable=wrong-import-position,import-self
    from . import config

except ImportError:
    VERSION = "< unknown version; ./configure incomplete >"
    LONGVERSION = VERSION

else:
    VERSION = config.VERSION

    LONGVERSION = (
        "openage {version}{devmode}\n"
        "{config_options}\n"
        "{compiler} [{compilerflags}]\n"
        "Cython {cython}"
    ).format(
        version=config.VERSION,
        devmode=(" [devmode]" if config.DEVMODE else ""),
        config_options=config.CONFIG_OPTIONS,
        compiler=config.COMPILER,
        compilerflags=config.COMPILERFLAGS,
        cython=config.CYTHONVERSION
    )

setup_logging()
