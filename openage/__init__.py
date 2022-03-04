# Copyright 2013-2022 the openage authors. See copying.md for legal info.

"""
The Python part of openage, a free engine re-write of
Age of Empires II.

See https://openage.dev and http://github.com/SFTtech/openage

Requires Python 3.6.
"""

from sys import version_info as py_version

from .log import setup_logging

if py_version < (3, 6):
    raise Exception("openage requires python 3.6 or higher.")


try:
    # TODO pylint: disable=wrong-import-position,import-self
    from . import config

except ImportError:
    VERSION = "< unknown version; ./configure incomplete >"
    LONGVERSION = VERSION

else:
    VERSION = config.VERSION

    LONGVERSION = (
        f"openage {config.VERSION}"
        f"{' [devmode]' if config.DEVMODE else ''}\n"
        f"{config.CONFIG_OPTIONS}, ci-cfg {config.CICFGVERSION}\n"
        f"{config.COMPILER} [{config.COMPILERFLAGS}]\n"
        "\n"
        "== Python ==\n"
        f"Python        {config.PYTHONINTERPRETER}\n"
        f"Python C API  {config.PYTHONCAPI}\n"
        f"Cython        {config.CYTHONVERSION}\n"
        f"Jinja2        {config.JINJAVERSION}\n"
        f"NumPy         {config.NUMPYVERSION}\n"
        f"Pillow        {config.PILVERSION}\n"
        f"Pygments      {config.PYGMENTSVERSION}\n"
        "\n"
        "== C++ =="
    )


setup_logging()
