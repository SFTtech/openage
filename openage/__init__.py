# Copyright 2013-2015 the openage authors. See copying.md for legal info.

"""
The Python part of openage, a free engine re-write of
Age of Empires II.

See https://openage.sft.mx and http://github.com/sfttech/openage.

Requires Python 3.4.
"""

from sys import version_info as py_version


if py_version < (3, 4):
    raise Exception("openage requires Python 3.4 or higher.")


try:
    import openage.config as config

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
