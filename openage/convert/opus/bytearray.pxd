cdef extern from "Python.h":
    char *PyByteArray_AS_STRING(object string) except NULL
    Py_ssize_t PyByteArray_GET_SIZE(object string)
    int PyByteArray_Resize(object string, Py_ssize_t len) except -1
