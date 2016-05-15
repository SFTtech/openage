from buildsystem.setuptools import setup, find_packages, Extension, OpenageDistribution


class OpenageExtension(Extension):
    def __init__(self, *args, **kwargs):
        Extension.__init__(self, *args, **kwargs)
        self.libraries.append('openage')


extensions = [
    Extension('openage.cabextract.cabchecksum', sources=['openage/cabextract/cabchecksum.pyx']),
    OpenageExtension('openage.cabextract.lzxd', sources=['openage/cabextract/lzxd.pyx']),
    OpenageExtension('openage.cvar.cvar', sources=['openage/cvar/cvar.pyx']),
    Extension('openage.convert.slp', sources=['openage/convert/slp.pyx']),
    OpenageExtension('openage.cppinterface.exctranslate', sources=['openage/cppinterface/exctranslate.pyx']),
    OpenageExtension('openage.cppinterface.exctranslate_tests', sources=['openage/cppinterface/exctranslate_tests.pyx']),
    OpenageExtension('openage.cppinterface.pyobject', sources=['openage/cppinterface/pyobject.pyx']),
    OpenageExtension('openage.cppinterface.setup_checker', sources=['openage/cppinterface/setup_checker.pyx']),
    OpenageExtension('openage.game.main_cpp', sources=['openage/game/main_cpp.pyx']),
    OpenageExtension('openage.log.log_cpp', sources=['openage/log/log_cpp.pyx']),
    OpenageExtension('openage.testing.cpp_testing', sources=['openage/testing/cpp_testing.pyx']),
    OpenageExtension('openage.testing.misc_cpp', sources=['openage/testing/misc_cpp.pyx']),
    OpenageExtension('openage.util.fslike.cpp', sources=['openage/util/fslike/cpp.pyx'])
]

executables = [
    OpenageExtension('run', sources=['run.py'])
]

version = __import__('openage').__version__

setup(
    name='openage',
    version=version,
    maintainer='openage authors',
    maintainer_email='openageauthors@sft.mx',
    url='http://openage.sft.mx/',
    description='Free engine clone of Age of Empires II',
    license='GNU GPLv3',
    classifiers=[
        'Development Status :: 2 - Pre-Alpha',
        'License :: OSI Approved :: GNU General Public License v3 (GPLv3)',
        'Intended Audience :: Developers',
        'Intended Audience :: End Users/Desktop',
        'Operating System :: Unix',
        'Operating System :: POSIX',
        'Operating System :: MacOS :: MacOS X',
        'Programming Language :: Python :: 3',
        'Programming Language :: Cython',
        'Programming Language :: C++',
        'Topic :: Games/Entertainment :: Real Time Strategy'
    ],
    platforms='any',
    packages=['openage'] + ['openage.%s' % package for package in find_packages('openage')],
    package_data={
        '': ['*.pxd'],
    },
    distclass=OpenageDistribution,
    ext_modules=extensions,
    exe_modules=executables,
    setup_requires=['cython >= 0.23'],
    zip_safe=False
)
