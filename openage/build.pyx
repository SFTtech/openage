# Copyright 2016-2016 the openage authors. See copying.md for legal info.

from libopenage.build cimport build_configuration


cdef class BuildConfiguration:
    cdef build_configuration *thisptr

    def __cinit__(self):
        self.thisptr = new build_configuration()

    def __dealloc__(self):
        del self.thisptr

    property global_asset_dir:
        def __get__(self): return self.thisptr.global_asset_dir.decode()

    property config_options:
        def __get__(self): return self.thisptr.config_options.decode()

    property build_src_dir:
        def __get__(self): return self.thisptr.build_src_dir.decode()

    property compiler:
        def __get__(self): return self.thisptr.compiler.decode()

    property compiler_flags:
        def __get__(self): return self.thisptr.compiler_flags.decode()

    property cython_version:
        def __get__(self): return self.thisptr.cython_version.decode()


def get_build_configuration():
    config = BuildConfiguration()
    return config
