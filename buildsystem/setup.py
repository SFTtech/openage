# Copyright 2015-2015 the openage authors. See copying.md for legal info.

# this file is exempt from style checking. pylint:skip-file

"""
Part of the openage distutils-cmake interface.
this file should never be called directly; instead, run it via
'make' and 'make install'.

See doc/buildsystem.md for further info.
"""


from configparser import ConfigParser
from Cython.Build import cythonize
from distutils.core import setup, Extension
from distutils.sysconfig import get_config_vars
from os import environ, path
import shlex
from sys import argv

# read config variables as supplied by cmake.

config = ConfigParser()
config.read("py/py.conf")

if not config.sections():
    print("Error: config file is empty or does not exist: bin/py/py.conf\n"
          "Call this script via CMake, or call it form the build "
          "directory, and create the file manually.")

    exit(1)


# read the package and cython module lists as supplied by cmake.
with open("py/packages") as package_list_file:
    packages = package_list_file.read().strip().split(';')

with open("py/cython_modules") as cython_module_file:
    cython_module_files = cython_module_file.read().strip().split(';')


# build the extension list
extensions = []
source_dir = config['build']['source_dir']
binary_dir = config['build']['binary_dir']
libopenage_dir = path.join(binary_dir, 'cpp')
cxxflags = shlex.split(config['cython']['cxxflags'])
ldflags = shlex.split(config['cython']['ldflags'])

pxd_dirs = [
    source_dir,
    config['cython']['pxd_dir']
]

# hack to add -rpath for in-place-built modules
if "--inplace" in argv:
    ldflags += shlex.split(config['cython']['inplaceldflags'])


for module_file in cython_module_files:
    module_name = path.relpath(module_file, source_dir)
    module_name = path.splitext(module_name)[0]
    module_name = module_name.replace('/', '.')

    extensions.append(Extension(
        name               = module_name,
        sources            = [module_file],
        language           = "c++",
        extra_compile_args = cxxflags,
        extra_link_args    = ldflags,
        libraries          = ['openage'],
        library_dirs       = [libopenage_dir],
        include_dirs       = [source_dir]
    ))


# distutils invokes gcc with -Wstrict-prototypes, which is an invalid flag for
# C++, leading to an annoying warning. Until this issue has been fixed, the
# following hack manually removes the compiler flag.
cfg_vars = get_config_vars()
for cfg_var, val in cfg_vars.items():
    if isinstance(val, str) and "-Wstrict-prototypes" in val:
        cfg_vars[cfg_var] = val.replace("-Wstrict-prototypes", "")


# invoke distutils.setup
environ["CC"] = config['cython']['cc']
environ["CXX"] = config['cython']['cxx']

setup(
    name        = 'openage',
    version     = config['project']['version'],
    description = 'openage python components',
    author      = '[SFT]Tech',
    url         = 'http://openage.sft.mx',
    license     = 'GPLv3+',
    package_dir = {'': source_dir},
    packages    = packages,
    ext_modules = cythonize(extensions,
                            language_level=3,
                            include_path=pxd_dirs)
)
