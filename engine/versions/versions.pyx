# Copyright 2020-2020 the openage authors. See copying.md for legal info.

from libcpp.map cimport map as cpp_map
from cython.operator import dereference, postincrement
from libcpp.string cimport string

from libopenage.versions.versions cimport get_version_numbers as c_get_version_numbers

def get_version_numbers():

    vn = c_get_version_numbers()
    toreturn = {}

    cdef cpp_map[string,string].iterator it = vn.begin()

    while(it != vn.end()):
        toreturn[dereference(it).first] = dereference(it).second
        postincrement(it) # Increment the iterator to the net element
    return toreturn
