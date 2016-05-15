# Copyright 2016-2016 the openage authors. See copying.md for legal info.

"""
Provides utility functions to get proper version information.
"""

import subprocess

from .. import config


def get_version(version=None):
    """
    Returns the version string given a tuple of version identifiers.
    """
    if version is None:
        from .. import VERSION as version

    main_version = '.'.join(str(x) for x in version[:3])

    sub = ''
    if version[3] == 'alpha' and version[4] == 0:
        git_revision = get_git_revision()
        if git_revision:
            sub = '.dev%s' % git_revision
    elif version[3] != 'final':
        mapping = {'alpha': 'a', 'beta': 'b', 'rc': 'rc'}
        sub = mapping[version[3]] + str(version[4])

    return str(main_version + sub)


def get_git_revision():
    """
    Returns epoch of the committer's date to be used as the development version number.
    """
    try:
        git_revision = subprocess.Popen(
            'git log --pretty=format:%ct --quiet -1 HEAD',
            stdout=subprocess.PIPE, stderr=subprocess.PIPE,
            shell=True, universal_newlines=True,
        ).communicate()[0]
    except OSError:
        git_revision = None

    return git_revision


def get_long_version(version=None):
    """
    Returns the long version string given a tuple of version identifiers.
    """
    version = get_version(version)

    try:
        from ..build import get_build_configuration
    except ImportError:
        longversion = version
    else:
        build_config = get_build_configuration()
        longversion = (
            "openage {version}{devmode}\n"
            "{config_options}\n"
            "{compiler} [{compilerflags}]\n"
            "Cython {cython}"
        ).format(
            version=version,
            devmode=(" [devmode]" if config.DEVMODE else ""),
            config_options=build_config.config_options,
            compiler=build_config.compiler,
            compilerflags=build_config.compiler_flags,
            cython=build_config.cython_version
        )

    return longversion
