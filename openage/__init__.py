# Copyright 2013-2020 the openage authors. See copying.md for legal info.

"""
The Python part of openage, a free engine re-write of
Age of Empires II.

See https://openage.sft.mx and http://github.com/sfttech/openage.

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
        "openage {version}{devmode}\n"
        "{config_options}, ci-cfg {cicfg}\n"
        "{compiler} [{compilerflags}]\n"
        "\n"
        "== Python ==\n"
        "Python        {python_interpreter}\n"
        "Python C API  {python_c_api}\n"
        "Cython        {cython}\n"
        "Jinja2        {jinja2}\n"
        "NumPy         {numpy}\n"
        "Pygments      {pygments}\n"
        "\n"
        "== C++ =="
    ).format(
        version=config.VERSION,
        devmode=(" [devmode]" if config.DEVMODE else ""),
        config_options=config.CONFIG_OPTIONS,
        compiler=config.COMPILER,
        compilerflags=config.COMPILERFLAGS,
        cicfg=config.CICFGVERSION,
        python_interpreter=config.PYTHONINTERPRETER,
        python_c_api=config.PYTHONCAPI,
        cython=config.CYTHONVERSION,
        jinja2=config.JINJAVERSION,
        numpy=config.NUMPYVERSION,
        pygments=config.PYGMENTSVERSION,
    )


setup_logging()
