# Copyright 2015-2017 the openage authors. See copying.md for legal info.

from libcpp.string cimport string
from libcpp cimport bool as cppbool

from cpython.ref cimport Py_XINCREF, Py_XDECREF, PyObject

from libopenage.pyinterface.functional cimport Func1

from libopenage.pyinterface.pyobject cimport (
    PyObjectRef,

    py_xincref,
    py_xdecref,

    py_str,
    py_repr,
    py_bytes,
    py_len,
    py_callable,
    py_call,
    py_hasattr,
    py_getattr,
    py_setattr,
    py_isinstance,
    py_to_bool,
    py_dir,
    py_equals,
    py_eval,
    py_exec,
    py_get,
    py_in,
    py_type,
    py_modulename,
    py_classname,

    py_builtin,
    py_import,
    py_createstr,
    py_createbytes,
    py_createint,
    py_createdict,

    None as none_obj,
    True as true_obj,
    False as false_obj,
)


import atexit
import builtins
import importlib


cdef void xincref(PyObject *ptr) with gil:
    Py_XINCREF(ptr)


cdef void xdecref(PyObject *ptr) with gil:
    Py_XDECREF(ptr)


cdef string str_impl(PyObject *ptr) except * with gil:
    return str(<object> ptr).encode()


cdef string repr_impl(PyObject *ptr) except * with gil:
    return repr(<object> ptr).encode()


cdef string bytes_impl(PyObject *ptr) except * with gil:
    return bytes(<object> ptr)


cdef int len_impl(PyObject *ptr) except * with gil:
    return len(<object> ptr)


cdef cppbool callable_impl(PyObject *ptr) except * with gil:
    return callable(<object> ptr)


cdef void call_impl(PyObjectRef *result_ref, PyObject *ptr) except * with gil:
    cdef object result_obj = (<object> ptr)()
    result_ref.set_ref(<PyObject *> result_obj)


cdef cppbool hasattr_impl(PyObject *ptr, string name) except * with gil:
    return hasattr(<object> ptr, name.decode())


cdef void getattr_impl(PyObjectRef *result_ref, PyObject *ptr, string name) except * with gil:
    cdef object result_obj = getattr(<object> ptr, name.decode())
    result_ref.set_ref(<PyObject *> result_obj)


cdef void setattr_impl(PyObject *ptr, string name, PyObject *attr) except * with gil:
    setattr(
        <object> ptr,
        name.decode(),
        <object> attr)


cdef cppbool isinstance_impl(PyObject *ptr, PyObject *typeptr) except * with gil:
    return isinstance(<object> ptr, <object> typeptr)


cdef cppbool to_bool_impl(PyObject *ptr) except * with gil:
    return bool(<object> ptr)


cdef void dir_impl(PyObject *ptr, Func1[void, string] callback) except * with gil:
    for name in dir(<object> ptr):
        callback.call(name.encode())


cdef cppbool equals_impl(PyObject *ptr, PyObject *other) except * with gil:
    return (<object> ptr) == (<object> other)


cdef void exec_impl(PyObject *context, string statement) except * with gil:
    exec(statement.decode(), <object> context)


cdef void eval_impl(PyObject *context, PyObjectRef *result_ref, string expr) except * with gil:
    cdef object result_obj = eval(expr.decode(), <object> context)
    result_ref.set_ref(<PyObject *> result_obj)


cdef void get_impl(PyObject *ptr, PyObjectRef *result_ref, PyObject *key) except * with gil:
    cdef object result_obj = (<object> <PyObject *> ptr)[<object> key]
    result_ref.set_ref(<PyObject *> result_obj)


cdef cppbool in_impl(PyObject *ptr, PyObject *container) except * with gil:
    return (<object> ptr) in (<object> container)


cdef void type_impl(PyObject *ptr, PyObjectRef *result_ref) except * with gil:
    cdef object result_obj = type(<object> <PyObject *> ptr)
    result_ref.set_ref(<PyObject *> result_obj)


cdef string modulename_impl(PyObject *ptr) except * with gil:
    return type(<object> ptr).__module__.encode()


cdef string classname_impl(PyObject *ptr) except * with gil:
    return type(<object> ptr).__name__.encode()


cdef void builtin_impl(PyObjectRef *result_ref, string name) except * with gil:
    cdef object result_obj = getattr(builtins, name.decode())
    result_ref.set_ref(<PyObject *> result_obj)


cdef void import_impl(PyObjectRef *result_ref, string name) except * with gil:
    cdef object result_obj = importlib.import_module(name.decode())
    result_ref.set_ref(<PyObject *> result_obj)


cdef void createstr_impl(PyObjectRef *result_ref, string value) except * with gil:
    cdef object result_obj = value.decode()
    result_ref.set_ref(<PyObject *> result_obj)


cdef void createbytes_impl(PyObjectRef *result_ref, const char *value) except * with gil:
    cdef object result_obj = bytes(value)
    result_ref.set_ref(<PyObject *> result_obj)


cdef void createint_impl(PyObjectRef *result_ref, int value) except * with gil:
    cdef object result_obj = value
    result_ref.set_ref(<PyObject *> result_obj)


cdef void createdict_impl(PyObjectRef *result_ref) except * with gil:
    cdef object result_obj = dict()
    result_ref.set_ref(<PyObject *> result_obj)


def setup():
    py_xincref.bind_noexcept0(xincref)
    py_xdecref.bind_noexcept0(xdecref)

    py_str.bind0(str_impl)
    py_repr.bind0(repr_impl)
    py_bytes.bind0(bytes_impl)
    py_len.bind0(len_impl)
    py_callable.bind0(callable_impl)
    py_call.bind0(call_impl)
    py_hasattr.bind0(hasattr_impl)
    py_getattr.bind0(getattr_impl)
    py_setattr.bind0(setattr_impl)
    py_isinstance.bind0(isinstance_impl)
    py_to_bool.bind0(to_bool_impl)
    py_dir.bind0(dir_impl)
    py_equals.bind0(equals_impl)
    py_eval.bind0(eval_impl)
    py_exec.bind0(exec_impl)
    py_get.bind0(get_impl)
    py_in.bind0(in_impl)
    py_type.bind0(type_impl)
    py_modulename.bind0(modulename_impl)
    py_classname.bind0(classname_impl)

    py_builtin.bind0(builtin_impl)
    py_import.bind0(import_impl)
    py_createstr.bind0(createstr_impl)
    py_createbytes.bind0(createbytes_impl)
    py_createint.bind0(createint_impl)
    py_createdict.bind0(createdict_impl)

    none_obj.set_ref(<PyObject *><object>None)
    true_obj.set_ref(<PyObject *><object>True)
    false_obj.set_ref(<PyObject *><object>False)

    # we need a teardown function as the none_obj etc are global in C++
    # and would call pyx_decref when the libopenage is unloaded.
    # but then the python interpreter is already shut down,
    # so we need to set the objects to nullptr when python shuts down.
    def teardown():
        none_obj.set_ref(<PyObject*> 0)
        true_obj.set_ref(<PyObject*> 0)
        false_obj.set_ref(<PyObject*> 0)

    atexit.register(teardown)
