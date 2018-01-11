# this file should be removed in the future.
# asdf
from distutils.core import setup
from distutils.extension import Extension
from Cython.Build import cythonize

setup(
    name = 'opusencoder',
    ext_modules = cythonize([
        Extension("opusenc",["opusenc.pyx"],
                   libraries=["ogg","opus"])
        ])
)

